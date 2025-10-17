
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataMatrix.h"
#include "datapool/DataAlterSetMatrix.h"
#include "datapool/DataReference.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetMatrix::DataAlterSetMatrix( const DataMatrix &m )
  : m_matrix( m ){
  m_miter = m_matrix.begin();
}

DataAlterSetMatrix::~DataAlterSetMatrix(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterSetMatrix::check( DataReference &ref ){
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

bool DataAlterSetMatrix::alterItem( DataItem &item
                                  , UpdateStatus &status
                                  , DataInx &inx
                                  , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_EXIT("indexed range does not exist." );
    return true; // create new elements ..
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterSetMatrix::UpdateStatus DataAlterSetMatrix::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  BUG_EXIT("Element is available");

  assert( m_vector != 0 );
  if( m_viter != (*m_vector).end() ){
    UpdateStatus status = el.setDataValue( (*m_viter).get_ptr(true) );
    m_viter++;
    return status;
  }
  return el.clearElement();
}

/* --------------------------------------------------------------------------- */
/* createWildcardElements --                                                   */
/* --------------------------------------------------------------------------- */

bool DataAlterSetMatrix::createWildcardElements( int wildcard_num ){
  // Es darf in jedem Fall nur genau ein Wildcardindex vorhanden sein
  // (siehe auch Funktion check() ).

  switch( wildcard_num ){
  case 1:
    return m_miter != m_matrix.end();
    break;
  case 2:
    assert( m_vector != 0 );
    return m_viter != (*m_vector).end();
  default:
    assert( false );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* startWithWildcard --                                                        */
/* --------------------------------------------------------------------------- */

void DataAlterSetMatrix::startWithWildcard( int wildcard_num, int max_elements ){
  if( wildcard_num == 2 ){
    if( m_miter == m_matrix.end() ){
      m_viter = (*m_vector).end();
    }
    else{
      m_vector = (*m_miter);
      m_viter = (*m_vector).begin();
      m_miter++;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* endWithWildcard --                                                          */
/* --------------------------------------------------------------------------- */

void DataAlterSetMatrix::endWithWildcard( int wildcard_num ){
}
