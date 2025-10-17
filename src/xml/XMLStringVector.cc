#include <qstring.h>

#include "utils/Debugger.h"
#include "datapool/DataStringValue.h"
#include "xml/XMLValue.h"
#include "xml/XMLStringVector.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLStringVector::XMLStringVector( XMLElement *parent, XferDataItem *xfer )
  : XMLElement( parent )
  , XMLVector( xfer ){
  m_type = e_Vector;
}

XMLStringVector::~XMLStringVector(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XMLStringVector::setValue( const std::string & value ){
  if( ignoreValues() ){
    return;
  }

  DataValue *data = new DataStringValue( value );
  insertValue( m_elements-1, data );
}

/* --------------------------------------------------------------------------- */
/* getXferDataItem --                                                          */
/* --------------------------------------------------------------------------- */

XferDataItem *XMLStringVector::getXferDataItem(){
  return getDataItem();
}

/* --------------------------------------------------------------------------- */
/* end --                                                                      */
/* --------------------------------------------------------------------------- */

void XMLStringVector::end(){
  if( m_type == e_Matrix ){
    return;
  }
  setValues( ignoreValues() );
}

/* --------------------------------------------------------------------------- */
/* newElement --                                                               */
/* --------------------------------------------------------------------------- */

XMLElement *XMLStringVector::newElement( const std::string &element ){
  if( element == "value" || element == "v"){
    if( m_elements == 0 ){
      setLastIndexWildcard();
    }
    ++m_elements;
    return new XMLValue( this );
  }

  if( element == "vector" ){
    m_type = e_Matrix;
    setLastIndexLowerbound( m_elements );

    ++m_elements;
    return new XMLStringVector( this, newDataItem() );
  }

  return new XMLElement( this );
}
