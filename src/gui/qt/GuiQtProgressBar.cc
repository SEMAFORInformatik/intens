#include <QProgressBar>
#include "utils/Debugger.h"
#include "gui/qt/GuiQtProgressBar.h"
#include "gui/qt/QtDialogProgressBar.h"

INIT_LOGGER();


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtProgressBar::GuiQtProgressBar(GuiElement *parent, const std::string &name)
  : GuiQtElement( parent, name )
  , DialogProgressBar(name)
  , m_progressBar(0)
{}

GuiQtProgressBar::GuiQtProgressBar(const GuiQtProgressBar &progressbar)
  : GuiQtElement(progressbar),
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

  switch( reason ) {
  case reason_FieldInput:
  case reason_Process:
    if( !isDataItemUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
      return;
    }
    break;
  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
  default:
    ResetLastWebUpdated();  // reason_Always for webtens
    break;
  }
  //  m_progressBar->setVisible(getMainPercentRate() > 0);
  m_progressBar->setValue(getMainPercentRate());
  QtDialogProgressBar::Instance().update(reason);
}


/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtProgressBar::manage(){
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
