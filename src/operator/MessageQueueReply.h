
#include "app/LSPItem.h"
#if !defined(MESSAGING_QUEUE_REPLY_INCLUDED_H)
#define MESSAGING_QUEUE_REPLY_INCLUDED_H

#include <string>
#ifdef HAVE_QT
#include <QtCore/QObject>
#endif

#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "gui/Timer.h"
#include "gui/TimerTask.h"
#include "gui/ConfirmationListener.h"
#include "operator/MessageQueue.h"

#include "zmq.hpp"

class Stream;
class JobFunction;
class MessageQueueReplyThread;
class ConnectionListener;

#ifdef HAVE_QT
class MessageQueueReply : public QObject, public ConfirmationListener, public LSPItem
#else
class MessageQueueReply : public ConfirmationListener, public LSPItem
#endif
{
public:
  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
  MessageQueueReply ( const std::string &s
                      , const std::string& host
                      , int port
                      , const std::vector<Stream*>& default_in_streams
                      , const std::vector<Stream*>& default_out_streams
                      , JobFunction *default_func);
  virtual ~MessageQueueReply();

  /** add reply header data
   */
  void addHeader( std::string header, std::vector<Stream*>& in,
                  std::vector<Stream*>&  out, std::vector<Plugin*> plugins, JobFunction* func);

  /** start message queue subscriber
   */
  void start();
  /** stop message queue subscriber
   */
  void stop();

  /** set connection listener for a single reply to message queue publisher
   */
  void setListener( ConnectionListener *listener ){
    m_listener = listener;
  }

  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();

  std::vector<Stream*> getDefaultInStreams()  { return m_default_in_streams; }
  std::vector<Stream*> getDefaultOutStreams() { return m_default_out_streams; }

  /* void processSubscribe(MessageQueue::HeaderData* subsType, const std::vector<std::string>& dataList ); */

#ifdef HAVE_QT
  Q_OBJECT
private slots:
  void slot_error(const std::string& messageShort, const std::string& messageLong);
  void slot_receivedReplyData(const std::string&, const std::vector<std::string>&);
#endif

// -----------------------------------------------------
// private definitions
// -----------------------------------------------------
private:
  class Trigger : public JobStarter
  {
  public:
    Trigger( MessageQueueReply *subs, JobFunction *f )
      : JobStarter( f )
      , m_reply( subs ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    MessageQueueReply  *m_reply;
  };

  //-------------------------------------------------
  // private TimerTask class with tick method
  //-------------------------------------------------
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(MessageQueueReply* msgQueue)
      : m_msgQueue(msgQueue){}
    void tick();
  private:
    MessageQueueReply* m_msgQueue;
  };

  void timerEvent(QTimerEvent * event);

private:
  std::string   m_host;
  int           m_portReply;
  MessageQueueReplyThread* m_replyThread;
  Timer                 *m_timer;
  TimerTask             *m_task;
  bool                   m_triggerStarted;

  std::vector<Stream*>   m_default_in_streams;
  std::vector<Stream*>   m_default_out_streams;
  JobFunction           *m_default_function;
  MessageQueue::HeaderDataMap  m_headerDataMap;
  ConnectionListener    *m_listener;
};

#endif
