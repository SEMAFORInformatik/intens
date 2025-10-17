
#include <string>
#include <fstream>
#include "operator/zhelpers.h"
#include <unistd.h>
#include "jsoncpp/json/writer.h"
#include "jsoncpp/json/reader.h"

int debug = 0;

Json::Value parseJson(const std::string& jsonString) {
  Json::Value root;   // will contain the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse( jsonString, root );
  if ( !parsingSuccessful ) {
    std::cerr << "JsonUtils::parseJson, size: " << jsonString.size() <<", data:" << jsonString.substr(0, 100)
              << "Failed to parse jsonString\n" << reader.getFormattedErrorMessages() << std::endl;
  }
  return root;
}

void usage(char *argv[]) {
  std::cout << "usage:" << std::endl
            << argv[0] << " -p port -c command <-h hostname> <-t timeout> <-a query answer:yes/no>"
            << "<-i inputFile> <-o outputFile>  <-d>" << std::endl;
}

int main (int argc, char *argv[])
{
  // variables
  std::string host("localhost");
  std::string answer("yes");
  std::string command;
  int port = 5560;
  int timeout = 0;
  std::string inputFile;
  std::string outputFile;

  // help
  for(int i=1; i < (argc); i++) {
    if (strcmp(argv[i], "--help") == 0) {
      usage(argv);
      return 0;
    }
    if (strcmp(argv[i], "-d") == 0) {
      debug = 1;
    }
  }

  // read arguments
  std::string nextArg;
  for(int i=1; i < (argc); i++) {
    nextArg = (i+1) < argc && argv[i+1][0] != '-' ? argv[i+1] : "";
    if (strcmp(argv[i], "-p") == 0) {
      port = atoi(nextArg.c_str());
    }
    if (strcmp(argv[i], "-h") == 0) {
      host = nextArg;
    }
    if (strcmp(argv[i], "-c") == 0) {
      command = argv[i+1];
    }
    if (strcmp(argv[i], "-t") == 0) {
      timeout = atoi(nextArg.c_str());
    }
    if (strcmp(argv[i], "-i") == 0) {
      inputFile = nextArg.size() ? nextArg : "cin";
    }
    if (strcmp(argv[i], "-o") == 0) {
      outputFile = nextArg.size() ? nextArg : "cout";
    }
    if (strcmp(argv[i], "-a") == 0) {
      answer = nextArg;
    }
  }
  std::stringstream os;
  os << "tcp://"<< host << ":" << port;
  if (debug) {
    std::cout << "Connect Str: ["<<os.str()<<"]\n";
    std::cout << "Command Str: ["<<command<<"]\n";
  }

  // valid arguments
  if (command.size() == 0|| port < 1) {
    usage(argv);
    exit(1);
  }

  // do request
  try {
#if defined WIN32 || defined ZMQ_VERSION_MAJOR < 4
    zmq::context_t context;
#else
    zmq::context_t context = zmq::context_t(1);
#endif
    zmq::socket_t reqClient(context, ZMQ_REQ);
    reqClient.connect (os.str().c_str());
    if (timeout) {
      if (debug) std::cout << "Timeout: " << timeout << std::endl;
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
      reqClient.set(zmq::sockopt::rcvtimeo, timeout);
#else
      reqClient.setsockopt (ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
#endif
    }
    int linger = 1000;
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    reqClient.set(zmq::sockopt::linger, linger);
#else
    reqClient.setsockopt (ZMQ_LINGER, &linger, sizeof(linger));
#endif
    s_sendmore(reqClient, command);
    if (debug) {
      std::cout << "SEND ["<<command<<"]\n";
    }

    // send data
    if (inputFile.size()) {
      std::stringstream buffer;
      if (inputFile == "cin") {
        buffer << std::cin.rdbuf();
      } else {
        std::ifstream ifs(inputFile.c_str());
        buffer << ifs.rdbuf();
      }
      s_send(reqClient, buffer.str().c_str());
      if (debug) {
        std::cout << "SEND ["<<buffer.str()<<"]\n";
      }
    } else {
      s_send(reqClient, "{}");
    }

    // read answer
    int more = 0; //  Multipart detection
#if CPPZMQ_VERSION < ZMQ_MAKE_VERSION(4, 7, 0)
    size_t more_size = sizeof(more);
#endif
    int i = 0;
    int send_answer = 0;
    do {
      std::string rstring = s_recv(reqClient);
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
      more = reqClient.get(zmq::sockopt::rcvmore);
#else
      reqClient.getsockopt (ZMQ_RCVMORE, &more, &more_size);
#endif
      if (debug) {
        if (rstring.size() < 1e3) {
          std::cout << "RECV idx["<<i<<"] size["<<rstring.size()<<"] ["<<rstring<<"]" << std::endl;
        }
        else std::cout << "RECV idx["<<i<<"] size["<<rstring.size()<<"]" << std::endl;
      }
      try {
        if (rstring.find("{") != std::string::npos && rstring.size() < 3) {
          Json::Value data = parseJson(rstring);
          if (data["status"] == "Query") {
            send_answer = 1;
          }
        }
      }
      catch (const std::exception& e) {
      }

      // save answer
      std::stringstream os;
      if (more) os << i << "_";
      if (outputFile.size()) {
        if (outputFile == "cout") {
          std::cout << rstring;
        } else {
          std::ofstream ofs((os.str() + outputFile).c_str());
          ofs << rstring;
          ofs.close();
        }
      }
      else{
        if (debug){
          if (rstring.size() < 1e3) {
            std::cout << "Answer: idx["<<i<<"] " << rstring << std::endl;
          }
          else std::cout << "Answer: idx["<<i<<"] ["<<rstring.substr(0, 100)<<"...]" << std::endl;
        }
      }
      ++i;

      // query
      if (!more && send_answer == 1) {
        std::string s("{\"return\":\"" + answer + "\"}");
        if (debug) std::cout << "Send answer s["<<s<<"].\n";
        s_sendmore(reqClient, "answer");
        s_send(reqClient, s);
        more = true;
        send_answer = 2;
        if (debug) std::cout << "Send answer Done.\n";
      }

    } while(more);

  }
  catch (const std::exception& e) {
    std::stringstream os;
    os << "Error occurs in ReqClient host '"<< host <<"' at port '"
       << port << "' msg '"<<e.what()<<"'";
    std::cerr << "Error Str: ["<<os.str()<<"]" << std::endl;
  }

  if (debug) {
    std::cout << "done" << std::endl;
  }
  exit(0);
}
