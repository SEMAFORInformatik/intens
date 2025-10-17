
#if !defined(STREAMER_SKIP_STREAM_PARAMETER_H)
#define STREAMER_SKIP_STREAM_PARAMETER_H

#include <string>

#include "streamer/StreamParameter.h"

class StreamSource;
class StreamDestination;

class SkipStreamParameter: public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a skip parameter
  SkipStreamParameter( int width=1 );
  virtual ~SkipStreamParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool isValid(bool validErrorMsg=false) { return true; }
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual size_t getDimensionSize(int ndim)const;
  virtual std::string getName()const{ return "<SKIP>"; }
  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src ){}

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  int       m_width;
};

#endif
