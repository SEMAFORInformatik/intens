#include <QTimerEvent>
#include <QTime>
#include <QThread>
#include <QMetaType>

#include <sstream>
#include <algorithm>

#include "streamer/Stream.h"
#include "xfer/XferDataItem.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/qt/QtDialogProgressBar.h"
#include "gui/Timer.h"
#include "operator/ConnectionListener.h"
#include "operator/MessageQueueReply.h"
#include "operator/MessageQueueThreads.h"
#include "operator/SocketException.h"
#include "job/JobManager.h"

INIT_LOGGER();

//-------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------
MessageQueueReply::MessageQueueReply ( const std::string &s
                                       , const std::string& host
                                       , int port
                                       , const std::vector<Stream*>& default_in_streams
                                       , const std::vector<Stream*>& default_out_streams
                                       , JobFunction *default_func)
  : m_replyThread(0)
  , m_host( host )
  , m_portReply(port)
  , m_default_in_streams(default_in_streams)
  , m_default_out_streams(default_out_streams)
  , m_default_function(default_func)
  , m_triggerStarted(false)
  , m_listener(0) {
  BUG_DEBUG("MessageQueueReply name" << s
	    << ", host: " << host
	    << ", portReply: " << port
	    );
  try {
    m_timer=GuiFactory::Instance()->createTimer( 1000 );
    m_task = new  MyTimerTask(this);
    m_timer->addTask( m_task );
    m_timer->start();
 } catch (const std::exception& e) {
    BUG_ERROR("MessageQueueReply Constructor Exception: " << e.what());
    throw SocketException( e.what() );
  }
}

MessageQueueReply::~MessageQueueReply(){
  delete m_timer;
  delete m_task;
}

// -----------------------------------------------------
// addHeader
// -----------------------------------------------------
void MessageQueueReply::addHeader( std::string header, std::vector<Stream*>& in,
                                   std::vector<Stream*>&  out, std::vector<Plugin*> plugins, JobFunction* func){
  BUG_DEBUG("MessageQueueReply::addHeader header: "<< header);
  if ( m_headerDataMap.find(header) == m_headerDataMap.end() )
    m_headerDataMap.insert( MessageQueue::HeaderDataMap::value_type(header,
                            new MessageQueue::HeaderData(in, out, plugins, func) ) );
  else {
    m_headerDataMap[header]->m_inVectorListener  = in;
    m_headerDataMap[header]->m_outVectorListener = out;
    m_headerDataMap[header]->m_pluginVector      = plugins;
    m_headerDataMap[header]->m_function    = func;
  }
}


//-------------------------------------------------
// stop
//-------------------------------------------------
void MessageQueueReply::stop(){
}

//-------------------------------------------------
// MyTimerTask::tick
//-------------------------------------------------
void MessageQueueReply::MyTimerTask::tick() {
  // init mode: start initialization
  if ( m_msgQueue->m_replyThread == 0) {
    m_msgQueue->start();
  } else {
    // monitoring mode:
    if( m_msgQueue->m_listener ) {
      BUG_DEBUG("Cannot establish a connection");
      GuiFactory::Instance()->showDialogConfirmation
       ( 0
         , _("Cannot establish a connection")
         , _("Abort this Function?")
         , m_msgQueue );
    }
  }
}

//-------------------------------------------------
// start
//-------------------------------------------------
void MessageQueueReply::start(){
  BUG_DEBUG("MessageQueueReply::start portReply: " << m_portReply);
  assert(m_replyThread==0);

  // start request connection to message queue
  if ( m_portReply > 0) {
    BUG_DEBUG("Create Reply Thread, port: "<< m_portReply);
    m_replyThread = new MessageQueueReplyThread(m_host, m_portReply, m_default_in_streams, *this);
    #ifdef HAVE_QT
    connect(m_replyThread, SIGNAL(receivedData(const std::string&, const std::vector<std::string>&)),
            this, SLOT(slot_receivedReplyData(const std::string&, const std::vector<std::string>&)),
            Qt::QueuedConnection);
    connect(m_replyThread, SIGNAL(error(const std::string&,const std::string&)),
            this, SLOT(slot_error(const std::string&,const std::string&)),
            Qt::QueuedConnection);
    #endif
    m_replyThread->start();
  }
}

void MessageQueueReply::confirmYesButtonPressed() {
  if (m_listener)
      m_listener->connectionClosed(true);
   m_listener = 0;
}

void MessageQueueReply::confirmNoButtonPressed() {
  // restart timer
  m_timer->start();
}

#ifdef HAVE_QT
//-------------------------------------------------
// slot_receivedReplyData
//-------------------------------------------------
void MessageQueueReply::slot_receivedReplyData(const std::string& header,
                                                   const std::vector<std::string>& dataList) {
  BUG_INFO("MessageQueueReply::slot_receivedReplyData, header: " << header);

  // default values
  std::vector<Stream*>      _in_streams(m_default_in_streams);
  std::vector<Stream*>      _out_streams(m_default_out_streams);
  JobFunction* _function   = m_default_function;

  if (header.size()) {
    MessageQueue::HeaderDataMap::const_iterator it = m_headerDataMap.find(header);
    if (it!= m_headerDataMap.end()) {
      BUG_DEBUG("found header ["<<(*it).first<<"]");
      _in_streams = (*it).second->m_inVectorListener;
      _out_streams = (*it).second->m_outVectorListener;
      _function = (*it).second->m_function;
    }
  }
  if (true) {
    // no header set (simple answer)
    // read data
    int idx=0;
    for (std::vector<Stream*>::iterator it = _in_streams.begin();
         it != _in_streams.end(); ++it, ++idx) {
      //      (*it)->clearRange( );
      if (idx < dataList.size()) {
        BUG_DEBUG("Request data: "
                  << (dataList[idx].find("LOGIN") == std::string::npos ? dataList[idx] : "LoginData"));
        std::stringstream is( dataList[idx] );
        (*it)->read( is );
      }
    }
    m_replyThread->setResponseStreams( _out_streams );

    // ProgressDialogAbortCommand
    if (header == "ProgressDialogAbortCommand") {
      _function = NULL;  // NO FUNCTION
      startTimer(1000);
    }

    // start function
    JobFunction* afterUpdateFormsFunction = JobManager::Instance().getFunction("AFTER_UPDATE_FORMS");
    if (afterUpdateFormsFunction)
      afterUpdateFormsFunction->setMessageQueueReplyThread(m_replyThread);
    if( _function ){
      Trigger *trigger = new Trigger( this, _function );
      trigger->setMessageQueueReplyThread(m_replyThread);
      _function->setMessageQueueReplyThread(m_replyThread);
      trigger->startJob();
      BUG_INFO("Function " << _function->Name() << " started");
    } else {
      m_replyThread->setStatus(MessageQueueReplyThread::RECEIVE_DONE);
      GuiFactory::Instance()->update( GuiElement::reason_Process );
    }
  }

}

//----------------------------------------------------
// timerEvent
//----------------------------------------------------
void MessageQueueReply::timerEvent(QTimerEvent * event) {
  killTimer(event->timerId());
  QtDialogProgressBar::Instance().execute_abort(false);
}

//-------------------------------------------------
// slot_error
//-------------------------------------------------
void MessageQueueReply::slot_error(const std::string& messageShort, const std::string& messageLong) {
  GuiFactory::Instance()->getLogWindow()->writeText(
                                                    compose(_("MessageQueue Error: %1\n"),messageLong)
                                                    );
  GuiFactory::Instance()->showDialogInformation(NULL, _("MessageQueue Reply Error"),
                                                compose(_("%1\n\n(Port: %2)\n(For details see log window)"),messageShort, m_portReply), NULL);
}
#endif

//-------------------------------------------------
// Trigger::backFromJobStarter
//-------------------------------------------------
void MessageQueueReply::Trigger::backFromJobStarter( JobAction::JobResult rslt ) {
  BUG_DEBUG("MessageQueueReply::Trigger::backFromJobStarter");
  //  m_reply->m_replyThread->setStatus(MessageQueueReplyThread::RECEIVE_DONE);
}
