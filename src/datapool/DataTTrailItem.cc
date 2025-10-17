
#include <assert.h>

#include "datapool/DataLogger.h"
#include "datapool/DataTTrailItem.h"
#include "datapool/DataItemContainer.h"
#include "datapool/DataItem.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataTTrailItem);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailItem::DataTTrailItem( DataItemContainer *cont, DataItem *item, int inx )
  : m_container( cont )
  , m_item( item )
  , m_index( inx ){
  BUG_INCR_COUNT;
  assert( cont != 0 );
  assert( item != 0 );
  assert( inx >= 0 );
}

DataTTrailItem::~DataTTrailItem(){
  m_container = 0;
  if( m_item != 0 ){
    DataItem::deleteItem( m_item );
  }
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* isEqual --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailItem::isEqual( DataTTrailElement &el ){
  return m_container->GetItem( m_index ) == el.getDataItem();
}

/* --------------------------------------------------------------------------- */
/* isEqual --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailItem::isEqual( DataItem *item ){
  return m_container->GetItem( m_index ) == item;
}

/* --------------------------------------------------------------------------- */
/* getDataItem --                                                              */
/* --------------------------------------------------------------------------- */

DataItem *DataTTrailItem::getDataItem(){
  return m_container->GetItem( m_index );
}

/* --------------------------------------------------------------------------- */
/* restore --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailItem::restore( DataPool &dp ){
  DataItem *item = m_container->GetItem( m_index );
  assert( item != 0 );

  LOG_DEBUG(item->dictionary().getName() << " (" << m_index << ")" );
  assert( m_item != 0 );
  assert( item != m_item );

  item->restoreItem( *m_item );
  return true;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrailItem::print( std::ostream &ostr ) const{
  ostr << "Item: " << m_container->GetItem( m_index )->getName();
}

/* --------------------------------------------------------------------------- */
/* name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string DataTTrailItem::name() const{
  return m_container->GetItem( m_index )->getName();
}
