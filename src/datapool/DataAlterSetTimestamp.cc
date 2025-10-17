#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataAlterSetTimestamp.h"
#include "datapool/DataInx.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetTimestamp::DataAlterSetTimestamp( TransactionNumber t
                                            , UpdateStatus status
                                            , bool update_items )
  : m_timestamp( t )
  , m_status( status )
  , m_wildcards( false )
  , m_update_items( update_items ){
}

DataAlterSetTimestamp::~DataAlterSetTimestamp(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterSetTimestamp::alterItem( DataItem &item
                                     , UpdateStatus &status
                                     , DataInx &inx
                                     , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;
  m_wildcards = false;

  if( inx.isLastLevel() ){
    if( inx.isIndexListEmpty() ){
      // Ohne Angabe von Indizes auf dem letzten Level werden alle Elemente mit
      // einem Timestamp versehen.
      m_wildcards = true;

      if( !m_update_items ){
        // Die Items werden nicht automatisch markiert. Wir nehmen das selber
        // in die Hand.
        setItemTimestamp( item );
      }
    }
    if( !m_update_items ){
      // Der Rollback-Timestamp wird nur für ein Gui-Update abgefragt.
      // Wir 'missbrauchen' den Timestamp, damit sicher ein Update stattfindet.
      item.markDataItemRollback();
    }
  }

  if( final ){
    BUG_EXIT( "indexed range does not exist." );
    return false; // abbrechen
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterSetTimestamp::UpdateStatus DataAlterSetTimestamp::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );
  TransactionNumber ts = 0;

  switch( m_status ){
  case DataUpdated:
    // ACHTUNG: Der DataTimestamp kann nicht kleiner als der ValueTimestamp sein.
    ts = el.getDataContainerValueUpdated();
    if( m_timestamp < ts ){
      // In diesem Fall wird der ValueTimestamp ebenfalls mitgezogen.
      el.setDataContainerValueUpdated( m_timestamp );
    }
    else{
      el.setDataContainerDataUpdated( m_timestamp );
    }
    break;

  case ValueUpdated:
    // ACHTUNG: Der DataTimestamp kann nicht kleiner als der ValueTimestamp sein.
    ts = el.getDataContainerDataUpdated();
    el.setDataContainerValueUpdated( m_timestamp );
    if( m_timestamp < ts ){
      // Der DataTimestamp wird wieder auf den alten Stand gebracht.
      el.setDataContainerDataUpdated( ts );
    }
    break;

  default:
    break;
  }

  BUG_EXIT( "Momentaner Timestamp ist " << m_timestamp );
  // Das Item wird durch alterData() immer mit dem neusten Timestamp versehen.
  return m_status; // no update
}

/* --------------------------------------------------------------------------- */
/* setItemTimestamp --                                                         */
/* --------------------------------------------------------------------------- */

void DataAlterSetTimestamp::setItemTimestamp( DataItem &item ){
  TransactionNumber ts = 0;

  switch( m_status ){
  case DataUpdated:
    // ACHTUNG: Der DataTimestamp kann nicht kleiner als der ValueTimestamp sein.
    ts = item.getDataItemValueUpdated();
    if( m_timestamp < ts ){
      // In diesem Fall wird der ValueTimestamp und der FullTimestamp ebenfalls
      // mitgezogen.
      item.setDataItemValueUpdated( m_timestamp );
      if( item.getDataItemFullUpdated() > m_timestamp ){
	item.setDataItemFullUpdated( m_timestamp );
      }
    }
    else{
      item.setDataItemDataUpdated( m_timestamp );
    }
    break;

  case ValueUpdated:
    // ACHTUNG: Der DataTimestamp kann nicht kleiner als der ValueTimestamp sein.
    ts = item.getDataItemDataUpdated();
    item.setDataItemValueUpdated( m_timestamp );
    if( item.getDataItemFullUpdated() > m_timestamp ){
      // Der FullTimestamp wird ebenfalls mitgezogen.
      item.setDataItemFullUpdated( m_timestamp );
    }
    if( m_timestamp < ts ){
      // Der DataTimestamp wird wieder auf den alten Stand gebracht.
      item.setDataItemDataUpdated( ts );
    }
    break;

  default:
    break;
  }
}
