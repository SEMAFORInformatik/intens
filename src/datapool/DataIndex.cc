
#include <assert.h>

#include "utils/utils.h"
#include "datapool/DataIndex.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataIndex);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataIndex::DataIndex()
  : m_index( 0 )
  , m_lowerbound( 0 )
  , m_upperbound( 0 ){
  BUG_INCR_COUNT;
}

DataIndex::DataIndex( int index )
  : m_index( index )
  , m_lowerbound( 0 )
  , m_upperbound( 0 ){
  BUG_INCR_COUNT;
}

DataIndex::DataIndex( const DataIndex &index )
  : m_index( index.m_index )
  , m_lowerbound( index.m_lowerbound )
  , m_upperbound( index.m_upperbound ){
  BUG_INCR_COUNT;
}

DataIndex::~DataIndex(){
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setLowerbound --                                                            */
/* --------------------------------------------------------------------------- */

void DataIndex::setLowerbound( int low ){
  assert( low >= 0 );
  m_lowerbound = low;
}

/* --------------------------------------------------------------------------- */
/* getLowerbound --                                                            */
/* --------------------------------------------------------------------------- */

int DataIndex::getLowerbound() const{
  return maximum( m_lowerbound, m_index );
}

/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

void DataIndex::setUpperbound( int up ){
  assert( up >= 0 );
  m_upperbound = up;
}

/* --------------------------------------------------------------------------- */
/* getUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

int DataIndex::getUpperbound() const{
  return maximum( getLowerbound(), m_upperbound );
}

/* --------------------------------------------------------------------------- */
/* setWildcard --                                                              */
/* --------------------------------------------------------------------------- */

void DataIndex::setWildcard(){
  m_index = -1;
}

/* --------------------------------------------------------------------------- */
/* isWildcard --                                                               */
/* --------------------------------------------------------------------------- */

bool DataIndex::isWildcard() const{
  return m_index == -1;
}

/* --------------------------------------------------------------------------- */
/* isZero --                                                                   */
/* --------------------------------------------------------------------------- */

bool DataIndex::isZero() const{
  if( isWildcard() ){
    return getLowerbound() == 0;
  }
  return m_index == 0;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void DataIndex::setIndex( int inx ){
  assert( inx >= 0 );
  m_index = inx;
}

/* --------------------------------------------------------------------------- */
/* incrementIndex --                                                           */
/* --------------------------------------------------------------------------- */

void DataIndex::incrementIndex(){
  m_index++;
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */

int DataIndex::getIndex() const{
  return maximum( m_index, 0 );
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataIndex::print( std::ostream & ostr, Separator &sep ) const{
  ostr << sep;
  if( isWildcard() ){
    if( m_lowerbound > 0 || m_upperbound > 0 ){
      ostr << maximum( 0, m_lowerbound )  << "-"
	   << maximum( m_lowerbound, m_upperbound ) << ":";
    }
    ostr << "*";
  }
  else{
    ostr << m_index;
  }
}

/* --------------------------------------------------------------------------- */
/* operator==  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataIndex::operator==( const DataIndex &inx ) const{
  return m_index      == inx.m_index      &&
         m_lowerbound == inx.m_lowerbound &&
         m_upperbound == inx.m_upperbound;
}

/* --------------------------------------------------------------------------- */
/* operator==  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataIndex::operator!=( const DataIndex &inx ) const{
  return !( *this == inx );
}
