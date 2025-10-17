
#include "xml/XMLXPathDocumentHandler.h"
#include "xml/XMLXPathRoot.h"

XMLXPathDocumentHandler::XMLXPathDocumentHandler()
  : m_currentCycle( 0 )
  , m_rootItem( 0 ){
}

XMLXPathDocumentHandler::~XMLXPathDocumentHandler(){
  clear();
  delete m_rootItem;
}

// ---------------------------------------------------------------------------
// startDocument --
// ---------------------------------------------------------------------------
void XMLXPathDocumentHandler::startDocument(){
  //  cout << "XMLXPathDocumentHandler::startDocument" << endl;
}

// ---------------------------------------------------------------------------
// endDocument --
// ---------------------------------------------------------------------------
void XMLXPathDocumentHandler::endDocument(){
  //   cout << "XMLXPathDocumentHandler::endDocument" << endl;
  clear();
}

// ---------------------------------------------------------------------------
// startElement --
// ---------------------------------------------------------------------------
void XMLXPathDocumentHandler::startElement( const std::string &element, XMLAttributeMap &attrs ){
  //  cout << "====> startElement : " << element << endl;
  XMLElement *elem = 0;
  if( m_elements.empty() && element == "DATAPOOL" )
    m_elements.push_back( createRoot() );
  else if( m_elements.empty() ){
    m_elements.push_back( createRoot() );
    elem = m_elements.back()->newElement( element );
    //AMG    elem -> clearRange();
    m_elements.push_back( elem );
  }
  else{
    elem = m_elements.back()->newElement( element );
    //AMG    elem -> clearRange();
    m_elements.push_back( elem );
  }
}

// ---------------------------------------------------------------------------
// endElement --
// ---------------------------------------------------------------------------
void XMLXPathDocumentHandler::endElement( const std::string &element ){
  //cout << "<==== endElement : " << element << endl;
  assert( !m_elements.empty() );
  if( m_elements.back() != 0 ){
    m_elements.back()->end();
    delete m_elements.back();
  }
  m_elements.pop_back();
}

// ---------------------------------------------------------------------------
// characters --
// ---------------------------------------------------------------------------
void XMLXPathDocumentHandler::characters( const std::string &value ){
  assert( !m_elements.empty() );
  //  cout << "characters" << endl;
  m_elements.back()->characters( value );
}

// ---------------------------------------------------------------------------
// setRoot --
// ---------------------------------------------------------------------------
void XMLXPathDocumentHandler::setRoot( XferDataItem *xfer ){
  m_currentCycle = 0;
  if( xfer != 0 )
    m_rootItem = new XferDataItem( *xfer );
  clear();
}

// ---------------------------------------------------------------------------
// clear --
// ---------------------------------------------------------------------------
void XMLXPathDocumentHandler::clear(){
  std::vector<XMLElement *>::iterator iter;
  for ( iter = m_elements.begin(); iter != m_elements.end(); ++iter )
    delete *iter;
  m_elements.clear();
}

XMLElement *XMLXPathDocumentHandler::createRoot(){
  XferDataItem *newItem = 0;
  if( m_rootItem != 0 ){
    newItem = new XferDataItem( *m_rootItem );
  }

  XMLElement *elem = new XMLXPathRoot( newItem, m_currentCycle++ );
  return elem;
}
