
#include <qlabel.h>
#include <qcursor.h>

#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtPulldownMenu.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtMenuToggle.h"
#include "gui/qt/QtMultiFontString.h"

#include "utils/Debugger.h"

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/*=============================================================================*/
/* Copy Constructor GuiQtPopupMenu                                             */
/*=============================================================================*/

GuiQtPopupMenu::GuiQtPopupMenu(const GuiQtPopupMenu& menu)
  : GuiQtElement( menu ),
    GuiPopupMenu(menu),
    m_menu(0) {
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtPopupMenu::myWidget(){
  return m_menu;
}

void GuiQtPopupMenu::create(){
  BUG_PARA(BugGui,"GuiQtPopupMenu::create"
                 ,"Elements = " << m_container.size());
  if( m_container.size() == 0 ){
    return;
  }

  // create menu
  if (!m_menu)
    m_menu = new QMenu();
  else if (isClearDefaultMenu()) m_menu->clear();

  // tearoff
  if(isTearOff()){
    m_menu->setTearOffEnabled(true);
  }

  // set popup font
  QFont font =  m_menu->font();
  m_menu->setFont( QtMultiFontString::getQFont( "@popup@", font ) );

  if ( getTitle().empty() ) {
    m_menu->setTitle( QString::fromStdString(getTitle()) ); // funktioniert hier nicht
  }
  GuiElementList::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it) {
    (*it)->create();
    if (0) (*it)->manage();
  }

  if( m_container.empty() ){
    m_menu->setHidden( true );
  }
}

/* --------------------------------------------------------------------------- */
/* popup --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtPopupMenu::popup(){
  m_menu->exec( QCursor::pos() );
}

/* --------------------------------------------------------------------------- */
/* serializeXML                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtPopupMenu::serializeXML(std::ostream &os, bool recursive){
  GuiPopupMenu::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson                                                                */
/* --------------------------------------------------------------------------- */
bool GuiQtPopupMenu::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiPopupMenu::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf                                                                */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtPopupMenu::serializeProtobuf(in_proto::ElementList* eles, in_proto::PopupMenu* element, bool onlyUpdated){
  return GuiPopupMenu::serializeProtobuf(eles, element, onlyUpdated);
}
#endif
