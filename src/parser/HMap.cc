
#include <string>
#include <utility>
#include "HMap.h"

HMap::HMap( int size )
  : m_tableSize( size )
  , m_tableIndex( size, size + 1)
  , m_hashTable( 1, RwTabMap() ){
}

HMap::~HMap(){
}

int HMap::hashFunction( const std::string &key ){
  int h;
  const char *t = key.c_str();
  for ( h = 0; *t; t++ )
    h = ( 64 * h + *t ) % m_tableSize;
  return h;
}

void HMap::insert( const std::string &newString, int token  ){
  int index;
  int hashNo = hashFunction( newString );
  if( (m_tableIndex)[ hashNo ] > m_tableSize ){
    m_hashTable.push_back( RwTabMap() );
    m_tableIndex[ hashNo ] = m_hashTable.size() - 1;
  }

  index = (m_tableIndex)[ hashNo ];
  m_hashTable[ index ].insert( RwTabPair( newString, token ) );

}

void HMap::statistic(){
  std::vector<int>::iterator iter = m_tableIndex.begin();
  while( iter != ( m_tableIndex).end() ){
    ++iter;
  }
}

HMap::Iterator::Iterator(){
}

HMap::Iterator::Iterator( const Iterator &iter )
  : m_currentPair( iter.m_currentPair ){
}

HMap::Iterator::Iterator( const RwTabMap::iterator &currentPair )
  : m_currentPair( currentPair ){
}

HMap::Iterator::~Iterator(){
}

HMap::Iterator HMap::find( const std::string &searchString ){
  int hashNo = hashFunction( searchString );
  int index = m_tableIndex[ hashNo ];

  if( index > m_tableSize ){
    return Iterator( m_hashTable.back().end() );
  }
  RwTabMapIter iter = m_hashTable[ index ].find( searchString );
  if( iter != m_hashTable[ index ].end() ){
    return Iterator( iter );
  }
  return Iterator( m_hashTable.back().end() );
}

HMap::Iterator HMap::begin(){
  return Iterator( m_hashTable.front().begin() );
}

HMap::Iterator HMap::end(){
return Iterator( m_hashTable.back().end() );
}

bool HMap::Iterator::operator==(const Iterator &iter){
  return m_currentPair == iter.m_currentPair;
}

bool HMap::Iterator::operator!=(const Iterator &iter){
  return m_currentPair != iter.m_currentPair;
}
