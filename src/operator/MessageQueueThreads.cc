#include <string>

#include "streamer/Stream.h"

#include "operator/MessageQueue.h"
#include "operator/MessageQueueRequest.h"
#include "operator/MessageQueueReply.h"
#include "operator/MessageQueueThreads.h"
#include "zhelpers.h"

INIT_LOGGER();

static std::string s_recv_write_to_file_stream(zmq::socket_t & socket, Stream* stream) {

    zmq::message_t message;
#ifdef CPPZMQ_VERSION
    zmq::recv_result_t nbytes =
    socket.recv(message);
#else
    socket.recv(&message);
#endif
    BUG_DEBUG("s_recv_write_to_file_stream, size: "<< message.size());
    return stream->writeFileData(message.data(), message.size());
}

static bool s_send_read_from_file_stream(zmq::socket_t & socket, Stream* stream) {
  BUG_DEBUG("s_send_read_from_file_stream, size: "<< stream->getStreamFileSize());
  zmq::message_t message(stream->getStreamFileSize()+1); // +1 for end character '\0'

  char *cp = static_cast<char*>(message.data());
  stream->readFileData(cp);
  cp[stream->getStreamFileSize()] = '\0';
#ifdef CPPZMQ_VERSION
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    zmq::recv_result_t nbytes;
#else
    zmq::detail::recv_result_t nbytes;
#endif
    nbytes = socket.send (message, zmq::send_flags::none);
    BUG_DEBUG("s_send_read_from_file_stream, really send size: "<< nbytes.value());
    bool rc;
    try {
      rc = (nbytes.value() >= 0);
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    } catch(const std::bad_optional_access& e) {
#else
    } catch(const std::exception& e) {
#endif
      std::cerr << e.what() << std::endl;
      rc = false;
    }
#else
    bool rc = socket.send (message);
#endif
    return (rc);
}

BaseThread::BaseThread()
  : m_abort(false)
{
}

BaseThread::~BaseThread() {
   m_mutex.lock();
   m_abort = true;
   m_mutex.unlock();

   wait();
}

bool BaseThread::isAborted() {
  return m_abort;
}

void BaseThread::stopThread() {
  m_mutex.lock();
  m_abort = true;
  m_mutex.unlock();
}

void BaseThread::terminateThread() {
  terminate();
}

bool BaseThread::readMultiPartMessage(std::vector<std::string>& resultDataList, zmq::socket_t& socket,
                                      std::string header) {
  resultDataList.clear();
#if ZMQ_VERSION_MAJOR == 2
  int64_t more; // Multipart detection
#elif ZMQ_VERSION_MAJOR >= 3
  int more; // Multipart detection
#endif
#if CPPZMQ_VERSION < ZMQ_MAKE_VERSION(4, 7, 0)
  size_t more_size = sizeof (more);
#endif

  // read subscriber connection
  bool headerNotDetected = true;

  BUG_DEBUG("readMultiPartMessage");
  int idx(0);
  do {
    std::string msg;
    bool isFile(idx < m_outStreams.size() ? m_outStreams[idx]->getFileFlag() : false);
    if (isFile) {
      // returns filename
      msg =  s_recv_write_to_file_stream(socket, m_outStreams[idx]);
    } else {
      msg = s_recv (socket);
    }
    BUG_DEBUG("msg, len: " << msg.size());

    // Multipart detection
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    more = socket.get(zmq::sockopt::rcvmore);
#else
    socket.getsockopt(ZMQ_RCVMORE, &more, &more_size);
#endif

    // if package is only the header
    // continue and read next package
    if ( header.size() && headerNotDetected &&
         msg.find_first_of(header) == 0) {
      headerNotDetected = false;
      continue;
    }
    // nur leere Datenpakete pushen, wenn später noch etwas kommt
    if (more || msg.size())
      // save in result list
      resultDataList.push_back(msg);
    idx++;
  }
  while (more);

  return true;
}

MessageQueueRequestThread::MessageQueueRequestThread(const std::string& host, int port,
                                                     const MessageQueueRequest& mq_request)
: m_host(host)
, m_port(port)
, m_mqRequest(mq_request)
, m_status(NONE)
{
  connect(this, SIGNAL(finished()), this, SLOT(slot_finished()));
}

MessageQueueRequestThread::~MessageQueueRequestThread() {
}

MessageQueueRequestThread::Status MessageQueueRequestThread::getStatus() {
  return m_status;
}

bool MessageQueueRequestThread::readInputData() {
  try  {
    m_status = STARTED;
    addMetaData();  // add metaData depends on start option

    // Lesen der InputStreams
    for(std::vector<Stream*>::const_iterator it = m_mqRequest.getRequestInStreams().begin();
        it != m_mqRequest.getRequestInStreams().end(); ++it) {
      std::ostringstream* os = new std::ostringstream();
      m_osVec.push_back(os);
      if ( !(*it)->write( *os )) {
        os->str("");
        *os << "error occured in input stream named '"<<(*it)->Name()<<"'";
        BUG_INFO("ErrorMessage Short: " << os->str() << "\nLong:" << (*it)->getErrorMessage());
        m_status = ABORTED;
        emit error(os->str(), (*it)->getErrorMessage());
        for(std::vector<std::ostringstream*>::iterator itOs = m_osVec.begin(); itOs != m_osVec.end(); ++itOs)
          delete *itOs;
        m_osVec.clear();
        return false;
      }
      BUG_DEBUG("send["<<os->str() <<"]");
    }
    return true;
  } catch (const std::exception& e) {
    std::ostringstream os;
    os << "Error occurs in RequestThread host '"<< m_host <<"' at port '"
       << m_port << "' msg '"<<e.what()<<"'";
    m_status = ABORTED;
    BUG_ERROR("MessageQueueRequestThread::readInputData Exception: " << os.str());
  }
  return false;
}

void MessageQueueRequestThread::run() {
  BUG_INFO("MessageQueueRequestThread::run");
  try  {
    std::ostringstream ostr;

    // synchronize with publisher
    zmq::socket_t syncclient(MessageQueue::getContext(), ZMQ_REQ);
    // timeout
    if (m_mqRequest.getRequestTimeout() > 0) {
      int timeout = m_mqRequest.getRequestTimeout() * 1000;
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
      syncclient.set(zmq::sockopt::rcvtimeo, timeout);
#else
      syncclient.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(int));
#endif
    }
    // linger
    int linger = 1000;
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    syncclient.set(zmq::sockopt::linger, linger);
#else
    syncclient.setsockopt(ZMQ_LINGER, &linger, sizeof(int));
#endif
    ostr << "tcp://" << m_host << ":" << m_port;
    // connect
    syncclient.connect(ostr.str().c_str()); //"tcp://localhost:5562");

    // - send a synchronization request
    if (m_osVec.size()) {
      if (m_mqRequest.getRequestHeader().size()) // only if set
        s_sendmore (syncclient, m_mqRequest.getRequestHeader());
      for(std::vector<std::ostringstream*>::iterator itOs = m_osVec.begin(); itOs != m_osVec.end(); ++itOs) {
        if ((itOs + 1) == m_osVec.end()) {
          s_send (syncclient, (*itOs)->str());
        } else {
          s_sendmore (syncclient, (*itOs)->str());
        }
        delete *itOs;
      }
      m_osVec.clear();
    } else {
      s_send (syncclient, m_mqRequest.getRequestHeader());
    }

    BUG_INFO("waiting for response");
    // - wait for synchronization reply
    m_status = WORKING;
    readMultiPartMessage(resultDataList, syncclient);
    m_status = FINISHED;
    syncclient.close();
  } catch (const std::exception& e) {
    std::ostringstream os;
    os << "Error occurs in RequestThread host '"<< m_host <<"' at port '"
       << m_port << "' msg '"<<e.what()<<"'";
    BUG_ERROR("MessageQueueRequestThread::run Exception: " << os.str());
    m_status = ABORTED;
    emit error(e.what(), os.str());
  }
}

void MessageQueueRequestThread::slot_finished() {
  emit receivedData(resultDataList);
  resultDataList.clear();
}

bool MessageQueueRequestThread::addMetaData() {
  if (AppData::Instance().sendMessageQueueWithMetadata()) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    jsonElem["host"] = AppData::Instance().HostName();
    std::ostringstream* os = new std::ostringstream();
    *os << ch_semafor_intens::JsonUtils::value2string(jsonElem);
    m_osVec.insert(m_osVec.begin(), os); // m_osVec.push_back(os);
    BUG_INFO("MessageQueueRequestThread::addMetaData: " << (*os).str());
    return true;
  }
  return false;
}

MessageQueueSubscriberThread::MessageQueueSubscriberThread(const std::string& host,
                                                           int port,
                                                           const std::string& header)
: m_host(host),
  m_port(port),
  m_header(header) {
}

MessageQueueSubscriberThread::~MessageQueueSubscriberThread() {
}

void MessageQueueSubscriberThread::run() {
  BUG_INFO("MessageQueueSubscriberThread::run for header: " << m_header);
  try  {
    std::ostringstream os;

    // connect our subscriber socket
    zmq::socket_t subscriber(MessageQueue::getContext(), ZMQ_SUB);
    os << "tcp://"<< m_host << ":" << m_port;
    subscriber.connect(os.str().c_str());
    os.str("");
    std::string header;
    if (AppData::Instance().sendMessageQueueWithMetadata()) {
      header = AppData::Instance().HostName();
    } else {
      header = m_header;
    }
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    subscriber.set(zmq::sockopt::subscribe, header);
#else
    subscriber.setsockopt(ZMQ_SUBSCRIBE,
                          header.c_str(),
                          header.size());
#endif
    sleep(1); //  0MQ is so fast, we need to wait a while...

    // read subscriber connection
    bool headerNotDetected = true;
    while (1) {
      std::vector<std::string> resultDataList;
      try  {
        bool result;
        if (AppData::Instance().sendMessageQueueWithMetadata()) {
          result = readMultiPartMessage(resultDataList, subscriber, AppData::Instance().HostName());
          if (resultDataList[0] != m_header) {
            continue;
          }
          // Remove the first element which is equal to m_Header when behind the load balancer
          resultDataList.erase(resultDataList.begin());
        } else {
          result = readMultiPartMessage(resultDataList, subscriber, m_header);
        }
        BUG_INFO("Read header: " << m_header <<", datasize: " << resultDataList.size());
        if (result){
          #ifdef HAVE_QT
          emit receivedData(m_header, resultDataList);
          #endif
        }
      }
      catch (const zmq::error_t& e) {
        // on ubuntu 10.04 (PSI), zmq::error_t exception 'Interrupted system call' is called
        // when a file dialog is opened
        if ( std::string(e.what()) == "Interrupted system call" ) {
          sleep(1);
        } else {
          throw;
        }
      }

      if (isAborted()) break;
    }
  } catch (const std::exception& e) {
    std::ostringstream os;
    os << "Error occurs in SubscriberThread host '"<< m_host <<"' at port '"
       << m_port << "' and header '" << m_header << "' msg '"<<e.what()<<"'";
    BUG_ERROR("MessageQueueSubscriberThread::run Exception: " << os.str());
    #ifdef HAVE_QT
    emit error(e.what(), os.str());
    #endif
  }
}


MessageQueueReplyThread::MessageQueueReplyThread(const std::string& host,
                                                 int port, const std::vector<Stream*>& inStreams,
                                                 MessageQueueReply& mq_reply)
: m_host(host)
, m_port(port)
, m_inStreams(inStreams)
, m_status(FINISHED)
, m_mqReply(mq_reply)
, m_fileStream(0)
{
}

MessageQueueReplyThread::~MessageQueueReplyThread() {}

MessageQueueReplyThread::Status MessageQueueReplyThread::getStatus() {
  return m_status;
}

void MessageQueueReplyThread::setStatus(MessageQueueReplyThread::Status state) {
  m_mutex.lock();
  m_status= state;
  m_mutex.unlock();
}

void MessageQueueReplyThread::setExpectedQueryResultMembers(const std::vector<std::string>& member){
  m_mutex.lock();
  m_expectedQueryResultMembers.clear();
  std::copy(member.begin(), member.end(), back_inserter(m_expectedQueryResultMembers));
  m_mutex.unlock();
}

void MessageQueueReplyThread::run() {
  BUG_INFO("MessageQueueReplyThread::run Port: " << m_port);
  std::ostringstream os;

  try {
    setStatus(STARTED);

    // Socket to receive signals
    zmq::socket_t syncservice(MessageQueue::getContext(), ZMQ_REP);
    os << "tcp://*:" << m_port;
    syncservice.bind(os.str().c_str());
    BUG_INFO("Reply Connect to, " << os.str().c_str());

    // Get synchronization from subscribers
    while (1) {
      std::string header;
      std::vector<std::string> resultDataList;
      BUG_DEBUG("Waiting ...");
      if (readMultiPartMessage(resultDataList, syncservice)) {
        setStatus(WORKING);

        // first data is the header
        header = resultDataList[0];
        BUG_INFO("Reply received data, Port: " << os.str().c_str() << ", header: " << header << ", data size: " << resultDataList.size());
        resultDataList.erase(resultDataList.begin());
        emit receivedData(header, resultDataList);
      }

      // wait if function has finished, or a function interrupt (Query)
      while (getStatus() != RECEIVE_DONE) {

        // send a query
        if (getStatus() == SEND_QUERY) {
          int idx=0;
          m_dataList.erase(m_dataList.begin(), m_dataList.end());  // reset
          for(std::vector<Stream*>::iterator it = m_responseStreams.begin();
              it != m_responseStreams.end(); ++it, ++idx) {
            std::ostringstream os;
            BUG_INFO("Send Query, " << idx <<  ".dataLen: " << os.str().size());
            if (idx+1 < m_responseStreams.size()) {
              (*it)->write(os);
              s_sendmore (syncservice, os.str());
            } else {
              if (m_fileStream) {  // special case, we use this stream
                s_send_read_from_file_stream(syncservice, m_fileStream);
              } else {
                (*it)->write(os);
                s_send (syncservice, os.str());
              }
            }
          }
          setStatus(WORKING_QUERY);

          // waiting for query answer
          BUG_DEBUG("Waiting for Answer");
          bool bHeader(false);
          m_queryResponseJsonObj = Json::Value(Json::objectValue);  // reset
          if (readMultiPartMessage(m_dataList, syncservice)) {
            BUG_INFO("Got Answer, size: "<< m_dataList.size()<< ", expectedEmpty: " << m_expectedQueryResultMembers.empty());
            setStatus(WORKING);
            if (m_dataList.size() > 1) {
              header = m_dataList[0];
              m_dataList.erase(m_dataList.begin());
              bHeader = true;
            }
            try {
              if (m_dataList.size() && bHeader) {
                BUG_INFO("Got Answer, header: " << header << ", data: " << m_dataList[0].substr(0, 250));
                m_queryResponseJsonObj = ch_semafor_intens::JsonUtils::parseJson(m_dataList[0]);
                // expected check
                if (!m_expectedQueryResultMembers.empty()){
                  bool bExpected(false);
                  for (auto expected: m_expectedQueryResultMembers){
                    BUG_DEBUG("Check expected : " << expected);
                    if (m_queryResponseJsonObj.isMember(expected)){
                      bExpected = true;
                      break;
                    }
                  }
                  if (!m_expectedQueryResultMembers.empty() && !bExpected){
                    BUG_INFO("Unexpected query answer, expectedEmpty: " << m_expectedQueryResultMembers.empty());
                    setStatus(WORKING);
                    emit receivedData(header, m_dataList);
                  }
                  m_dataList.erase(m_dataList.begin());
                }
              }
            }
            catch (const std::exception& e) {
              BUG_INFO("Got Query Response not JSON: " << e.what());
            }
          }
          setStatus(WORKING_QUERY_DONE);
        }

        usleep(100);
      }
      BUG_INFO("Reply send response, header: " << header << ", responseStreamSize: " << m_responseStreams.size());

      int idx=0;
      for(std::vector<Stream*>::iterator it = m_responseStreams.begin();
          it != m_responseStreams.end(); ++it, ++idx) {
        std::ostringstream os;
        (*it)->write( os );
        BUG_INFO("Send stream, " << idx <<  ".dataLen: " << os.str().size() << ", data: " << os.str().substr(0, 100));
        if (idx+1 < m_responseStreams.size())
          s_sendmore (syncservice, os.str());
        else
          s_send (syncservice, os.str());
      }
    }
  } catch (const std::exception& e) {
    os.str("");
    os << "Error occurs at port '"<<m_port<<  "' msg '"<<e.what()<<"'";
    BUG_ERROR("MessageQueueReplyThread::run Exception: " << os.str());
    emit error(e.what(), os.str());
  }
  setStatus(FINISHED);
}

const Json::Value& MessageQueueReplyThread::doQuery(const Json::Value& jsonObj,
                                                    const std::vector<std::string>& expected_members,
                                                    const std::string& sendData,
                                                    Stream* file_stream) {
  m_fileStream = file_stream;  // use file stream if set
  while (getStatus() == WORKING_QUERY || getStatus() == SEND_QUERY) {
    usleep(100);
  }

  // clear data
  DataReference *ref = DataPoolIntens::Instance().getDataReference("mqReply_function_response.data");
  if ( ref != 0 ) {
    ref->SetValue(sendData);  // old: ""
    delete ref;
  }
  // set status
  DataReference* refStatus = DataPoolIntens::Instance().getDataReference("mqReply_response.status");
  if ( refStatus != 0 ) {
    refStatus->SetValue("Query");
  }
  // set message
  DataReference*refMsg = DataPoolIntens::Instance().getDataReference("mqReply_response.message");
  if ( refMsg != 0 ) {
    BUG_INFO("Query: " << ch_semafor_intens::JsonUtils::value2string(jsonObj).substr(0, 150));
    refMsg->SetValue(ch_semafor_intens::JsonUtils::value2string(jsonObj));
  }
  setStatus(SEND_QUERY);
  setExpectedQueryResultMembers(expected_members);

  while (getStatus() != WORKING_QUERY_DONE) {
    usleep(100);
  }

  // reset query data
  if ( refStatus != 0 ) {
    refStatus->SetValue("OK");
    delete refStatus;
  }
  if ( refMsg != 0 ) {
    refMsg->SetValue("");
    delete refMsg;
  }

  BUG_INFO("Query Done. Valid RetObj: " << !m_queryResponseJsonObj.isNull());
  return m_queryResponseJsonObj;
}
