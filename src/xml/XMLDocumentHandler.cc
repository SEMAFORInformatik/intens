
#include <sstream>
#include "gui/GuiManager.h"

#include "xml/XMLFactory.h"

#include "xml/XMLDocumentHandler.h"
#include "utils/gettext.h"

void XMLAttributeMap::dispAll(void){
  AttrMap::iterator I;
  for(I=_attrmap.begin();I!=_attrmap.end();++I){
    std::cout<<"Attrs: "<<(*I).first<<" ["<<(*I).second<<"]\n";
  }
}

std::string XMLAttributeMap::operator[]( const std::string & indx )const{
  AttrMap::const_iterator pos = _attrmap.find( indx );
  if( pos == _attrmap.end() )
    return "";

  return pos -> second;
}

INTENSXMLDocumentHandler::INTENSXMLDocumentHandler(){
}

INTENSXMLDocumentHandler::~INTENSXMLDocumentHandler(){
}

// ---------------------------------------------------------------------------
// gnomeStartDocument --
// ---------------------------------------------------------------------------
void DocumentHandler::gnomeStartDocument( void *userData ){
  // std::cout << "XMLDocumentHandler::gnomeStartDocument" << std::endl;

  ((XMLFactory*)userData) -> getDocumentHandler() -> startDocument();
}

// ---------------------------------------------------------------------------
// gnomeEndDocument --
// ---------------------------------------------------------------------------
void DocumentHandler::gnomeEndDocument( void *userData ){
  //  std::cout << "XMLDocumentHandler::gnomeEndDocument" << std::endl;
  ((XMLFactory*)userData) -> getDocumentHandler() -> endDocument();
}

// ---------------------------------------------------------------------------
// gnomeStartElement --
// ---------------------------------------------------------------------------
void DocumentHandler::gnomeStartElement( void *userData, const xmlChar *name, const xmlChar **atts){
  // cout << "XMLDocumentHandler::gnomeStartElement" << endl;
  static int n = 0;
  if( n == 10000 ){
    n = 0;
#ifdef __MINGW32__  // QT_VERSION >= 0x040100
    std::cerr << _("WARNING QT-Bug, cannot process pending events") << std::endl << std::flush;
#else
    GuiManager::Instance().dispatchPendingEvents();
#endif
  }
  ++n;

  std::string element( (char*)name );
  XMLAttributeMap attrs;
  if(  atts != NULL ){
    int i = 0;
    while( i >= 0 ){
      if( atts[i] != NULL ){
	std::string name( (char*)atts[i] );
	std::string value( (char*)atts[++i] );
	attrs[name] = value;
	++i;
      }
      else
	i = -1;
    }
  }

  ((XMLFactory*)userData) -> getDocumentHandler() -> startElement( element, attrs );
}

// ---------------------------------------------------------------------------
// gnomeEndElement --
// ---------------------------------------------------------------------------
void DocumentHandler::gnomeEndElement( void *userData, const xmlChar *name ){
  // cout << "XMLDocumentHandler::gnomeEndElement" << endl;
  std::string element( (char*)name );
  ((XMLFactory*)userData) -> getDocumentHandler() -> endElement( element );
}

// ---------------------------------------------------------------------------
// gnomeCharacters --
// ---------------------------------------------------------------------------
void DocumentHandler::gnomeCharacters( void *userData, const xmlChar *ch, int len){
  // cout << "XMLDocumentHandler::gnomeCharacters" << endl;
  ((XMLFactory*)userData) -> getDocumentHandler() -> characters( std::string( (char*)ch, len ) );
}
