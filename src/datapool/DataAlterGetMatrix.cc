
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataMatrix.h"
#include "datapool/DataAlterGetMatrix.h"
#include "datapool/DataReference.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetMatrix::DataAlterGetMatrix( DataMatrix &m )
  : m_matrix( m ){

}

DataAlterGetMatrix::~DataAlterGetMatrix(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetMatrix::check( DataReference &ref ){
  if( ref.hasWildcardsNotLastLevel() ){
    return true; // nicht möglich
  }
  if( ref.numberOfWildcards() != 2 ){
    return true; // nicht möglich
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetMatrix::alterItem( DataItem &item
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

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetMatrix::UpdateStatus DataAlterGetMatrix::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  BUG_EXIT("Element is available");

  DataValue::Ptr data( el.getDataValue() );
  assert( m_vector != 0 );
  m_vector->appendValue( data );
  return NoUpdate; // no update
}

/* --------------------------------------------------------------------------- */
/* startWithWildcard --                                                        */
/* --------------------------------------------------------------------------- */

void DataAlterGetMatrix::startWithWildcard( int wildcard_num, int max_elements ){
  if( wildcard_num == 2 ){
    m_vector = m_matrix.appendVector();
  }
}

/* --------------------------------------------------------------------------- */
/* endWithWildcard --                                                          */
/* --------------------------------------------------------------------------- */

void DataAlterGetMatrix::endWithWildcard( int wildcard_num ){
}
