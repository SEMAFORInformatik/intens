
#include <iostream>

#include "datapool/DataIntegerValue.h"
#include "datapool/DataIntegerElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataIntegerValue::DataIntegerValue()
  : DataValue( 0 )
  , m_Value( 0 ){
  setInvalid();
}

DataIntegerValue::DataIntegerValue( int i )
  : DataValue( 0 )
  , m_Value( i ){
  setValid();
}

DataIntegerValue::DataIntegerValue( int i, DATAAttributeMask mask )
  : DataValue( mask )
  , m_Value( i ){
}

DataIntegerValue::~DataIntegerValue(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerValue::setValue( const int val ){
  m_Value = val;
  setValid();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerValue::getValue( int &val ) const {
  if( isValid() ){
    val = m_Value;
    return true;
  }
  val = 0;
  return false;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataIntegerValue::print( std::ostream &ostr ) const{
  if( isValid() ){
    std::cout << m_Value;
  }
  else{
    std::cout << "<invalid int>";
  }
}

/* --------------------------------------------------------------------------- */
/* createEmptyElement --                                                       */
/* --------------------------------------------------------------------------- */

DataElement *DataIntegerValue::createEmptyElement(){
  return new DataIntegerElement();
}
