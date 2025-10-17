
#include <stdarg.h>
#include <sstream>


// only needed to prevent undeclared compile errors with QT
#ifdef HAVE_QT
typedef short Position;
#endif
// FIX IT!

#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"

#include "xml/XMLFactory.h"
#include "xml/XMLErrorHandler.h"

// ---------------------------------------------------------------------------
// Constructor / Destructor --
// ---------------------------------------------------------------------------
XMLErrorHandler::XMLErrorHandler(){}


XMLErrorHandler::~XMLErrorHandler(){}

// ---------------------------------------------------------------------------
// warning --
// ---------------------------------------------------------------------------
void ErrorHandler::warning( void *ctx, const char *fmt, ... ){
  char        msg[BUFSIZ];
  va_list     ap;
  va_start( ap, fmt );
  vsprintf( msg, fmt, ap );
  va_end( ap );
  std::string message( "XML-Warning : ");
  message += msg;
  ((XMLFactory*)ctx)->getErrorHandler()->printMessage( message );
}

// ---------------------------------------------------------------------------
// error --
// ---------------------------------------------------------------------------
void ErrorHandler::error( void *ctx, const char *fmt, ... ){
  char        msg[BUFSIZ];
  va_list     ap;
  va_start( ap, fmt );
  vsprintf( msg, fmt, ap );
  va_end( ap );
  ((XMLFactory*)ctx)->addErrorMessage( msg );
}

// ---------------------------------------------------------------------------
// fatalError --
// ---------------------------------------------------------------------------
void ErrorHandler::fatalError( void *ctx, const char *fmt, ... ){
  char        msg[BUFSIZ];
  va_list     ap;
  va_start( ap, fmt );
  vsprintf( msg, fmt, ap );
  va_end( ap );
  ((XMLFactory*)ctx)->addErrorMessage( msg );
}

// ---------------------------------------------------------------------------
// printMessage --
// ---------------------------------------------------------------------------
void XMLErrorHandler::printMessage( const std::string &message ){
  GuiFactory::Instance()->getLogWindow() -> writeText( message );
}
