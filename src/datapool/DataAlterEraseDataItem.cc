#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterEraseDataItem.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterEraseDataItem::DataAlterEraseDataItem( bool incl_itemattr )
  : m_incl_itemattr( incl_itemattr ){
}

DataAlterEraseDataItem::~DataAlterEraseDataItem(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterEraseDataItem::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    // nothing to delete
    BUG_EXIT("indexed range does not exist." );
    return false; // abbrechen
  }

  if( inx.isLastLevel() ){
    status = item.eraseContainer( m_incl_itemattr );
    return false;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterEraseDataItem::UpdateStatus DataAlterEraseDataItem::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );
  assert( false );
  return NoUpdate;
}
