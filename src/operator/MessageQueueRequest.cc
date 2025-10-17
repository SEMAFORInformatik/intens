
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
#include "operator/MessageQueueRequest.h"
#include "operator/MessageQueueThreads.h"

INIT_LOGGER();

//-------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------
MessageQueueRequest::MessageQueueRequest(const std::string& host
                                         , int port
                                         , int timeout)
  : m_requestThread(0)
  , m_host( host )
  , m_requestPort(port)
  , m_defaultTimeout( timeout )
{
}

MessageQueueRequest::~MessageQueueRequest(){
}

//-------------------------------------------------
// startRequest
//-------------------------------------------------
void MessageQueueRequest::startRequest(){
  BUG_DEBUG("MessageQueueRequest::startRequest");
  //  assert(m_requestThread);
  if (m_requestThread == 0) {
    m_requestThread = new MessageQueueRequestThread(m_host, m_requestPort, *this);
  }

#ifdef HAVE_QT
  if (m_requestThread != 0) {
    // disconnect(m_requestThread, SIGNAL(receivedData(const std::vector<std::string>&)),
    //            this, SLOT(slot_receivedInitRequestData(const std::vector<std::string>&)));
    disconnect(m_requestThread, SIGNAL(receivedData(const std::vector<std::string>&)),
               this, SLOT(slot_receivedRequestData(const std::vector<std::string>&)));
  }
  connect(m_requestThread, SIGNAL(receivedData(const std::vector<std::string>&)),
          this, SLOT(slot_receivedRequestData(const std::vector<std::string>&)), Qt::QueuedConnection);
#endif

  // set input streams and start request thread
  // m_requestThread->setInputStreams( getRequestInStreams() );
  // m_requestThread->setHeader( getRequestHeader() );
  m_requestThread->readInputData();
  m_requestThread->setOutputStreams(getRequestOutStreams());
  m_requestThread->start();

}

//-------------------------------------------------
// checkRequest
//-------------------------------------------------
bool MessageQueueRequest::checkRequest(){
  if (m_requestThread != 0) {
    if (m_requestThread->isRunning()) {
      switch (m_requestThread->getStatus()) {
        case MessageQueueRequestThread::STARTED:
          m_errMsg = "Old request is already started.\n\n(Maybe, request socket of publisher is not running!)";
          break;
        case MessageQueueRequestThread::WORKING:
          m_errMsg = "Old request is waiting for receive.\n\n(Maybe, publisher is not running!)";
          break;
       case MessageQueueRequestThread::ABORTED:
       case MessageQueueRequestThread::FINISHED:
         // state is finished => it will finished soon
         return true;
       default:
         m_errMsg = "Old request is already running.";
         break;
      }
      return false;
    }
  }
  return true;
}

//-------------------------------------------------
// getErrorMessage
//-------------------------------------------------
std::string MessageQueueRequest::getErrorMessage() const {
  return m_errMsg;
}

//-------------------------------------------------
// terminateRequest
//-------------------------------------------------
void MessageQueueRequest::terminateRequest() {
  BUG_DEBUG("MessageQueueRequest::terminateThread");

  // monitoring mode: abort this thread (timeout reached)
  if ( getRequestListener() )
      getRequestListener()->connectionClosed(true);
  setRequestListener( 0 );

   // stop request thread
   if (m_requestThread->isRunning()) {
     std::ostringstream os;
     os << "message queue monitoring mode: timeout reached or user aborted, abort" << std::endl
        << "host: " << m_host << ", request port: " << m_requestPort
        << ", header: " << getRequestHeader() << std::endl;
     GuiFactory::Instance()->getLogWindow()->writeText( os.str() );
     BUG_DEBUG(os.str());
     m_requestThread->terminateThread();
   }
}

#ifdef HAVE_QT

//-------------------------------------------------
// slot_error
//-------------------------------------------------
void MessageQueueRequest::slot_error(const std::string& messageShort, const std::string& messageLong) {
  BUG_DEBUG("ErrorMessage Short: " << messageShort << "\nLong:" << messageLong);
  GuiFactory::Instance()->getLogWindow()->writeText(
                                                    compose(_("MessageQueue Error: %1\n"),messageLong)
                                                    );
  GuiFactory::Instance()->showDialogInformation(NULL, _("MessageQueue Request Error"),
                                                compose(_("%1\n\n%2\n\n(For details see log window)"),messageShort, messageLong), NULL);
  terminateRequest();
}

//-------------------------------------------------
// receivedRequestData
//-------------------------------------------------
void MessageQueueRequest::slot_receivedRequestData(const std::vector<std::string>& dataList) {
  BUG_DEBUG("MessageQueueRequest::slot_receivedRequestData, size: " << dataList.size()
            << ", requestListener: " <<  getRequestListener());

  // read data
  int idx = 0;
  if (getRequestOutStreams().size()) {
    for(std::vector<Stream*>::const_iterator it = getRequestOutStreams().begin();
        it != getRequestOutStreams().end(); ++it, ++idx) {
      (*it)->clearRange( );
      if (idx < dataList.size()) {
        BUG_DEBUG("Size of Data: " << dataList[idx]);
        std::istringstream is( dataList[idx] );
        (*it)->read( is );
      }
      else {
        std::istringstream is((*it)->hasJSONFlag() ? "{}" : "");
        (*it)->read( is );
      }
    }
  }
  if( getRequestListener() )
    getRequestListener()->connectionClosed(dataList.size() ? false : true);
  setRequestListener( 0 );
}

#endif

#ifdef HAVE_QT
/* --------------------------------------------------------------------------- */
/* timerEvent --                                                               */
/* --------------------------------------------------------------------------- */
void MessageQueueRequest::timerEvent ( QTimerEvent * event ) {
  killTimer( event->timerId() );
  GuiFactory::Instance()->update( GuiElement::reason_Process );

}
#endif

//-------------------------------------------------
// getTimeout
//-------------------------------------------------
int MessageQueueRequest::getTimeout() {
  int timeout = getRequestTimeout();
  if (timeout == -1)
    timeout = m_defaultTimeout;
  return timeout;
}
