
#include <string>
#include <iostream>
#include <list>

class Flexer;
class Configurator;

class App {
 public:
  virtual ~App();
  static App &Instance( int &argc, char **argv );
  static App &Instance();
  static bool HasInstance();

  bool HasFlexer();

  bool parse( int &argc, char **argv );
  bool parse( const std::string& pstring );
  bool parseFile( const std::string& filename );
  int run();
  Flexer *getFlexer();
  Configurator *getConfigurator();
  void createReflist( std::ostream & );

  void log( const std::string &s );
  void printLog();
  static const char* FeaturePackage() { return FEATURE_PACKAGE; }

 private:
  App( int &argc, char **argv );

  Flexer       *m_flexer;
  Configurator *m_configurator;
  static App   *s_instance;
  std::list<std::string> m_log;
  static const char* FEATURE_PACKAGE;
  static const char* FEATURE_MATLAB;
  static const char* FEATURE_MATHEMATICA;
 public:
  static const char* INTENS_NAMESPACE;
  static const char* TOKEN_INTENS_NAMESPACE;
};
