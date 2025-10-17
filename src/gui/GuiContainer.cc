
#include <QWidget>
#include <QGridLayout>
#include <QBoxLayout>

/* System headers */
#include <assert.h>

#include "gui/GuiContainer.h"
#if HAVE_HEADLESS
#elif HAVE_QT
#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/qt/GuiQtForm.h"
#endif
#include "gui/GuiOrientationContainer.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiContainer::GuiContainer( GuiElement *elem, GuiElement::ElementType type )
  : m_element_type( type )
  , m_element(elem)
  , m_horizontal_scrollbar( GuiElement::scrollbar_undefined )
  , m_vertical_scrollbar( GuiElement::scrollbar_undefined )
  , m_with_paned( GuiElement::status_Undefined )
  , m_with_frame( GuiElement::status_Undefined )
  , m_useRuler( false )
{
}

GuiContainer::GuiContainer( const GuiContainer &gc )
  : m_element_type( gc.m_element_type )
  , m_element( gc.m_element )
  , m_horizontal_scrollbar( gc.m_horizontal_scrollbar )
  , m_vertical_scrollbar( gc.m_vertical_scrollbar )
  , m_with_paned( gc.m_with_paned )
  , m_with_frame( gc.m_with_frame )
  , m_useRuler( false )
{
}


/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Type --                                                                     */
/* --------------------------------------------------------------------------- */

GuiElement::ElementType GuiContainer::Type() {
  return m_element_type;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiContainer::manage(){
  for( iterator it = begin(); it != end(); ++it ){
    (*it)->manage();
  }
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiContainer::unmanage(){
  BUG(BugGui,"GuiContainer::unmanage");
  for(iterator it = begin(); it != end(); ++it ){
    (*it)->unmanage();
  }
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiContainer::update( GuiElement::UpdateReason reason ){
  BUG(BugGui,"GuiContainer::update");
  for( iterator it = begin(); it != end(); ++it ){
    (*it)->update( reason );
  }
}

/* --------------------------------------------------------------------------- */
/* resize --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiContainer::resize(){
  BUG(BugGui,"GuiContainer::resize");
  for( iterator it = begin(); it != end(); ++it ){
    (*it)->resize();
  }
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiContainer::getSize( int &x, int &y ){
  x = 0;
  y = 0;
  bool vertical(true);
  if ( m_element ) {
    GuiElement::ElementType type = m_element->Type();
    BUG_DEBUG("m_element type '"<<type );
    if ( type == GuiElement::type_Folder ) {
      vertical = false;
    } else if ( type == GuiElement::type_Container ) {
      GuiOrientationContainer* c = dynamic_cast<GuiOrientationContainer*>(m_element);
      if ( c ) {
        std::cout << "IIIIIIIIIIIIIIIIIIIIIIIIII GuiContainer::getSize orientation: " << c->getOrientation() << std::endl;
        if ( c->getOrientation() == GuiElement::orient_Horizontal) {
          vertical = false;
        }
      } else {
        BUG_DEBUG("m_element is Container but not Orientation Container");
      }
    }
  }
  for( iterator it = begin(); it != end(); ++it ){
	int xE(0), yE(0);
	BUG_DEBUG("Child Type:"<<(*it)->Type());
    (*it)->getSize(xE, yE);
	BUG_DEBUG("getSize Child '"<<(*it)->getName()<<"', Type:"<<(*it)->Type()
			  <<" w: " << xE << ", h = " << yE );
	// assume vertical orientation
    if ( vertical ) {
      x = std::max(x, xE);
      y += yE;
    } else {
      x += xE;
      y = std::max(y, yE);
    }
  }
  BUG_DEBUG("getSize All w: " << x << ", h = " << y );
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiContainer::enable(){
  for( iterator it = begin(); it != end(); ++it ){
    (*it)->enable();
  }
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiContainer::disable(){
  for( iterator it = begin(); it != end(); ++it ){
    (*it)->disable();
  }
}

/* --------------------------------------------------------------------------- */
/* attach --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiContainer::attach( GuiElement *elmnt ){
  push_back( elmnt );
}

/* --------------------------------------------------------------------------- */
/* front --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiContainer::front( GuiElement *elmnt ){
  if( empty() == 0 ){
    push_back( elmnt );
  }
  else{
    insert( begin(), elmnt );
  }
}

/* --------------------------------------------------------------------------- */
/* setScrollbar --                                                             */
/* --------------------------------------------------------------------------- */

void GuiContainer::setScrollbar( GuiElement::ScrollbarType sb ){
  if( m_horizontal_scrollbar == GuiElement::scrollbar_undefined ){
    m_horizontal_scrollbar = sb;
  }
  if( m_vertical_scrollbar == GuiElement::scrollbar_undefined ){
    m_vertical_scrollbar = sb;
  }
}

/* --------------------------------------------------------------------------- */
/* setScrollbar --                                                             */
/* --------------------------------------------------------------------------- */

void GuiContainer::setScrollbar( GuiElement::Orientation o,
				 GuiElement::ScrollbarType sb ){
  switch( o ){
  case GuiElement::orient_Horizontal:
    m_horizontal_scrollbar = sb;
    if( sb == GuiElement::scrollbar_ON ){
      m_vertical_scrollbar = GuiElement::scrollbar_AS_NEEDED;
    }
    break;
  case GuiElement::orient_Vertical:
    m_vertical_scrollbar = sb;
    if( sb == GuiElement::scrollbar_ON ){
      m_horizontal_scrollbar = GuiElement::scrollbar_AS_NEEDED;
    }
    break;
  default:
    setScrollbar( sb );
  }
}

/* --------------------------------------------------------------------------- */
/* withHorizontalScrollbar --                                                  */
/* --------------------------------------------------------------------------- */

bool GuiContainer::withHorizontalScrollbar() const{
  return !(m_horizontal_scrollbar == GuiElement::scrollbar_undefined ||
	   m_horizontal_scrollbar == GuiElement::scrollbar_OFF);
}

/* --------------------------------------------------------------------------- */
/* withVerticalScrollbar --                                                    */
/* --------------------------------------------------------------------------- */

bool GuiContainer::withVerticalScrollbar() const{
  return !(m_vertical_scrollbar == GuiElement::scrollbar_undefined ||
	   m_vertical_scrollbar == GuiElement::scrollbar_OFF);
}

/* --------------------------------------------------------------------------- */
/* withScrollbars --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiContainer::withScrollbars() const{
  return withHorizontalScrollbar() || withVerticalScrollbar();
}

/* --------------------------------------------------------------------------- */
/* getHorizontalScrollbar --                                                   */
/* --------------------------------------------------------------------------- */

GuiElement::ScrollbarType GuiContainer::getHorizontalScrollbar() const{
  return m_horizontal_scrollbar;
}

/* --------------------------------------------------------------------------- */
/* getVerticalScrollbar --                                                     */
/* --------------------------------------------------------------------------- */

GuiElement::ScrollbarType GuiContainer::getVerticalScrollbar() const{
  return m_vertical_scrollbar;
}

/* --------------------------------------------------------------------------- */
/* setPanedWindow --                                                           */
/* --------------------------------------------------------------------------- */

void GuiContainer::setPanedWindow( GuiElement::FlagStatus s ){
  m_with_paned = s;
}

/* --------------------------------------------------------------------------- */
/* withPanedWindow --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiContainer::withPanedWindow(){
  switch( m_with_paned ){
  case GuiElement::status_Undefined:
    if( m_element == 0 ) return false;
    if( m_element->getParent() == 0 ) return false;
    return m_element->getParent()->withPanedWindow();
  case GuiElement::status_ON:
    return true;
  case GuiElement::status_OFF:
    return false;
  default:
    break;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setFrame --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiContainer::setFrame( GuiElement::FlagStatus s ){
  if( m_with_frame == GuiElement::status_Undefined ){
    m_with_frame = s;
  }
}

/* --------------------------------------------------------------------------- */
/* withFrame --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiContainer::withFrame(){
  switch( m_with_frame ){
  case GuiElement::status_Undefined:
    if( m_element == 0 ) return false;
    if( m_element->getParent() == 0 ) return false;
   return false; // m_parent->withFrame();
  case GuiElement::status_ON:
    return true;
  case GuiElement::status_OFF:
    return false;
  default:
    break;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setUseFrame --                                                              */
/* --------------------------------------------------------------------------- */

void GuiContainer::setUseFrame(){
  if( m_with_frame == GuiElement::status_Undefined ){
    m_with_frame = GuiElement::status_ON;
  }
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiContainer::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui,"GuiContainer::hasChanged");
  for( iterator it = begin(); it != end(); ++it )
    if( (*it)->hasChanged(trans, xfer, show) )
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiContainer::destroy(){
  BUG(BugGui,"GuiContainer::destroy");
  for( iterator it = begin(); it != end(); ++it ){
    if( !(*it)->destroy() ){
      BUG_EXIT("Element of Type " << (*it)->Type() << " not destroyed");
      return false;
    }
    BUG_MSG("Element destroyed successfully");
  }
//   m_width  = 0;
//   m_height = 0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* replace --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiContainer::replace( GuiElement *old_el, GuiElement *new_el ){
  BUG(BugGui,"GuiContainer::replace");
  assert( old_el != 0 );
  assert( new_el != 0 );

  for( iterator it = begin(); it != end(); ++it ){
    if( (*it) == old_el ){
      BUG_MSG("found");
      if( old_el->destroy() ) BUG_MSG("old Element destroyed");
      old_el->setParent( 0 );
      new_el->setParent( m_element );
      (*it) = new_el;
      m_element->install( new_el );
      return false;
    }
    if( (*it)->replace( old_el, new_el ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* doLayoutRuler --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiContainer::doLayoutRuler(bool vertical) {

  LayoutRuler ruler(vertical);
#if HAVE_HEADLESS
#elif HAVE_QT
  QSize hs_pre = m_element->getQtElement()->myWidget()->sizeHint();

  // get all gridlayouts if possible
  GuiElementList::iterator it;
  for( it = begin(); it != end(); ++it ){
    if ((*it)->Type() == GuiElement::type_Void ||
	(*it)->Type() == GuiElement::type_Separator ||
	(*it)->Type() == GuiElement::type_Stretch) {
      continue;
    }
    if ((*it)->Type() == GuiElement::type_Fieldgroup ||
	(*it)->Type() == GuiElement::type_FieldgroupLine) {
      QGridLayout *gridLayout = 0;
      if ((*it)->Type() == GuiElement::type_FieldgroupLine) {
	GuiQtFieldgroupLine *fg_line =static_cast<GuiQtFieldgroupLine*>(*it);
	if (static_cast<GuiQtFieldgroupLine*>(*it)->getElementList().size() != 1 ||
	    static_cast<GuiQtFieldgroupLine*>(*it)->getElementList()[0]->Type() != GuiElement::type_Fieldgroup) {
	  if ( fg_line->getElementList()[0]->Type() == GuiElement::type_Void ||
	       fg_line->getElementList()[0]->Type() == GuiElement::type_Stretch) {
	    continue;
	  }
	  std::cerr << "  WARNING:: (Maybe-)Problems while justify Fieldgroup named '" << (*it)->getParent()->getName() << "'\n" << std::flush;
	  continue;
	  return false;
	}
	gridLayout = dynamic_cast<QGridLayout*>(static_cast<GuiQtFieldgroupLine*>(*it)->getElementList()[0]->getQtElement()->myWidget()->layout());
      } else
	gridLayout = dynamic_cast<QGridLayout*>((*it)->getQtElement()->myWidget()->layout());
      assert( gridLayout );
      ruler.addGridLayout( gridLayout );
    } else if ( (*it)->Type() == GuiElement::type_Container ) {
      GuiOrientationContainer *cont =dynamic_cast<GuiOrientationContainer*>(*it);
      assert(cont);
      QBoxLayout* layout = dynamic_cast<QBoxLayout*>((*it)->getQtElement()->myWidget()->layout());
      assert( layout );
      ruler.addBoxLayout( layout );
    } else {
      std::cerr << "  WARNING:: Don't know, how to  justify vertically GuiElementType '" << (*it)->Type() << "'\n" << std::flush;
      return false;
    }
  }

  // call ruler execute
  ruler.execute();

  // extent parent dialog
  GuiQtForm *form = dynamic_cast<GuiQtForm*>(m_element->getMyForm()->getElement());
  QSize hs_post = m_element->getQtElement()->myWidget()->sizeHint();
  form->extendSize(hs_post.width()-hs_pre.width(), hs_post.height()-hs_pre.height());

#endif
  return true;
}

/* --------------------------------------------------------------------------- */
/* alignWithVRuler --                                                          */
/* --------------------------------------------------------------------------- */

void GuiContainer::alignWithVRuler(){
  if( !m_useRuler ) return;
  doLayoutRuler(true);
}

/* --------------------------------------------------------------------------- */
/* alignWithHRuler --                                                          */
/* --------------------------------------------------------------------------- */

void GuiContainer::alignWithHRuler(){
  if( !m_useRuler ) return;
  doLayoutRuler(false);
  return;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiContainer::serializeXML(std::ostream &os, bool recursive){
  serializeAttrs( os );
}

void GuiContainer::serializeAttrs( std::ostream &os ){
  GuiElementList::iterator it;
  for( it = begin(); it != end(); ++it ){
    (*it)->serializeXML(os);
  }
}

void GuiContainer::serializeAttrs(Json::Value& jsonObj, bool onlyUpdated){
  GuiElementList::iterator it;
  for( it = begin(); it != end(); ++it ){
    Json::Value jsonElem = Json::Value(Json::objectValue);
    (*it)->serializeJson(jsonElem, onlyUpdated);
    jsonObj.append(jsonElem);
  }
}

#if HAVE_PROTOBUF
void GuiContainer::serializeAttrs(in_proto::ElementList* eles, google::protobuf::RepeatedPtrField<in_proto::ElementRef>* reflist, bool onlyUpdated){
  GuiElementList::iterator it;
  for( it = begin(); it != end(); ++it ){
    (*it)->serializeProtobuf(eles, onlyUpdated);
    auto ref = reflist->Add();
    ref->set_id((*it)->getElementIntId());
    ref->set_type((*it)->ProtobufType());
  }
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleElement --                                                        */
/* --------------------------------------------------------------------------- */

void GuiContainer::getVisibleElement(GuiElementList& res) {
  for( GuiElementList::iterator iter = begin(); iter != end(); ++iter ){
    if ( GuiElement::isVisibleElementType( (*iter)->Type() ) ) {
      res.push_back(*iter);
      if ((*iter)->Type() != GuiElement::type_Fieldgroup) {
        continue;
      }
    }
    (*iter)->getVisibleElement( res );
  }
}

#if HAVE_HEADLESS
#elif HAVE_QT
/*=============================================================================*/
/* local class GuiContainer::LayoutRuler                                     */
/*=============================================================================*/
/* --------------------------------------------------------------------------- */
/* addGridLayout --                                                            */
/* --------------------------------------------------------------------------- */
void GuiContainer::LayoutRuler::addGridLayout( QGridLayout* gl ) {
  m_gridLayouts.push_back( gl );
  int cLen =  m_vertical ? gl->columnCount() : gl->rowCount();

  for (int i=0; i < gl->count(); ++i) {
    // get item position info
    int row, column, rowSpan, columnSpan;
    gl->getItemPosition(i, &row, &column, &rowSpan, &columnSpan);
    QRect re = gl->cellRect(row, column);
    int maxlen = m_vertical ?  re.width() : re.height();

    // update max length list
    int index =  m_vertical ? column : row;
    while (index >= max_lengths.size())
      max_lengths.push_back(0);
    if (columnSpan == 1 && columnSpan != cLen && m_vertical || rowSpan == 1 && rowSpan != cLen  && !m_vertical) {
      if (max_lengths[index] < maxlen) {
	max_lengths[index] = maxlen;
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getSingleWidget --                                                          */
/* --------------------------------------------------------------------------- */
QWidget*  GuiContainer::LayoutRuler::getSingleWidget(QLayoutItem * item) {
  QWidget* w = 0;
  if (item) {
    if (item->layout()) {
      if (item->layout()->count() == 1)
	w = item->layout()->itemAt(0)->widget();
    } else
      if (item->widget())
	w = item->widget ();
  }
  if ( item->spacerItem() ) {  // work todo!!!
    std::cerr << "  WARNING:: Don't know, how to justify spacerItem.\n" << std::flush;
  }
  return w;
}

/* --------------------------------------------------------------------------- */
/* addBoxLayout --                                                             */
/* --------------------------------------------------------------------------- */
void GuiContainer::LayoutRuler::addBoxLayout( QBoxLayout* bl ) {
  m_boxLayouts.push_back( bl );
  int len =  bl->count();

  bl->activate(); // sehr wichtig!!!
  int cdiff = 0;
  for (int c=0; c<len; ++c ) {
    if (c-cdiff == max_lengths.size())
      max_lengths.push_back(0);
    assert(bl->itemAt(c));
    QWidget *w = getSingleWidget( bl->itemAt(c) );
    if (!w) continue; // unable to find a widget (=> more work must be done)
    assert(w!=0);
    int maxlen = m_vertical ?  w->sizeHint().width() : w->sizeHint().height();
    if (!maxlen) { ++cdiff;  continue; }
    if (max_lengths[c-cdiff]  < maxlen) max_lengths[c-cdiff] = maxlen;
  }
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiContainer::LayoutRuler::execute() {
  // rule with gridlayout
  if (m_gridLayouts.size()) {
    if ( m_boxLayouts.size() ) {  // work todo!!!
      std::cerr << "  WARNING:: Unknown layout mixture.\n" << std::flush;
    }
    std::vector<QGridLayout*>::iterator it;
    for( it = m_gridLayouts.begin(); it != m_gridLayouts.end(); ++it ){
      QGridLayout *gridLayout = (*it);
      int cLen =  m_vertical ? gridLayout->columnCount() : gridLayout->rowCount();
      int cdiff = 0;
      (*it)->setSizeConstraint(QLayout::SetFixedSize);

      for (int c=0; c<max_lengths.size() && c+cdiff < cLen; ++c ) {
        if (m_vertical) {
          if (gridLayout->cellRect(0, c+cdiff).width() && (c+cdiff+1) < max_lengths.size()) {   // and not last column
            gridLayout->setColumnMinimumWidth( c+cdiff, max_lengths[c] );
          }
          else{
            --c, ++cdiff;
          }
        } else {
          if (gridLayout->cellRect(c+cdiff, 0).height()) {
            if ( (c+cdiff+1) < max_lengths.size() ){  // not last row
              gridLayout->setRowMinimumHeight( c+cdiff, max_lengths[c] );
            }else{
              --c, ++cdiff;
            }
          }
        }
      }
      (*it)->activate(); // sehr wichtig!!!
    }
  } else
    // rule with gridlayout
    if (m_boxLayouts.size()) {
      std::vector<QBoxLayout*>::iterator it;
      for( it = m_boxLayouts.begin(); it != m_boxLayouts.end(); ++it ){
        int len =  (*it)->count();
        int cdiff = 0;
        (*it)->setSizeConstraint(QLayout::SetFixedSize);

        for (int c=0; c<max_lengths.size() && c+cdiff < len; ++c ) {
          QWidget *w = getSingleWidget( (*it)->itemAt(c+cdiff) );
          if (!w) continue; // unable to find a widget (=> more work must be done)
          if (m_vertical) {
            if (w->width()) {
              w->setMinimumWidth( max_lengths[c] );
            }
            else --c, ++cdiff;
          } else {
            if (w->height()) {
              w->setMinimumHeight( max_lengths[c] );
            }
            else --c, ++cdiff;
          }
        }
        (*it)->activate(); // sehr wichtig!!!
      }
    }
}
#endif
