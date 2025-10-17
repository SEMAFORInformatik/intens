
#if !defined(XML_STREAM_PARAMETER_H)
#define XML_STREAM_PARAMETER_H

#include <string>

#include "streamer/StreamParameter.h"

class StreamSource;
class StreamDestination;
class HardCopyListener;

class XMLStreamParameter : public StreamParameter
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /// constructs a plotgroup stream parameter
  XMLStreamParameter( XferDataItem *xfer
		      , bool stand_alone = true
		      , bool valid_items_only = true );
  virtual ~XMLStreamParameter();
 private:
  XMLStreamParameter(const XMLStreamParameter &r);
  void operator=(const XMLStreamParameter&);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream &is );
#ifdef __MINGW32__
  virtual bool read( const std::string &filename );
#endif
  virtual bool write( std::ostream &os );
  virtual void fixupItemStreams( BasicStream &stream ) {}
  virtual std::string getName()const;
  virtual void putValues( StreamDestination *dest, bool transposed ){}
  virtual void getValues( StreamSource *src ){}
  virtual bool isValid(bool validErrorMsg=false);
  void setAttributes( const std::vector<std::string> &attrs ){ m_xmlAttributes = attrs; }
  void addAttribute( const std::string &attr ){ m_xmlAttributes.push_back( attr ); }
  void withAllCycles(){ m_allCycles = true; }

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );
  void setXMLSchema( const std::string &schema ){ m_schema=schema; }
  void setXMLNamespace( const std::string &nspace ){ m_namespace=nspace; }
  void setXMLVersion( const std::string &version ){ m_version=version; }
  void setXMLStylesheet( const std::string &stylesheet ){ m_stylesheet=stylesheet; }

  /** Diese Funktion ist die Implementation der GuiIndexListener-Funktion.
   */
  virtual void setIndex( const std::string &name, int index );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  bool                     m_valid_items_only;
  bool                     m_stand_alone;
  XferDataItem            *m_xfer;
  std::vector<std::string> m_xmlAttributes;
  bool                     m_allCycles;
  std::string              m_schema;
  std::string              m_namespace;
  std::string              m_version;
  std::string              m_stylesheet;
};

#endif
