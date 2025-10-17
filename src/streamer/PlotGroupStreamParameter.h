
#if !defined(PLOTGROUP_STREAM_PARAMETER_H)
#define PLOTGROUP_STREAM_PARAMETER_H

#include <string>

#include "streamer/StreamParameter.h"
#include "xml/Serializable.h"

class StreamSource;
class StreamDestination;
class XferDataItem;
class Stream;

class StreamParameterGroup;
class Scale;

class PlotGroupStreamParameter: public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a plotgroup stream parameter
  PlotGroupStreamParameter( const std::string &name );
  virtual ~PlotGroupStreamParameter();

/*=============================================================================*/
/* private definitions                                                         */
/*=============================================================================*/
 private:
  class Range : public Serializable {
  public:
    Range():
      m_startScale(0)
      , m_endScale(0){};
    Range( double start, double end ) :
      m_start(start)
      , m_end(end)
      , m_xferStart(0)
      , m_xferEnd(0){}
    Range( XferDataItem *start, XferDataItem *end
	   , Scale *startScale, Scale *endScale );
    virtual ~Range();
    bool getRange( double &start, double &end );

    void marshal( std::ostream &os );
    Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
    void setText( const std::string &text );

  private:
    double m_start;
    double m_end;
    Scale        *m_startScale;
    Scale        *m_endScale;
    XferDataItem *m_xferStart;
    XferDataItem *m_xferEnd;
  };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool isValid(bool validErrorMsg=false) { return true; }
  virtual bool read( std::istream &is );
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual size_t getDimensionSize(int ndim)const;
  virtual std::string getName()const{ return m_name; }

  virtual void putValues( StreamDestination *dest, bool transposed );
  virtual void getValues( StreamSource *src ){}
  /** sets Plot XRange with const values
      @param start start of plot range
      @param end end of plot range
      @return should always return true
  **/
  bool setXRange( const double start, const double end );
  /** sets Plot XRange with variable values
      @param start start of plot range
      @param end end of plot range
      @return true if parameters are simple numeric data types
  **/
  bool setXRange( XferDataItem *start, XferDataItem *end
		  , Scale *startScale, Scale *endScale
		  , StreamParameterGroup *s, char delimiter= ' ');
  /** sets Background transparency
      @param transparent background transparency
  **/
  void setTransparentBackground( bool transparent );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  std::string     m_name;
  std::string     m_tmpFileName;
  Range          *m_range;
  bool            m_transparentBg;
};

#endif
