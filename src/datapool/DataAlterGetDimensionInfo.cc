
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterGetDimensionInfo.h"
#include "datapool/DataReference.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetDimensionInfo::DataAlterGetDimensionInfo( int dimension, int level )
  : m_size_of_dim( 0 )
  , m_level( level )
  , m_dimension( dimension )
  , m_sizelist( 0 ){
}

DataAlterGetDimensionInfo::DataAlterGetDimensionInfo( std::vector<int> *sizelist, int level )
  : m_size_of_dim( 0 )
  , m_level( -1 )
  , m_dimension( -1 )
  , m_sizelist( sizelist ){
}

DataAlterGetDimensionInfo::~DataAlterGetDimensionInfo(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetDimensionInfo::check( DataReference &ref ){
  if( ref.hasWildcardsNotLastLevel() ){
    assert( false ); // nicht möglich
  }
  if( m_level >= 0 && m_level >= ref.GetNumberOfLevels() ){
    m_level       = -1;
    m_size_of_dim = -1;
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetDimensionInfo::alterItem( DataItem &item
                                         , UpdateStatus &status
                                         , DataInx &inx
                                         , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_EXIT("indexed range does not exist." );
    return false;
  }

  if( inx.isLastLevel() || m_level == inx.getLevel() ){
    if( m_dimension >= 0 ){
      m_size_of_dim = item.getDataDimensionSizeOf( m_dimension );
    }
    else
    if( m_sizelist != 0 ){
      item.getAllDataDimensionSize( *m_sizelist );
    }
    return false;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetDimensionInfo::UpdateStatus DataAlterGetDimensionInfo::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  BUG_FATAL("Element is available");
  // this should not happen
  assert( false );
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* sizeOfDimension --                                                          */
/* --------------------------------------------------------------------------- */

int DataAlterGetDimensionInfo::sizeOfDimension() const{
  return m_size_of_dim;
}
