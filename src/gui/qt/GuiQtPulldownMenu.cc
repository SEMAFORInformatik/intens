
#include <qmenubar.h>
#include <QMenuBar>
#include <QActionGroup>
#include <qmainwindow.h>
//#include "json/json.h"

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtPulldownMenu.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtMenuToggle.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtPulldownMenu::GuiQtPulldownMenu( GuiElement *parent,
                                      const std::string &name )
  : GuiQtElement( parent, getMenuName(parent, name) )
  , m_container( parent, GuiElement::type_PulldownMenu )
  , m_name( name )
  , m_show_always( false )
  , m_tear_off( true )
  , m_radio( false )
  , m_menu( 0 ) {
}

GuiQtPulldownMenu::~GuiQtPulldownMenu(){
  delete m_menu;
}
/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtPulldownMenu::create(){
  BUG_PARA(BugGui,"GuiQtPulldownMenu::create"
                 ,"Elements = " << m_container.size());
  if( m_container.size() == 0 && !m_show_always ){
    return;
  }
  std::string str = m_label.empty() ? m_name : m_label;

  // get menu parent
  QMenu* menu = 0;
  QMenuBar* menubar = 0;
  if ( getParent()->Type() == type_Menubar ) {
    menubar  = dynamic_cast<QMenuBar*>( getParent()->getQtElement()->myWidget() );
    assert( menubar );
  } else if ( getParent()->Type() == type_PulldownMenu ) {
    menu  = dynamic_cast<QMenu*>( getParent()->getQtElement()->myWidget() );
    assert( menu );
  } else if ( getParent()->Type() == type_PopupMenu ) {
    menu  = dynamic_cast<QMenu*>( getParent()->getQtElement()->myWidget() );
    assert( menu );
  }

  // create popup menu
  if( !m_mnemonic.empty() ){
    std::string::size_type pos = str.find_first_of(m_mnemonic);
    if (pos != std::string::npos)
      str.insert(pos, "&");
  }
  if (menu) {
    m_menu = menu->addMenu( QString::fromStdString(str) );
  } else {
    m_menu = menubar->addMenu( QString::fromStdString(str) );
  }

  // set menu properties
  m_menu->setTearOffEnabled(m_tear_off);
  if( m_tear_off ){
    m_menu->setWindowTitle( QString::fromStdString(str).remove('&') );
  }

  // set Font
  QFont font = m_menu->menuAction()->font();
  if (getParent()->Type() == type_PopupMenu) {
    font = QtMultiFontString::getQFont( "@popup@", font );
  } else if (getParent()->Type() == type_PulldownMenu) {
    font = QtMultiFontString::getQFont( "@pulldown@", font );
  }
  else font = QtMultiFontString::getQFont( "@button@", font );
  m_menu->menuAction()->setFont( font );

  GuiContainer::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it) {
    (*it)->create(); // false => normales create
    (*it)->manage();
  }
  if( m_container.empty() ){
      m_menu->setEnabled(false);
  }

  // add radio functionallity
  if (m_radio){
    QActionGroup *actGroup = new QActionGroup(this);
    for(it = m_container.begin(); it != m_container.end(); ++it) {
      if ((*it)->Type() == type_MenuToggle) {
        QAction* m_toggleAct   = static_cast<GuiQtMenuToggle*>(*it)->getMenuAction();
        actGroup->addAction( m_toggleAct );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* unmap --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtPulldownMenu::unmap(){
  if (m_menu && m_menu->menuAction()->isVisible())
    m_menu->menuAction()->setVisible(false);
  GuiQtElement::unmap();
}

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtPulldownMenu::setLabel( const std::string &name ){
  m_label = name;

  // set mnemonic (short cut)
  std::string::size_type pos = m_label.find( "&" );
  if( pos != std::string::npos ){
    if( pos + 1 < m_label.length() ){
      m_mnemonic = m_label.at( pos+1 );
    }
    std::string::iterator s = m_label.begin() + pos;
    m_label.erase( s );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getParentMenu --                                                            */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *GuiQtPulldownMenu::getParentMenu(){
  assert( getParent() != 0 );
  if( getParent()->Type() == type_PulldownMenu ){
    return static_cast<GuiQtPulldownMenu *>(getParent());
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */

GuiElement *GuiQtPulldownMenu::getElement(){
  return this;
}

/* --------------------------------------------------------------------------- */
/* ehemals Container Methoden                                                  */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtPulldownMenu::myWidget() {
  return m_menu;
}

void GuiQtPulldownMenu::update( UpdateReason r) {
  GuiContainer::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it) {
    (*it)->update(r);
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtPulldownMenu::serializeXML(std::ostream &os, bool recursive){
  GuiPulldownMenu::serializeXML( os, recursive );
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtPulldownMenu::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  jsonObj["disable"] = m_container.empty();
  return GuiPulldownMenu::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtPulldownMenu::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiPulldownMenu::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

void GuiQtPulldownMenu::serializeContainerElements( std::ostream &os ){
  GuiElementList::iterator el;
  for( el = m_container.begin(); el != m_container.end(); ++el ){
    (*el)->serializeXML(os);
  }
}

/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtPulldownMenu::serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated){
  GuiElementList::iterator el;
  bool ret(false);
  if (m_menu) {
    if (!m_menu->menuAction()->isVisible())
      return false;
  }

  for( el = m_container.begin(); el != m_container.end(); ++el ){
    if (!(*el)->isShownWebApi()) continue;
    if (!(*el)->getVisibleFlag()){
      continue;
    }
    Json::Value jsonElem = Json::Value(Json::objectValue);
    (*el)->serializeJson(jsonElem, onlyUpdated);
    if ((*el)->getHiddenFlag())
      continue;
    jsonObj.append(jsonElem);
  }
  return m_container.size() > 0;
}

/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtPulldownMenu::serializeContainerElements(in_proto::ElementList* eles, in_proto::PulldownMenu* element, bool onlyUpdated){
  GuiElementList::iterator el;
  bool ret(false);
  if (m_menu) {
    if (!m_menu->menuAction()->isVisible())
      return false;
  }

  for( el = m_container.begin(); el != m_container.end(); ++el ){
    if (!(*el)->isShownWebApi()) continue;
    if (!(*el)->getVisibleFlag()){
      continue;
    }
    (*el)->serializeProtobuf(eles, onlyUpdated);
    if ((*el)->getHiddenFlag())
      continue;
    auto ref = element->add_elements();
    ref->set_id((*el)->getElementIntId());
    ref->set_type((*el)->ProtobufType());
  }
  return m_container.size() > 0;
}
#endif
