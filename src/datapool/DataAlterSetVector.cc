
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataVector.h"
#include "datapool/DataAlterSetVector.h"
#include "datapool/DataReference.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetVector::DataAlterSetVector( const DataVector &v )
  : m_vector( v ){
  m_iter = m_vector.begin();

}

DataAlterSetVector::~DataAlterSetVector(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterSetVector::check( DataReference &ref ){
  if( ref.hasWildcardsNotLastLevel() ){
    return true; // nicht möglich
  }
  if( ref.numberOfWildcards() != 1 ){
    return true; // nicht möglich
  }
  if( m_iter == m_vector.end() ){
    return true; // keine Daten
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterSetVector::alterItem( DataItem &item
                                  , UpdateStatus &status
                                  , DataInx &inx
                                  , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_EXIT("indexed range does not exist." );
    return true; // neue Elemente müssen angelgt werden. Es geht weiter
  }

  BUG_EXIT( "alterItem: continue" );
  return m_iter != m_vector.end();
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterSetVector::UpdateStatus DataAlterSetVector::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  UpdateStatus status = NoUpdate;

  if( m_iter == m_vector.end() ){
    status = el.clearElement();
    BUG_EXIT("Element cleared");
  }
  else{
    status = el.setDataValue( (*m_iter).get_ptr(true) );
    m_iter++;
    BUG_EXIT("Element updated");
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* createWildcardElements --                                                   */
/* --------------------------------------------------------------------------- */

bool DataAlterSetVector::createWildcardElements( int wildcard_num ){
  // Es darf in jedem Fall nur genau ein Wildcardindex vorhanden sein
  // (siehe auch Funktion check() ).
  assert( wildcard_num == 1 );
  // Solange noch Werte im Vektor vorhanden sind, muss erweitert werden.
  return m_iter != m_vector.end();
}
