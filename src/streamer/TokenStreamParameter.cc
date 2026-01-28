
#include <stdlib.h>   // abs
#include <sstream>

#include <string>
#include <limits.h>
#include <jsoncpp/json/value.h>
#include "utils/Debugger.h"
#include "datapool/DataReference.h"
#include "streamer/TokenStreamParameter.h"
#include "xml/XMLDocumentHandler.h"
#include "utils/LaTeXConverter.h"
#include "app/AppData.h"
#include "gui/UnitManager.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

TokenStreamParameter::TokenStreamParameter(const std::string &value, int length)
  : StreamParameter()
  , m_token(value)
  , m_toksiz(value.size())
  , m_width(length)
  , m_leftAdjusted(false){
  if( m_width <0){
    m_width=-m_width;
    m_leftAdjusted=true;
  }
  // special case UNIT(data.xxx)
  // token is json data
  std::string text;
  if (AppData::Instance().hasUnitManagerFeature()) {
    UserAttr* userAttr = UnitManager::Instance().extractValue(value, text);
    if (userAttr || !text.empty()){
      m_token = userAttr ? userAttr->getOriginUnit() : text;
    }
    m_toksiz = m_token.size();
  }
}

TokenStreamParameter::~TokenStreamParameter(){}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

size_t TokenStreamParameter::getDimensionSize(int ndim) const {
  return SSIZE_MAX;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool TokenStreamParameter::read( std::istream &is ){
  //
  int c;
  int p=0;
  BUG_PARA(BugStreamer, "read(istream &) ", m_token );
  while( (c=is.get()) != EOF ){
    if( (char)c == m_token[p] ){
      p++;
      if( p == m_toksiz ){
        BUG_MSG( "FOUND: " << m_token );
        return true;
      }
    }
    else {
      p=0;
    }
  }
  // std::cout << " Token read: " << s.substr(0, 30) << " token("<<m_token<<") m_toksiz["
  //           <<m_toksiz<<"]  lastTC: "<<m_token[m_toksiz-1] << std::endl;
  return false;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool TokenStreamParameter::write( std::ostream &os ){
  BUG_PARA(BugStreamer, "write", "token='" << m_token << "'" );

  if(m_width >0 ) {
    if( m_toksiz > m_width )
      os << m_token.substr(0,m_width);
    else {
      int i=0;
      if( !m_leftAdjusted ){
	BUG_MSG("just right");
	for( ; i<m_width-m_toksiz; ++i ) os << ' ';
      }
      for( i=0; i<m_toksiz; ++i ) os << m_token[i];
      if( m_leftAdjusted ){
	BUG_MSG("just left");
	for( ; i<m_width; ++i ) os << ' ';
      }
    }
    return os.good();
  }

  // keine Breite gesetzt => (Vielleicht LatexFormat ?)
  std::string s( m_token );
  if( m_format == HardCopyListener::LaTeX ) {
    LaTeXConverter::convert2LaTeX( s );
  }

  if( os << s ){
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void TokenStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 )
    dest -> putValue(*this, m_token );
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void TokenStreamParameter::marshal( std::ostream &os ){
  os << "<TokenStreamParameter";
  std::string::size_type pos = m_token.find( '\n', 0 );
  if( pos == std::string::npos ){
    if( m_width != 0 ){
      os << " width=\"" << (m_leftAdjusted ? "-" : "" ) << m_width << "\"";
    }
  }
  os << '>';
  if( pos != std::string::npos && m_token.size()==1 )
    os << "<eoln/>";
  else {
    // Convert token before writing out ( "&" --> "&amp;" )
    xmlConvertToken();
    os << m_token;
  }
  os << "</TokenStreamParameter>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *TokenStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "eoln" ){
    m_token ="\n";
    m_toksiz=1;
    m_leftAdjusted=false;
    return this;
  }
  m_toksiz=m_token.size();
  std::string swidth=attributeList["width"];
  if( !swidth.empty() ){
    std::istringstream is( swidth );
    is >> m_width;
  }
  m_leftAdjusted = (m_width < 0);
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void TokenStreamParameter::setText( const std::string &value ){
  std::string text(value);
  if (AppData::Instance().hasUnitManagerFeature()) {
    if (UnitManager::Instance().extractValue(value, text)){
      m_token = text;
    }
  }
  if ( m_token == "<EMPTY>" )
    m_token = text;
  else
    // Add text to same token while not endElement
    // xml parser (sax) generates several setText when parsing control sequences like '&amp;'
    m_token += text;
  m_toksiz=m_token.size();
}

/* --------------------------------------------------------------------------- */
/* xmlConvertToken --                                                          */
/* --------------------------------------------------------------------------- */

void TokenStreamParameter::xmlConvertToken(){
  // "&" --> "&amp;"
  std::string::size_type pos = m_token.find( '&', 0 );
  while( pos != std::string::npos ){
    m_token.replace( pos++, 1, "&amp;" );
    pos = m_token.find( '&', ++pos );
  }
}
