
#if !defined(XML_PLOTGROUP_STREAM_PARAMETER_H)
#define XML_PLOTGROUP_STREAM_PARAMETER_H

#include <string>

#include "streamer/StreamParameter.h"

class StreamSource;
class StreamDestination;
class HardCopyListener;

class XMLPlotGroupStreamParameter : public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a plotgroup stream parameter
  XMLPlotGroupStreamParameter( HardCopyListener *hcl );
  virtual ~XMLPlotGroupStreamParameter();
 private:
  XMLPlotGroupStreamParameter(const XMLPlotGroupStreamParameter &r);
  void operator=(const XMLPlotGroupStreamParameter&);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool isValid(bool validErrorMsg=false) { return true; }
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual size_t getDimensionSize(int ndim)const;
  virtual std::string getName()const;
  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src ){}
  virtual void setFrameBoxSize( const int x, const int y );
  virtual bool getFrameBoxSize( int &x, int &y );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  HardCopyListener *m_listener;
  std::string       m_tmpFileName;
  int               m_frameBoxSize_x;
  int               m_frameBoxSize_y;
};

#endif
