
#ifndef HMAP_H
#define HMAP_H

#include <string>
#include <vector>
#include <map>
#include <utility>

class HMap{
  typedef std::map< std::string, int > RwTabMap;
  typedef RwTabMap::iterator RwTabMapIter;
  typedef RwTabMap::value_type RwTabPair;

 public:
  class Iterator{
  public:
    Iterator();
    Iterator(const Iterator &iter);
    Iterator(const RwTabMap::iterator &currentPair );
    ~Iterator();

    bool operator==(const Iterator &iter);
    bool operator!=(const Iterator &iter);
    RwTabPair &operator*(){ return *m_currentPair; }
    RwTabPair *operator->(){ return &(*m_currentPair); }
  protected:
    RwTabMap::iterator m_currentPair;
  };

  HMap( int size );
  virtual ~HMap();

  Iterator begin();
  Iterator end();
  Iterator find( const std::string &searchString );

  int hashFunction( const std::string &key );
  void insert( const std::string &newString, int token );

  void statistic();

  const int m_tableSize;
  std::vector<int> m_tableIndex;
  std::vector<RwTabMap> m_hashTable;
};

#endif
