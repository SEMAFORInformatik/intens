
#if !defined(XFER_DATAITEM_INDEX_INCLUDED_H)
#define XFER_DATAITEM_INDEX_INCLUDED_H

#include <iostream>
#include "datapool/DataPoolDefinition.h"

#include "xml/Serializable.h"

class XferDataItemIndex: public Serializable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XferDataItemIndex( int level, bool isGuiIndex );
  XferDataItemIndex( XferDataItemIndex &index );

  virtual ~XferDataItemIndex(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setIndexName( const std::string &, const std::string &  );
  void setIndexNum( int );
  void setLowerbound( int );
  int getLowerbound( )const{ return m_lowerbound; }
  void setUpperbound( int );
  int getUpperbound( )const{ return m_upperbound; }
  void setWildcard();
  void resetWildcard();
  int getDimensionSize( DataReference *dref, bool incl_attr=false )const;
  int getDimensionSize( const DataReference &dataref, bool incl_attr=false )const;
  bool acceptIndex( const std::string &, int );
  bool setIndex( DataReference *,const std::string &, int );
  void setIndex( DataReference *,int );
  int getValue();
  const std::string &IndexName();
  const std::string &IndexOrFgName();
  int Level();
  void setLevel(int l);
  int IndexNum();
  int InitialIndex();
  int Lowerbound();
  int Upperbound();
  bool isWildcard();
  bool isIndexed();
  void erase( DataReference * );
  bool isGuiIndex() const;
  void insertData( const DataReference &ref, int inx );
  void deleteData( const DataReference &ref, int inx );
  void duplicateData( const DataReference &ref, int inx );
  void clearData( const DataReference &ref, int inx );
  void packData( const DataReference &ref, int inx );
  // debug function
  void print( std::ostream &o ) const;

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
   void exchangeDataPointers( DataReference * &ref1, DataReference * &ref2 );

/*=============================================================================*/
/* private member variables                                                    */
/*=============================================================================*/
private:
  std::string  m_indexname;
  std::string  m_fg_name;
  int          m_level;
  int          m_indexnum;
  int          m_lowerbound;
  int          m_upperbound;
  int          m_indexval;
  bool         m_wildcard;
  bool         m_isGuiIndex;

};

#endif
