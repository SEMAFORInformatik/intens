#if !defined(XML_STREAM_DOC_HANDLER_H)
#define XML_STREAM_DOC_HANDLER_H
#include <string>
#include <vector>
#include "xml/XMLDocumentHandler.h"

class XMLElement;
class XferDataItem;

class XMLStreamDocumentHandler : public INTENSXMLDocumentHandler{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLStreamDocumentHandler();
  virtual ~XMLStreamDocumentHandler();
 private:
  XMLStreamDocumentHandler(const XMLStreamDocumentHandler &);
  XMLStreamDocumentHandler &operator=(const XMLStreamDocumentHandler&);
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
  void withAllCycles(){ m_allCycles = true; }
 private:
  XMLElement *createRoot( const std::string & );
  void clear();
  bool isElementStream();
  bool isDatapoolStream();
  bool isCycleStream();

  //========================================================================
  // private members
  //========================================================================
 private:
  std::vector<XMLElement *> m_elements;
  XferDataItem             *m_rootItem;
  int                       m_currentCycle;
  bool                      m_allCycles;
};

#endif
