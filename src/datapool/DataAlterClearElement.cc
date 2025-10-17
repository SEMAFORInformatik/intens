
#include "datapool/DataElement.h"
#include "datapool/DataAlterClearElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterClearElement::DataAlterClearElement()
  : DataAlterSetElement( false ){
}

DataAlterClearElement::~DataAlterClearElement(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterClearElement::UpdateStatus DataAlterClearElement::setValue( DataElement &el ){
  BUG( BugRef, "alterData" );

  // Eigentlich sollten wir hier nicht vorbei kommen!!
  return el.clearElement();
}
