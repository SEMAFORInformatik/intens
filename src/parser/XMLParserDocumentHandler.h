
#if !defined(XML_PARSER_DOC_HANDLER_H)
#define XML_PARSER_DOC_HANDLER_H
#include <string>
#include <vector>
#include "xml/XMLDocumentHandler.h"


class Serializable;

class XMLParserDocumentHandler : public INTENSXMLDocumentHandler{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLParserDocumentHandler();
  virtual ~XMLParserDocumentHandler();
 private:
  XMLParserDocumentHandler(const XMLParserDocumentHandler &);
  XMLParserDocumentHandler &operator=(const XMLParserDocumentHandler&);
  //========================================================================
  // public functions of DocumentHandler
  //========================================================================
 public:
  virtual void startDocument();
  virtual void endDocument();
  virtual void startElement( const std::string &element, XMLAttributeMap &attrs );
  virtual void endElement( const std::string &element );
  virtual void characters( std::string &value );
  //========================================================================
  // public
  //========================================================================
 public:

  //========================================================================
  // private members
  //========================================================================
 private:
  std::vector< Serializable * > m_serializer;
};

#endif
