
#include <sstream>

#include "xfer/XferConverter.h"
#include "datapool/DataComplexValue.h"
#include "xml/XMLValue.h"
#include "XMLComplexVector.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLComplexVector::XMLComplexVector( XMLElement *parent, XferDataItem *xfer )
  : XMLElement( parent )
  , XMLVector( xfer ){
}

XMLComplexVector::~XMLComplexVector(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XMLComplexVector::setValue( const std::string & value ){
  if( ignoreValues() ){
    return;
  }

  ComplexConverter conv( 0, -1, 0, false, true );
  std::istringstream is( value );
  dComplex dc;
  if( conv.read( is, dc ) ){
    DataValue *data = new DataComplexValue( dc );
    insertValue( m_elements-1, data );
  }
}

/* --------------------------------------------------------------------------- */
/* getXferDataItem --                                                          */
/* --------------------------------------------------------------------------- */

XferDataItem *XMLComplexVector::getXferDataItem(){
  return getDataItem();
}

/* --------------------------------------------------------------------------- */
/* end --                                                                      */
/* --------------------------------------------------------------------------- */

void XMLComplexVector::end(){
  if( m_type == e_Matrix ){
    return;
  }
  setValues( ignoreValues() );
}

/* --------------------------------------------------------------------------- */
/* newElement --                                                               */
/* --------------------------------------------------------------------------- */

XMLElement *XMLComplexVector::newElement( const std::string &element ){
  if( element == "value" || element=="v"){
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
    return new XMLComplexVector( this, newDataItem() );
  }

  return new XMLElement( this );
}
