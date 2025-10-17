
#include <string>
#include <fstream>
#include "operator/zhelpers.h"
#include <unistd.h>

int debug = 0;


void usage(char *argv[]) {
  std::cout << "usage:" << std::endl
            << argv[0] << " -p port -m msg <-h hostname> "
            << "  <-d>" << std::endl;
}

int main (int argc, char *argv[])
{
  // variables
  std::string host("localhost");
  std::string msg("interrupt");
  int port = 5560;

  // help
  for(int i=1; i < (argc); i++) {
    if (strcmp(argv[i], "--help") == 0) {
      usage(argv);
      return 0;
    }
  }

  // read arguments
  std::string nextArg;
  int i = 1;
  while(i < (argc)){
    nextArg = (i+1) < argc && argv[i+1][0] != '-' ? argv[i+1] : "";
    if (strcmp(argv[i], "-p") == 0) {
      port = atoi(nextArg.c_str());
      i = i+2;
    }
    else if (strcmp(argv[i], "-h") == 0) {
      host = nextArg;
      i = i+2;
    }
    else if (strcmp(argv[i], "-m") == 0) {
      msg = argv[i+1];
      i = i+2;
    }
    else if (strcmp(argv[i], "-d") == 0) {
      debug = 1;
      i = i+1;
    }
    else {
      std::cerr << "Ignored Argument " << i << ": " <<argv[i] << std::endl;
    }
  }
  std::stringstream os;
  os << "tcp://"<< host << ":" << port;
  if (debug) {
    std::cerr << "Connect Str: ["<<os.str()<<"]\n";
    std::cerr << "Message Str: ["<<msg<<"]\n";
  }

  // valid arguments
  if (msg.size() == 0|| port < 1) {
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
    zmq::socket_t reqClient(context, ZMQ_PUSH);
    reqClient.connect (os.str().c_str());
    int linger = 1000;
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    reqClient.set(zmq::sockopt::linger, 1000);
#else
    reqClient.setsockopt (ZMQ_LINGER, &linger, sizeof(linger));
#endif
    s_send(reqClient, msg);
    if (debug) {
      std::cout << "PUSH ["<<msg<<"]\n";
    }

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
