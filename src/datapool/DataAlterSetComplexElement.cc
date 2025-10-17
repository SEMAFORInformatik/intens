
#include "datapool/DataElement.h"
#include "datapool/DataAlterSetComplexElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetComplexElement::DataAlterSetComplexElement()
  : DataAlterSetElement( false )
  , m_value( 0 ){
}

DataAlterSetComplexElement::DataAlterSetComplexElement( dComplex value )
  : DataAlterSetElement( true )
  , m_value( value ){
}

DataAlterSetComplexElement::~DataAlterSetComplexElement(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterSetComplexElement::UpdateStatus DataAlterSetComplexElement::setValue( DataElement &el ){
  BUG( BugRef, "alterData" );

  const DataUserAttr * attr = dictionary().getDataUserAttr();
  int rundung = attr != 0 ? attr->getRundung() : -1;

  if( el.setValue( m_value, rundung ) ){
    return ValueUpdated;
  }
  else{
    return NoUpdate;
  }
}
