
#include <iostream>
#include <iomanip>

#include "datapool/DataRealValue.h"
#include "datapool/DataRealElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataRealValue::DataRealValue()
  : DataValue( 0 )
  , m_Value( 0.0 ){
  setInvalid();
}

DataRealValue::DataRealValue( double d )
  : DataValue( 0 )
  , m_Value( d ){
  setValid();
}

DataRealValue::DataRealValue( double d, DATAAttributeMask mask )
  : DataValue( mask )
  , m_Value( d ){
}

DataRealValue::~DataRealValue(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealValue::setValue( const double val ){
  m_Value = val;
  setValid();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealValue::getValue( double &val ) const {
  if( isValid() ){
    val = m_Value;
    return true;
  }
  val = 0.0;
  return false;
}


/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealValue::getValue( std::string &val ) const {
  if( isValid() ){
    // double to string, as in DataStringElement::setValue( double val, ... )
    // todo: write and use common double/int to string method?
    std::ostringstream os;
    os << std::setprecision( 17 ) << m_Value;
    val = os.str();
    return true;
  }
  val = "0.0";
  return false;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataRealValue::print( std::ostream &ostr ) const{
  if( isValid() ){
    std::cout << m_Value;
  }
  else{
    std::cout << "<invalid real>";
  }
}

/* --------------------------------------------------------------------------- */
/* createEmptyElement --                                                       */
/* --------------------------------------------------------------------------- */

DataElement *DataRealValue::createEmptyElement(){
  return new DataRealElement();
}
