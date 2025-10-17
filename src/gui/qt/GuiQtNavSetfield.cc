
#include <sstream>

#include "utils/Debugger.h"

#include "app/DataPoolIntens.h"
#include "app/DataSet.h"
#include "gui/qt/GuiQtNavSetfield.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtNavSetfield::GuiQtNavSetfield( GuiElement *parent, XferDataItem *dataitem )
  : GuiQtNavTextfield( parent, dataitem )
  , m_conv(new StringConverter)
  , m_dataset( 0 ){
  installDataset( dataitem );
}

GuiQtNavSetfield::~GuiQtNavSetfield(){
  delete m_conv;
}

/* --------------------------------------------------------------------------- */
/* setWidth --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtNavSetfield::setWidth( int width ){
  assert( m_conv != 0 );
  m_conv->setWidth( width );
  GuiQtNavTextfield::setWidth( width );
  m_conv->setAdjustLeft(true);
}

/* --------------------------------------------------------------------------- */
/* FieldIsEditable --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiQtNavSetfield::FieldIsEditable(){
  return false;
}

/* --------------------------------------------------------------------------- */
/* getFormattedValue --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQtNavSetfield::getFormattedValue( std::string &s ){
  BUG_DEBUG("getFormattedValue('" << s << "'");
  std::string value;
  s = "";
  m_param->getFormattedValue( value );

  bool use_input( false );
  if( m_param->DataItem()->Data()->getDataType() == DataDictionary::type_String ){
    use_input = true;
    int typ = 0;
    if( m_dataset->strings()->GetValue( typ ) ){
      if( typ > 0 ){
	use_input = false;
      }
    }
  }
  if( use_input ){
    int inx = m_dataset->getInputPosition( value );
    if( inx < 0 ){
      return false;
    }
  }
  else{
    int inx = m_dataset->getOutputPosition( value );
    if( inx < 0 ){
      return false;
    }
    if( !m_dataset->getInputValue( value, inx ) ){
      return false;
    }
  }
  s = value;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtNavSetfield::setInputValue(){
  return false;
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtNavSetfield::installDataset( XferDataItem *dataitem ){
  std::string datasetName = dataitem->getUserAttr()->DataSetName();
  m_dataset = DataPoolIntens::Instance().getDataSet( datasetName );
  assert( m_dataset != 0 );
  return true;
}
