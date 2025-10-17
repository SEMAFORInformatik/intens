
#include "datapool/DataElement.h"
#include "datapool/DataAlterAssignElement.h"
#include "datapool/DataReference.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterAssignElement::DataAlterAssignElement()
  : DataAlterSetElement( false )
  , m_element( 0 ){
}

DataAlterAssignElement::DataAlterAssignElement( const DataElement *el )
  : DataAlterSetElement( true )
  , m_element( el ){
  assert( el != 0 );
}

DataAlterAssignElement::~DataAlterAssignElement(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignElement::check( DataReference &ref ){
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterAssignElement::UpdateStatus DataAlterAssignElement::setValue( DataElement &el ){
  BUG( BugRef, "alterData" );

  return el.assignDataElement( *m_element );
}
