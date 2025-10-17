
#include <qsplitter.h>
#include <QScrollArea>
#include <QGroupBox>
#include <qlayout.h>
#include <QEnterEvent>

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtOrientationContainer.h"
#include "gui/qt/GuiQtFolder.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/QtMultiFontString.h"

#include "gui/GuiVoid.h"
#include "gui/GuiStretch.h"
#include "gui/GuiFieldgroup.h"
#include "app/Plugin.h"
#include "utils/JsonUtils.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* Private class functions                                                     */
/*=============================================================================*/
/** MyQSplitter **/
GuiQtOrientationContainer::MyQSplitter::MyQSplitter(Qt::Orientation o, GuiQtElement *e )
  : QSplitter(o), m_element(e) {}
GuiQtOrientationContainer::MyQSplitter::~MyQSplitter() {}

void GuiQtOrientationContainer::MyQSplitter::enterEvent ( QEnterEvent *e ) {
  m_element->enterEvent( e );
  QWidget::enterEvent( e );
}
void GuiQtOrientationContainer::MyQSplitter::leaveEvent ( QEvent *e ) {
  m_element->clearMessage();
  QWidget::leaveEvent( e );
}

/** MyQScrollArea **/
GuiQtOrientationContainer::MyQScrollArea::MyQScrollArea(GuiQtElement *e ) : m_element(e) {}
GuiQtOrientationContainer::MyQScrollArea::~MyQScrollArea() {}

void GuiQtOrientationContainer::MyQScrollArea::enterEvent ( QEnterEvent *e ) {
  m_element->enterEvent( e );
  QWidget::enterEvent( e );
}
void GuiQtOrientationContainer::MyQScrollArea::leaveEvent ( QEvent *e ) {
  m_element->clearMessage();
  QWidget::leaveEvent( e );
}

/** MyQGroupBox **/
GuiQtOrientationContainer::MyQGroupBox::MyQGroupBox(GuiQtElement *e ) : m_element(e) {}
GuiQtOrientationContainer::MyQGroupBox::~MyQGroupBox() {}

void GuiQtOrientationContainer::MyQGroupBox::enterEvent ( QEnterEvent *e ) {
  m_element->enterEvent( e );
  QWidget::enterEvent( e );
}
void GuiQtOrientationContainer::MyQGroupBox::leaveEvent ( QEvent *e ) {
  m_element->clearMessage();
  QWidget::leaveEvent( e );
}

/** MyQWidget **/
GuiQtOrientationContainer::MyQWidget::MyQWidget(GuiQtElement *e ) : m_element(e) {}
GuiQtOrientationContainer::MyQWidget::~MyQWidget() {}

void GuiQtOrientationContainer::MyQWidget::enterEvent ( QEnterEvent *e ) {
  m_element->enterEvent( e );
  QWidget::enterEvent( e );
}
void GuiQtOrientationContainer::MyQWidget::leaveEvent ( QEvent *e ) {
  m_element->clearMessage();
  QWidget::leaveEvent( e );
}

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtOrientationContainer::GuiQtOrientationContainer( Orientation orientation, GuiElement *parent )
  : GuiQtElement(parent)
  , m_frame(0)
  , m_scrollview(0)
  , m_splitter(0)
  , m_container( this, GuiElement::type_Container )
  , m_orientation(orientation)
  , m_vertical(false) {
}

GuiQtOrientationContainer::GuiQtOrientationContainer( const GuiQtOrientationContainer &cont )
  : GuiQtElement( cont )
  , m_frame(0), m_scrollview(0), m_splitter(0)
  , m_container( cont.m_container ), m_orientation( cont.m_orientation )
  , m_vertical( cont.m_vertical )
{
  GuiElementList::const_iterator it;
  for( it = cont.m_container.begin(); it != cont.m_container.end(); ++it ) {
    if ((*it)->cloneable()) {
      GuiElement *elem = (*it)->clone();
      m_container.attach( elem );
      elem->setParent( this );
    }
    else {
      std::cerr << "GuiElement not clonable   GuiType["<<(*it)->Type()<<"] name["<<(*it)->getName()<<"]\n"<<std::endl<<std::flush;
      //      assert( false ); // sollte nie sein!!!
    }
  }
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */
GuiElement* GuiQtOrientationContainer::clone() {
  m_clonedList.push_back( new GuiQtOrientationContainer( *this ) );
  return m_clonedList.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtOrientationContainer::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_clonedList.begin(), m_clonedList.end());
}

Qt::Orientation GuiQtOrientationContainer::getQtOrientation() {
  return (m_orientation == orient_Vertical) ? Qt::Vertical : Qt::Horizontal;
}

/* --------------------------------------------------------------------------- */
/* setScrollbar --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtOrientationContainer::setScrollbar( ScrollbarType sb ){
  if (!m_frame) {
    m_container.setScrollbar( sb );
    return;
  }
}

/* --------------------------------------------------------------------------- */
/* setScrollbar --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtOrientationContainer::setScrollbar( Orientation o, ScrollbarType sb ){
  if (!m_frame) {
    m_container.setScrollbar( o, sb );
  }
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtOrientationContainer::create(){
  BUG_PARA(BugGui,"GuiQtOrientationContainer::create"
	   ,"Elements = " << m_container.size());
  QWidget   *sp = NULL;
  QBoxLayout  *boxlayout = NULL;
  QBoxLayout::Direction   direction =  (m_orientation == orient_Vertical) ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;
  int maxFieldgroupExpandSize=0;
  bool no_stretch(true);

  QWidget *parent = getParent()->getQtElement()->myWidget();
  assert(parent != 0);
  assert( m_frame == 0 );

  if( withPanedWindow() ){
    // ein splitter, etwas kompliziertes Verhalten nur mit (vielen) layout zu gebrauchen
    sp = new MyQSplitter( getQtOrientation(), this );
  }
  else if( withScrollbars() ){
    // ein ScollView
    QScrollArea* sv = new MyQScrollArea(this);
    sv->setWidgetResizable( true );
    m_frame = new QFrame();
    sv->setWidget( m_frame );
    sv->setVerticalScrollBarPolicy( getVerticalScrollbarPolicy() );
    sv->setHorizontalScrollBarPolicy( getHorizontalScrollbarPolicy() );
    m_scrollview = sv;
  }
  else  if( withFrame() ) {
    // einfache GroupBox
    QGroupBox* groupbox = new MyQGroupBox(this);
    m_frame = groupbox;
    int h = 2, spacing = 2;
    if( m_title.size() ){
      groupbox->setTitle( QtMultiFontString::getQString( m_title ) );

      // set font
      QFont font =  groupbox->font();
      groupbox->setFont( QtMultiFontString::getQFont( "@groupboxTitle@", font ) );
      h = QFontInfo(QtMultiFontString::getQFont( "@groupboxTitle@", font )).pixelSize();
    }
    groupbox->setContentsMargins( spacing, h, spacing, spacing );
    groupbox->setFlat( false );
  }
  else
    // einfaches Widget
    m_frame = new MyQWidget(this);

  // alle ausser das panedWindow benoetigen nur ein boxlayout
  if( !withPanedWindow() ){
    boxlayout = new QBoxLayout( direction );
    int w = withFrame() ? 2 : 0;
    boxlayout->setContentsMargins(w,w,w,w);
    boxlayout->setSpacing( 4 );
  }

  // Zuerst erstellen wir alle horizontal angeordneten Elemente
  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ) {
    // Ausnahme panedWindow welches fuer jedes Child ein boxlayout benoetigt
    if ( withPanedWindow() ) {
      m_frame = new QFrame( sp );
      boxlayout = new QBoxLayout( direction );
      boxlayout->setContentsMargins(0,0,0,0);
      boxlayout->setSpacing( 0 );
    }
    if ((*it)->Type() == GuiElement::type_Separator) {
      dynamic_cast<GuiQtSeparator*>(*it)->setOrientation( ( (m_orientation == orient_Vertical) ? GuiElement::orient_Horizontal : GuiElement::orient_Vertical));
    }
    (*it)->create();

    // get expand policy
    GuiElement::Orientation exp = orient_Default;
    if ( (*it)->Type() == GuiElement::type_Plugin ) {
      QWidget* qw = reinterpret_cast<QWidget*>( (*it)->getPlugin()->getBaseWidget() );
      exp = getGuiOrientation(qw->sizePolicy().expandingDirections());
      // exp = Qt::Horizontal | Qt::Vertical;
    } else {
      if ((*it)->getQtElement())
        exp = (*it)->getQtElement()->getContainerExpandPolicy();

      // special case: Handle Expanded HideButton Folder
      if ( (*it)->Type() == GuiElement::type_Fieldgroup ) {
        if (getParent()->Type() == GuiElement::type_Folder &&
            getParent()->getFolder()->IsHideButton()) {
          exp = getGuiOrientation((*it)->getQtElement()->getExpandPolicy());
        }
      }
    }

    // get stretch factor
    int stretchFac = (exp == 0) ? 0 : ((exp&m_orientation) ? GuiQtManager::DefaultStretchFactor(): 0) ;

    // insert fieldgroup widgets into layout
    if ( (*it)->Type() == GuiElement::type_Fieldgroup ) {
      stretchFac = dynamic_cast<GuiFieldgroup*>(*it)->getStretchFactor(m_orientation);
      Qt::AlignmentFlag _align = m_orientation == orient_Vertical ? (Qt::AlignLeft) : (Qt::AlignTop);
      // Alignment setzen, hoffentlich macht das keine Probleme
      if (exp == 0 ) {
        Qt::Orientations expIn = (*it)->getQtElement()->getExpandPolicy();
        if ( expIn == 0)  // keine Expand => TopLeft Alignment
          _align = (Qt::AlignmentFlag) (int)(Qt::AlignLeft|Qt::AlignTop);
        else if (!(expIn & m_orientation)) { //  => nur Alignment nicht in expand-Richtung
          _align = (Qt::AlignmentFlag) (int)((m_orientation&expIn) ?
                                             ((m_orientation==GuiElement::orient_Horizontal) ? (Qt::AlignTop) : (Qt::AlignLeft)) :
                                             ((m_orientation==GuiElement::orient_Horizontal) ? (Qt::AlignLeft) : (Qt::AlignTop)) );
        }
        BUG_MSG( "FG Name["<<(*it)->getName()<<"] expIn["<<expIn<<"]");
      } else if (exp == (orient_Horizontal|orient_Vertical)) {
        _align = (Qt::AlignmentFlag) 0;
      } else {
        if (exp & GuiElement::orient_Horizontal)
          _align = (Qt::AlignmentFlag) ((int)Qt::AlignTop);
        else
          _align = (Qt::AlignmentFlag) ((int)Qt::AlignLeft);
      }
      BUG_MSG( "FG Name["<<(*it)->getName()<<"] exp["<<exp<<"] align["<<_align<<"] ContainerOrientation["<<m_orientation<<"] stretchFac["<<stretchFac<<"]");

      // a margin around a fieldgroup with a frame is only here posible
      if ( (*it)->withFrame() || GuiQtManager::backgroundColor() != GuiQtManager::fieldgroupBackgroundColor()) {
        QBoxLayout* bl = new QHBoxLayout();
        bl->setContentsMargins(0,0,0,0);
        bl->addWidget( (*it)->getQtElement()->myWidget(), stretchFac, _align);
        boxlayout->addLayout( bl);

        QSize size = (*it)->getQtElement()->myWidget()->sizeHint();
        maxFieldgroupExpandSize =  (m_orientation == orient_Vertical) ? std::max(size.width(), maxFieldgroupExpandSize) : std::max(size.height(), maxFieldgroupExpandSize);
      } else {
        boxlayout->addWidget( (*it)->getQtElement()->myWidget(), stretchFac, _align);
      }

    } else {
      // insert other types of widgets into layout
      if ( (*it)->Type() == GuiElement::type_Plugin ) {
        QWidget* qw = reinterpret_cast<QWidget*>( (*it)->getPlugin()->getBaseWidget() );
        boxlayout->addWidget( qw, 100);
      }
      else if ( (*it)->Type() == GuiElement::type_Void ) {
        GuiVoid *v = static_cast<GuiVoid*>( (*it) );
        int x,y;
        v->getSize(x,y);
        boxlayout->addSpacing(m_orientation==GuiElement::orient_Horizontal ? x : y);
      }
      else if ( (*it)->Type() == GuiElement::type_Stretch ) {
        GuiStretch *s = static_cast<GuiStretch*>( (*it) );
        int hsFac, vsFac;
        s->getStretchFactor( hsFac, vsFac );
      	boxlayout->addStretch( hsFac );
        no_stretch = false;
      }
      else {
        if ( (*it)->Type() != GuiElement::type_Thermo && (*it)->Type() != GuiElement::type_Separator &&
             exp ==  0 ) {
          Qt::AlignmentFlag _align = (Qt::AlignmentFlag)0;
          Qt::Orientations expIn = (*it)->getQtElement()->getExpandPolicy();
          if ( expIn == 0)  // keine Expand => TopLeft Alignment
            _align = (Qt::AlignmentFlag) (int)(Qt::AlignLeft|Qt::AlignTop);
          else if (expIn != (Qt::Horizontal|Qt::Vertical)) { //  => nur Alignment nicht in expand-Richtung
            _align = (Qt::AlignmentFlag) (int)((m_orientation&expIn) ?
                                               ((m_orientation==GuiElement::orient_Horizontal) ? (Qt::AlignTop) : (Qt::AlignLeft)) :
                                               ((m_orientation==GuiElement::orient_Horizontal) ? (Qt::AlignLeft) : (Qt::AlignTop)) );
          }
          BUG_MSG( "Type["<<(*it)->Type()<<"] Name["<<(*it)->getName()<<"] exp["<<exp<<"] expIn["<<expIn<<"] align["<<_align<<"] ContainerOrientation["<<m_orientation<<"] stretchFac["<<stretchFac<<"]");
          boxlayout->addWidget( (*it)->getQtElement()->myWidget(), stretchFac, _align);
        } else {
          BUG_MSG( "Expand Type["<<(*it)->Type()<<"] Name["<<(*it)->getName()<<"] exp["<<exp<<"] ContainerOrientation["<<m_orientation<<"] stretchFac["<<stretchFac<<"]");
          boxlayout->addWidget( (*it)->getQtElement()->myWidget(), stretchFac);
        }
      }
    }
    if ( withPanedWindow() ) {  // set Layout
      m_frame->setLayout( boxlayout );
    }
  }

  if ( !withPanedWindow() ) {  // set Layout
    if (m_frame)
      m_frame->setLayout( boxlayout );
    else
      if (m_scrollview)
        m_scrollview->setLayout( boxlayout );
  }

  // fix container, if child has no expand policy AND parent is a form
  if (no_stretch) {
    if (getContainerExpandPolicy() == GuiElement::orient_Default) {
      if (getExpandPolicy() == 0)
        boxlayout->addStretch( 0 ); // default Sollbruchstelle
    }
    else if ((((int)getQtOrientation() & (int)getContainerExpandPolicy()) == GuiElement::orient_Default)) {
      boxlayout->addStretch( 0 ); // default Sollbruchstelle
    }
  }

  // alle Widgets fuer dies spaetere Verarbeitung richtig setzen
  if( withPanedWindow() ) {
    m_splitter   = sp;
    m_frame      = sp;
  }

  // Korrektur der Groesse der Fieldgroups mit Frames
  if (maxFieldgroupExpandSize) {
    for( it = m_container.begin(); it != m_container.end(); ++it ) {
      if ( (*it)->Type() == GuiElement::type_Fieldgroup ) {
	if ( m_orientation == orient_Vertical )
	  (*it)->getQtElement()->myWidget()->setMinimumWidth( maxFieldgroupExpandSize );
	else
	  (*it)->getQtElement()->myWidget()->setMinimumHeight( maxFieldgroupExpandSize );
      }
    }
  }

  //  installMessageEvent( m_frame );

  setDebugTooltip();
}

/* --------------------------------------------------------------------------- */
/* resize --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtOrientationContainer::resize(){
  BUG(BugGui,"GuiForm::resize");

  // Zuerst erhält jedes Unter-Element den Auftrag resize()
  m_container.resize();
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtOrientationContainer::manage(){
  m_container.manage();
  if( withScrollbars() && m_scrollview ) m_scrollview->show();
  if( withPanedWindow() && m_splitter ) m_splitter->show();

  (m_orientation == orient_Vertical) ? m_container.alignWithVRuler() : m_container.alignWithHRuler();;
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtOrientationContainer::destroy(){
  bool rslt = m_container.destroy();
  if( m_scrollview ) m_scrollview->deleteLater();
  if( m_splitter ) m_splitter->deleteLater();
  if( m_frame ) m_frame->deleteLater();
  m_splitter   = 0;
  m_scrollview = 0;
  m_frame      = 0;

  return rslt;
}

/* --------------------------------------------------------------------------- */
/* replace --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtOrientationContainer::replace( GuiElement *old_el, GuiElement *new_el ){
  BUG(BugGui,"GuiQtOrientationContainer::replace");

  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    if( (*it) == old_el ){ // Es hat ihn !
      BUG_MSG("found");
      (*it) = new_el;
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtOrientationContainer::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show)  {
  return m_container.hasChanged(trans, xfer, show);
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* install --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtOrientationContainer::install( GuiElement *el ){
  BUG(BugGui,"GuiQtOrientationContainer::install");

//   if( myParentWidget() == 0 ) return;

  BUG_MSG("-> before create");
  el->create();
//   BUG_MSG("-> before align");
//   alignWithRuler();
  BUG_MSG("-> before manage");
  el->manage();
  BUG_MSG("-> before  update");
  el->update( reason_Always );
  BUG_EXIT("install ok");
}

/* --------------------------------------------------------------------------- */
/* getHorizontalScrollbarPolicy --                                             */
/* --------------------------------------------------------------------------- */

Qt::ScrollBarPolicy GuiQtOrientationContainer::getHorizontalScrollbarPolicy() const{
  return getScrollbarPolicy( m_container.getHorizontalScrollbar() );
}

/* --------------------------------------------------------------------------- */
/* getVerticalScrollbarPolicy --                                               */
/* --------------------------------------------------------------------------- */

Qt::ScrollBarPolicy GuiQtOrientationContainer::getVerticalScrollbarPolicy() const{
  return getScrollbarPolicy( m_container.getVerticalScrollbar() );
}

/* --------------------------------------------------------------------------- */
/* serialize --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtOrientationContainer::serializeXML( std::ostream &os, bool recursive ){
  os << "<intens:OrientationContainer orientation=\""
     <<((m_orientation == orient_Vertical) ?"vertical":"horizontal")<<"\">" << std::endl;
  m_container.serializeAttrs( os );
  os << "</intens:OrientationContainer>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtOrientationContainer::serializeJson(Json::Value& jsonObj, bool recursive) {
  jsonObj["orientation"] = (m_orientation == orient_Vertical) ?"vertical":"horizontal";

  Json::Value jsonElem = Json::Value(Json::arrayValue);
  m_container.serializeAttrs(jsonElem);

  jsonObj["element"] = jsonElem;
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["scrollbars"] = withScrollbars();
  jsonObj["panedWindow"] = withPanedWindow();
  jsonObj["frame"] = withFrame();
  jsonObj["title"] = m_title;
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtOrientationContainer::serializeProtobuf(in_proto::ElementList* eles, bool recursive) {
  auto element = eles->add_containers();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_orientation((m_orientation == orient_Vertical) ?
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical :
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal);
  element->set_scrollbars(withScrollbars());
  element->set_paned_window(withPanedWindow());
  element->set_frame(withFrame());
  element->set_title(m_title);
  m_container.serializeAttrs(eles, element->mutable_elements());
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQtOrientationContainer::serializeProtobuf(in_proto::ElementList* eles, in_proto::Folder::Page* page, bool recursive) {
  auto element = page->mutable_container();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_orientation((m_orientation == orient_Vertical) ?
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical :
                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal);
  element->set_scrollbars(withScrollbars());
  element->set_paned_window(withPanedWindow());
  element->set_frame(withFrame());
  element->set_title(m_title);
  m_container.serializeAttrs(eles, element->mutable_elements());
  return false;
}
#endif


/* --------------------------------------------------------------------------- */
/* getVisibleElement --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtOrientationContainer::getVisibleElement(GuiElementList& res) {
  for( GuiElementList::iterator iter = m_container.begin(); iter != m_container.end(); ++iter ){
    if ( GuiElement::isVisibleElementType( (*iter)->Type() ) ) {
      res.push_back(*iter);
      if ((*iter)->Type() != type_Fieldgroup) {
        continue;
      }
    }
    (*iter)->getVisibleElement( res );
  }
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtOrientationContainer::getSize( int &w, int &h ) {
  BUG_DEBUG("getSize: orientation = "<<m_orientation);
  w = h = 0;
  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ) {
    int x(0), y(0);
    (*it)->getSize(x, y);
	BUG_DEBUG("getSize Child '"<<(*it)->getName()<<"', Type:"<<(*it)->Type()
			  <<" w: " << x << ", h = " << y );
	if ( m_orientation == orient_Vertical ) {
	  w = std::max(w,x);
	  h += y;
	} else {
	  w += x;
	  h = std::max(h,y);
	}
  }
  BUG_DEBUG("getSize w: " << w << ", h = " << h );
  if( withFrame() ) {
	h += 26;
	w += 6;
	BUG_DEBUG("with Title/Frame w: " << w << ", h = " << h );
  }
}

/* --------------------------------------------------------------------------- */
/* printSizeInfo --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtOrientationContainer::printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds) {
  GuiQtElement::printSizeInfo(os, intent, onlyMaxChilds);

  GuiElementList::iterator it;
  int newIntent = intent + 1;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    (*it)->printSizeInfo(os, newIntent, onlyMaxChilds);
  }
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtOrientationContainer::myWidget() {
//   assert( m_scrollview  == 0 );
  if( withScrollbars() && m_scrollview ){
    return m_scrollview;
  }
  if( withPanedWindow() && m_splitter ){
    return m_splitter;
  }
  return m_frame;
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtOrientationContainer::getExpandPolicy() {
  int ed = myWidget() ? (int)myWidget()->sizePolicy().expandingDirections() : 0;
  GuiElementList::iterator iter;

  for( iter = m_container.begin(); iter != m_container.end(); ++iter ){

    if ( (*iter)->Type() == GuiElement::type_Plugin ) {
      QWidget* qw = reinterpret_cast<QWidget*>( (*iter)->getPlugin()->getBaseWidget() );
      ed |= qw->sizePolicy().expandingDirections();
      // ed |= (Qt::Vertical|Qt::Horizontal);
    } else
      if ( (*iter)->Type() == GuiElement::type_Stretch ) {
	ed |=  getQtOrientation();
    } else
      if ((*iter)->getQtElement())
	ed |= (int) (*iter)->getQtElement()->getExpandPolicy();
  }
  return ( Qt::Orientations ) ed;
}

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtOrientationContainer::getContainerExpandPolicy() {
  int ed = myWidget() ? (int)myWidget()->sizePolicy().expandingDirections() : 0;
  GuiElementList::iterator iter;

  for( iter = m_container.begin(); iter != m_container.end(); ++iter ){

    if ( (*iter)->Type() == GuiElement::type_Plugin ) {
      QWidget* qw = reinterpret_cast<QWidget*>( (*iter)->getPlugin()->getBaseWidget() );
      ed |= qw->sizePolicy().expandingDirections();
      // ed |= (Qt::Vertical|Qt::Horizontal);
    } else {
      if ((*iter)->getQtElement())
        ed |= (int) (*iter)->getQtElement()->getContainerExpandPolicy();
    }
  }
  return (GuiElement::Orientation) ed;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtOrientationContainer::getDialogExpandPolicy() {
  int ed = myWidget() ? (int)myWidget()->sizePolicy().expandingDirections() : 0;
  GuiElementList::iterator iter;

  for( iter = m_container.begin(); iter != m_container.end(); ++iter ){

    if ( (*iter)->Type() == GuiElement::type_Plugin ) {
      QWidget* qw = reinterpret_cast<QWidget*>( (*iter)->getPlugin()->getBaseWidget() );
      ed |= qw->sizePolicy().expandingDirections();
    } else
      if ((*iter)->getQtElement())
        ed |= (int) (*iter)->getQtElement()->getDialogExpandPolicy();
  }
  return (GuiElement::Orientation) ed;
}
