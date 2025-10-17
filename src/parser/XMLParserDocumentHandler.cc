
#include "parser/XMLParserDocumentHandler.h"
#include "streamer/Stream.h"
#include "streamer/StreamManager.h"

XMLParserDocumentHandler::XMLParserDocumentHandler(){}

XMLParserDocumentHandler::~XMLParserDocumentHandler(){}

// ---------------------------------------------------------------------------
// startDocument --
// ---------------------------------------------------------------------------
void XMLParserDocumentHandler::startDocument(){}

// ---------------------------------------------------------------------------
// endDocument --
// ---------------------------------------------------------------------------
void XMLParserDocumentHandler::endDocument(){
//   cout<<"       XMLParserDocumentHandler::endDocument"<<endl;
  std::cout << std::endl;
}

// ---------------------------------------------------------------------------
// startElement --
// ---------------------------------------------------------------------------
void XMLParserDocumentHandler::startElement( const std::string &element, XMLAttributeMap &attrs ){
  if( element == "stream" ){
//     cout << "\n****** XMLParserDocumentHandler::startElement stream: " <<attrs["name"]<<"\n";
    std::cout <<"          "<< attrs["name"];
    m_serializer.push_back( StreamManager::Instance().newStream( attrs["name"] ) );
    m_serializer.back() -> unmarshal( element, attrs );
    return;
  }
//   attrs.dispAll();
  if( !m_serializer.empty() ){
    if( m_serializer.back() != 0 )
      m_serializer.push_back( m_serializer.back() -> unmarshal( element, attrs ) );
  }

  return;
}

// ---------------------------------------------------------------------------
// endElement --
// ---------------------------------------------------------------------------
void XMLParserDocumentHandler::endElement( const std::string &element ){
  if( !m_serializer.empty() ){
    if( m_serializer.back() != 0 ){
      XMLAttributeMap attrs;
      m_serializer.back() -> unmarshal( "/"+element, attrs );
    }
    m_serializer.pop_back();
  }
}

// ---------------------------------------------------------------------------
// characters --
// ---------------------------------------------------------------------------
void XMLParserDocumentHandler::characters( std::string &value ){
  if( !m_serializer.empty() ){
    if( m_serializer.back() != 0 )
      m_serializer.back()->setText( value );
    else
      assert( !value.empty() );
  }
}
