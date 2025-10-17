
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataAlterGetItem.h"
#include "datapool/DataReference.h"
#include "datapool/DataInx.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetItem::DataAlterGetItem( bool create )
  : m_item( 0 )
  , m_create( create ){
}

DataAlterGetItem::~DataAlterGetItem(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItem::check( DataReference &ref ){
  // Mit Wildcards kann kein Item gesucht werden.
  return ref.hasWildcardsNotLastLevel();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItem::alterItem( DataItem &item
                                , UpdateStatus &status
                                , DataInx &inx
                                , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( inx.isLastLevel() ){
    m_item = &item;
    BUG_EXIT( "Item auf dem letzten Level erreicht" );
    return false; // Ziel erreicht
  }

  if( final ){
    // kcreate an element if requested
    BUG_EXIT("indexed range does not exist." );
    return m_create;
  }


  BUG_EXIT( "continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetItem::UpdateStatus DataAlterGetItem::alterData( DataElement &el ){
  // wird nicht benötigt
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* getItem --                                                                  */
/* --------------------------------------------------------------------------- */

const DataItem *DataAlterGetItem::getItem() const{
  return m_item;
}

/* --------------------------------------------------------------------------- */
/* getItemNoUpdate --                                                          */
/* --------------------------------------------------------------------------- */

DataItem *DataAlterGetItem::getItemNoUpdate() const{
  return m_item;
}
