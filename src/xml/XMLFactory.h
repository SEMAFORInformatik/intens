
#if !defined(XML_FACTORY_H)
#define XML_FACTORY_H

#include <libxslt/xsltInternals.h>

#include <sstream>
#include <string>

class DocumentHandler;
class ErrorHandler;

class XMLFactory{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  virtual ~XMLFactory();
 private:
  XMLFactory();
  XMLFactory(const XMLFactory &);
  XMLFactory &operator=(const XMLFactory&);

  //========================================================================
  // public functions
  //========================================================================
 public:
  //  std::string &getEncoding(){ return m_encoding; }
  static XMLFactory &Instance();
  bool saxParse( std::istream &xmlstream );
  bool saxParse( std::ostringstream &xmlStream );
#ifdef __MINGW32__
  bool saxParse( const std::string &filename );
#endif
  void setXalanFormatterToXML( bool flag = true ){ m_ascii = flag; }
  bool process( std::ostringstream &xmlStream
		     , const std::string &_xslFileName
		     , std::ostringstream &outputStream );
  void setSAXDocumentHandler( DocumentHandler *handler );
  void setErrorHandler( ErrorHandler *handler );
  ErrorHandler *getErrorHandler(){ return m_errorHandler;}
  void reset();
  void addErrorMessage( char *msg );
  DocumentHandler *getDocumentHandler() { return m_documentHandler; }
 private:
  //========================================================================
  // private members
  //========================================================================
  xmlSAXHandlerPtr getSAXHandler();
 private:
  std::string m_encoding;
  static XMLFactory *s_instance;
  DocumentHandler   *m_documentHandler;
  ErrorHandler      *m_errorHandler;
  bool               m_ascii;
  bool               m_defaultErrorHandler;
  std::string        m_errorMessage;
};

#endif
