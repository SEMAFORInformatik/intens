
#include <QMouseEvent>
#include <qlabel.h>
#include "gui/GuiFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtIndexMenu.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"


//------------------------------------------------------------------------------
// lokale Hilfsklasse
//------------------------------------------------------------------------------
class GuiQtIndexLabel : public QLabel {
public:
  GuiQtIndexLabel( GuiIndexMenu *menu, QWidget *parent, const char *name = 0 )
    : QLabel( parent)
    , m_menu( menu )
  {
    setObjectName( "IndexLabel" );
  }
  ~GuiQtIndexLabel(){}

public:
  virtual void mousePressEvent( QMouseEvent *ev ){
    if( ev->button() == Qt::RightButton )
      m_menu->popupMenu();
  }
private:
  GuiIndexMenu *m_menu;

};

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtIndexMenu::create() {
  BUG(BugGui,"GuiQtIndexMenu::create");
  assert( m_labelwidget == 0 );

  QWidget *parent = getParent()->getQtElement()->myWidget();
  assert(parent != 0);
  m_labelwidget = new GuiQtIndexLabel( this, parent, "labeltext");
  QFont font = m_labelwidget->font();
  m_labelwidget->setFont( QtMultiFontString::getQFont( "@index@", font ) );
  m_labelwidget->show();
  QPalette pal = m_labelwidget->palette();
  pal.setColor( QPalette::Window, GuiQtManager::indexBackgroundColor() );
  pal.setColor( QPalette::WindowText, GuiQtManager::indexForegroundColor() );
  m_labelwidget->setPalette( pal );

  setIndex( "", 0 );

  if( !(m_hideFlag & HideMenu) ){
    if( m_menu == 0 ){
      createPopupMenu();
    }
    m_menu->getElement()->create();
  }
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtIndexMenu::destroy(){
  return false;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtIndexMenu::update( UpdateReason reason ){
}

/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtIndexMenu::setIndex( const std::string &name, int inx ){
  GuiIndexMenu::setIndex(name, inx);

  std::ostringstream number;
  if( m_hideFlag != HideLabel && m_labelwidget){
    number << (m_index + m_min_index);
    m_labelwidget->setText( QString::fromStdString(number.str()) );
  }
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */
void GuiQtIndexMenu::createPopupMenu(){
  GuiIndexMenu::createPopupMenu();

  QWidget* w = m_menu_title ? m_menu_title->getElement()->getQtElement()->myWidget() : 0;//->getElement()->getQtElement()->myWidget();
  if (w) {
    QFont font =  w->font();
    w->setFont( QtMultiFontString::getQFont( "@popup@", font ) );
  }
}

/* --------------------------------------------------------------------------- */
/* popupMenu --                                                                */
/* --------------------------------------------------------------------------- */
// void GuiQtIndexMenu::popupMenu(){
//   if( m_menu == 0 ) return;
//   std::ostringstream title;
//   title << " Index " << (m_index + m_min_index);
//   m_menu_title->setLabel( title.str() );
//   m_menu->popup();
// }

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtIndexMenu::serializeXML(std::ostream &os, bool recursive){
//   os << "<GuiQtIndexMenu></GuiQtIndexMenu>"<<endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtIndexMenu::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiIndexMenu::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtIndexMenu::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiIndexMenu::serializeProtobuf(eles, onlyUpdated);
}
#endif

QWidget*  GuiQtIndexMenu::myWidget() { return m_labelwidget; }
