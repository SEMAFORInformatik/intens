
#include <iostream>

#include "datapool/DataStructValue.h"
#include "datapool/DataStructElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataStructValue::DataStructValue()
  : DataValue( 0 )
  , m_struct_element( 0 ){
  setAttributes( 0, DATAisValid );
}

DataStructValue::DataStructValue( const DataStructElement *el )
  : DataValue( 0 )
  , m_struct_element( el ){
  assert( el != 0 );
  if( m_struct_element->hasValidElements( false ) ){
    setAttributes( DATAisValid, 0 );
  }
  else{
    setAttributes( 0, DATAisValid );
  }
}

DataStructValue::DataStructValue( const DataStructElement *el, DATAAttributeMask mask )
  : DataValue( mask )
  , m_struct_element( el ){
  assert( el != 0 );
  if( m_struct_element->hasValidElements( false ) ){
    setAttributes( DATAisValid, 0 );
  }
  else{
    setAttributes( 0, DATAisValid );
  }
}

DataStructValue::~DataStructValue(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getStructValue --                                                           */
/* --------------------------------------------------------------------------- */

const DataStructValue &DataStructValue::getStructValue() const{
  return *this;
}

/* --------------------------------------------------------------------------- */
/* getStructElement --                                                         */
/* --------------------------------------------------------------------------- */

const DataStructElement *DataStructValue::getStructElement() const{
  return m_struct_element;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataStructValue::print( std::ostream &ostr ) const{
  if( isValid() ){
    assert( m_struct_element != 0 );
    std::cout << "<valid struct>";
  }
  else{
    std::cout << "<invalid struct>";
  }
}

/* --------------------------------------------------------------------------- */
/* createEmptyElement --                                                       */
/* --------------------------------------------------------------------------- */

DataElement *DataStructValue::createEmptyElement(){
  return 0;
}
