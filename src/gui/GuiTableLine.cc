
#include <stdlib.h>

#include "gui/GuiTable.h"
#include "gui/GuiTableLine.h"
#include "gui/GuiFactory.h"
#include "datapool/DataReference.h"
#include "xfer/XferDataItem.h"
#include "app/UserAttr.h"
#include "utils/Debugger.h"
#include "utils/JsonUtils.h"

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

// void GuiTableLine::update( UpdateReason ){
//   BUG(BugGuiTable,"GuiTable::update empty");
// }

/* --------------------------------------------------------------------------- */
/* setLabelAlignment --                                                        */
/* --------------------------------------------------------------------------- */

void GuiTableLine::setLabelAlignment( GuiElement::Alignment align ){
  m_label_alignment = align;
}

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiTableLine::setLabel( const std::string &label ){
  m_label = label;
  return true;
}

/* --------------------------------------------------------------------------- */
/* Label --                                                                    */
/* --------------------------------------------------------------------------- */

std::string &GuiTableLine::Label(){
  return m_label;
}

/* --------------------------------------------------------------------------- */
/* setTooltip --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiTableLine::setTooltip( const std::string &tooltip ){
  m_tooltip = tooltip;
  return true;
}

/* --------------------------------------------------------------------------- */
/* Tooltip --                                                                  */
/* --------------------------------------------------------------------------- */

std::string &GuiTableLine::Tooltip(){
  return m_tooltip;
}

/* --------------------------------------------------------------------------- */
/* addTableDataItem --                                                         */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTableLine::addTableDataItem( XferDataItem *dataitem ){
  DataReference *data = dataitem->Data();
  UserAttr      *attr = static_cast<UserAttr*>( data->getUserAttr() );

  GuiTableItem *item = 0;

  switch( attr->GuiKind() ){
  case UserAttr::gui_kind_toggle:
  case UserAttr::gui_kind_radiobutton:
  case UserAttr::gui_kind_field:
  case UserAttr::gui_kind_button:
    item =  GuiFactory::Instance()->createTableDataItem( getGuiTable()->getElement() );
    break;

//   case UserAttr::gui_kind_optionmenu:
  case UserAttr::gui_kind_combobox:
    {
      item =  GuiFactory::Instance()->createTableComboBoxItem( getGuiTable()->getElement(), attr->DataSetName() );
    break;
    }
  default:
    std::cout << "unknown Gui Type" << std::endl;
    exit(1);
  }

  item->getDataField()->installDataItem( dataitem );
  item->setDataItem( dataitem );
  m_tableitems.push_back( item );
  // tooltip
  if (!m_tooltip.empty()) m_tooltip += ";";
  m_tooltip += dataitem->userAttr().Helptext();

  return item;
}

/* --------------------------------------------------------------------------- */
/* addTableLabelItem --                                                        */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTableLine::addTableLabelItem( const std::string &label, GuiElement::Alignment align ){
  if( align == GuiElement::align_Default ){
    align = m_label_alignment;
  }
  GuiTableItem *item = GuiFactory::Instance()->createTableLabelItem( getGuiTable()->getElement(), label, align );
  m_tableitems.push_back( item );
  return item;
}

/* --------------------------------------------------------------------------- */
/* getTableItem --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTableLine::getTableItem( int col ){
  BUG_PARA(BugGuiTable,"GuiTableLine::getTableItem", col );
  if( col < 0 ){
    return 0;
  }
  if( m_tableitems.empty() ){
    return 0;
  }
  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Matrix ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getTableItem( 0, col );
  }

  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Array ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getTableItem( col );
  }

  if( col < (int)m_tableitems.size() ){
    if( m_tableitems[col] == 0 ){
      return 0;
    }
  return m_tableitems[col]->getTableItem();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataItem --                                                             */
/* --------------------------------------------------------------------------- */

XferDataItem *GuiTableLine::getDataItem( int col ){
  BUG_PARA(BugGuiTable,"GuiTableLine::getDataItem", col );
  if( col < 0 ){
    return 0;
  }
  if( m_tableitems.empty() ){
    return 0;
  }
  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Matrix ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getDataField()->DataItem();
  }

  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Array ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getDataField()->DataItem();
  }

  if( col < (int)m_tableitems.size() ){
    if( m_tableitems[col] == 0 ){
      return 0;
    }
  return m_tableitems[col]->getDataField()->DataItem();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getTableItem --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiTableLine::getTableItem( int row, int col ){
  BUG_PARA(BugGui,"GuiTableLine::getTableItem", row << "," << col );
  assert( m_tableitems.size() == 1 );
  return m_tableitems[0]->getTableItem( row, col );
}

/* --------------------------------------------------------------------------- */
/* syncroniseTableItemLength --                                                */
/* --------------------------------------------------------------------------- */

void GuiTableLine::syncroniseTableItemLength(){
  if( m_tableitems.size() < 2 ){
    return;
  }
  int len = getMaxLength();
  setMaxLength( len );
}

/* --------------------------------------------------------------------------- */
/* getMaxLength --                                                             */
/* --------------------------------------------------------------------------- */

int GuiTableLine::getMaxLength(){
  int len, maxlen = 0;
  GuiTableItemList::iterator it;
  for( it = m_tableitems.begin(); it != m_tableitems.end(); ++it ){
    len = (*it)->getMaxLength();
    maxlen = len > maxlen ? len : maxlen;
  }
  return maxlen;
}

/* --------------------------------------------------------------------------- */
/* setMaxLength --                                                             */
/* --------------------------------------------------------------------------- */

void GuiTableLine::setMaxLength( int len ){
  GuiTableItemList::iterator it;
  for( it = m_tableitems.begin(); it != m_tableitems.end(); ++it ){
    (*it)->setMaxLength( len );
  }
}

/* --------------------------------------------------------------------------- */
/* getLength --                                                                */
/* --------------------------------------------------------------------------- */

int GuiTableLine::getLength( int col ){
  if( col < 0 ){
    return 0;
  }
  if( m_tableitems.empty() ){
    return 0;
  }
  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Matrix ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getMaxLength();
  }

  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Array ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getMaxLength();
  }

  if( col < (int)m_tableitems.size() ){
    if( m_tableitems[col] == 0 ){
      return 0;
    }
    return m_tableitems[col]->getMaxLength();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

void GuiTableLine::setLength( int len, int col ){
  if( col < 0 ){
    return;
  }
  if( m_tableitems.empty() ){
    return;
  }
  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Matrix ){
    assert( m_tableitems.size() == 1 );
    m_tableitems[0]->setMaxLength( len );
    return;
  }

  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Array ){
    assert( m_tableitems.size() == 1 );
    m_tableitems[0]->setMaxLength( len );
    return;
  }

  if( col < (int)m_tableitems.size() ){
    if( m_tableitems[col] != 0 ){
      m_tableitems[col]->setMaxLength( len );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getDataItem --                                                             */
/* --------------------------------------------------------------------------- */

Scale *GuiTableLine::getScalefactor( int col ){
  BUG_PARA(BugGuiTable,"GuiTableLine::getDataItem", col );
  if( col < 0 ){
    return 0;
  }
  if( m_tableitems.empty() ){
    return 0;
  }
  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Matrix ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getScalefactor();
  }

  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Array ){
    assert( m_tableitems.size() == 1 );
    return m_tableitems[0]->getScalefactor();
  }

  if( col < (int)m_tableitems.size() ){
    if( m_tableitems[col] == 0 ){
      return 0;
    }
  return m_tableitems[col]->getScalefactor();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool GuiTableLine::check(){
  if( m_tableitems.empty() ){
    return true;
  }
  if( m_tableitems[0]->ItemType() == GuiTableItem::item_Matrix ||
      m_tableitems[0]->ItemType() == GuiTableItem::item_Array   ){
    if( m_tableitems.size() != 1 ){
      return true;
    }
    else{
      return false; // ok
    }
  }

  GuiTableItemList::iterator it;
  for( it = m_tableitems.begin(); it != m_tableitems.end(); ++it ){
    if( (*it)->ItemType() == GuiTableItem::item_Matrix ||
        (*it)->ItemType() == GuiTableItem::item_Array   ){
      return true;
    }
  }
  return false; // ok
}

/* --------------------------------------------------------------------------- */
/* ItemType --                                                                 */
/* --------------------------------------------------------------------------- */

GuiTableItem::TableItemType GuiTableLine::ItemType(){
  assert( !m_tableitems.empty() );
  return m_tableitems[0]->ItemType();
}

/* --------------------------------------------------------------------------- */
/* LineSize --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiTableLine::LineSize( int &rows, int &cols ){
  if( m_tableitems.size() == 1 ){
    m_tableitems[0]->getTableItemSize( rows, cols );
    return;
  }
  rows = 1;
  cols = (int)m_tableitems.size();
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiTableLine::serializeXML(std::ostream &os, bool recursive){
  int rows; int cols;
  m_tableitems[0]->getTableItemSize( rows, cols );

  os << "<GuiTableLine";
  os << " size=\"" << m_tableitems.size() <<"\"";
  os << " rows=\"" << rows <<"\"";
  os << " cols=\"" << cols <<"\"";
  os << " label=\""<< m_label <<"\"";
  os << ">" << std::endl;

  for( int i1=0;i1<rows;++i1 ){
    for( int i2=0;i2<cols;++i2 ){
      m_tableitems[0]->getTableItem(i1,i2)->getElement()->serializeXML(os, recursive);
    }
  }

  os<<"</GuiTableLine>"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiTableLine::isDataItemUpdated( TransactionNumber trans ){
  BUG_PARA( BugGuiTable, "isDataItemUpdated", "TransactionNUmber=" << trans );

  GuiTableItemList::iterator it;
  for( it = m_tableitems.begin(); it != m_tableitems.end(); ++it ){
    if( (*it)->isDataItemUpdated( trans ) || m_visibility_updated){
      m_visibility_updated = false;
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setVisible --                                                               */
/* --------------------------------------------------------------------------- */

void GuiTableLine::setVisible(bool visible){
  m_visible = visible;
  m_visibility_updated = true;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiTableLine::acceptIndex( const std::string &name, int inx ){
  BUG(BugGui,"GuiTableLine::acceptIndex");
  GuiTableItemList::iterator it;
  for( it = m_tableitems.begin(); it != m_tableitems.end(); ++it ){
    if ( !dynamic_cast<GuiDataField*>(*it)->DataItem()->acceptIndex( name, inx ) ) {
      BUG_EXIT("table line item '"<<dynamic_cast<GuiDataField*>(*it)->DataItem()->getFullName(true)<<"' does not accept index.");
      return false;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiTableLine::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiTableLine::setIndex",name << " =[" << inx << "]");
  assert( inx >= 0 );

  GuiTableItemList::iterator it;
  for( it = m_tableitems.begin(); it != m_tableitems.end(); ++it )
    dynamic_cast<GuiDataField*>(*it)->DataItem()->setIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiTableLine::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui, "GuiTableLine::hasChanged");
  for( int i=0; i < m_tableitems.size(); ++i) {
    if ( getTableItem(i)->hasChanged(trans, xfer, show) ) {
      BUG_EXIT("item '"<<getTableItem(i)->getDataField()->DataItem()->getFullName(true)<<"' has Changed.");
      return true;
    }
  }
  BUG_EXIT("noChanges.");
  return false;
}
