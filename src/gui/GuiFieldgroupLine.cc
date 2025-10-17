
#include <string>

#include "gui/GuiElement.h"
#include "gui/GuiVoid.h"
#include "gui/GuiIndex.h"
#include "gui/GuiDataField.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "xfer/XferDataItemIndex.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiFieldgroupLine::GuiFieldgroupLine(GuiElement *parent)
  : m_arrowbar( false )
  , m_tableposition( 1 )
  , m_tablesize_visible_lines( -1 )
{
}

GuiFieldgroupLine::GuiFieldgroupLine(const GuiFieldgroupLine &line)
  : m_arrowbar(line.m_arrowbar)
  , m_tableposition( line.m_tableposition )
  , m_tablesize_visible_lines( line.m_tablesize_visible_lines )
{
}

GuiFieldgroupLine::~GuiFieldgroupLine(){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    delete (*it);
  }
}

/* --------------------------------------------------------------------------- */
/* attach --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiFieldgroupLine::attach( GuiElement *el ){
  if( getElement()->getParent()->Type() == GuiElement::type_Fieldgroup ){
    GuiFieldgroup *fg = getElement()->getParent()->getFieldgroup();
    assert(fg);

    if( fg->getTableSize() > 0 && !isArrowbar() ){
      if( attachTableElement( fg, el ) ){
        return; // attached
      }
    }
  }
  m_elements.push_back( el );
}

/* --------------------------------------------------------------------------- */
/* getActionDataField --                                                       */
/* --------------------------------------------------------------------------- */
GuiDataField *GuiFieldgroupLine::getActionDataField(){
  BUG(BugGui,"GuiFieldgroupLine::getActionDataField");
  // Wir nehmen immer der Einfachheit halber das erste Tabellen-Feld, da
  // dies keine Rolle spielt. Daher geben wir als 'size' einfach 1 mit.
  return getTableDataField( 0, 1 );
}

/* --------------------------------------------------------------------------- */
/* getTableDataField --                                                        */
/* --------------------------------------------------------------------------- */
GuiDataField *GuiFieldgroupLine::getTableDataField( int no, int size ){
  assert( size > 0 );
  assert( no >= 0 && no < size );
  if( isArrowbar() ) return 0;
  int inx = m_tableposition + no;
  if( inx < (int)m_elements.size() ){
    GuiElement *el = m_elements[inx];
    if( el->Type() == GuiElement::type_Text       ||
        el->Type() == GuiElement::type_Textfield  ||
        el->Type() == GuiElement::type_OptionMenu ||
        el->Type() == GuiElement::type_ComboBox   ||
        el->Type() == GuiElement::type_Toggle     ||
        el->Type() == GuiElement::type_RadioButton ||
        el->Type() == GuiElement::type_FieldButton ){
      return el->getDataField();
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* attachTableElement --                                                       */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroupLine::attachTableElement( GuiFieldgroup *fg, GuiElement *el ){
  BUG(BugGui,"GuiFieldgroupLine::attachTableElement");
  assert( fg->getTablePosition() > 0 );
  assert( fg->getTableSize() > 0 );
  BUG_MSG("TablePosition = " << fg->getTablePosition() <<
          ", Position = " << (int)m_elements.size());

  m_tableposition = fg->getTablePosition();

  if( m_tableposition != (int)m_elements.size() ){
    BUG_EXIT("not TablePosition");
    return false;
  }

  if( !el->cloneableForFieldgroupTable() ){
    for( int i=0; i< fg->getTableSize(); i++ ){
      BUG_MSG("insert GuiVoid");
      m_elements.push_back( new GuiVoid(getElement()) );
    }
    BUG_EXIT("not cloneable");
    return false;
  }

  GuiDataField *clone = 0;
  XferDataItemIndex *inx = 0;

  GuiDataField *field = static_cast<GuiDataField *>(el->getDataField());
  int wildcard = field->getNumOfWildcards();
  if( wildcard == 0 ){
    BUG_MSG("no Wildcard found: create it");
    inx = field->newDataItemIndex();
    inx->setWildcard();
    wildcard = 1;
  }

  inx = field->getDataItemIndexWildcard( wildcard );
  GuiElement *parent = getElement()->getParent();
  std::string fg_name;
  if( parent )
    fg_name = parent->getName();
  inx->setIndexName( "@fg_index@", fg_name );
  m_elements.push_back( field->getElement() );

  for( int i=1; i < fg->getTableSize(); i++ ){
    BUG_MSG("insert Clone");
    clone = field->CloneForFieldgroupTable();
    clone->getElement()->setParent(this->getElement());
    inx = clone->getDataItemIndexWildcard( wildcard );
    inx->setIndexName( "@fg_index@", fg_name );
    BUG_MSG("before Lowerbound: " << inx->Lowerbound());
    inx->setLowerbound( inx->Lowerbound() + i );
    BUG_MSG("after  Lowerbound: " << inx->Lowerbound());
    m_elements.push_back( clone->getElement() );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setTableIndexRange --                                                       */
/* --------------------------------------------------------------------------- */

void GuiFieldgroupLine::setTableIndexRange( int minIndex, int maxIndex ){
  if( !isArrowbar() ) return;

  GuiIndex *index = 0;
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it)->Type() == GuiElement::type_Index ){
      index = (*it)->getGuiIndex();
      assert(index);
      index->setMinIndex( minIndex );
      index->setMaxIndex( maxIndex );
    }
    else if( (*it)->Type() == GuiElement::type_IndexMenu ){
      if ((*it)->getGuiIndex())
        (*it)->getGuiIndex()->setMinIndex(minIndex);
    }
  }
  if( index != 0 ){
    index->getElement()->update( GuiElement::reason_Always );
  }
}

/* --------------------------------------------------------------------------- */
/* setTableSizeVisibleLines --                                                 */
/* --------------------------------------------------------------------------- */

void GuiFieldgroupLine::setTableSizeVisibleLines( int tablesize ){
  getElement()->getParent()->setAttributeChangedFlag(m_tablesize_visible_lines == tablesize);
  m_tablesize_visible_lines = tablesize;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiFieldgroupLine::serializeXML(std::ostream &os, bool recursive){
	if( !isArrowbar() ){ // wollen wir das wirklich ??
	  os << "<intens:FieldgroupLine>" << std::endl;
	  GuiElementList::iterator it;
	  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
		(*it)->serializeXML(os, recursive);
	  }
	  os<<"</intens:FieldgroupLine>"<<std::endl;
	}
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiFieldgroupLine::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  GuiFieldgroup *fg = getElement()->getParent()->getFieldgroup();
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    Json::Value jsonElem = Json::Value(Json::objectValue);
    (*it)->serializeJson(jsonElem, onlyUpdated);
    if (m_tablesize_visible_lines == -1 || (it-m_elements.begin()) < (m_tablesize_visible_lines + fg->getTablePosition()))
      jsonObj.append(jsonElem);
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiFieldgroupLine::serializeProtobuf(in_proto::ElementList* eles, in_proto::FieldGroup *element,  bool onlyUpdated){
  GuiFieldgroup *fg = getElement()->getParent()->getFieldgroup();
  auto line = element->add_lines();
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if (m_tablesize_visible_lines == -1 || (it-m_elements.begin()) < (m_tablesize_visible_lines + fg->getTablePosition())) {
      (*it)->serializeProtobuf(eles, onlyUpdated);
      auto ref = line->add_elements();
      ref->set_id((*it)->getElementIntId());
      ref->set_type((*it)->ProtobufType());
    }
  }
  return false;
}
#endif
