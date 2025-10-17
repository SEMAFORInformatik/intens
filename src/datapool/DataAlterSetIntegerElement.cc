
#include "datapool/DataElement.h"
#include "datapool/DataAlterSetIntegerElement.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetIntegerElement::DataAlterSetIntegerElement()
  : DataAlterSetElement( false )
  , m_value( 0 ){
  BUG_DEBUG( "Constructor @" << this );
}

DataAlterSetIntegerElement::DataAlterSetIntegerElement( int value )
  : DataAlterSetElement( true )
  , m_value( value ){
  BUG_DEBUG( "Constructor @" << this << ": Value " << value );
}

DataAlterSetIntegerElement::~DataAlterSetIntegerElement(){
  BUG_DEBUG( "Destructor @" << this );
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterSetIntegerElement::UpdateStatus DataAlterSetIntegerElement::setValue( DataElement &el ){
  BUG_DEBUG( "alterData" );

  const DataUserAttr * attr = dictionary().getDataUserAttr();
  int rundung = attr != 0 ? attr->getRundung() : -1;

  BUG_DEBUG( "Set Integer Value " << m_value );
  if( el.setValue( m_value, rundung ) ){
    return ValueUpdated;
  }
  else{
    BUG_DEBUG( "No update" );
    return NoUpdate;
  }
}
