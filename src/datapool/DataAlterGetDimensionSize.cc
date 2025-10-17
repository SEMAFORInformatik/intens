
#include "utils/utils.h"
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterGetDimensionSize.h"
#include "datapool/DataReference.h"
#include "datapool/DataDimension.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetDimensionSize::DataAlterGetDimensionSize( DataContainer::SizeList &dimsize )
  : m_dimsize( dimsize )
  , m_num_dimensions( 0 ){
  m_dimsize.clear();
}

DataAlterGetDimensionSize::~DataAlterGetDimensionSize(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetDimensionSize::check( DataReference &ref ){
  if( ref.hasWildcardsNotLastLevel() ){
    assert( false ); // nicht möglich
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetDimensionSize::alterItem( DataItem &item
                                         , UpdateStatus &status
                                         , DataInx &inx
                                         , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( inx.isLastLevel() ){
    assert( m_dimsize.empty() );
    m_num_dimensions = maximum( item.getNumberOfDimensions(), inx.remainingIndicesInIndexList() );
    m_dimsize.resize( m_num_dimensions, 0 );
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

DataAlterGetDimensionSize::UpdateStatus DataAlterGetDimensionSize::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  // Dieser Aufruf darf nie geschehen, sonst ist etwas oberfaul.
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* startDimension --                                                           */
/* --------------------------------------------------------------------------- */

bool DataAlterGetDimensionSize::startDimension( const DataDimension &dim, DataInx &inx ){
  if( !inx.isLastLevel() ){
    return true;
  }

  if( !inx.isWildcard() || m_num_dimensions > 0 ){
    if( inx.remainingIndicesInIndexList() < dim.getNumberOfDimensions() ){
      return true;
    }
  }

  int x = dim.getNumberOfDimensions() -1;
  if( m_num_dimensions == 0 ){
    while( (int)m_dimsize.size() <= x ){
      m_dimsize.push_back( 0 );
    }
  }

  assert( x < (int)m_dimsize.size() );

  int sz = 0;
  if( inx.isWildcard() ){
    sz = dim.getValidDimensionSize();
  }
  else
  if( x == 0 ){
    sz = dim.isValid( inx.getIndex() ) ? 1 : 0;
  }
  m_dimsize.at( x ) = maximum( m_dimsize.at( x ), sz );
  return x > 0;
}

/* --------------------------------------------------------------------------- */
/* startDimension --                                                           */
/* --------------------------------------------------------------------------- */

void DataAlterGetDimensionSize::epilog( bool result ){
  if( m_dimsize.empty() ){
    m_dimsize.push_back( 0 );
    return;
  }
  if( m_dimsize[0] > 0 ){
    for( int x = 1; x < (int)m_dimsize.size(); ++x ){
      m_dimsize.at( x ) = maximum( m_dimsize.at( x ), 1 );
    }
  }
}
