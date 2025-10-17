
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterAssignDataItem.h"
#include "datapool/DataReference.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterAssignDataItem::DataAlterAssignDataItem( const DataItem *item )
  : m_item( item ){
}

DataAlterAssignDataItem::~DataAlterAssignDataItem(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignDataItem::check( DataReference &ref ){
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignDataItem::alterItem( DataItem &item
                                       , UpdateStatus &status
                                       , DataInx &inx
                                       , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_FATAL("indexed range does not exist." );
    assert( !isValid() );
    return false;
  }

  if( inx.isLastLevel() ){
    status = item.assignDataItem( m_item );
    return false; // work is done
  }

  BUG_EXIT( "continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterAssignDataItem::UpdateStatus DataAlterAssignDataItem::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignDataItem::createElements() const{
  return isValid();
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignDataItem::isValid() const{
  return m_item != 0;
}
