
#include "datapool/DataElement.h"
#include "datapool/DataAlterEraseElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterEraseElement::DataAlterEraseElement()
  : DataAlterSetElement( false ){
}

DataAlterEraseElement::~DataAlterEraseElement(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterEraseElement::UpdateStatus DataAlterEraseElement::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  return el.eraseContainer();
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterEraseElement::UpdateStatus DataAlterEraseElement::setValue( DataElement &el ){
  BUG( BugRef, "alterData" );

  // Eigentlich sollten wir hier nicht vorbei kommen!!
  return el.eraseContainer();
}
