#include <sstream>

#include <curl/curl.h>
#include "gui/GuiFactory.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "utils/Date.h"

#include "utils/UrlConverter.h"

std::string UrlConverter::chars_extendedAscii;
std::map<char, std::string> UrlConverter::replStr_extendedAscii;

/* --------------------------------------------------------------------------- */
/* urlQuote --                                                              */
/* --------------------------------------------------------------------------- */

void UrlConverter::urlQuote( std::string &text ) {
  if( text.empty() ) {
    return;
  }

  CURL *curl = curl_easy_init();
  if( !curl ) {
    GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : Fatal Error : curl_easy_init() failed" );
    return;
  }

  char *output = curl_easy_escape( curl, text.c_str(), 0 );
  if( output ) {
    text = output;
    curl_free( output );
  } else {
    GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : Fatal Error : curl_easy_escape() failed" );
  }

  curl_easy_cleanup( curl );
}

/* --------------------------------------------------------------------------- */
/* init_extendedAscii --                                                       */
/* --------------------------------------------------------------------------- */

void UrlConverter::init_extendedAscii() {
  if( !replStr_extendedAscii.empty() ) {
    return;
  }

  for( size_t i = 128; i < 256; ++i ) {
    std::ostringstream os;
    os << "%" << std::hex << std::uppercase << i;
    replStr_extendedAscii[(char)i] = os.str();
    chars_extendedAscii.append( 1, (char)i );
  }
}

/* --------------------------------------------------------------------------- */
/* urlQuote_extendedAscii --                                                   */
/* --------------------------------------------------------------------------- */

void UrlConverter::urlQuote_extendedAscii( std::string &text ) {
  init_extendedAscii();
  std::string::size_type pos = text.find_first_of( chars_extendedAscii );
  while( pos != std::string::npos ) {
    text.replace( pos, 1, replStr_extendedAscii[text[pos]] );
    pos = text.find_first_of( chars_extendedAscii, pos + 3 );
  }
}
