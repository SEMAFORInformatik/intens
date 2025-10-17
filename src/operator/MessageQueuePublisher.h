#if !defined(MESSAGING_QUEUE_PUBLISHER_INCLUDED_H)
#define MESSAGING_QUEUE_PUBLISHER_INCLUDED_H

#include <string>
#include <QObject>

#include "job/JobStarter.h"
#include "gui/Timer.h"
#include "gui/TimerTask.h"
#include "gui/ConfirmationListener.h"
#include "operator/MessageQueue.h"

#include "zmq.hpp"

class Stream;
class MessageQueueReplyThread;
class ConnectionListener;

class MessageQueuePublisher : public QObject, public ConfirmationListener
{
public:
  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
  MessageQueuePublisher ( const std::string &s
                          , const std::string& host
                          , int port);
  virtual ~MessageQueuePublisher();

  /** set publish output Streams for next operation
   */
  void setPublishOutStreams( const std::vector<Stream*>& out_streams );
  /** set publish header for next operation
   */
  void setPublishHeader( const std::string& header );

  /** start a single reply to message queue publisher
   */
  void startPublish(bool duplicateCheck=false);
  /** check input for a single request to message queue publisher
   */
  bool checkPublish();

  /** set connection listener for a single reply to message queue publisher
   */
  void setListener( ConnectionListener *listener ){
    m_listener = listener;
  }

  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();

  /* void processSubscribe(MessageQueue::HeaderData* subsType, const std::vector<std::string>& dataList ); */

  Q_OBJECT
private slots:
  void slot_error(const std::string& messageShort, const std::string& messageLong);
  void slot_receivedReplyData(const std::string&, const std::vector<std::string>&);

// -----------------------------------------------------
// private definitions
// -----------------------------------------------------
private:
  bool publishHeader();

  class Trigger : public JobStarter
  {
  public:
    Trigger( MessageQueuePublisher *subs, JobFunction *f )
      : JobStarter( f )
      , m_reply( subs ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    MessageQueuePublisher  *m_reply;
  };

  //-------------------------------------------------
  // private TimerTask class with tick method
  //-------------------------------------------------
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(MessageQueuePublisher* msgQueue)
      : m_msgQueue(msgQueue){}
    void tick();
  private:
    MessageQueuePublisher* m_msgQueue;
  };

private:
  std::string   m_host;
  int           m_port;
  zmq::socket_t* m_publisher;
  Timer                 *m_timer;
  TimerTask             *m_task;

  std::vector<Stream*>   m_publish_out_streams;
  std::string            m_publishHeader;
  ConnectionListener    *m_listener;
  std::vector<std::string> m_lastPublishString;
};

#endif
