
#if !defined(STREAMER_DATA_DATASET_STREAM_PARAMETER_H)
#define STREAMER_DATA_DATASET_STREAM_PARAMETER_H

#include <vector>
#include <string>

#include "streamer/DataStreamParameter.h"

class DataSet;
class XferDataParameter;

class DatasetStreamParameter: public DataStreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DatasetStreamParameter();
  DatasetStreamParameter( XferDataParameter *param, int level, bool mand, DataSet *set );
  virtual ~DatasetStreamParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );

  virtual void setValue( double v );
  virtual bool getValue( double &v )const;
  virtual void setValue( const std::string &sval ){ DataStreamParameter::setValue(sval); }
  virtual bool getValue( std::string &sval )const{ return DataStreamParameter::getValue(sval); }

  virtual void setValue( int v ){ setValue((double)v); }
  virtual bool getValue( int &v )const{
    double d; if(getValue(d)){ v=static_cast<int>(d);return true;} return false; }

  virtual void putValues( StreamDestination *dest, bool transposed );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  StringConverter      m_converter;
  XferDataParameter   *m_param;
  DataSet             *m_dataset;
  //  char                 m_delimiter;
};

#endif
