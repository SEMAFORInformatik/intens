
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterSetAttributes.h"
#include "datapool/DataReference.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetAttributes::DataAlterSetAttributes( DATAAttributeMask set_mask
                                              , DATAAttributeMask reset_mask
                                              , bool item_only )
  : m_item_only( item_only )
  , m_set_mask( set_mask )
  , m_reset_mask( reset_mask )
  , m_create( false ){
}

DataAlterSetAttributes::~DataAlterSetAttributes(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterSetAttributes::check( DataReference &ref ){
  if( ref.numberOfWildcards() == 0 ){
    m_create = true;
  }
  // Es können auch für mehrere Elemente dieselben Attribute gesetzt werden.
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterSetAttributes::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    if( !m_create ){
      BUG_DEBUG("indexed range does not exist." );
    }
    else{
      BUG_FATAL("indexed range does not exist." );
      assert( false );
    }
    return false; // abbrechen
  }

  if( inx.isLastLevel() && inx.isIndexListEmpty() ){
    if( item.setAttributes( m_set_mask, m_reset_mask ) ){
      status = DataUpdated;
    }

    if( m_item_only ){
      return false; // Abbruch
    }
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterSetAttributes::UpdateStatus DataAlterSetAttributes::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  if( el.setAttributes( m_set_mask, m_reset_mask ) ){
    return DataUpdated;
  }
  return NoUpdate; // kein update
}
