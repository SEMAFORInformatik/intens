

/* System headers */
#include "gui/qt/GuiQtTableLabelItem.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setFieldLabelAttributes --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtTableLabelItem::setFieldLabelAttributes( int rowFrom, int colFrom
			                       , int rowTo  , int colTo ){
  BUG_DEBUG(  "GuiQtTableLabelItem::setFieldLabelAttributes" );
}

/* --------------------------------------------------------------------------- */
/* getTableItem --                                                             */
/* --------------------------------------------------------------------------- */

GuiTableItem *GuiQtTableLabelItem::getTableItem(){
  return this;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtTableLabelItem::serializeXML(std::ostream &os, bool recursive){
  os << "<GuiTableLabelItem";
  os << ">" << std::endl;
  os<<"</GuiTableLabelItem>"<<std::endl;
}
