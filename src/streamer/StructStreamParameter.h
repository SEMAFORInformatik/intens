
#if !defined(STRUCT_STREAM_PARAMETER_H)
#define STRUCT_STREAM_PARAMETER_H

#include "streamer/DataStreamParameter.h"

//*************************************************
// STRUCT_STREAM_PARAMETER
//*************************************************

class StructStreamParameter: public DataStreamParameter {
 public:
  /// constructs a StructStreamParameter object
  StructStreamParameter( XferDataItem *ditem, int level
						 , StructStreamParameter *p, char delimiter, bool locale );
  StructStreamParameter( XferDataItem *ditem, char delimiter, bool locale ):
  DataStreamParameter(ditem,0,0,false, delimiter, locale)
    , m_recreateIndex(true) {}

  /// Destructor
  virtual ~StructStreamParameter();

  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual void setValue( const std::string &v );
  virtual bool getValue( std::string &v )const;
  virtual void setValue( int v );
  virtual bool getValue( int &v )const;
  virtual void setValue( double v );
  virtual bool getValue( double &v )const;
  virtual bool isValid(bool validErrorMsg=false);

  virtual void setItemIndex(int pos, int v);

  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src );
  virtual size_t getDimensionSize( int idim )const{
    return m_dataitem == 0 ? 0 : m_dataitem->getDimensionSize( idim );
  }
  virtual void setDimensionSize( int sz );
  virtual void setIndex( int pos, int inx );
  int createDataItemIndexes( XferDataItem *d );
  void removeDataItemIndexes( XferDataItem *d );
  void setClassname( const std::string &name ){ m_classname=name; }
  const std::string &getClassname( )const{ return m_classname; }
  virtual std::string getName()const{ return m_name; }
  void keepIndex(){ m_recreateIndex=false; }

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

 private:
  void getFields( DataReference *dref );

  std::vector<DataStreamParameter *> m_parfields;
  std::string                   m_classname;
  std::vector<int>                   m_indexvec;
  std::string                   m_name;
  bool                          m_recreateIndex;
  //  char                          m_delimiter;
};
#endif
