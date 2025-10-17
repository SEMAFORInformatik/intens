
#include "datapool/DataElement.h"
#include "datapool/DataAlterSetRealElement.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetRealElement::DataAlterSetRealElement()
  : DataAlterSetElement( false )
  , m_value( 0.0 ){
  BUG_DEBUG( "Constructor @" << this );
}

DataAlterSetRealElement::DataAlterSetRealElement( double value )
  : DataAlterSetElement( true )
  , m_value( value ){
  BUG_DEBUG( "Constructor @" << this << ": Value " << value );
}

DataAlterSetRealElement::~DataAlterSetRealElement(){
  BUG_DEBUG( "Destructor @" << this );
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterSetRealElement::UpdateStatus DataAlterSetRealElement::setValue( DataElement &el ){
  BUG_DEBUG( "alterData" );

  const DataUserAttr * attr = dictionary().getDataUserAttr();
  int rundung = attr != 0 ? attr->getRundung() : -1;

  BUG_DEBUG( "Set Real Value " << m_value );
  if( el.setValue( m_value, rundung ) ){
    return ValueUpdated;
  }
  else{
    BUG_DEBUG( "No update" );
    return NoUpdate;
  }
}
