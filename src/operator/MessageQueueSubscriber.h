
#ifndef MESSAGING_QUEUE_SUBSCRIBER_INCLUDED_H
#define MESSAGING_QUEUE_SUBSCRIBER_INCLUDED_H

#include <string>
#include <QtCore/QObject>

#include "app/LSPItem.h"
#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "operator/MessageQueue.h"

class Stream;
class JobFunction;
class MessageQueueRequestThread;

#ifdef HAVE_QT
class MessageQueueSubscriber : public QObject, public LSPItem
#else
class MessageQueueSubscriber : public LSPItem
#endif
{
public:
  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
  MessageQueueSubscriber( const std::string &name,
			  const std::string& host,
			  int port);
  virtual ~MessageQueueSubscriber();

  /** add subscribe header data
   */
  void addHeader( std::string header,
		  std::vector<Stream*>& in,
                  std::vector<Stream*>& out,
		  std::vector<Plugin*> plugins,
		  JobFunction* func);

  /** set hostname and start
   */
  bool setHost( std::string &host );

  /** start message queue subscriber
   */
  void start();

  /** stop message queue subscriber
   */
  void stop();

  /** processSubscribe
   */
  void processSubscribe(MessageQueue::HeaderData* subsType,
                        const std::vector<std::string>& dataList );

#ifdef HAVE_QT
  /** timer event for GuiUpdates
   */
  void timerEvent ( QTimerEvent * event );

  Q_OBJECT
private slots:
  void slot_error(const std::string& messageShort,
		  const std::string& messageLong);
  void slot_receivedSubscriberData(const std::string& header,
				   const std::vector<std::string>& dataList);
#endif

// -----------------------------------------------------
// private definitions
// -----------------------------------------------------
private:
  class Trigger : public JobStarter
  {
  public:
    Trigger( MessageQueueSubscriber *subs, JobFunction *f )
      : JobStarter( f )
      , m_subscriber( subs ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    MessageQueueSubscriber  *m_subscriber;
  };

  //-------------------------------------------------
  // private SubscriberData class
  //-------------------------------------------------
  class SubscriberData {
  public:
  SubscriberData(const std::vector<std::string>& data,
		 MessageQueue::HeaderData* subscriberType,
                 MessageQueueSubscriber* subscriber)
    : m_data(data),
      m_subscriberType(subscriberType),
      m_subscriber(subscriber) {
    }
    std::vector<std::string>  m_data;
    MessageQueue::HeaderData* m_subscriberType;
    MessageQueueSubscriber   *m_subscriber;
  };

private:
  std::string   m_name;
  std::string   m_host;
  int           m_port;
  int           m_timerId;

  static std::vector<SubscriberData*> s_procSubsVec;
  static bool s_triggerStarted;

  MessageQueue::HeaderDataMap  m_headerDataMap;
};

#endif
