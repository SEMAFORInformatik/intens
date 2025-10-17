
#include <qmenubar.h>

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtMenuToggle.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/GuiToggleListener.h"
#include "gui/GuiEventData.h"

#include "utils/Debugger.h"
#include "utils/StringUtils.h"

/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/

GuiQtMenuToggle::GuiQtMenuToggle( GuiElement *parent
			      , GuiToggleListener *listener
			      , GuiEventData *event )
    : GuiQtElement( parent )
      , m_toggleAct( 0 )
    , m_listener( listener )
    , m_toggle_state( false )
    , m_event( event ){
  if( event == 0 ){
    if( listener != 0 )
      listener->setToggle( this );
  }
  else
    m_event->m_element = this;
}

GuiQtMenuToggle::GuiQtMenuToggle( const GuiQtMenuToggle &menutoggle )
  : GuiQtElement( menutoggle )
  , m_toggleAct( menutoggle.m_toggleAct )
  , m_listener( menutoggle.m_listener )
  , m_event( menutoggle.m_event )
  , m_toggle_state(menutoggle.m_toggle_state)
  , m_accelerator_key(menutoggle.m_accelerator_key)
  , m_accelerator_text(menutoggle.m_accelerator_text)
  , m_label(menutoggle.m_label) {
}

GuiQtMenuToggle::~GuiQtMenuToggle(){
  delete m_event;
}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::create(){
  BUG(BugGui,"GuiQtMenuToggle::create");
  QMenu* menudata = dynamic_cast<QMenu*>( getParent()->getQtElement()->myWidget() );
  assert( menudata );

  // create menu entry
  m_toggleAct = menudata->addAction( QString::fromStdString(m_label), this, SLOT(ValueChanged()) );
  m_toggleAct->setCheckable( true );
  m_toggleAct->setChecked( m_toggle_state );

  // set Font
  QFont font = m_toggleAct->font();
  if (getParent()->Type() == type_PopupMenu) {
    font = QtMultiFontString::getQFont( "@popup@", font );
  } else if (getParent()->Type() == type_PulldownMenu) {
    font = QtMultiFontString::getQFont( "@pulldown@", font );
  }
  else font = QtMultiFontString::getQFont( "@button@", font );
  m_toggleAct->setFont( font );

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
    grabShortcutAction(m_accelerator_key, m_toggleAct);

    m_toggleAct->setShortcut( QString::fromStdString(m_accelerator_key) );
    if(m_accelerator_text == "Widget") {
      m_toggleAct->setShortcutContext(Qt::WidgetShortcut);
    } else if(m_accelerator_text == "WidgetWithChildren") {
      m_toggleAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    } else if(m_accelerator_text == "Window") {
      m_toggleAct->setShortcutContext(Qt::WindowShortcut);
    } else {  // default
      m_toggleAct->setShortcutContext( Qt::ApplicationShortcut );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::enable(){
   if ( m_toggleAct)
     m_toggleAct->setEnabled( true );
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::disable(){
   if ( m_toggleAct)
     m_toggleAct->setDisabled( true );
}

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtMenuToggle::setLabel( const std::string &name ){
  m_label = name;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setDialogLabel --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::setDialogLabel( const std::string &name ){
  m_label = name + " ...";
}

/* --------------------------------------------------------------------------- */
/* setToggleStatus --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::setToggleStatus( bool state ){
  m_toggle_state = state;
  if (m_toggleAct)
    m_toggleAct->setChecked( m_toggle_state );
}

/* --------------------------------------------------------------------------- */
/* getToggleStatus --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiQtMenuToggle::getToggleStatus(){
  return m_toggle_state;
}

/* --------------------------------------------------------------------------- */
/* myWidget--                                                                  */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtMenuToggle::myWidget() {
  assert(false);
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */

GuiElement *GuiQtMenuToggle::getElement(){
  return this;
}

/* --------------------------------------------------------------------------- */
/* ValueChanged --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::ValueChanged() {
  if( m_listener == 0 )
    return;
  assert( m_toggleAct );
  bool set = m_toggleAct->isChecked();
  if( m_event != 0 ){
    m_event->m_toggleStatus = ( set != 0 );
    m_listener->ToggleStatusChanged( m_event );
  }
   else
     m_listener->ToggleStatusChanged( set != 0 );
}

/* --------------------------------------------------------------------------- */
/* setAccelerator --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::setAccelerator( const std::string &acc_key, const std::string &acc_text ){
  m_accelerator_key  = acc_key;
  m_accelerator_text = acc_text;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtMenuToggle::serializeXML(std::ostream &os, bool recursive){
  GuiMenuToggle::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtMenuToggle::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiMenuToggle::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtMenuToggle::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiMenuToggle::serializeProtobuf(eles, onlyUpdated);
}
#endif
