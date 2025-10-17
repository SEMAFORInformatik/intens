
#include "utils/utils.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataReference.h"
#include "datapool/DataAlterGetItemUpdated.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetItemUpdated::DataAlterGetItemUpdated( TransactionNumber t )
  : m_item_data_timestamp( 0 )
  , m_item_value_timestamp( 0 )
  , m_item_full_timestamp( 0 )
  , m_item_gui_timestamp( 0 )
  , m_item_back_timestamp( 0 )
  , m_transactionnumber( t ){
}

DataAlterGetItemUpdated::~DataAlterGetItemUpdated(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItemUpdated::check( DataReference &ref ){
  if( ref.hasWildcardsNotLastLevel() ){
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItemUpdated::alterItem( DataItem &item
                                       , UpdateStatus &status
                                       , DataInx &inx
                                       , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  // Der Full-Timestamp ist nur wichtig, wenn auf Element-Ebene keine Daten verfügbar sind.
  m_item_full_timestamp  = maximum( m_item_full_timestamp , item.getDataItemFullUpdated() );

  // Der Rollback-Timestamp ist nur für das Erkennen eines nötigen GUI-Updates zu verwenden.
  m_item_back_timestamp  = maximum( m_item_back_timestamp , item.getLastRollbackTimestamp() );

  // Der GUI-Timestamp ist nur auf dem letzten verfügbaren Level relevant, da Updates
  // in einem Seitenast nicht beachtet werden dürfen! Der Timestamp wird daher immer wieder
  // überschrieben.
  m_item_gui_timestamp   = item.getDataItemGuiUpdated();

  if( inx.isLastLevel() ){
    // Wir sind auf dem letzten Level angelangt. Die Timestamps von tieferen Levels sind
    // nicht mehr von Bedeutung.
    m_item_data_timestamp  = item.getDataItemDataUpdated();
    m_item_value_timestamp = item.getDataItemValueUpdated();
    return false; // Ende
  }

  if( final ){
    BUG_EXIT("indexed range does not exist." );
    return false;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetItemUpdated::UpdateStatus DataAlterGetItemUpdated::alterData( DataElement &el ){
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItemUpdated::isUpdated( UpdateStatus status ) const{
  return getUpdated( status ) >= m_transactionnumber;
}

/* --------------------------------------------------------------------------- */
/* getUpdated --                                                               */
/* --------------------------------------------------------------------------- */

TransactionNumber DataAlterGetItemUpdated::getUpdated( UpdateStatus status ) const{
  switch( status ){
  case NoUpdate:
    return 0;
  case DataUpdated:
    return maximum( m_item_full_timestamp, m_item_data_timestamp );
  case ValueUpdated:
    return maximum( m_item_full_timestamp, m_item_value_timestamp );
  case GuiUpdated:
    return maximum( m_item_gui_timestamp , m_item_data_timestamp, m_item_back_timestamp );
  default:
    return 0;
  }
}
