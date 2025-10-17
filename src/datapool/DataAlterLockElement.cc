
#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterLockElement.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterLockElement::DataAlterLockElement( bool lock )
  : m_lock( lock ){
}

DataAlterLockElement::~DataAlterLockElement(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterLockElement::alterItem( DataItem &item
                                    , UpdateStatus &status
                                    , DataInx &inx
                                    , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_FATAL("indexed range does not exist." );
    assert( !m_lock );
    return false; // abbrechen
  }

  BUG_EXIT( "continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterLockElement::UpdateStatus DataAlterLockElement::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  if( m_lock ){
    return el.setLocked();
  }
  else{
    return el.setUnlocked();
  }
}
