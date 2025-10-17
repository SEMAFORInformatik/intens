
#include <qmenubar.h>
#include <QApplication>

#include "job/JobAction.h"

#include "gui/GuiEventData.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/GuiButtonListener.h"
#include <gui/qt/GuiQtForm.h>

#include "utils/Debugger.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtMenuButton::GuiQtMenuButton( GuiElement *parent
				, GuiButtonListener *listener
				, GuiEventData *event
				, bool hide_disabled, std::string name )
  : GuiQtElement( parent, getMenuName(parent, name))
  , m_buttonAct( 0 )
  , m_listener( listener )
  , m_event( event )
  , m_hideDisabled(hide_disabled)
{
  if (getMenuName(parent, name).size()) {
    BUG_DEBUG("MenuButton, MenuName: " << getMenuName(parent, name));
  }
  if( listener != 0 )
    listener->setButton( this );
  if( event != 0 )
    event->m_element = this;
#if defined(GUI_STANDALONE_TEST)
  m_helptext = "GuiQtMenuButton";
#endif
}

GuiQtMenuButton::~GuiQtMenuButton(){
  delete m_event;
}

GuiQtMenuButton::GuiQtMenuButton( const GuiQtMenuButton &menubutton )
  : GuiQtElement( menubutton )
  , m_buttonAct( menubutton.m_buttonAct )
  , m_listener( menubutton.m_listener )
  , m_event( menubutton.m_event )
  , m_hideDisabled(menubutton.m_hideDisabled)
  , m_label(menubutton.m_label)
  , m_accelerator_key(menubutton.m_accelerator_key)
  , m_accelerator_text(menubutton.m_accelerator_text) {
  GuiButtonListener *m_listener;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
#include "gui/qt/QtMultiFontString.h"

void GuiQtMenuButton::create(){
  BUG(BugGui,"GuiQtMenuButton::create");
  QMenu* menudata = dynamic_cast<QMenu*>(getParent()->getQtElement()->myWidget());
  assert( menudata );

  // create menu entry
  m_buttonAct = menudata->addAction( QString::fromStdString(m_label), this, SLOT( Activate() ) );
  // case NavigatorMenu, override parent
  if (getButtonListener())
    getButtonListener()->setParent(getParent());

  // get Font
  QFont font = m_buttonAct->font();
  if (getParent()->Type() == type_PopupMenu) {
    font = QtMultiFontString::getQFont( "@popup@", font );
  } else if (getParent()->Type() == type_PulldownMenu) {
    font = QtMultiFontString::getQFont( "@pulldown@", font );
  }
  else font = QtMultiFontString::getQFont( "@button@", font );

  // set Font
  if (!m_listener) {
    font.setWeight(m_listener ? (QFont::Light) : (QFont::Black));
    font.setWeight(m_listener ? (QFont::Normal) : (QFont::Bold));
  }
  else m_listener->isAllowed() ? enable() : disable();

  m_buttonAct->setFont( font );
  qobject_cast<QWidget*>(m_buttonAct->parent())->setStyleSheet( "background-color: #e0e0e0;" );

  // shortcut
  if( !m_accelerator_key.empty() ){
    if( m_accelerator_text.size() ){
      std::string search = "<key>";
      std::string::size_type pos;
      pos = lower(m_accelerator_key).find(search);
      while (pos != std::string::npos) {
	if ( pos != std::string::npos) {
	  m_accelerator_key.replace(pos, search.size(), "+");
	}
 	pos = lower(m_accelerator_key).find(search, pos+search.size());
      }
    }
    // beim Popup haben geht das leider nicht mit dem Shortcut
    // deshalb muessen wir uns darum selber sorgen
    grabShortcutAction(m_accelerator_key, m_buttonAct);

    m_buttonAct->setShortcut( QString::fromStdString(m_accelerator_key) );
    if(m_accelerator_text == "Widget") {
      m_buttonAct->setShortcutContext(Qt::WidgetShortcut);
    } else if(m_accelerator_text == "WidgetWithChildren") {
      m_buttonAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    } else if(m_accelerator_text == "Window") {
      m_buttonAct->setShortcutContext(Qt::WindowShortcut);
    } else {  // default
      m_buttonAct->setShortcutContext(Qt::ApplicationShortcut);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

 void GuiQtMenuButton::manage(){
   if ( m_buttonAct)
     m_buttonAct->setVisible( true );
 }

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

 void GuiQtMenuButton::unmanage(){
   if ( m_buttonAct)
     m_buttonAct->setVisible( false );
 }

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

 void GuiQtMenuButton::enable(){
   if ( m_buttonAct)
     m_buttonAct->setEnabled( true );
   if (m_hideDisabled)
     manage();
 }

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

 void GuiQtMenuButton::disable(){
   if ( m_buttonAct)
     m_buttonAct->setDisabled( true );
   if (m_hideDisabled)
     unmanage();
 }

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtMenuButton::setLabel( const std::string &name ){
  m_label = name;
  if ( m_buttonAct)
    m_buttonAct->setText( QString::fromStdString(m_label) );

  // set shortcut
  QString lbl = QString::fromStdString( name );
  if (lbl.contains("@")) {
    QStringList lbls = lbl.split("@");
    setLabel( lbls[0].toStdString() );
    if (lbls.size() == 3)
      setAccelerator( lbls[1].toStdString(), lbls[2].toStdString() );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setDialogLabel --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtMenuButton::setDialogLabel( const std::string &name ){
  m_label = name + " ...";
  if ( m_buttonAct)
    m_buttonAct->setText( QString::fromStdString(m_label) );
}

/* --------------------------------------------------------------------------- */
/* setAccelerator --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtMenuButton::setAccelerator( const std::string &acc_key,
                                      const std::string &acc_text )
{
  m_accelerator_key  = acc_key;
  m_accelerator_text = acc_text;
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtMenuButton::myWidget() {
  return m_buttonAct->menu();
}

/* --------------------------------------------------------------------------- */
/* Activate --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtMenuButton::Activate(){
  BUG_DEBUG("Activate");
  clearMessage();
  // Spezialfall Menus im Navigator, die bei verschieden Parents haben koennen
  if (getParent()->getParent() &&
     getParent()->getParent()->Type() == GuiElement::type_Navigator)
   dynamic_cast <GuiButtonListener*>(m_listener)->setParent( getParent() );
  if( m_listener == 0 )
    return;

  QWidget *w = QApplication::focusWidget();
  if( w != 0 ){
    // Welches Widget auch immer den Focus hat, soll ihn verlieren, damit vor
    // der Funktion alle Daten korrekt im Datapool landen.
    w->clearFocus();
  }

  if( m_event != 0 ){
    BUG_DEBUG("ButtonPressed with event");
    m_listener->ButtonPressed( m_event );
  }
  else{
    BUG_DEBUG("ButtonPressed");
    m_listener->ButtonPressed();
  }
  if( w != 0 ){
    // Nun sollten alle Daten im Datapool gelandet sein.
    w->setFocus();
  }
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */

GuiElement *GuiQtMenuButton::getElement(){
  return this;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtMenuButton::serializeXML(std::ostream &os, bool recursive){
  GuiMenuButton::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtMenuButton::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiMenuButton::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtMenuButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiMenuButton::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* unmap --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtMenuButton::unmap(){
  if (m_buttonAct && m_buttonAct->isVisible())
    m_buttonAct->setVisible(false);
  BUG_DEBUG("unmap: " << getName());
  GuiQtElement::unmap();
}
