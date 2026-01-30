
#include <QTimerEvent>
#include <QTime>
#include <QThread>
#include <QMetaType>

#include <sstream>
#include <algorithm>
#include "zhelpers.h"
#include "streamer/Stream.h"
#include "xfer/XferDataItem.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/Timer.h"
#include "operator/ConnectionListener.h"
#include "operator/MessageQueuePublisher.h"
#include "operator/MessageQueueThreads.h"
#include "operator/SocketException.h"

INIT_LOGGER();

//-------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------
MessageQueuePublisher::MessageQueuePublisher ( const std::string &s
                                               , const std::string& host
                                               , int port)
  : m_host( host )
  , m_port(port)
  , m_publisher(0)
  , m_listener(0) {
  BUG_DEBUG("MessageQueuePublisher name" << s
	    << ", host: " << host
	    << ", port: " << port
	    );

  try {
    if (m_port > 0) {
      m_publisher = new  zmq::socket_t(MessageQueue::getContext(), ZMQ_PUB);
      std::ostringstream os;
      os << "tcp://*:" << m_port;
      m_publisher->bind(os.str().c_str());
    }

    m_timer=GuiFactory::Instance()->createTimer( 1000 );
    m_task = new  MyTimerTask(this);
    m_timer->addTask( m_task );
    m_timer->start();
 } catch (const std::exception& e) {
    BUG_ERROR("MessageQueuePublisher Constructor Exception: " << e.what());
    if ( m_publisher)
      delete m_publisher;
    throw SocketException( e.what() );
  }
}

MessageQueuePublisher::~MessageQueuePublisher(){
  delete m_timer;
  delete m_task;
}


//-------------------------------------------------
// setPublishOutStreams
//-------------------------------------------------
void MessageQueuePublisher::setPublishOutStreams( const std::vector<Stream*>& out_streams )  {
  m_publish_out_streams = out_streams;
}

//-------------------------------------------------
// setPublishHeader
//-------------------------------------------------
void MessageQueuePublisher::setPublishHeader( const std::string& header )  {
  m_publishHeader = header;
}

//-------------------------------------------------
// setPublishData
//-------------------------------------------------
void MessageQueuePublisher::setPublishData( const std::string& data )  {
  m_publishData = data;
}

//-------------------------------------------------
// MyTimerTask::tick
//-------------------------------------------------
void MessageQueuePublisher::MyTimerTask::tick() {
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

//-------------------------------------------------
// startPublish
//-------------------------------------------------

void MessageQueuePublisher::startPublish(bool duplicateCheck){
  BUG_DEBUG("MessageQueuePublisher::startPublish");
  assert( m_publisher );
  int dup=0;

  if (m_publish_out_streams.size() == 0) {
    if (m_publishData.empty()){
      publishHeader();
    }else{
      if (duplicateCheck && m_lastPublishString.size()){
        if (m_lastPublishString[0] == m_publishData){
          return;  // already published
        }
        m_lastPublishString[0] = m_publishData;
      }else{
        m_lastPublishString.push_back(m_publishData);
      }
    }
  } else {
    int idx=0;
    for(std::vector<Stream*>::iterator it = m_publish_out_streams.begin();
        it != m_publish_out_streams.end(); ++it, ++idx) {
      std::ostringstream os;
      (*it)->write( os );
      if (duplicateCheck) {
        if (idx < m_lastPublishString.size() &&
            m_lastPublishString[idx] == os.str()) {
          BUG_DEBUG("ignore Publish, idx: " << idx);
          ++dup;
          continue;
        }
        if (idx < m_lastPublishString.size()) {
          m_lastPublishString[idx] = os.str();
        } else {
          m_lastPublishString.push_back(os.str());
        }
      } else {
        m_lastPublishString.push_back(os.str());
      }
    }
  }

  // publish data
  if (!duplicateCheck || dup < m_lastPublishString.size()) {
    publishData();
  }

  if (!duplicateCheck) {
    m_lastPublishString.clear();
  }
  m_publishData.clear();
  if (m_listener)
    m_listener->connectionClosed();
  m_listener = 0;
}

//-------------------------------------------------
// publishHeader
//-------------------------------------------------
bool MessageQueuePublisher::publishHeader(){
  if (!m_publish_out_streams.empty() || !m_publishData.empty())
    s_sendmore (*m_publisher, m_publishHeader); // send header
  else
    s_send (*m_publisher, m_publishHeader); // send header
  return true;
}

//-------------------------------------------------
// publishData
//-------------------------------------------------
bool MessageQueuePublisher::publishData(){
  for (int i=0; i < m_lastPublishString.size(); ++i) {
    if (!i) { // first time
      publishHeader();
    }
    if (i < m_publish_out_streams.size()-1)
      s_sendmore (*m_publisher, m_lastPublishString[i]);
    else
      s_send (*m_publisher, m_lastPublishString[i]);
  }
  return true;
}

//-------------------------------------------------
// checkPublish
//-------------------------------------------------
bool MessageQueuePublisher::checkPublish(){
  // no cause, why not true
  return true;
}

void MessageQueuePublisher::confirmYesButtonPressed() {
  if (m_listener)
      m_listener->connectionClosed(true);
   m_listener = 0;
}

void MessageQueuePublisher::confirmNoButtonPressed() {
  // restart timer
  m_timer->start();
}

//-------------------------------------------------
// slot_receivedReplyData
//-------------------------------------------------
void MessageQueuePublisher::slot_receivedReplyData(const std::string& header,
                                                   const std::vector<std::string>& dataList) {
}

//-------------------------------------------------
// slot_error
//-------------------------------------------------
void MessageQueuePublisher::slot_error(const std::string& messageShort, const std::string& messageLong) {
  GuiFactory::Instance()->getLogWindow()->writeText(
                                                    compose(_("MessageQueue Error: %1\n"),messageLong)
                                                    );
  GuiFactory::Instance()->showDialogInformation(NULL, _("MessageQueue Publish Error"),
                                                compose(_("%1\n\n(For details see log window)"),messageShort), NULL);
}
