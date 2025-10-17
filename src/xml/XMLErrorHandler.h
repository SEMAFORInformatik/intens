#if !defined(XML_ERROR_HANDLER_H)
#define XML_ERROR_HANDLER_H

class ErrorHandler {
  public :
    virtual ~ErrorHandler() {}
    static void warning( void *ctx, const char *fmt, ... );
    static void error( void *ctx, const char *fmt, ... );
    static void fatalError( void *ctx, const char *fmt, ... );
    virtual void printMessage( const std::string &message ) = 0;
};
class XMLFactory;
class XalanSourceTreeParserLiaison;

class XMLErrorHandler : public ErrorHandler{
  //========================================================================
  // Constructor / Destructor
  //========================================================================
 public:
  XMLErrorHandler();
  virtual ~XMLErrorHandler();
 private:
  XMLErrorHandler(const XMLErrorHandler &);
  XMLErrorHandler &operator=(const XMLErrorHandler&);
  //========================================================================
  // public functions of ErrorHandler
  //========================================================================
 public:
  //========================================================================
  // public functions
  //========================================================================
  virtual void printMessage( const std::string &message );
  //========================================================================
  // private members
  //========================================================================

};
#endif
