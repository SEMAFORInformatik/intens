#include <QProgressBar>
#include "utils/Debugger.h"
#include "gui/qt/GuiQtProgressBar.h"
#include "gui/qt/QtDialogProgressBar.h"
#include "operator/MessageQueue.h"
#include "operator/MessageQueuePublisher.h"
#include "streamer/StreamManager.h"

INIT_LOGGER();


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtProgressBar::GuiQtProgressBar(GuiElement *parent, std::string name)
  : GuiQtDataField( parent, name )
  , m_progressBar(0)
  , m_publisher(0)
{}

GuiQtProgressBar::GuiQtProgressBar(const GuiQtProgressBar &progressbar)
  : GuiQtDataField(progressbar)
  , m_progressBar(0)
{
}

GuiQtProgressBar::~GuiQtProgressBar(){
  delete m_progressBar;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtProgressBar::create(){
  m_progressBar = new QProgressBar;
  std::string css= "QProgressBar::chunk {background-color: #5aafe8; width: 20px; margin: 0.5px;}";
  css +="QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;}";
  m_progressBar->setStyleSheet( QString::fromStdString(css) );
  m_param->DataItem()->setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtProgressBar::myWidget(){
  return m_progressBar;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtProgressBar::update( UpdateReason reason ){
  if(m_progressBar == 0 ) return;

  updateWidgetProperty();
  publishData();
  bool changed = getAttributes();
  switch( reason ) {
  case reason_FieldInput:
  case reason_Process:
    if( !m_param->DataItem()->isUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
      return;
    }
    break;
  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
  default:
    setUpdated();
    changed = true;  // to make sure the color is set
    break;
  }
  if(!isUpdated() && !changed){
    return;
  }

  double value = 0.0;
  if( m_param->DataItem()->getValue( value ) ){
    m_progressBar->setValue(value);
  }
  else m_progressBar->setValue(value);
}


/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtProgressBar::manage(){
  if (myWidget())
    myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtProgressBar::unmanage(){
  myWidget()->hide();
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */
GuiElement* GuiQtProgressBar::clone() {
  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement())
    m_cloned.push_back( new GuiQtProgressBar( *this ) );
  else
    return baseElem->clone();
  return m_cloned.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtProgressBar::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_cloned.begin(), m_cloned.end());
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtProgressBar::getDialogExpandPolicy() {
  return (GuiElement::Orientation) 0;
}

/* --------------------------------------------------------------------------- */
/* publishData --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtProgressBar::publishData() {
  if (!AppData::Instance().HeadlessWebMode()) return;
  BUG_DEBUG("QtDialogProgressBar::publishData");
  if( !m_param->DataItem()->isUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
    return;
  }

  // get data
  Json::Value jsonElem = Json::Value(Json::objectValue);
  std::string s;
  serializeJson(jsonElem, true);
  s = ch_semafor_intens::JsonUtils::value2string(jsonElem);
  DataReference *ref = DataPoolIntens::Instance().getDataReference("mqReply_uimanager_response.data");
  if ( ref != 0 ) {
    ref->SetValue( s );
    delete ref;
  }

  if (!m_publisher) {
    // publish data
    std::string pubname("mqReply_publisher_mq");
    std::string streamname("mqReply_uimanager_response_stream");
    m_publisher = MessageQueue::getPublisher(pubname);
    Stream* stream = StreamManager::Instance().getStream(streamname);
    if (m_publisher && stream) {
      std::vector<Stream*> streamVec;
      streamVec.push_back(stream);
      m_publisher->setPublishOutStreams(streamVec);
      m_publisher->setPublishHeader("progressbar_data");
    } else {
      if (!m_publisher) {
        BUG_WARN(compose("Undefined '%1' MESSAGE_QUEUE Publisher in MessageQueueReply.inc", pubname));
      }
      if (!stream) {
        BUG_WARN(compose("Undefined '%1' Stream in MessageQueueReply.inc", streamname));
      }
      return;
    }
  }
  m_publisher->startPublish(true);
}

