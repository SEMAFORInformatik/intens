
#include <iostream>
#include <locale.h>
#include <sstream>

#include "streamer/XMLStreamDocumentHandler.h"
#include "datapool/DataStringElement.h"

#include <libxml/parser.h>
#include <libxml/parserInternals.h>

#include <libxslt/xsltInternals.h>
#include <libxslt/xsltutils.h>
#include <libxslt/extensions.h>
#include <libxslt/transform.h>

extern "C" {
#include <libexslt/exslt.h>
}

#include "xml/XMLErrorHandler.h"

#include "gui/GuiFactory.h"
#include "gui/GuiElement.h"
#include "gui/GuiScrolledText.h"

#include "datapool/DataDictionary.h"
#include "xfer/XferDataItem.h"

#include "xml/XMLFactory.h"
#include "xml/XMLErrorHandler.h"

class myBuffer : public std::streambuf{
public :
  myBuffer( std::string s ){
    setbuf( (char*)s.c_str(), s.size() );}
};


XMLFactory *XMLFactory::s_instance = 0;

// ---------------------------------------------------------------------------
// Constructor / Destructor --
// ---------------------------------------------------------------------------
XMLFactory::XMLFactory()
  : m_documentHandler( 0 )
    , m_errorHandler( 0 )
    , m_defaultErrorHandler( true )
    , m_ascii( false )
    , m_encoding( "UTF-8" )
{
}

XMLFactory::~XMLFactory(){
  if( m_errorHandler )
    delete m_errorHandler;
  if( m_documentHandler )
    delete m_documentHandler;
}

// ---------------------------------------------------------------------------
// Instance --
// ---------------------------------------------------------------------------
XMLFactory &XMLFactory::Instance(){
  if( s_instance == 0 ){
    s_instance = new XMLFactory();
  }
  return *s_instance;
}

// ---------------------------------------------------------------------------
// setSAXDocumentHandler --
// ---------------------------------------------------------------------------
void XMLFactory::setSAXDocumentHandler(DocumentHandler *handler){
  if( m_documentHandler )
    delete m_documentHandler;
  m_documentHandler = handler;
}

// ---------------------------------------------------------------------------
// setErrorHandler --
// ---------------------------------------------------------------------------
void XMLFactory::setErrorHandler(ErrorHandler *handler){
  if( m_defaultErrorHandler && m_errorHandler != 0 )
    delete m_errorHandler;
  m_errorHandler = handler;
  m_defaultErrorHandler = false;
}

// ---------------------------------------------------------------------------
// getSAXHandlerPtr --
// ---------------------------------------------------------------------------
xmlSAXHandlerPtr XMLFactory::getSAXHandler(){
  xmlSAXHandlerPtr h = (xmlSAXHandlerPtr) xmlMalloc(sizeof(xmlSAXHandler));
  h->internalSubset = 0;
  h->isStandalone = 0;
  h->hasInternalSubset = 0;
  h->hasExternalSubset = 0;
  h->resolveEntity = 0;
  h->getEntity = 0;
  h->entityDecl = 0;
  h->notationDecl = 0;
  h->attributeDecl = 0;
  h->elementDecl = 0;
  h->unparsedEntityDecl = 0;
  h->setDocumentLocator = 0;
  h->startDocument = DocumentHandler::gnomeStartDocument;
  h->endDocument = DocumentHandler::gnomeEndDocument;
  h->startElement = DocumentHandler::gnomeStartElement;
  h->endElement = DocumentHandler::gnomeEndElement;
  h->reference = 0;
  h->characters = DocumentHandler::gnomeCharacters;
  h->ignorableWhitespace = 0;
  h->processingInstruction = 0;
  h->comment = 0;
  h->warning = ErrorHandler::warning;
  h->error = ErrorHandler::error;
  h->fatalError = ErrorHandler::fatalError;
  h->getParameterEntity = 0;
  h->cdataBlock = 0;
  h->externalSubset = 0;
  return h;
}

// ---------------------------------------------------------------------------
// parse --
// ---------------------------------------------------------------------------
bool XMLFactory::saxParse( std::ostringstream &xmlStream ){
  std::istringstream is( xmlStream.str() );
  return saxParse( is );
}

bool XMLFactory::saxParse( std::istream &is ){
  xsltStylesheetPtr stylesheet(0);
  xmlDocPtr document(0), result(0);

  /*
   * Replace entities with their content.
   */
  // xmlSubstituteEntitiesDefault(1);

  try{
    m_errorMessage = "";

    if( m_errorHandler == 0 ){
      m_defaultErrorHandler = true;
      m_errorHandler = new XMLErrorHandler();
    }

    xmlSetGenericErrorFunc( this, (xmlGenericErrorFunc)XMLErrorHandler::error);

    const int BUFSIZE = 100000;
    char c[BUFSIZE+1];
    std::string s;
    while( is.get( c, BUFSIZE, EOF ) )
      s+=c;
    // erase control characters (invalid XML file)
    DataStringElement::convertControlCharacters(s, true);
    // get encoding
    std::string::size_type pos = s.find("encoding");
    m_encoding = "";
    if( pos!=std::string::npos ){
      std::string::size_type pos2=s.find_first_of( '"', pos );
      pos = s.find_first_of( '"', ++pos2 );
      m_encoding = std::string( s, pos2, pos - pos2 );
    }
    if( m_encoding.empty() ){
      GuiElement::printMessage( 0, "No xml file", GuiElement::msg_Error );
      m_encoding = "UTF-8";
      return false;
    }

    xmlParserCtxtPtr ctxt = xmlCreateMemoryParserCtxt( s.c_str(), s.size() );
    s.erase();
    if( ctxt == 0 )
      throw( "Memory allocate failed ( XMLFactory )" );
    ctxt->sax = getSAXHandler();
    ctxt->userData = this;
    xmlParseDocument(ctxt);
    if( !m_errorMessage.empty() ){
      xmlFreeDoc(ctxt->myDoc);
      xmlFreeParserCtxt(ctxt);
      throw( "xmlParseDocument : " + m_errorMessage );
    }

    //cleanup
    xmlFreeDoc(ctxt->myDoc);
    xmlFreeParserCtxt(ctxt);
    xsltCleanupGlobals();
    xmlCleanupParser();
    return true;
  }
  catch( std::string &message ){
    assert( m_errorHandler != 0 );
    m_errorHandler->printMessage( message );
    //cleanup
    xsltCleanupGlobals();
    xmlCleanupParser();
    return true;
  }
  return false;
}

#ifdef __MINGW32__
bool XMLFactory::saxParse( const std::string &filename ) {
  xsltStylesheetPtr stylesheet(0);
  xmlDocPtr document(0), result(0);

  /*
   * Replace entities with their content.
   */
  //  xmlSubstituteEntitiesDefault(1);

  try{
    m_errorMessage = "";

    if( m_errorHandler == 0 ){
      m_defaultErrorHandler = true;
      m_errorHandler = new XMLErrorHandler();
    }

    xmlSetGenericErrorFunc( this, (xmlGenericErrorFunc)XMLErrorHandler::error);

    xmlParserCtxtPtr ctxt = xmlCreateFileParserCtxt( filename.c_str() );
    if( ctxt == 0 )
      throw( "Memory allocate failed ( XMLFactory )" );
    ctxt->sax = getSAXHandler();
    ctxt->userData = this;
    xmlParseDocument(ctxt);
    if( !m_errorMessage.empty() ){
      xmlFreeDoc(ctxt->myDoc);
      xmlFreeParserCtxt(ctxt);
      throw( "xmlParseDocument : " + m_errorMessage );
    }

    //cleanup
    xmlFreeDoc(ctxt->myDoc);
    xmlFreeParserCtxt(ctxt);
    xsltCleanupGlobals();
    xmlCleanupParser();
    return true;
  }
  catch( std::string &message ){
    assert( m_errorHandler != 0 );
    m_errorHandler->printMessage( message );
    //cleanup
    xsltCleanupGlobals();
    xmlCleanupParser();
    return true;
  }
  return false;
}
#endif

// ---------------------------------------------------------------------------
// process --
// ---------------------------------------------------------------------------
bool XMLFactory::process( std::ostringstream &xmlStream
			       , const std::string &_xslFileName
			       , std::ostringstream &outputStream ){
  bool rslt = false;
  rslt = true;
  int i;
  xsltStylesheetPtr stylesheet(0);
  xmlDocPtr document(0), result(0);

  /*
   * Replace entities with their content.
   */
  //  xmlSubstituteEntitiesDefault(1);

  try{
    m_errorMessage = "";
    xmlSetGenericErrorFunc( this, (xmlGenericErrorFunc)XMLErrorHandler::error);

    //Parse Stylesheet
    stylesheet = xsltParseStylesheetFile( (const xmlChar*)_xslFileName.c_str() );
    if( !m_errorMessage.empty() )
      throw( "parseStylesheet : " + m_errorMessage );
    std::string xmlString( xmlStream.str() );

    //Parse Document
    document = xmlParseMemory( const_cast<char *>(xmlString.c_str()) , xmlString.size() );
    if( !m_errorMessage.empty() )
      throw( "parseXML :" + m_errorMessage );

    //Apply Stylesheet to Document
    result = xsltApplyStylesheet( stylesheet, document, NULL );
    if( !m_errorMessage.empty() )
      throw( "applyStylesheet : " + m_errorMessage );

    //copy result to stream
    xmlChar *buffer;
    int *size = 0;

    xmlDocDumpMemory( result, &buffer, size );
    if( !m_errorMessage.empty() )
      throw( m_errorMessage );
    if( !m_ascii ){
      std::string::size_type pos;
      std::string outputString( (char*)buffer );
      pos = outputString.find( "<?xml", 0 );
      pos = outputString.find( '\n', pos );
      pos = outputString.find_first_not_of( " \n\t", pos );

      outputString = outputString.substr( pos );
      outputStream << outputString << '\0';
    }
    else
      outputStream << (char*)buffer << '\0';

    //cleanup
    xsltFreeStylesheet( stylesheet );
    xmlFreeDoc( result );
    xmlFreeDoc( document );
    xmlFree( buffer );
    xsltCleanupGlobals();
    xmlCleanupParser();
  }
  catch( std::string &message ){
    // Error was reportet
    GuiFactory::Instance()->getLogWindow() -> writeText( message );

    //cleanup
    xsltFreeStylesheet( stylesheet );
    xmlFreeDoc( result );
    xmlFreeDoc( document );
    xsltCleanupGlobals();
    xmlCleanupParser();

    outputStream << '\0';
    rslt = false;
  }
  return rslt;
}

// ---------------------------------------------------------------------------
// reset --
// ---------------------------------------------------------------------------
void XMLFactory::reset(){
  if( m_documentHandler ){
    delete m_documentHandler;
    m_documentHandler = 0;
  }
  if( m_defaultErrorHandler && m_errorHandler ){
    delete m_errorHandler;
    m_errorHandler = 0;
  }
  m_ascii = false;
}


// ---------------------------------------------------------------------------
// reset --
// ---------------------------------------------------------------------------
void XMLFactory::addErrorMessage( char *msg ){
  m_errorMessage += msg;
}
