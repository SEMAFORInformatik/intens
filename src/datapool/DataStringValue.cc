
#include <iostream>

#include "datapool/DataStringValue.h"
#include "datapool/DataStringElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataStringValue::DataStringValue()
  : DataValue( 0 ){
  setAttributes( 0, DATAisValid );
}

DataStringValue::DataStringValue( const std::string &s )
  : DataValue( 0 )
  , m_Value( s ){
  setAttributes( DATAisValid, 0 );
}

DataStringValue::DataStringValue( const std::string &s, DATAAttributeMask mask )
  : DataValue( mask )
  , m_Value( s ){
}

DataStringValue::~DataStringValue(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringValue::setValue( const std::string &val ){
  m_Value = val;
  setAttributes( DATAisValid, 0 );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringValue::getValue( std::string &val ) const {
  if( isValid() ){
    val = m_Value;
    return true;
  }
  val.erase();
  return false;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataStringValue::print( std::ostream &ostr ) const{
  if( isValid() ){
    std::cout << m_Value;
  }
  else{
    std::cout << "<invalid string>";
  }
}

/* --------------------------------------------------------------------------- */
/* createEmptyElement --                                                       */
/* --------------------------------------------------------------------------- */

DataElement *DataStringValue::createEmptyElement(){
  return new DataStringElement();
}
