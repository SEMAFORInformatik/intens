
/* System headers */
#include <QWidget>

#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/GuiVoid.h"
#include "gui/GuiStretch.h"
#include "gui/qt/GuiQtIndex.h"
#include "gui/qt/GuiQtFieldgroup.h"
#include "gui/qt/GuiQtDataField.h"
#include "gui/qt/GuiQtIndexMenu.h"
#include "xfer/XferDataItemIndex.h"
#include "streamer/BasicStream.h"
#include "utils/Debugger.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtFieldgroupLine::GuiQtFieldgroupLine(GuiElement *parent)
  : GuiFieldgroupLine(parent)
  , GuiQtElement( parent )
  , m_widget( 0 )
{
  assert( parent->Type() == type_Fieldgroup );
}

GuiQtFieldgroupLine::GuiQtFieldgroupLine(const GuiQtFieldgroupLine &line )
  : GuiFieldgroupLine(line)
  , GuiQtElement( 0 )
  , m_widget( 0 )
{
  GuiElementList::const_iterator it;
  GuiElement *el = 0;
  for( it = line.m_elements.begin(); it != line.m_elements.end(); ++it ){
    if( (*it)->cloneable() ){
      el = (*it)->clone();
      assert( el != 0 );
      el->setParent(this->getElement());
      m_elements.push_back(el);
    }
    else{
      m_elements.push_back(new GuiVoid(this));
    }
  }
}

GuiQtFieldgroupLine::~GuiQtFieldgroupLine(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroupLine::create(){
  BUG_PARA( BugGui, "create", "Elements = " << m_elements.size() );

  QWidget *parent = getParent()->getQtElement()->myWidget();
  assert(parent != 0);
  m_widget = parent;

  // create werden die childs in GuiQtFieldgroup::create
  // wegen der TabOrder
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldgroupLine::destroy(){
  BUG(BugGui,"GuiQtFieldgroupLine::destroy");
  bool rslt = true;
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( !(*it)->destroy() ){
      rslt = false;
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* replace --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldgroupLine::replace(GuiElement *old_el, GuiElement *new_el ){
  BUG(BugGui,"GuiQtFieldgroupLine::replace");
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it) == old_el ){ // Es hat ihn !
      BUG_MSG("found");
      (*it) = new_el;
      return true;
    }
  }
  return false;
}


/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroupLine::manage() {
  BUG(BugGui,"GuiQtFieldgroupLine::manage");
  GuiElementList::iterator it;

  int i=-1;
  for( it = m_elements.begin(); it != m_elements.end(); ++it, ++i ) {
    (*it)->manage();

    // Anzahl der sichtbaren Linien könnte ändern
    if (getTableSizeVisibleLines() > 0 && (*it)->getQtElement() && (*it)->getQtElement()->myWidget()) {
      if (getTableSizeVisibleLines() > i) {
        (*it)->getQtElement()->myWidget()->show();
      } else {
        (*it)->getQtElement()->myWidget()->hide();
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroupLine::update( UpdateReason reason ){
  BUG_PARA( BugGui, "update", "Reason " << reason );
  GuiFieldgroup *fg = getParent()->getFieldgroup();
  int cnt = 0;
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    BUG_MSG( "Type is " << (*it)->Type() );
    (*it)->update( reason );

    // arrowbar => continue
    if (dynamic_cast<GuiQtFieldgroupLine*>(*it) && dynamic_cast<GuiQtFieldgroupLine*>(*it)->isArrowbar()) continue;

    // Anzahl der sichtbaren Linien könnte ändern
    if ( it - m_elements.begin() >= fg->getTablePosition() &&
	 (*it)->Type() != type_Index &&
	 getTableSizeVisibleLines() > 0 && (*it)->getQtElement() &&
	 (*it)->getQtElement()->myWidget()) {
      if (getTableSizeVisibleLines() >= ++cnt) {
        (*it)->getQtElement()->myWidget()->show();
      } else {
        (*it)->getQtElement()->myWidget()->hide();
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldgroupLine::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui,"GuiQtFieldgroupLine::hasChanged");
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it)->Type() != type_Index ){
      if ((*it)->hasChanged( trans, xfer, show )) {
	BUG_EXIT("item has Changed");
	return true;
      }
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroupLine::enable(){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    (*it)->enable();
  }
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroupLine::disable(){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    (*it)->disable();
  }
}

/* --------------------------------------------------------------------------- */
/* getLastElement --                                                           */
/* --------------------------------------------------------------------------- */

GuiElement* GuiQtFieldgroupLine::getLastElement() {
  if (m_elements.size())
    return m_elements.back();
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldgroupLine::acceptIndex( const std::string &name, int inx ){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( !(*it)->acceptIndex( name, inx ) ){
      return false;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroupLine::setIndex( const std::string &name, int inx ){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    (*it)->setIndex( name, inx );
  }
}

/* --------------------------------------------------------------------------- */
/* streamableElement --                                                        */
/* --------------------------------------------------------------------------- */
BasicStream* GuiQtFieldgroupLine::streamableObject() {
  // search for first streamable child
  BasicStream* retStream=0;
  for( GuiElementList::iterator it = m_elements.begin();
       it != m_elements.end(); ++it ){
    BasicStream* stream = (*it)->streamableObject();
    if (stream) {
      if (!retStream)
	retStream = stream;
      else {
	std::cerr << "Mulitiple streamable GuiElements found in '"
		  <<  getParent()->getName() << "', will use first.\n";
      }
    }
  }
  return retStream;
}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtFieldgroupLine::serializeXML(std::ostream &os, bool recursive){
  GuiFieldgroupLine::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtFieldgroupLine::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return GuiFieldgroupLine::serializeJson(jsonObj, onlyUpdated);
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtFieldgroupLine::serializeProtobuf(in_proto::ElementList* eles, in_proto::FieldGroup *element, bool onlyUpdated){
  return GuiFieldgroupLine::serializeProtobuf(eles, element, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleElement --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtFieldgroupLine::getVisibleElement(GuiElementList& res) {
  for( GuiElementList::iterator iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if (isVisibleElementType( (*iter)->Type() )) {
      res.push_back(*iter);
      if ((*iter)->Type() != type_Fieldgroup) {
        continue;
      }
    }
    (*iter)->getVisibleElement( res );
  }

}


void GuiQtFieldgroupLine::getSize(int &w, int &h){
  GuiElementList::iterator it;
  int x=0, y = 0;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
     (*it)->getSize(x,y);
//     w  += x;
//     h = h > y ? h : y;
  }
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtFieldgroupLine::getExpandPolicy() {
  int ed = 0;
  GuiElementList::iterator iter;

  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ((*iter)->getQtElement())
      ed |= (int) (*iter)->getQtElement()->getExpandPolicy();
    else if ( (*iter)->Type() == GuiElement::type_Stretch ) {
      int hsFac, vsFac;
      dynamic_cast<GuiStretch*>( (*iter) )->getStretchFactor( hsFac, vsFac );
      if (hsFac) ed |= (int) (Qt::Horizontal);
      if (vsFac) ed |= (int) (Qt::Vertical);
    }
  }
  return ( Qt::Orientations ) ed;
}

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtFieldgroupLine::getContainerExpandPolicy() {
  int ed = 0;
  GuiElementList::iterator iter;

  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ((*iter)->getQtElement())
      ed |= (int) (*iter)->getQtElement()->getContainerExpandPolicy();
    else if ( (*iter)->Type() == GuiElement::type_Stretch ) {
      int hsFac, vsFac;
      dynamic_cast<GuiStretch*>( (*iter) )->getStretchFactor( hsFac, vsFac );
      if (hsFac) ed |= (int) (Qt::Horizontal);
      if (vsFac) ed |= (int) (Qt::Vertical);
    }
  }
  return (GuiElement::Orientation) ed;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtFieldgroupLine::getDialogExpandPolicy() {
  int ed = 0;
  GuiElementList::iterator iter;

  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ((*iter)->getQtElement())
      ed |= (int) (*iter)->getQtElement()->getDialogExpandPolicy();
    //
    // simple trick if scrollFactor equal 69 (Summer of 69)
    // stretch is also expandable for dialog
    //
    else if ( (*iter)->Type() == GuiElement::type_Stretch ) {
      int hsFac, vsFac;
      dynamic_cast<GuiStretch*>( (*iter) )->getStretchFactor( hsFac, vsFac );
      if (hsFac==69) ed |= (int) (Qt::Horizontal);
      if (vsFac==69) ed |= (int) (Qt::Vertical);
    }
  }
  return (GuiElement::Orientation) ed;
}

/* --------------------------------------------------------------------------- */
/* isHiddenLine --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtFieldgroupLine::isHiddenLine(){
  GuiQtFieldgroup *fg = static_cast<GuiQtFieldgroup *>(getParent());
  if ( fg->getTableSize() == 0 || m_elements.size() < 2) return false;
  GuiQtDataField* datafield = dynamic_cast<GuiQtDataField*>(m_elements[1]);
  if (datafield && datafield->getLength() == 0)
    return true;
  return false;
}
