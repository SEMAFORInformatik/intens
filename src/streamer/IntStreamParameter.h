
#if !defined(INT_STREAM_PARAMETER_H)
#define INT_STREAM_PARAMETER_H

#include "streamer/DataStreamParameter.h"

//*************************************************
// INT_STREAM_PARAMETER
//*************************************************

class IntStreamParameter: public DataStreamParameter {
 public:
  /// constructs a IntStreamParameter object
  IntStreamParameter( XferDataItem *ditem, int level, int width, Scale *scale
		      , char delimiter, bool mand );
  IntStreamParameter( XferDataItem *ditem, int level
		      , StructStreamParameter *p, char delimiter );

  /// Destructor
  virtual ~IntStreamParameter();

  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual void setValue( const std::string &v );
  virtual bool getValue( std::string &v )const;
  virtual void setValue( int v ){ setValue((double)v); }
  virtual bool getValue( int &v )const{
    double d; if(DataStreamParameter::getValue(d)){ v=static_cast<int>(d);return true;} return false;
  }
  virtual void setValue( double v ){ DataStreamParameter::setValue(v); }
  virtual bool getValue( double &v )const{ return DataStreamParameter::getValue(v); }

  virtual void putValues( StreamDestination *dest, bool transpose );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

 private:
  int putTransposedMatrix( std::vector<double>&vals, int ndims, int idim
                         , DataContainer::SizeList &dims, int indexnum );
  int putMatrix( std::vector<double>&vals, int ndims, int idim
               , DataContainer::SizeList &dims, int indexnum );
  IntConverter m_conv;
  //  char m_delimiter;
};
#endif
