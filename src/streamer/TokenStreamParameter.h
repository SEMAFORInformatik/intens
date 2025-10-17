
#if !defined(STREAMER_TOKEN_STREAM_PARAMETER_H)
#define STREAMER_TOKEN_STREAM_PARAMETER_H

#include <string>

#include "streamer/StreamParameter.h"

class StreamSource;
class StreamDestination;

class TokenStreamParameter: public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a token parameter
  TokenStreamParameter( const std::string &s="\n", int length=0 );
  virtual ~TokenStreamParameter();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool isValid(bool validErrorMsg=false) { return true; }
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual size_t getDimensionSize(int ndim)const;
  virtual std::string getName()const{ return m_token; }

  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src ){}

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );
  void xmlConvertToken();

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  std::string     m_token;
  int             m_toksiz;
  int             m_width;
  bool            m_leftAdjusted;
};

#endif
