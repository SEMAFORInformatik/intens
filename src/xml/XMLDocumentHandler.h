#if !defined(INTENS_XML_DOC_HANDLER_H)
#define INTENS_XML_DOC_HANDLER_H
#include <string>
#include <map>
#include <iostream>

class XMLAttributeMap {
public:
  XMLAttributeMap(){}
  virtual ~XMLAttributeMap(){}
  std::string operator[]( const std::string & ) const;
  std::string &operator[]( const std::string &indx ){ return _attrmap[indx]; }
  void dispAll(void);

private:
  typedef std::map<std::string, std::string> AttrMap;
  AttrMap _attrmap;
};

#include <libxslt/xsltutils.h>
class XMLFactory;

class DocumentHandler{
  public :
  virtual ~DocumentHandler(){}
  static void gnomeStartDocument( void *userData );
  static void gnomeEndDocument( void *userData );
  static void gnomeStartElement(void *userData, const xmlChar *name, const xmlChar **atts);
  static void gnomeEndElement(void *userData, const xmlChar *name );
  static void gnomeCharacters( void *userData, const xmlChar *ch, int len);
  virtual void startDocument() = 0;
  virtual void endDocument() = 0;

  virtual void startElement( const std::string &element, XMLAttributeMap &attrs ) = 0;
  virtual void endElement( const std::string &element ) = 0;
  virtual void characters( const std::string &value ) = 0;
};

class XMLElement;
class XferDataItem;

class INTENSXMLDocumentHandler :
  public DocumentHandler{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  INTENSXMLDocumentHandler();
  virtual ~INTENSXMLDocumentHandler();
 private:
  INTENSXMLDocumentHandler(const INTENSXMLDocumentHandler &);
  INTENSXMLDocumentHandler &operator=(const INTENSXMLDocumentHandler&);
  //========================================================================
  // private definitions
  //========================================================================
 private:
  //========================================================================
  // public functions of DocumentHandler
  //========================================================================
 public:
  virtual void startDocument() = 0;
  virtual void endDocument() = 0;
  virtual void startElement( const std::string &element
			     , XMLAttributeMap &attrs ) = 0;
  virtual void endElement( const std::string &element ) = 0;
  virtual void characters( const std::string &value ) = 0;
};

#endif
