
#include <assert.h>
#include "utils/Separator.h"
#include "datapool/DataIndexList.h"
#include "datapool/DataDictionary.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataIndexList);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataIndexList::DataIndexList()
  : m_item_idnr( -1 ){
  BUG_INCR_COUNT;
}

DataIndexList::DataIndexList( int item_idnr )
  : m_item_idnr( item_idnr ){
  BUG_INCR_COUNT;
}

DataIndexList::DataIndexList( const DataIndexList &indexlist )
  : m_item_idnr( indexlist.m_item_idnr )
  , std::list<DataIndex>( indexlist ){
  BUG_INCR_COUNT;
}

DataIndexList::~DataIndexList(){
  clear();
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setItemIdnr --                                                              */
/* --------------------------------------------------------------------------- */

void DataIndexList::setItemIdnr( int idnr ){
  m_item_idnr = idnr;
}

/* --------------------------------------------------------------------------- */
/* getItemIdnr --                                                              */
/* --------------------------------------------------------------------------- */

int DataIndexList::getItemIdnr() const{
  return m_item_idnr;
}

/* --------------------------------------------------------------------------- */
/* newIndex --                                                                 */
/* --------------------------------------------------------------------------- */

DataIndex &DataIndexList::newIndex( int inx, bool front ){
  if( front ){
    push_front( DataIndex( inx ) );
    return *begin();
  }
  else{
    push_back( DataIndex( inx ) );
    return *rbegin();
  }
}

/* --------------------------------------------------------------------------- */
/* newWildcard --                                                              */
/* --------------------------------------------------------------------------- */

DataIndex &DataIndexList::newWildcard( bool front ){
  if( front ){
    push_front( DataIndex( -1 ) );
    return *begin();
  }
  else{
    push_back( DataIndex( -1 ) );
    return *rbegin();
  }
}

/* --------------------------------------------------------------------------- */
/* deleteFirstIndex --                                                         */
/* --------------------------------------------------------------------------- */

void DataIndexList::deleteFirstIndex(){
  pop_front();
}

/* --------------------------------------------------------------------------- */
/* deleteLastIndex --                                                          */
/* --------------------------------------------------------------------------- */

void DataIndexList::deleteLastIndex(){
  pop_back();
}

/* --------------------------------------------------------------------------- */
/* index --                                                                    */
/* --------------------------------------------------------------------------- */

DataIndex &DataIndexList::index( int index ){
  BUG_PARA( BugRef, "index", "index=" << index );
  assert( index >= 0 && index < (int)size() );
  int inx = 0;
  for( iterator it = begin(); it != end(); ++it ){
    if( index == inx ){
      return *it;
    }
    inx++;
  }
  BUG_FATAL( "index not available" );
  assert( false );
  throw( 0 );
}

/* --------------------------------------------------------------------------- */
/* firstIndex --                                                               */
/* --------------------------------------------------------------------------- */

DataIndex &DataIndexList::firstIndex(){
  BUG( BugRef, "firstIndex" );
  iterator it = begin();
  if( it != end() ){
    return *it;
  }
  BUG_FATAL( "index not available" );
  assert( false );
  throw( 0 );
}

/* --------------------------------------------------------------------------- */
/* lastIndex --                                                                */
/* --------------------------------------------------------------------------- */

DataIndex &DataIndexList::lastIndex(){
  BUG( BugRef, "lastIndex" );
  reverse_iterator it = rbegin();
  if( it != rend() ){
    return *it;
  }
  BUG_FATAL( "index not available" );
  assert( false );
  throw( 0 );
}

/* --------------------------------------------------------------------------- */
/* resizeList --                                                               */
/* --------------------------------------------------------------------------- */

void DataIndexList::resizeList( int sz ){
  int dims = (int)size();
  while( dims++ < sz ){
    newIndex();
  }
}

/* --------------------------------------------------------------------------- */
/* setIndexList --                                                             */
/* --------------------------------------------------------------------------- */

void DataIndexList::setIndexList( const DataIndexList &indexlist ){
  clear();
  for( const_iterator it = indexlist.begin(); it != indexlist.end(); ++it ){
    push_back( *it );
  }
}

/* --------------------------------------------------------------------------- */
/* setIndexList --                                                             */
/* --------------------------------------------------------------------------- */

void DataIndexList::setIndexList( int num_inx, const int *inx ){
  assert( num_inx >= 0 );
  assert( num_inx > 0 ? inx != 0 : true );

  clear();
  while( num_inx > 0 ){
    newIndex( *inx );
    inx++;
    num_inx--;
  }
}

/* --------------------------------------------------------------------------- */
/* getIndexList --                                                             */
/* --------------------------------------------------------------------------- */

bool DataIndexList::getIndexList( int &num_inx, int *inx ) const{
  if( num_inx < getDimensions() ){
    // Der Parameter soll zeigen, wieviel Platz in inx vorhanden ist.
    // Es reicht leider nicht.
    return false; // bad
  }
  for( const_iterator it = begin(); it != end(); ++it ){
    if( it->isWildcard() ){
      *inx = -1;
    }
    else{
      *inx = it->getIndex();
    }
  }
  num_inx = getDimensions();
  return true; // ok
}

/* --------------------------------------------------------------------------- */
/* hasWildcards --                                                             */
/* --------------------------------------------------------------------------- */

bool DataIndexList::hasWildcards() const{
  for( const_iterator it = begin(); it != end(); ++it ){
    if( it->isWildcard() ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* numberOfWildcards --                                                        */
/* --------------------------------------------------------------------------- */

int DataIndexList::numberOfWildcards() const{
  int num = 0;
  for( const_iterator it = begin(); it != end(); ++it ){
    if( it->isWildcard() ){
      num++;
    }
  }
  return num;
}

/* --------------------------------------------------------------------------- */
/* isEmpty --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataIndexList::isEmpty() const{
  return empty();
}

/* --------------------------------------------------------------------------- */
/* getDimensions --                                                            */
/* --------------------------------------------------------------------------- */

int DataIndexList::getDimensions() const{
  return (int)size();
}

/* --------------------------------------------------------------------------- */
/* printItem --                                                                */
/* --------------------------------------------------------------------------- */

void DataIndexList::printItem( std::ostream & ostr
                             , const DataDictionary *dict
                             , int level
                             , bool withIndices ) const
{
  if( dict != 0 ){
    ostr << dict->getName();
  }
  else{
    ostr << "???";
  }
  if( withIndices ){
    // ostr << "[Idnr=" << getItemIdnr() << ", level=" << level;
    // ostr << ", Indexlist=";
    printIndices( ostr );
    // ostr << "]";
  }
}

/* --------------------------------------------------------------------------- */
/* printIndices --                                                             */
/* --------------------------------------------------------------------------- */

void DataIndexList::printIndices( std::ostream & ostr ) const{
  Separator sep('[',',',']');
  if( empty() ){
    ostr << sep << "0";
  }
  else{
    for( const_iterator it = begin(); it != end(); ++it ){
      it->print( ostr, sep );
    }
  }
  ostr << sep.close();
}

/* --------------------------------------------------------------------------- */
/* indices --                                                                  */
/* --------------------------------------------------------------------------- */

std::string DataIndexList::indices() const{
  Separator sep('[',',',']');
  std::ostringstream ostr;
  if( empty() ){
    ostr << sep << "0";
  }
  else{
    for( const_iterator it = begin(); it != end(); ++it ){
      it->print( ostr, sep );
    }
  }
  ostr << sep.close();
  return ostr.str();
}

/* --------------------------------------------------------------------------- */
/* operator==  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataIndexList::operator==( const DataIndexList &inxlist ) const{
  if( size() != inxlist.size() ){
    return false;
  }

  const_iterator it2 = inxlist.begin();
  for( const_iterator it = begin(); it != end(); ++it ){
    if( ( *it != *it2 ) ){
      return false;
    }
    it2++;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* operator!=  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataIndexList::operator!=( const DataIndexList &inxlist ) const{
  return !( *this == inxlist );
}
