#include <QProgressBar>
#include "utils/Debugger.h"
#include "gui/qt/GuiQtProgressBar.h"
#include "gui/qt/QtDialogProgressBar.h"

INIT_LOGGER();


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtProgressBar::GuiQtProgressBar(GuiElement *parent, std::string name)
  : GuiQtDataField( parent, name )
  , DialogProgressBar(name)
  , m_progressBar(0)
{}

GuiQtProgressBar::GuiQtProgressBar(const GuiQtProgressBar &progressbar)
  : GuiQtDataField(progressbar),
    DialogProgressBar(progressbar)
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
  bool changed = getAttributes();
  switch( reason ) {
  case reason_FieldInput:
  case reason_Process:
    if( !isDataItemUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
      ///      return;
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
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtProgressBar::serializeXML(std::ostream &os, bool recursive) {
}


/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtProgressBar::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return DialogProgressBar::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtProgressBar::serializeProtobuf(in_proto::Progressbar* element, bool onlyUpdated) {
  return DialogProgressBar::serializeProtobuf(element, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtProgressBar::getDialogExpandPolicy() {
  return (GuiElement::Orientation) 0;
}
