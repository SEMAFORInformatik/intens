
#include "utils/utils.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataReference.h"
#include "datapool/DataAlterGetElementUpdated.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetElementUpdated::DataAlterGetElementUpdated( TransactionNumber t )
  : m_element_data_timestamp( 0 )
  , m_element_value_timestamp( 0 )
  , m_element_full_timestamp( 0 )
  , m_element_gui_timestamp( 0 )
  , m_element_back_timestamp( 0 )
  , m_transactionnumber( t ){
}

DataAlterGetElementUpdated::~DataAlterGetElementUpdated(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElementUpdated::check( DataReference &ref ){
  if( ref.hasWildcardsNotLastLevel() ){
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElementUpdated::alterItem( DataItem &item
                                          , UpdateStatus &status
                                          , DataInx &inx
                                          , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  // Der Full-Timestamp ist nur wichtig, wenn auf Element-Ebene keine Daten verfügbar sind.
  m_element_full_timestamp = maximum( m_element_full_timestamp, item.getDataItemFullUpdated() );

  // Der Rollback-Timestamp ist nur für das Erkennen eines nötigen GUI-Updates zu verwenden.
  m_element_back_timestamp = maximum( m_element_back_timestamp, item.getLastRollbackTimestamp() );

  // Der GUI-Timestamp ist nur auf dem letzten verfügbaren Level relevant, da Updates
  // in einem Seitenast nicht beachtet werden dürfen! Der Timestamp wird daher immer wieder
  // überschrieben.
  m_element_gui_timestamp  = item.getDataItemGuiUpdated();

  if( final ){
    BUG_FATAL("indexed range does not exist." );
    return false;
  }

  if( inx.isLastLevel() ){
    if( inx.isIndexListEmpty() ){
      // eval timestamp only
      m_element_data_timestamp  = item.getDataItemDataUpdated();
      m_element_value_timestamp = item.getDataItemValueUpdated();
      return false;
    }
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetElementUpdated::UpdateStatus DataAlterGetElementUpdated::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  // Das Element ist vorhanden. Die Timestamps sind eindeutig.
  m_element_data_timestamp  = el.getDataContainerDataUpdated();
  m_element_value_timestamp = el.getDataContainerValueUpdated();
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElementUpdated::isUpdated( UpdateStatus status ) const{
  return getUpdated( status ) >= m_transactionnumber;
}

/* --------------------------------------------------------------------------- */
/* getUpdated --                                                               */
/* --------------------------------------------------------------------------- */

TransactionNumber DataAlterGetElementUpdated::getUpdated( UpdateStatus status ) const{
  switch( status ){
  case NoUpdate:
    return 0;
  case DataUpdated:
    return maximum( m_element_full_timestamp, m_element_data_timestamp );
  case ValueUpdated:
    return maximum( m_element_full_timestamp, m_element_value_timestamp );
  case GuiUpdated:
    return maximum( m_element_gui_timestamp , m_element_data_timestamp, m_element_back_timestamp );
  default:
    return 0;
  }
}
