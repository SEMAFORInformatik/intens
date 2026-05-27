// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <iostream>
#include <list>

class Flexer;
class Configurator;
class UserPasswordListener;
class OAuthClient;

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
  void runOAuthClient(UserPasswordListener* listener=0 );
  std::string OAuthToken();
  void runDashboardClient(UserPasswordListener* listener=0 );
  std::string getAppStatus();

 private:
  App( int &argc, char **argv );

  Flexer       *m_flexer;
  Configurator *m_configurator;
  static App   *s_instance;
  std::list<std::string> m_log;
  static const char* FEATURE_PACKAGE;
  static const char* FEATURE_MATLAB;
  static const char* FEATURE_MATHEMATICA;
  OAuthClient*      m_oauthClient;
 public:
  static const char* INTENS_NAMESPACE;
  static const char* TOKEN_INTENS_NAMESPACE;
};
