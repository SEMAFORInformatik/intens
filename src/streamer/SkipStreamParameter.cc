
#include <sstream>
#include <string>
#include <limits.h>
#ifdef __MINGW32__
#if !defined(SSIZE_MAX)
#define SSIZE_MAX INT_MAX
#endif
#endif
#include "utils/Debugger.h"
#include "datapool/DataReference.h"
#include "streamer/SkipStreamParameter.h"
#include "xml/XMLDocumentHandler.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

SkipStreamParameter::SkipStreamParameter(int width):
  m_width(width){}

SkipStreamParameter::~SkipStreamParameter(){}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

size_t SkipStreamParameter::getDimensionSize(int n) const {
  return SSIZE_MAX;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool SkipStreamParameter::read( std::istream &is ){
  //
  BUG(BugStreamer, "SkipStreamParameter::read(istream &)" );
  return is.ignore(m_width).good();
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool SkipStreamParameter::write( std::ostream &os ) {
  BUG(BugStreamer, "SkipStreamParameter::write(ostream &)" );
  for( int i=0; i<m_width; ++i ){
    os << ' ';
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void SkipStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 )
    dest -> putValue(*this, " " );
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void SkipStreamParameter::marshal( std::ostream &os ){
  os << "<SkipStreamParameter"
     << " width=\"" << m_width << "\""
     << ">"
     << "</SkipStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *SkipStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "SkipStreamParameter" ){
    std::string swidth=attributeList["width"];
    if( !swidth.empty() ){
      std::istringstream is( swidth );
      is >> m_width;
    }
    else {
      m_width=0;
    }
  }
  return 0;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void SkipStreamParameter::setText( const std::string &text ){
}
