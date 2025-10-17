
#if !defined(STREAMER_DATA_STRINGDATE_STREAM_PARAMETER_H)
#define STREAMER_DATA_STRINGDATE_STREAM_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/DataStreamParameter.h"
//#include "xfer/XferDataItem.h"
#include "xfer/XferConverter.h"
//#include "datapool/DataContainer.h"

class XferDataParameter;

class StringDateStreamParameter: public DataStreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  StringDateStreamParameter(char delimiter);
  StringDateStreamParameter( XferDataParameter *param, int level, int length, bool mand
			     , UserAttr::STRINGtype type, char delimiter );
  virtual ~StringDateStreamParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

//   virtual void setValue( double v );
//   virtual bool getValue( double &v )const;
//   virtual void setValue( const std::string &sval ){ DataStreamParameter::setValue(sval); }
//   virtual bool getValue( std::string &sval )const{ return DataStreamParameter::getValue(sval); }

//   virtual void setValue( int v ){ setValue((double)v); }
//   virtual bool getValue( int &v )const{
//     double d; if(getValue(d)){ v=static_cast<int>(d);return true;} return false; }

  virtual void putValues( StreamDestination *dest, bool transposed );

//   void marshal( std::ostream &os );
//   Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
//   void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  StringConverter      m_converter;
  XferDataParameter   *m_param;
  UserAttr::STRINGtype   m_type;
  //  char                 m_delimiter;
};

#endif
