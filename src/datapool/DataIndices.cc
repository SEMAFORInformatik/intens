
#include <iostream>

#include "utils/utils.h"
#include "datapool/DataIndices.h"
#include "datapool/DataPool.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataIndices);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataIndices::DataIndices(){
  BUG_INCR_COUNT;
}

DataIndices::DataIndices( const DataIndices &indices )
  : m_indexlists( indices.m_indexlists ){
  BUG_INCR_COUNT;
}

DataIndices::DataIndices( int levels, const int *inx ){
  BUG_INCR_COUNT;
  convert( *this, levels, inx );
}

DataIndices::~DataIndices(){
  m_indexlists.clear();
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* addIndexList --                                                             */
/* --------------------------------------------------------------------------- */

DataIndexList &DataIndices::addIndexList( int item_idnr ){
  BUG_PARA( BugRef, "addIndexList", "idnr=" << item_idnr );

  int level = 0;
  if( !m_indexlists.empty() ){
    IndexLists::reverse_iterator rit = m_indexlists.rbegin();
    BUG_MSG( "Last Level is " << rit->first );
    level = rit->first + 1;
  }

  m_indexlists.insert( IndexLists::value_type( level, DataIndexList( item_idnr ) ) );
  IndexLists::iterator it =  m_indexlists.find( level );
  assert( it != m_indexlists.end() );
  BUG_EXIT( "indexlist for level " << it->first << " inserted" );
  return it->second;
}

/* --------------------------------------------------------------------------- */
/* getIndexList --                                                             */
/* --------------------------------------------------------------------------- */

DataIndexList *DataIndices::getIndexList( int level ){
  IndexLists::iterator it = m_indexlists.find( level );
  if( it == m_indexlists.end() ){
    return 0;
  }
  return &(it->second);
}

/* --------------------------------------------------------------------------- */
/* indexList --                                                                */
/* --------------------------------------------------------------------------- */

DataIndexList &DataIndices::indexList( int level ){
  BUG_PARA( BugRef, "indexList", "level=" << level );
  assert( !m_indexlists.empty() );

  if( level < 0 ){
    return m_indexlists.rbegin()->second;
  }
  DataIndexList *inxlist = getIndexList( level );

  if( inxlist == 0 ){
    BUG_FATAL( "FATAL: " << print() );
    assert( false );
  }
  return *inxlist;
}

/* --------------------------------------------------------------------------- */
/* indexList --                                                                */
/* --------------------------------------------------------------------------- */

const DataIndexList &DataIndices::indexList( int level ) const{
  assert( !m_indexlists.empty() );

  if( level < 0 ){
    return m_indexlists.rbegin()->second;
  }
  IndexLists::const_iterator it = m_indexlists.find( level );
  assert( it != m_indexlists.end() );
  return it->second;
}

/* --------------------------------------------------------------------------- */
/* saveIndexList --                                                            */
/* --------------------------------------------------------------------------- */

const DataIndexList *DataIndices::saveIndexList( int level ) const{
  return new DataIndexList( indexList( level ) );
}

/* --------------------------------------------------------------------------- */
/* restoreIndexList --                                                         */
/* --------------------------------------------------------------------------- */

void DataIndices::restoreIndexList( const DataIndexList &inxlist, int level ){
  DataIndexList &mylist = indexList( level );
  assert( mylist.getItemIdnr() == inxlist.getItemIdnr() );
  mylist = inxlist;
}

/* --------------------------------------------------------------------------- */
/* addIndex --                                                                 */
/* --------------------------------------------------------------------------- */

DataIndex &DataIndices::addIndex( int inx ){
  DataIndex &index = indexList().newIndex();
  index.setIndex( inx );
  return index;
}

/* --------------------------------------------------------------------------- */
/* addWildcard --                                                              */
/* --------------------------------------------------------------------------- */

DataIndex &DataIndices::addWildcard(){
  DataIndex &index = indexList().newIndex();
  index.setWildcard();
  return index;
}

/* --------------------------------------------------------------------------- */
/* clearIndices --                                                             */
/* --------------------------------------------------------------------------- */

void DataIndices::clearIndices(){
  indexList().clear();
}

/* --------------------------------------------------------------------------- */
/* hasWildcards --                                                             */
/* --------------------------------------------------------------------------- */

bool DataIndices::hasWildcards() const{
  for( IndexLists::const_iterator it = m_indexlists.begin(); it != m_indexlists.end(); ++it ){
    if( it->second.hasWildcards() ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* numberOfWildcards --                                                        */
/* --------------------------------------------------------------------------- */

int DataIndices::numberOfWildcards() const{
  int num = 0;
  for( IndexLists::const_iterator it = m_indexlists.begin(); it != m_indexlists.end(); ++it ){
    num += it->second.numberOfWildcards();
  }
  return num;
}

/* --------------------------------------------------------------------------- */
/* hasWildcardsNotLastLevel --                                                 */
/* --------------------------------------------------------------------------- */

bool DataIndices::hasWildcardsNotLastLevel() const{
  if( numberOfLevels() < 2 ){
    return false;
  }
  IndexLists::const_reverse_iterator rit = m_indexlists.rbegin();

  for( ++rit; rit != m_indexlists.rend(); ++rit ){
    if( rit->second.hasWildcards() ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* numberOfLevels --                                                           */
/* --------------------------------------------------------------------------- */

int DataIndices::numberOfLevels() const{
  return getLastLevel() + 1;
}

/* --------------------------------------------------------------------------- */
/* getLastLevel --                                                             */
/* --------------------------------------------------------------------------- */

int DataIndices::getLastLevel() const{
  assert( !m_indexlists.empty() );
  return m_indexlists.rbegin()->first;
}

/* --------------------------------------------------------------------------- */
/* isLastLevel --                                                              */
/* --------------------------------------------------------------------------- */

bool DataIndices::isLastLevel( int level ) const{
  BUG_PARA( BugRef, "isLastLevel", "current level=" << level );
  if( m_indexlists.empty() ){
    return false;
  }
  BUG_EXIT( "last level is " <<  m_indexlists.rbegin()->first );
  return m_indexlists.rbegin()->first == level;
}

/* --------------------------------------------------------------------------- */
/* deleteLastLevel --                                                          */
/* --------------------------------------------------------------------------- */

void DataIndices::deleteLastLevel(){
  assert( m_indexlists.size() > 1 );

  IndexLists::reverse_iterator rit = m_indexlists.rbegin();
  assert( rit != m_indexlists.rend() );
  int level = rit->first;
  IndexLists::iterator it = m_indexlists.find( level );
  m_indexlists.erase( it );
}

/* --------------------------------------------------------------------------- */
/* remainingLevels --                                                          */
/* --------------------------------------------------------------------------- */

int DataIndices::remainingLevels( int level ) const{
  if( m_indexlists.empty() ){
    return 0;
  }
  return maximum( 0, getLastLevel() - level );
}

/* --------------------------------------------------------------------------- */
/* convert --                                                                  */
/* --------------------------------------------------------------------------- */

DataIndices *DataIndices::convert( int levels, const int *inx ){
  BUG_PARA( BugRef, "convert", "levels=" << levels );

  DataIndices *indices = new DataIndices();
  convert( *indices, levels, inx );
  return indices;
}

/* --------------------------------------------------------------------------- */
/* convert --                                                                  */
/* --------------------------------------------------------------------------- */

void DataIndices::convert( DataIndices &indices, int levels, const int *inx ){
  BUG_PARA( BugRef, "convert", "levels=" << levels );

  for( int level = 0; level < levels; level++ ){
    DataIndexList &inxlist = indices.addIndexList( inx[0] );
    int numinx    = inx[1];
    inx = inx + 2;
    for( int n = 0; n < numinx; n++ ){
      if( inx[n] >= 0 ){
	inxlist.newIndex( inx[n] );
      }
      else{
	inxlist.newWildcard();
      }
    }
    inx += numinx;
  }
}

/* --------------------------------------------------------------------------- */
/* getDictionary --                                                            */
/* --------------------------------------------------------------------------- */

DataDictionary *DataIndices::getDictionary( DataPool &datapool ) const{
  BUG( BugRef, "DataIndices::getDictionary" );
  DataDictionary *dict = datapool.GetRootDictVar();
  assert( dict != 0 );
  assert( !m_indexlists.empty() );

  // Diese Funktion kontrolliert gnadenlos. Wenn hier Unregelmässigkeiten vorkommen,
  // ist die Indexliste in einem katastrophalen Zustand. (26.9.2015/hob)

  IndexLists::const_iterator it = m_indexlists.begin();
  dict = dict->SearchByIndex( it->second.getItemIdnr() );
  assert( dict != 0 );
  BUG_MSG("Name=" << dict->getName() << ", Type=" << dict->getDataType() );

  for( ++it; it != m_indexlists.end(); ++it ){
    dict = dict->getStructure()->getStructure();
    assert( dict != 0 );
    dict = dict->SearchByIndex( it->second.getItemIdnr() );
    assert( dict != 0 );
    BUG_MSG("Name=" << dict->getName() << ", Type=" << dict->getDataType() << ", Idnr=" <<  it->second.getItemIdnr() );
  }
  return dict;
}

/* --------------------------------------------------------------------------- */
/* getUserDataAttributes --                                                    */
/* --------------------------------------------------------------------------- */

DataAttributes DataIndices::getUserDataAttributes( DataPool &datapool ) const{
  DataDictionary *dict = datapool.GetRootDictVar();
  assert( dict != 0 );
  assert( !m_indexlists.empty() );

  // Diese Funktion kontrolliert gnadenlos. Wenn hier Unregelmässigkeiten vorkommen,
  // ist die Indexliste in einem katastrophalen Zustand. (24.8.2017/hob)

  IndexLists::const_iterator it = m_indexlists.begin();
  dict = dict->SearchByIndex( it->second.getItemIdnr() );
  assert( dict != 0 );

  const DataUserAttr *user_attr = dict->getDataUserAttr();
  assert( user_attr != 0 );
  DataAttributes data_attr( user_attr->getAttributes() );

  for( ++it; it != m_indexlists.end(); ++it ){
    dict = dict->getStructure()->getStructure();
    assert( dict != 0 );
    dict = dict->SearchByIndex( it->second.getItemIdnr() );
    assert( dict != 0 );

    user_attr = dict->getDataUserAttr();
    assert( user_attr != 0 );
    data_attr.setAttribute( user_attr->getAttributes() );
  }
  return data_attr;
}

/* --------------------------------------------------------------------------- */
/* getBaseItemIdnr --                                                          */
/* --------------------------------------------------------------------------- */

int DataIndices::getBaseItemIdnr() const{
  assert( !m_indexlists.empty() );
  return m_indexlists.begin()->second.getItemIdnr();
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataIndices::print( std::ostream &ostr, const DataDictionary *dict, bool withIndices ) const{
  if( m_indexlists.empty() ){
    ostr << "<no item address>";
    return;
  }

  Separator sep( '.', '.', 0 );
  IndexLists::const_iterator it = m_indexlists.begin();
  if( dict != 0 ){
    dict = dict->SearchByIndex( it->second.getItemIdnr() );
  }
  it->second.printItem( ostr, dict, it->first, withIndices );

  for( ++it; it != m_indexlists.end(); ++it ){
    ostr << sep;
    if( dict != 0 ){
      dict = dict->getStructure()->getStructure();
      dict = dict->SearchByIndex( it->second.getItemIdnr() );
    }
    it->second.printItem( ostr, dict, it->first, withIndices );
  }
  ostr << sep.close();
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

const std::string DataIndices::print( const DataDictionary *dict, bool withIndices ) const{
  std::ostringstream ostr;
  print( ostr, dict, withIndices );
  return ostr.str();
}

/* --------------------------------------------------------------------------- */
/* operator==  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataIndices::operator==( const DataIndices &ind ) const{
  if( numberOfLevels() != ind.numberOfLevels() ){
    return false;
  }

  IndexLists::const_iterator it2 = ind.m_indexlists.begin();
  for( IndexLists::const_iterator it = m_indexlists.begin(); it != m_indexlists.end(); ++it ){
    if( ( it->first != it2->first ) ){
      return false;
    }
    if( ( it->second != it2->second ) ){
      return false;
    }
    it2++;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* operator==  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataIndices::operator!=( const DataIndices &ind ) const{
  return !( *this == ind );
}
