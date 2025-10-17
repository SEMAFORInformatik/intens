
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataAlterCompare.h"
#include "datapool/DataReference.h"
#include "datapool/DataInx.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterCompare::DataAlterCompare( DataItem &rslt_item, const DataElement *el_left, int next_fall )
  : m_rslt_item( rslt_item )
  , m_count_elements( false )
  , m_valid_elements( 0 )
  , m_fall( next_fall ){
  // init result if first element exist
  if( el_left != 0 ){
    assert( m_fall == 0 );
    m_rslt_item.prepareCompareResult( *el_left );
    m_fall++;
    m_valid_elements = 1;
  }
}

DataAlterCompare::~DataAlterCompare(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterCompare::check( DataReference &ref ){
  // Wildcards only on last level
  return ref.hasWildcardsNotLastLevel();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterCompare::alterItem( DataItem &item
                                , UpdateStatus &status
                                , DataInx &inx
                                , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    // create element if requested
    BUG_EXIT("indexed range does not exist." );
    return false;
  }

  if( !m_count_elements && inx.isLastLevel() ){
    m_count_elements = true;
    DataInx temp_inx( inx );
    item.alterData( *this, temp_inx );
    m_count_elements = false;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterCompare::UpdateStatus DataAlterCompare::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  if(! el.hasValidElements( false ) ){
    return NoUpdate;
  }

  if( m_count_elements ){
    m_valid_elements++;
    return NoUpdate;
  }

  if( m_fall == 0 ){
    m_rslt_item.prepareCompareResult( el );
    m_fall++;
    BUG_EXIT("Left Element");
  }
  else{
    m_rslt_item.compare( el, m_fall, m_valid_elements );
    BUG_EXIT("Element #" << m_fall << " compared");
    m_fall++;
  }

  return NoUpdate;
}
