
#include "utils/utils.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataReference.h"
#include "datapool/DataAlterGetAttributes.h"
#include "app/DataPoolIntens.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetAttributes::DataAlterGetAttributes( TransactionNumber t )
  : m_mask( 0 )
  , m_editable( false )
  , m_element_gui_timestamp( 0 )
  , m_element_back_timestamp( 0 )
  , m_transactionnumber( t ){
}

DataAlterGetAttributes::~DataAlterGetAttributes(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetAttributes::check( DataReference &ref ){
  if( ref.hasWildcards() ){
    return true; // nicht möglich
  }

  DataAttributes attr( ref.getUserDataAttributes() );
  m_editable = attr.isAttributeSet( DATAeditable );

  if( DataPool::isLastClearTransaction( m_transactionnumber ) ){
    // do GUI update
    m_mask = m_mask | DATAisGuiUpdated;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetAttributes::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  DataAttributes attr( item.getAttributes() );
  if( attr.isAttributeSet( DATAeditable ) ){
    m_editable = true;
  }

  m_mask = m_mask | item.getAttributes();

  // Der Rollback-Timestamp ist nur für das Erkennen eines nötigen GUI-Updates zu verwenden.
  m_element_back_timestamp = maximum( m_element_back_timestamp, item.getLastRollbackTimestamp() );

  // Der GUI-Timestamp ist nur auf dem letzten verfügbaren Level relevant, da Updates
  // in einem Seitenast nicht beachtet werden dürfen! Der Timestamp wird daher immer wieder
  // überschrieben.
  m_element_gui_timestamp  = item.getDataItemGuiUpdated();

  if( final ){
    // access not possible
    BUG_FATAL( "indexed range does not exist." );
    return false;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterStructData --                                                          */
/* --------------------------------------------------------------------------- */

void DataAlterGetAttributes::alterStructData( DataStructElement &el ){
  m_mask = m_mask | el.getAttributes();
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetAttributes::UpdateStatus DataAlterGetAttributes::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  m_mask = m_mask | el.getAttributes();

  // Wenn des Element vorhanden ist, interessiert uns der GuiTimestamp des DataItems
  // nicht mehr, ausser das Element wurde noch nie verändert.
  if( el.getDataContainerDataUpdated() > 0 ){
    m_element_gui_timestamp = maximum( el.getDataContainerDataUpdated(), m_element_back_timestamp );
  }
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* epilog --                                                                   */
/* --------------------------------------------------------------------------- */

void DataAlterGetAttributes::epilog( bool result ){
  if( m_editable ){
    m_mask = DataAttributes::setAttrBits( m_mask, DATAeditable );
  }
  else{
    m_mask = DataAttributes::resetAttrBits( m_mask, DATAIsEditable );
    m_mask = DataAttributes::resetAttrBits( m_mask, DATAeditable );
  }
}

/* --------------------------------------------------------------------------- */
/* get --                                                                      */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataAlterGetAttributes::get() const{
  if( m_element_gui_timestamp >= m_transactionnumber ){
    return m_mask | DATAisGuiUpdated;
  }
  return m_mask;
}
