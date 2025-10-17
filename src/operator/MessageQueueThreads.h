#if !defined(MESSAGING_QUEUE_THREADS_INCLUDED_H)
#define MESSAGING_QUEUE_THREADS_INCLUDED_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include <vector>
#include <jsoncpp/json/value.h>
#include <zmq.hpp>

class Stream;
class MessageQueueRequest;
class MessageQueueReply;

class BaseThread : public QThread {
public:
  BaseThread();
  virtual ~BaseThread();

  void stopThread();
  void terminateThread();
  bool isAborted();
  void setOutputStreams(const std::vector<Stream*>& outStreams) { m_outStreams = outStreams; }

  bool readMultiPartMessage(std::vector<std::string>& resultDataList, zmq::socket_t& socket,
                            std::string header=std::string());

private:
  bool   m_abort;
  std::vector<Stream*> m_outStreams;
  QMutex m_mutex;
};


class MessageQueueRequestThread : public BaseThread {
public:
  MessageQueueRequestThread(const std::string& host, int port, const MessageQueueRequest& mq_request);
  virtual ~MessageQueueRequestThread();

  void run();
  enum Status {
    NONE      = 0,
    STARTED      = 1,
    WORKING      = 2,
    FINISHED     = 3,
    ABORTED      = 4
  };
  Status getStatus();
  bool readInputData();
  Q_OBJECT
signals:
  void receivedData(const std::vector<std::string>& dataList);
  void error(const std::string& messageShort, const std::string& messageLong);
private slots:
  void slot_finished();

private:
  bool addMetaData();
private:
  std::vector<std::string> resultDataList;

  std::string m_host;
  int         m_port;
  const MessageQueueRequest& m_mqRequest;
  std::vector<std::ostringstream*> m_osVec;
  Status      m_status;
};

class MessageQueueSubscriberThread : public BaseThread {
public:
  MessageQueueSubscriberThread(const std::string& host, int port,
                               const std::string& header);
  virtual ~MessageQueueSubscriberThread();

  void run();

  Q_OBJECT
signals:
  void receivedData(const std::string& header, const std::vector<std::string>& dataList);
  void error(const std::string& messageShort, const std::string& messageLong);

private:
  std::string m_host;
  int         m_port;
  std::string m_header;
};

class MessageQueueReplyThread : public BaseThread {
public:
  MessageQueueReplyThread(const std::string& host, int port, const std::vector<Stream*>& inStreams,
                          MessageQueueReply& mq_reply);
  virtual ~MessageQueueReplyThread();

  void setResponseStreams(const std::vector<Stream*>& responseStreams) { m_responseStreams = responseStreams; }
  void run();
  enum Status {
    STARTED      = 1,
    WORKING      = 2,
    RECEIVE_DONE = 3,
    SEND_QUERY   = 4,
    WORKING_QUERY = 5,
    WORKING_QUERY_DONE = 6,
    FINISHED = 7
  };
  void setStatus(Status state);
  Status getStatus();
  const Json::Value& doQuery(const Json::Value& jsonObj, const std::string& sendData=std::string(),
                             Stream* file_stream=0);
  const std::vector<std::string>& getQueryData() { return m_dataList; }

  Q_OBJECT
signals:
  void receivedData(const std::string& header, const std::vector<std::string>& dataList);
  void error(const std::string& messageShort, const std::string& messageLong);

private:
  std::string m_host;
  int         m_port;
  std::vector<Stream*> m_inStreams;
  std::vector<Stream*> m_responseStreams;
  Status      m_status;
  MessageQueueReply& m_mqReply;
  Json::Value m_queryResponseJsonObj;
  std::vector<std::string> m_dataList;
  Stream*     m_fileStream;
  QMutex      m_mutex;
};

#endif
