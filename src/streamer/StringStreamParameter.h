
#if !defined(STRING_STREAM_PARAMETER_H)
#define STRING_STREAM_PARAMETER_H

#include "streamer/DataStreamParameter.h"

//*************************************************
// STRING_STREAM_PARAMETER
//*************************************************

class StringStreamParameter: public DataStreamParameter {
 public:
  /// constructs a StringDataParameter
  StringStreamParameter( XferDataItem *ditem, int level=0,
  						 int width=0, char delimiter=' ', bool locale=false, bool mand=false );
  StringStreamParameter( XferDataItem *ditem, int level,
			 StructStreamParameter *p, char delimiter, bool locale );

  /// Destructor
  virtual ~StringStreamParameter();

  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual void setValue( double v );
  virtual bool getValue( double &v )const;
  virtual void setValue( const std::string &sval ){
    DataStreamParameter::setValue(sval);
  }
  virtual bool getValue( std::string &sval )const{
    return DataStreamParameter::getValue(sval);
  }

  virtual void setValue( int v ){ setValue((double)v); }
  virtual bool getValue( int &v )const{
    double d;
    if(getValue(d)){
      v=static_cast<int>(d);return true;
    }
    return false;
  }

  virtual void putValues( StreamDestination *dest, bool transposed );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element,
			   const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

 private:
  StringConverter m_conv;
  //  char m_delimiter
;
};
#endif
