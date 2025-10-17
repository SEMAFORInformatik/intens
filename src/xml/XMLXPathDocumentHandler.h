
#if !defined(XML_XPATH_DOC_HANDLER_H)
#define XML_XPATH_DOC_HANDLER_H
#include <string>
#include <vector>

#include "xml/XMLDocumentHandler.h"

class XMLElement;
class XferDataItem;

class XMLXPathDocumentHandler : public INTENSXMLDocumentHandler{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLXPathDocumentHandler();
  virtual ~XMLXPathDocumentHandler();
 private:
  XMLXPathDocumentHandler(const XMLXPathDocumentHandler &);
  XMLXPathDocumentHandler &operator=(const XMLXPathDocumentHandler&);
  //========================================================================
  // public functions of DocumentHandler
  //========================================================================
 public:
  virtual void startDocument();
  virtual void endDocument();
  virtual void startElement( const std::string &element, XMLAttributeMap &attrs );
  virtual void endElement( const std::string &element );
  virtual void characters( const std::string &value );
  //========================================================================
  // public
  //========================================================================
 public:
  void setRoot( XferDataItem *xfer );
 private:
  XMLElement *createRoot();
  void clear();

  //========================================================================
  // private members
  //========================================================================
 private:
  std::vector<XMLElement *> m_elements;
  XferDataItem             *m_rootItem;
  int                       m_currentCycle;
};
#endif
