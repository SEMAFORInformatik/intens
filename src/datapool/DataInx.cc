
#include <assert.h>
#include <sstream>

#include "utils/Separator.h"
#include "datapool/DataInx.h"
#include "datapool/DataIndex.h"
#include "datapool/DataIndexList.h"
#include "datapool/DataIndices.h"

#define CERR_WRITE(X) std::cerr << X << std::endl;

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataInx);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataInx::DataInx( DataIndices &indices, int level )
  : m_indices( indices )
  , m_level( level )
  , m_indexlist( indices.getIndexList( level ) )
  , m_index( 0 )
  , m_use_wildcard( false )
  , m_wildcard_nr( 0 )
  , m_wildcard_index( 0 )
  , m_freeze_last_level( false ){
  BUG_INCR_COUNT;

  assert( m_level >= 0 );
  if( m_indexlist == 0 ){
    std::cout << "Constructor of DataInx on Level " << level << " failed: "
              << m_indices.print( 0, true ) << std::endl;
    assert( false );
  }

  firstIndex();
}

DataInx::DataInx( const DataInx &inx )
  : m_indices( inx.m_indices )
  , m_level( inx.m_level )
  , m_indexlist( inx.m_indexlist )
  , m_index( inx.m_index )
  , m_use_wildcard( inx.m_use_wildcard )
  , m_wildcard_nr( inx.m_wildcard_nr )
  , m_iter( inx.m_iter )
  , m_wildcard_index( inx.m_wildcard_index )
  , m_freeze_last_level( inx.m_freeze_last_level ){
  BUG_INCR_COUNT;

  assert( m_level >= 0 );
  if( m_freeze_last_level ){
    assert( m_indexlist == 0 );
  }
  else{
    assert( m_indexlist != 0 );
  }
}

DataInx::~DataInx(){
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* isIndexListEmpty --                                                         */
/* --------------------------------------------------------------------------- */

bool DataInx::isIndexListEmpty() const{
  if( m_indexlist == 0 ){
    return true;
  }
  return m_indexlist->isEmpty();
}

/* --------------------------------------------------------------------------- */
/* isIndexListValid --                                                         */
/* --------------------------------------------------------------------------- */

bool DataInx::isIndexListValid() const{
  if( m_indexlist == 0 ){
    return false;
  }
  return  m_index != 0;
}

/* --------------------------------------------------------------------------- */
/* isntIndexListValid --                                                       */
/* --------------------------------------------------------------------------- */

bool DataInx::isntIndexListValid() const{
  return !isIndexListValid();
}

/* --------------------------------------------------------------------------- */
/* remainingIndicesInIndexList --                                              */
/* --------------------------------------------------------------------------- */

int DataInx::remainingIndicesInIndexList() const{
  if( isntIndexListValid() ){
    return 0;
  }
  int i = 0;
  DataIndexList::const_iterator it = m_iter;
  for( ; it != m_indexlist->end(); ++it ){
    i++;
  }
  return i;
}

/* --------------------------------------------------------------------------- */
/* positionOfRemainingWildcards --                                             */
/* --------------------------------------------------------------------------- */

void DataInx::positionOfRemainingWildcards( DataContainer::SizeList &wc_pos_list ) const{
  assert( isIndexListValid() );
  wc_pos_list.clear();

  DataIndexList::const_iterator it = m_iter;
  for( ; it != m_indexlist->end(); ++it ){
    wc_pos_list.push_back( (*it).isWildcard() ? 1 : 0 );
  }
}

/* --------------------------------------------------------------------------- */
/* firstIndex --                                                               */
/* --------------------------------------------------------------------------- */

void DataInx::newFrontIndex(){
  assert( m_indexlist != 0 );
  m_indexlist->newIndex( 0, true );
  firstIndex();
}

/* --------------------------------------------------------------------------- */
/* firstIndex --                                                               */
/* --------------------------------------------------------------------------- */

void DataInx::newFrontWildcard(){
  assert( m_indexlist != 0 );
  m_indexlist->newWildcard( true );
  firstIndex();
}

/* --------------------------------------------------------------------------- */
/* firstIndex --                                                               */
/* --------------------------------------------------------------------------- */

bool DataInx::firstIndex(){
  if( m_indexlist == 0 ){
    std::cout << "first() failed: " << m_indices.print( 0, true ) << std::endl;
    assert( false );
  }
  m_iter = m_indexlist->begin();
  return setIndex();
}

/* --------------------------------------------------------------------------- */
/* nextIndex --                                                                */
/* --------------------------------------------------------------------------- */

bool DataInx::nextIndex(){
  if( m_freeze_last_level ){
    return false;
  }
  assert( m_indexlist != 0 );
  if( m_iter != m_indexlist->end() ){
    m_iter++;
  }
  return setIndex();
}

/* --------------------------------------------------------------------------- */
/* cloneIndexList --                                                           */
/* --------------------------------------------------------------------------- */

DataIndexList *DataInx::cloneIndexList() const{
  assert( m_indexlist != 0 );
  return new DataIndexList( *m_indexlist );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataInx::setIndex(){
  assert( m_indexlist != 0 );
  m_use_wildcard = false;
  if( m_iter != m_indexlist->end() ){
    m_index = &(*m_iter);
    if( m_index->isWildcard() ){
      m_wildcard_nr++;
    }
    return true;
  }
  m_index = 0;
  return false;
}

/* --------------------------------------------------------------------------- */
/* firstLevel --                                                               */
/* --------------------------------------------------------------------------- */

bool DataInx::firstLevel(){
  if( m_level != 0 ){
    m_level = 0;
    m_indexlist = m_indices.getIndexList( 0 );
    if( m_indexlist == 0 ){
      CERR_WRITE( "DataInx::firstLevel() failed" );
      return false;
    }
  }
  firstIndex();
  return true;
}

/* --------------------------------------------------------------------------- */
/* nextLevel --                                                                */
/* --------------------------------------------------------------------------- */

bool DataInx::nextLevel(){
  BUG_PARA( BugRef, "nextLevel", "current level=" << m_level );
  m_level++;
  m_indexlist = m_indices.getIndexList( m_level );
  if( m_indexlist == 0 ){
    CERR_WRITE( "DataInx::nextLevel(" << m_level << ") failed" );
    assert( false );
  }
  firstIndex();
  return true;
}

/* --------------------------------------------------------------------------- */
/* lastLevel --                                                                */
/* --------------------------------------------------------------------------- */

bool DataInx::lastLevel(){
  BUG( BugRef, "lastLevel" );
  m_level = m_indices.getLastLevel();
  m_indexlist = m_indices.getIndexList( m_level );
  if( m_indexlist == 0 ){
    CERR_WRITE( "DataInx::lastLevel(" << m_level << ") failed" );
    assert( false );
  }
  firstIndex();
  return true;
}

/* --------------------------------------------------------------------------- */
/* freezeLastLevel --                                                          */
/* --------------------------------------------------------------------------- */

void DataInx::freezeLastLevel(){
  assert( isLastLevel() );
  m_indexlist = 0;
  m_index     = 0;
  m_freeze_last_level = true;
}

/* --------------------------------------------------------------------------- */
/* getLevel --                                                                 */
/* --------------------------------------------------------------------------- */

int DataInx::getLevel() const{
  if( m_indexlist == 0 && !m_freeze_last_level ){
    return -1; // Index fehlt
  }
  return m_level;
}

/* --------------------------------------------------------------------------- */
/* isLastLevel --                                                              */
/* --------------------------------------------------------------------------- */

bool DataInx::isLastLevel() const{
  BUG_PARA( BugRef, "isLastLevel", "current level=" << m_level );
  return m_indices.isLastLevel( m_level );
}

/* --------------------------------------------------------------------------- */
/* isLastLevelFrozen --                                                        */
/* --------------------------------------------------------------------------- */

bool DataInx::isLastLevelFrozen() const{
  BUG_PARA( BugRef, "isLastLevelFrozen", "current level=" << m_level );
  return m_indices.isLastLevel( m_level ) && m_freeze_last_level;
}

/* --------------------------------------------------------------------------- */
/* remainingLevels --                                                          */
/* --------------------------------------------------------------------------- */

int DataInx::remainingLevels() const{
  return m_indices.remainingLevels( m_level );
}

/* --------------------------------------------------------------------------- */
/* getDimensions --                                                            */
/* --------------------------------------------------------------------------- */

int DataInx::getDimensions() const{
  if( m_freeze_last_level ){
    return 0; // Keine Indizes vorhanden
  }
  if( m_indexlist == 0 ){
    return -1;
  }
  return m_indexlist->getDimensions();
}

/* --------------------------------------------------------------------------- */
/* setWildcard --                                                              */
/* --------------------------------------------------------------------------- */

void DataInx::setWildcard(){
  assert( isntIndexListValid() );
  if( !m_use_wildcard ){
    m_use_wildcard = true;
    m_wildcard_nr++;
  }
}

/* --------------------------------------------------------------------------- */
/* isWildcard --                                                               */
/* --------------------------------------------------------------------------- */

bool DataInx::isWildcard() const{
  if( m_use_wildcard ){
    return true;
  }
  if( isIndexListValid() ){
    return m_index->isWildcard();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasWildcards --                                                             */
/* --------------------------------------------------------------------------- */

bool DataInx::hasWildcards() const{
  if( m_indexlist == 0 ){
    return false;
  }
  return m_indexlist->hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* wildcardNumber --                                                           */
/* --------------------------------------------------------------------------- */

int DataInx::wildcardNumber() const{
  return m_wildcard_nr;
}

/* --------------------------------------------------------------------------- */
/* setWildcardIndex --                                                         */
/* --------------------------------------------------------------------------- */

void DataInx::setWildcardIndex( int i ){
  m_wildcard_index = i;
}

/* --------------------------------------------------------------------------- */
/* wildcardIndex --                                                            */
/* --------------------------------------------------------------------------- */

int DataInx::wildcardIndex() const{
  return m_wildcard_index;
}

/* --------------------------------------------------------------------------- */
/* newIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void DataInx::newIndex( int inx ){
  m_indices.indexList( m_level ).newIndex( inx );
  // Wir fangen wieder von vorne an.
  firstIndex();
}

/* --------------------------------------------------------------------------- */
/* hasRemainingWildcards --                                                    */
/* --------------------------------------------------------------------------- */

bool DataInx::hasRemainingWildcards( int anzahl ) const{
  if( m_indexlist == 0 ){
    return false;
  }
  if( isWildcard() ){
    return true;
  }
  DataIndexList::const_iterator it = m_iter;
  for( int x = anzahl; x > 0; x++ ){
    if( it == m_indexlist->end() ){
      return false;
    }
    if( it->isWildcard() ){
      return true;
    }
    ++it;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasntRemainingWildcards --                                                  */
/* --------------------------------------------------------------------------- */

bool DataInx::hasntRemainingWildcards( int anzahl ) const{
  return !hasRemainingWildcards( anzahl );
}

/* --------------------------------------------------------------------------- */
/* remainingWildcards --                                                       */
/* --------------------------------------------------------------------------- */

int DataInx::remainingWildcards() const{
  if( m_indexlist == 0 ){
    return 0;
  }
  int x = 0;
  for( DataIndexList::const_iterator it = m_iter; it != m_indexlist->end(); it++ ){
    if( it->isWildcard() ){
      x++;
    }
  }
  return x;
}

/* --------------------------------------------------------------------------- */
/* getLowerbound --                                                            */
/* --------------------------------------------------------------------------- */

int DataInx::getLowerbound() const{
  if( m_use_wildcard ){
    return 0;
  }
  if( isIndexListValid() ){
    return m_index->getLowerbound();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

int DataInx::getUpperbound() const{
  if( isIndexListValid() ){
    return m_index->getUpperbound();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */

int DataInx::getIndex() const{
  if( isIndexListValid() ){
    return m_index->getIndex();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* isZero --                                                                   */
/* --------------------------------------------------------------------------- */

bool DataInx::isZero() const{
  if( isIndexListValid() ){
    return m_index->isZero();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getItemIdnr --                                                              */
/* --------------------------------------------------------------------------- */

int DataInx::getItemIdnr() const{
  if( m_indexlist == 0 ){
    return -9; // Index fehlt
  }
  return m_indexlist->getItemIdnr();
}

/* --------------------------------------------------------------------------- */
/* printAllIndices --                                                          */
/* --------------------------------------------------------------------------- */

const std::string DataInx::printAllIndices() const{
  std::ostringstream ostr;
  Separator sep('[',',',']');
  DataIndexList::const_iterator it;
  for( it = m_indexlist->begin(); it != m_indexlist->end(); ++it ){
    it->print( ostr, sep );
  }
  ostr << sep.close();
  return ostr.str();
}


/* --------------------------------------------------------------------------- */
/* printRemainingIndices --                                                    */
/* --------------------------------------------------------------------------- */

const std::string DataInx::printRemainingIndices() const{
  //  assert( m_indexlist != 0 );
  if (m_indexlist == 0) {
    return "<no indices>";
  }

  std::ostringstream ostr;
  Separator sep('[',',',']');
  DataIndexList::const_iterator it = m_iter;
  for( ; it != m_indexlist->end(); ++it ){
    it->print( ostr, sep );
  }
  ostr << sep.close();
  return ostr.str();
}

/* --------------------------------------------------------------------------- */
/* printRemainingLevels --                                                     */
/* --------------------------------------------------------------------------- */

const std::string DataInx::printRemainingLevels() const{
  if( m_indexlist == 0 ){
    return "<no addressing>";
  }

  int level = m_level;
  const DataIndexList *indexlist = m_indexlist;
  std::ostringstream ostr;
  Separator sep(0,'.',0);
  while( indexlist != 0 ){
    ostr << sep;
    indexlist->printItem( ostr, 0, level, true );
    indexlist = m_indices.getIndexList( ++level );
  }
  return ostr.str();
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

const std::string DataInx::print() const{
  return m_indices.print();
}
