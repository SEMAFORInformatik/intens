#include <QProgressBar>
#include "gui/qt/GuiQtDataField.h"
#include "utils/Debugger.h"
#include "gui/qt/GuiQtProgressBar.h"
#include "operator/MessageQueue.h"
#include "operator/MessageQueuePublisher.h"
#include "streamer/StreamManager.h"

INIT_LOGGER();

int GuiQtProgressBar::s_timerId(0);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtProgressBar::GuiQtProgressBar(GuiElement *parent, std::string name)
  : GuiQtDataField( parent, name )
  , m_progressBar(0)
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
  if (AppData::Instance().HeadlessWebMode() && !s_timerId)
    s_timerId = startTimer(1000);
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtProgressBar::unmanage(){
  myWidget()->hide();
  if (s_timerId)
    killTimer( s_timerId );
  s_timerId = 0;
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
/* timerEvent --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtProgressBar::timerEvent ( QTimerEvent * event ) {
  BUG_DEBUG("GuiQtProgressBar::timerEvent");
  if (myWidget()->isVisible())
    update(GuiElement::reason_FieldInput);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtProgressBar::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiQtDataField::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtProgressBar::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiQtDataField::serializeProtobuf(eles->add_data_fields(), onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* publishData --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtProgressBar::publishData() {
  if (!AppData::Instance().HeadlessWebMode()) return;
  BUG_DEBUG("GuiQtProgressBar::publishData");
  if( !m_param->DataItem()->isUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
    return;
  }

  // get data
  Json::Value jsonElem = Json::Value(Json::objectValue);
  std::string s;
  std::ostringstream os;
#if HAVE_PROTOBUF
  auto reply = in_proto::WebAPIResponse();
  serializeProtobuf(reply.mutable_elements(), false);
  reply.SerializePartialToOstream(&os);
#else
  serializeJson(jsonElem, false);
  os << ch_semafor_intens::JsonUtils::value2string(jsonElem);
  BUG_WARN("publishData (progressbar_data): "<<os.str().substr(0, 250) << ", len: " << os.str().size());
#endif

  // publish data
  std::string pubname("mqReply_publisher_mq");
  MessageQueuePublisher* publisher = MessageQueue::getPublisher(pubname);
  if (publisher) {
    publisher->setPublishHeader("progressbar_data");
    publisher->setPublishData(os.str());
  } else {
    if (!publisher) {
      BUG_WARN(compose("Undefined '%1' MESSAGE_QUEUE Publisher in MessageQueueReply.inc", pubname));
    }
    return;
  }
  publisher->startPublish(true);
}

