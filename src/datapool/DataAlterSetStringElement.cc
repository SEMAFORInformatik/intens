
#include "datapool/DataElement.h"
#include "datapool/DataAlterSetStringElement.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetStringElement::DataAlterSetStringElement()
  : DataAlterSetElement( false )
  , m_value( "" ){
  BUG_DEBUG( "Constructor @" << this );
}

DataAlterSetStringElement::DataAlterSetStringElement( const std::string &value )
  : DataAlterSetElement( true )
  , m_value( value ){
  BUG_DEBUG( "Constructor @" << this << ": Value " << value );
}

DataAlterSetStringElement::~DataAlterSetStringElement(){
  BUG_DEBUG( "Destructor @" << this );
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterSetStringElement::UpdateStatus DataAlterSetStringElement::setValue( DataElement &el ){
    BUG_DEBUG( "alterData" );

  const DataUserAttr * attr = dictionary().getDataUserAttr();
  int rundung = attr != 0 ? attr->getRundung() : -1;

  BUG_DEBUG( "Set String Value " << m_value );
  if( el.setValue( m_value, rundung ) ){
    return ValueUpdated;
  }
  else{
    BUG_DEBUG( "No update" );
    return NoUpdate;
  }
}
