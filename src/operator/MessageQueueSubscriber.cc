
#include <QTimerEvent>
#include <QTime>
#include <QThread>
#include <QMetaType>

#include <sstream>

#include "streamer/Stream.h"
#include "xfer/XferDataItem.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "app/Plugin.h"
#include "gui/Timer.h"
#include "operator/ConnectionListener.h"
#include "operator/MessageQueueSubscriber.h"
#include "operator/MessageQueueThreads.h"

INIT_LOGGER();

std::vector<MessageQueueSubscriber::SubscriberData*> MessageQueueSubscriber::s_procSubsVec;
bool MessageQueueSubscriber::s_triggerStarted = false;

//-------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------
MessageQueueSubscriber::MessageQueueSubscriber( const std::string &name,
						const std::string &host,
						int port)
  : m_name( name )
  , m_host( host )
  , m_port(port)
  , m_timerId( 0 )
{
}

MessageQueueSubscriber::~MessageQueueSubscriber(){
}

// -----------------------------------------------------
// addHeader
// -----------------------------------------------------
void MessageQueueSubscriber::addHeader( std::string header,
					std::vector<Stream*>& in,
					std::vector<Stream*>&  out,
					std::vector<Plugin*> plugins,
					JobFunction* func){
  BUG_DEBUG("MessageQueueSubscriber::setSubscriberTypeData header: "<< header);
  if ( m_headerDataMap.find(header) == m_headerDataMap.end() )
    m_headerDataMap.insert( MessageQueue::HeaderDataMap::value_type(header,
                            new MessageQueue::HeaderData(in, out, plugins, func) ) );
  else {
    m_headerDataMap[header]->m_inVectorListener  = in;
    m_headerDataMap[header]->m_outVectorListener = out;
    m_headerDataMap[header]->m_pluginVector      = plugins;
    m_headerDataMap[header]->m_function          = func;
  }
}

// -----------------------------------------------------
// setHost
// -----------------------------------------------------
bool MessageQueueSubscriber::setHost( std::string &host ){
  if( !m_host.empty() ){
    if( m_host == host ){
      BUG_WARN("Hostname of Messagequeue " << m_name <<
	       " is already set to " << m_host);
      return true;
    }
    BUG_ERROR("Hostname of Messagequeue " << m_name <<
	      " is set to " << m_host <<
	      ": " << host << " refused");
    return false;
  }

  m_host = host;
  start();
  return true;
}

//-------------------------------------------------
// stop
//-------------------------------------------------
void MessageQueueSubscriber::stop(){
}

//-------------------------------------------------
// start
//-------------------------------------------------
void MessageQueueSubscriber::start(){
  BUG_DEBUG("MessageQueueSubscriber::start, port: " << m_port);

  // start subscriber connection(s) to message queue
  if ( m_port && !m_host.empty() ) {
    BUG_INFO("Start Subscriber " << m_name << " on " << m_host << "(" << m_port << ")" );

    MessageQueueSubscriberThread *thrS;
    MessageQueue::HeaderDataMap::iterator it = m_headerDataMap.begin();
    for ( ; it!= m_headerDataMap.end(); ++it) {
      BUG_DEBUG("Create Subscriber Thread for Header ["<<(*it).first<<"]");
      thrS = new MessageQueueSubscriberThread( m_host, m_port, (*it).first );
#ifdef HAVE_QT
      connect(thrS, SIGNAL(receivedData(const std::string&, const std::vector<std::string>&)),
              this, SLOT(slot_receivedSubscriberData(const std::string&, const std::vector<std::string>&)),
              Qt::QueuedConnection);
      connect(thrS, SIGNAL(error(const std::string&,const std::string&)),
              this, SLOT(slot_error(const std::string&,const std::string&)),
              Qt::QueuedConnection);
#endif
      thrS->start();
    }
  }
  else{
    BUG_WARN("Start Subscriber " << m_name << " on " << m_host << "(" << m_port << ") canceled" );
  }
}

#ifdef HAVE_QT

//-------------------------------------------------
// slot_error
//-------------------------------------------------
void MessageQueueSubscriber::slot_error(const std::string& messageShort,
					const std::string& messageLong) {
  BUG_DEBUG("ErrorMessage Short: " << messageShort << "\nLong:" << messageLong);
  GuiFactory::Instance()->getLogWindow()->writeText(compose(_("MessageQueue Error: %1\n"),
							    messageLong) );
  GuiFactory::Instance()->showDialogInformation(NULL,
						_("MessageQueue Request Error"),
                                                compose(_("%1\n\n%2\n\n(For details see log window)"),
							messageShort, messageLong), NULL);
}

//-------------------------------------------------
// slot_receivedSubscriberData
//-------------------------------------------------
void MessageQueueSubscriber::slot_receivedSubscriberData(const std::string& header,
                                                         const std::vector<std::string>& dataList) {
  BUG_DEBUG("MessageQueueSubscriber::slot_receivedSubscriberData, header: " << header);

  // find header, read data and call function
  MessageQueue::HeaderDataMap::iterator it = m_headerDataMap.find(header);
  if ( it != m_headerDataMap.end() ) {
    processSubscribe((*it).second, dataList);
  }
}

#endif

//-------------------------------------------------
// processSubscribe
//-------------------------------------------------
void MessageQueueSubscriber::processSubscribe(MessageQueue::HeaderData* subsType,
                                              const std::vector<std::string>& dataList) {
  BUG_DEBUG("MessageQueueSubscriber::processSubscribe");
  if (s_triggerStarted && subsType->m_function) {
    s_procSubsVec.push_back( new SubscriberData(dataList, subsType, this) );
  } else {
    // read data
    if ( subsType->m_outVectorListener.size() ) {
      int idx=0;
      for(std::vector<Stream*>::iterator it = subsType->m_outVectorListener.begin();
          it != subsType->m_outVectorListener.end(); ++it, ++idx) {
        if (idx < dataList.size()) {
          BUG_DEBUG("Size of Data: " << dataList[idx].size());
          std::istringstream is(dataList[idx]);
          (*it)->read( is );
        }
      }
    }
    // read plugin data
    int x=0;
    for (std::vector<Plugin*>::iterator pit = subsType->m_pluginVector.begin();
         pit !=  subsType->m_pluginVector.end(); ++pit) {
      if ( (*pit) ) {
        // find header
        MessageQueue::HeaderDataMap::iterator it = m_headerDataMap.begin();
        for (;it != m_headerDataMap.end(); ++it )
          if (it->second == subsType)
            break;
        if ( it != m_headerDataMap.end()) {
          (*pit)->messageQueueSubscribe(it->first, dataList);
        } else
          BUG_DEBUG("Header not found");
      }
    }

    // start function
    if( subsType->m_function ){
      s_triggerStarted = true;
      Trigger *trigger = new Trigger( this, subsType->m_function );

      // omit some GuiUpdates if process vector has entries
      if (s_procSubsVec.size()) {
        static int omitGuiCounter = 0;
        if ( ++omitGuiCounter < 5 ) {
          trigger->setUpdateForms( false );
        } else
          omitGuiCounter = 0;
      }

      trigger->startJob();
    } else {
#ifdef HAVE_QT
      if (m_timerId == 0)
        m_timerId = startTimer(100);
#endif
    }
  }
}

#ifdef HAVE_QT
/* --------------------------------------------------------------------------- */
/* timerEvent --                                                               */
/* --------------------------------------------------------------------------- */
void MessageQueueSubscriber::timerEvent ( QTimerEvent * event ) {
  killTimer( event->timerId() );
  m_timerId = 0;
  GuiFactory::Instance()->update( GuiElement::reason_Process, true );
}
#endif

//-------------------------------------------------
// Trigger::backFromJobStarter
//-------------------------------------------------
void MessageQueueSubscriber::Trigger::backFromJobStarter( JobAction::JobResult rslt ) {
  MessageQueueSubscriber::s_triggerStarted = false;
  if (MessageQueueSubscriber::s_procSubsVec.size()) {
    SubscriberData *subsData = (*(MessageQueueSubscriber::s_procSubsVec.begin()));
    MessageQueueSubscriber::s_procSubsVec.erase(MessageQueueSubscriber::s_procSubsVec.begin());
    subsData->m_subscriber->processSubscribe(subsData->m_subscriberType,
                                             subsData->m_data);
    delete subsData;
  }
}
