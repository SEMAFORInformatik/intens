
#include <qapplication.h>
#include <qmenubar.h>

#include "gui/GuiContainer.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtMenubar.h"
#include "gui/qt/GuiQtPulldownMenu.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/Debugger.h"
class GuiQtSeparator;
#include <QApplication>
#include <QStyleFactory>
#if QT_VERSION < 0x050000
#include <QMotifStyle>
#endif

class MainMenuBar : public QMenuBar{
public:
  MainMenuBar() : QMenuBar() {
#if QT_VERSION < 0x050000
    if (dynamic_cast<QMotifStyle*>(QApplication::style())) {
      // QMenubar hat innerhalb von Intens ein Problem mit MotifStyles
      // (kleine Qt-Programme haben keine Probleme, aber innerhalb von Intens
      // gibt es irgendwo eine Sache die hier Probleme bereitet
      // => Dialoggroesse vergroessert sich laufend (endlos)
      // == Loesung ==> ein Wechsel zum Windows Style
      std::cerr << " + + + WARNING in QT 4 :: QMenuBar-Widget containing Help menu has problems with the dialog width." <<std::endl<<std::flush;
      setStyle( QStyleFactory::create("Windows") );
    }
#endif
  }
  virtual QSize minimumSizeHint() const {
    QSize s = QMenuBar::minimumSizeHint();
    return QMenuBar::sizeHint();
  }

#if QT_VERSION < 0x050000
  virtual bool event(QEvent* event) {
    if (event->type()== QEvent::Polish) {
      if (dynamic_cast<QMotifStyle*>(QApplication::style())) {
	// siehe Problem oben
	// hier ist das Problem, wenn man später zum Motifstyle wechselt
	event->ignore();
	return false;
      }
    }
    return QMenuBar::event(event);
  }
#endif

};

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

GuiQtMenubar::GuiQtMenubar( GuiElement *parent )
  : GuiQtElement( parent )
  , m_container( parent, GuiElement::type_Menubar )
  , m_helpmenu( 0 )
  , m_qmenubar( 0 ){
}

GuiQtMenubar::~GuiQtMenubar(){ /* empty */ }

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtMenubar::create(){
  BUG_PARA(BugGui,"GuiQtMenubar::create"
	   ,"Elements = " << m_container.size());
  assert( m_qmenubar == 0 );

  // create menubar
  m_qmenubar = new MainMenuBar();
  m_qmenubar->setObjectName( "GuiMenubar" );

  QFont font =  m_qmenubar->font();
  m_qmenubar->setFont( QtMultiFontString::getQFont( "@menu@", font ) );
  m_qmenubar->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  if (GuiQtManager::backgroundColor().isValid()) {
    QPalette pal=  m_qmenubar->palette();
    pal.setColor(m_qmenubar->backgroundRole(), GuiQtManager::backgroundColor() );
    m_qmenubar->setPalette(pal);
  }

  GuiContainer::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it) {
    if( withScrollbars() ){
      (*it)->setScrollbar( scrollbar_AS_NEEDED );
    }
    else{
      (*it)->setScrollbar( scrollbar_OFF );
    }
    if( (*it)->Type() == type_Separator ){
      static_cast<GuiQtSeparator *>( (*it) )->setOrientation( orient_Vertical );
    }
    // dieser Separator ordnet alle nachfolgenden Menues rechtbuendig an
    // (wenn der Style dies unterstuetzt)
    if((*it) == m_helpmenu){
      m_qmenubar->addSeparator();
    }
    (*it)->create(); // true
    (*it)->manage();
  }
}

/* --------------------------------------------------------------------------- */
/* attachHelp --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtMenubar::attachHelp( GuiPulldownMenu *menu ){
  GuiQtElement *el = menu->getElement()->getQtElement();
  if( el == 0 ) return;
  if( el->Type() != type_PulldownMenu ) return;
  m_helpmenu = static_cast<GuiQtPulldownMenu *>(el);

  attach( el );
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtMenubar::manage(){
  if (m_qmenubar)
    m_qmenubar->show();
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */

GuiElement *GuiQtMenubar::getElement(){
  return this;
}


/* --------------------------------------------------------------------------- */
/* ehemals Container Methoden                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtMenubar::getSize( int &w, int &h ) {
  QSize  hs   =  m_qmenubar->sizeHint();
  h = hs.height();
  w = hs.width();
}

void GuiQtMenubar::update( UpdateReason r) {
  GuiContainer::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it) {
    (*it)->update( r );
  }
}

QWidget* GuiQtMenubar::myWidget() {
  return m_qmenubar;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtMenubar::serializeXML(std::ostream &os, bool recursive){
  GuiMenubar::serializeXML( os, recursive );
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtMenubar::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiMenubar::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtMenubar::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return GuiMenubar::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

void GuiQtMenubar::serializeContainerElements( std::ostream &os ){
  GuiElementList::iterator el;
  for( el = m_container.begin(); el != m_container.end(); ++el ){
    (*el)->serializeXML(os);
  }
}

/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtMenubar::serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated){
  GuiElementList::iterator el;
  bool ret(false);
  for( el = m_container.begin(); el != m_container.end(); ++el ){
    if (!(*el)->isShownWebApi()) continue;
    Json::Value jsonElem = Json::Value(Json::objectValue);
    bool _ret = (*el)->serializeJson(jsonElem, onlyUpdated);
    ret |= _ret;
    if (!_ret) continue;
    jsonObj.append(jsonElem);
  }
  return ret;
}


/* --------------------------------------------------------------------------- */
/* serializeContainerElements --                                               */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtMenubar::serializeContainerElements(in_proto::ElementList* eles, in_proto::Menubar* element, bool onlyUpdated){
  GuiElementList::iterator el;
  bool ret(false);
  for( el = m_container.begin(); el != m_container.end(); ++el ){
    Json::Value jsonElem = Json::Value(Json::arrayValue);
    ret |= (*el)->serializeProtobuf(eles, onlyUpdated);
    auto ref = element->add_elements();
    ref->set_id((*el)->getElementIntId());
    ref->set_type((*el)->ProtobufType());
  }
  return ret;
}
#endif

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtMenubar::getDialogExpandPolicy() {
  return (GuiElement::Orientation) 0;
}
