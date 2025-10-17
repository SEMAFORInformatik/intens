
#include <iostream>

#include "datapool/DataComplexValue.h"
#include "datapool/DataComplexElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataComplexValue::DataComplexValue()
  : DataValue( 0 )
  , m_Value( 0.0, 0.0 ){
  setInvalid();
}

DataComplexValue::DataComplexValue( dComplex d )
  : DataValue( 0 )
  , m_Value( d ){
  setValid();
}

DataComplexValue::DataComplexValue( dComplex d, DATAAttributeMask mask )
  : DataValue( mask )
  , m_Value( d ){
}

DataComplexValue::~DataComplexValue(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexValue::setValue( const dComplex val ){
  m_Value = val;
  setValid();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexValue::getValue( dComplex &val ) const {
  if( isValid() ){
    val = m_Value;
    return true;
  }
  val = 0.0;
  return false;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataComplexValue::print( std::ostream &ostr ) const{
  if( isValid() ){
    std::cout << m_Value;
  }
  else{
    std::cout << "<invalid complex>";
  }
}

/* --------------------------------------------------------------------------- */
/* createEmptyElement --                                                       */
/* --------------------------------------------------------------------------- */

DataElement *DataComplexValue::createEmptyElement(){
  return new DataComplexElement();
}
