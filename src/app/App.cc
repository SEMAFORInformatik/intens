
#include "gui/GuiElement.h"
#include "gui/GuiFolderGroup.h"
#include "operator/MessageQueue.h"
#ifdef __MINGW32__
#include <winsock2.h>
#endif
#include "job/JobManager.h"

#if HAVE_LOG4CPLUS
#include <log4cplus/logger.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/configurator.h>
#endif

#define CLASSNAME  "Intens"

#include <stdio.h>
#include <locale.h>
#include <limits.h>
#include <filesystem>

#ifdef __MINGW32__
#include "utils/utsname.h"
#include "utils/pwd.h"
#include <windows.h>
#else

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/utsname.h>
#endif

#include "job/JobManager.h"

#include "streamer/XMLStreamParameter.h"
#include "utils/Debugger.h"
#include "utils/Date.h"
#include "utils/gettext.h"
#include "app/App.h"
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "app/UiManager.h"
#include "app/ModuleFactory.h"
#include "gui/GuiManager.h"
#include "gui/UnitManager.h"
#include "datapool/DataPool.h"
#include "operator/ProcessGroup.h"
#include "streamer/StreamManager.h"
#include "operator/FileStream.h"
#include "parser/Flexer.h"
#include "parser/Configurator.h"
#include "parser/InterpreterConfigurator.h"

#ifdef HAVE_HEADLESS
#include "gui/headless/HeadlessGuiFactory.h"
#else
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtManager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <qglobal.h>
#include <QApplication>
#endif

#include <ltdl.h>

INIT_LOGGER();

const char* App::FEATURE_PACKAGE = "intens";
const char* App::FEATURE_MATLAB = "MATLAB";
const char* App::INTENS_NAMESPACE = "intens_namespace_";
const char* App::TOKEN_INTENS_NAMESPACE = "{NS}";

App *App::s_instance = 0;


extern FILE * yyerfp;

extern int yyparse();

/*-----------------------------------------------------------------
  get_groups
    returns the groups and the number of groups the user belongs to.

  -----------------------------------------------------------------
*/
static bool get_groups( const char *user )
{
#ifdef __MINGW32__
  return false;
#else
  struct group *grpptr;
  int           i;
  int           n;
  AppData &appdata = AppData::Instance();
  //#define NGROUPS_MAX 32
  gid_t     gids[NGROUPS_MAX];

  switch( (n = getgroups( NGROUPS_MAX, gids ))){
  case -1:
    (void)perror("get_groups() failed" );
    return false;

  case 0:
    if( (grpptr = getgrgid( getgid() )) == NULL ){
      return false;
    }
    appdata.addUserGroup( grpptr->gr_name );
    break;

  default:
    for( i=0; i<n; i++ ){
      if( (grpptr = getgrgid( gids[i] )) == NULL ){
	return false;
      }
      appdata.addUserGroup( grpptr->gr_name );
    }
  }
  return true;
#endif
}

static char *
next_word( char *s )
{
  static char *cp, *start;

  if( s )
    cp = s;

  if( *cp == '\0' )
    return (char*) "";

  for( ; isspace( *cp ) && *cp != '\0'; cp++ );
  start = cp;
  for( ; !isspace( *cp ) && *cp != '\0'; cp++ );
  if( *cp ){
    *cp = '\0';
    cp++;
  }
  return start;
}


// --------------------------------------------------------------------------- //
// Instance --                                                                 //
// --------------------------------------------------------------------------- //

App &App::Instance( int &argc, char ** argv ){

  if( s_instance == 0 ){
    s_instance = new App( argc, argv );
  }
  return *s_instance;
}

bool App::HasInstance() {
  return s_instance != 0;
}

bool App::HasFlexer() {
  return m_flexer != 0;
}

App &App::Instance(){
  assert( s_instance != 0 );
  return *s_instance;
}

// --------------------------------------------------------------------------- //
// getFlexer --                                                                //
// --------------------------------------------------------------------------- //

Flexer *App::getFlexer(){
    assert( m_flexer != 0 ) ;
    return m_flexer;
}

// -- Get Home Directory
#include <string.h>
std::string getHomeDir(){
  char buf[BUFSIZ+1];
  char PATHSEP;
  strncpy(buf, "\0", BUFSIZ);
#ifdef __MINGW32__
  PATHSEP='\\';
  GetModuleFileName( NULL, buf, BUFSIZ );
#else
  PATHSEP='/';
  int l=readlink("/proc/self/exe", buf, BUFSIZ);
#endif
  std::string progname = buf;
  for( size_t i=progname.rfind(PATHSEP)-1; i>0; i-- ){
    // MinGW hat evtl ein .libs Unterverzeichnis
    if( progname[i]==PATHSEP && progname.substr(i+1, 4) != ".lib"){
      return progname.substr(0,i);
    }
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor of our Application  --                             */
/* --------------------------------------------------------------------------- */

App::App( int &argc, char **argv )
  : m_flexer( 0 )
  , m_configurator( 0 ){
  char *p;

#ifdef YYDEBUG
   extern int yydebug;

   yydebug = 1;
#endif

  struct passwd *pwptr;
  const char          *cp;

  setlocale( LC_ALL, "" );   /* use the internationalization utilities */
  AppData &appdata = AppData::Instance();

  std::string home=getHomeDir();
  std::string::size_type pos = home.size();
  if( pos > 0 ){
    if( home.at( pos-1 ) != '/' ){
      home += "/";
    }
    const std::filesystem::directory_entry datadir(home+"share/bitmaps");
    if(datadir.exists()){
      appdata.setIntensHome(home + "share");
    }
    else{
      const std::filesystem::directory_entry datadir("/usr/share/intens/bitmaps");
      if(datadir.exists()){
        appdata.setIntensHome("/usr/share/intens");
      }
      else{
        appdata.setIntensHome(home);
      }
    }
  }
  appdata.getOpt(argc, argv);

  char* isOtel = getenv("INTENS_OTEL_ENABLED");
  if (isOtel != NULL) {
    appdata.setOpenTelemetryMetadata();
  }

  std::string fn = appdata.Log4cplusPropertiesFilename();
  if (!appdata.LspWorker() && std::filesystem::exists(fn)) {
#if HAVE_LOG4CPLUS
    // tempoary check "ch.semafor.intens" or "org.semafor.intens"
    QFile f(fn.c_str());
    if (f.open(QIODeviceBase::ReadOnly)){
      std::string oldPath("ch.semafor.intens");
      auto ret = f.readAll().contains(oldPath);
      if (ret) {
        Debugger::overrideBaseCategoryPath(oldPath);
        BUG_WARN("Deprecated Category path: '" << oldPath << "' please use 'ch.semafor.intens'");
      }
      f.close();
    }
    // do configure
    log4cplus::PropertyConfigurator::doConfigure(fn);
#endif
  }else{
#if HAVE_LOG4CPLUS
    // we must set a second time a log level
    log4cplus::Logger root = log4cplus::Logger::getRoot();
    root.setLogLevel(log4cplus::WARN_LOG_LEVEL);
#endif
#if _DEBUG && HAVE_LOG4CPLUS
    if(!appdata.DisplayVersion() && !appdata.DisplayHelp() && !appdata.LspWorker())
      std::cerr << "Info log4c[pp,plus] configuration file '" << fn << "' not found." << std::endl;
#endif
  }

  if (appdata.LspWorker()) {
#if HAVE_LOG4CPLUS
    log4cplus::Logger::getRoot().shutdown();
#endif
  }

  // init debugger
  Debugger::InitializeDebugger();

  // debug info
  lconv *lc = localeconv();
  BUG_INFO("LC_ALL currency: "<<lc->int_curr_symbol
           << ", decimal point: " << lc->mon_decimal_point
           << ", thousand sep: "<< lc->thousands_sep);

  appdata.setLocaleDir( appdata.IntensHome()+"share/locale" );
  appdata.setLocaleDomain( PACKAGE );

  textdomain( PACKAGE );

  if ( (pwptr = getpwuid( getuid () )) != NULL ){
    appdata.setUserName( pwptr->pw_name );
  }
  if (pwptr) {
    std::string realname = pwptr->pw_gecos;
    std::string::size_type pos = realname.find(',');
    appdata.setRealName( realname.substr(0,pos) );
    //std::cerr << realname.substr(0,pos) << std::endl;
  } else {
    appdata.setRealName( "NOBODY" );
  }
  //   p = create_passwd( appdata.UserName().c_str() );
  //  appdata.setPassword( p );

  if( (cp = strrchr( argv[0], '/' ) ) == NULL )
    cp = argv[0];
  else
    cp++;

  appdata.setProgName( cp );           /* Set Program-Name */
  appdata.setClassName( CLASSNAME );   /* set classname    */
  appdata.setVersion( VERSION );

  BUG_INFO(compose(_("This is %1 version %2")," I N T E N S ", AppData::Instance().Version()));
  BUG_INFO(compose(_("Revision: %1"), AppData::Instance().Revision()));
  BUG_INFO(compose(_("Build Date: %1"), AppData::Instance().RevisionDate()));

  get_groups( appdata.UserName().c_str() );
  struct utsname unamebuf;
  if ( uname(&unamebuf) != -1 )
    appdata.setHostName( unamebuf.nodename );
  else {
    appdata.setHostName( _("(unknown)") );
  }

  GuiFactory *guifactory;
#ifdef HAVE_HEADLESS
  guifactory = new HeadlessGuiFactory();
#else
  guifactory = new GuiQtFactory();
#endif

  UImanager::Instance().initApplication( &argc, argv, appdata.ClassName() );
  GuiManager::loadResourceFile( AppData::Instance().ResourceFile() );

  DataPoolIntens &dp = DataPoolIntens::Instance(); // init forcieren

  std::string outputFormat = AppData::Instance().OutputFormat();
  if( outputFormat.empty() )
    m_configurator = new InterpreterConfigurator;
  else if( outputFormat == "xml" )
    m_configurator = new InterpreterConfigurator;
  if ( argc > 1 ){
    int i=1;
    // now shift left all remaining arguments
    for(int j=i ; j<argc; ++j ){
      argv[j-i+1] = argv[j];
    }
    argc -= i-1;
  }

#ifndef __MINGW32__   // Absturz auf MinGW 2018-07-13
  // Initialize dynamic loader:
  if (lt_dlinit() != 0) {
    BUG_ERROR(  "Error during LTDL initialization:" << lt_dlerror() );
  }
#endif

}

App::~App(){
  delete m_flexer;
  delete m_configurator;
}

/* --------------------------------------------------------------------------- */
/* parseFile --                                                                */
/* --------------------------------------------------------------------------- */

//#include <qdatetime.h>
extern int   PAlineno;
bool App::parseFile(const std::string &filename) {
  std::ifstream ifs (filename.c_str());

  if (!ifs) {
    BUG_DEBUG("cannot open file: "<< filename);
    return false; // nothing to parse
  }
  PAlineno = 0; // reset line number
  //  DataPoolIntens::Instance().getDataPool().commitAllTransactions(); // needed for new datapool sections
  AppData &appdata = AppData::Instance();

  try {
    if ( ifs ){
      int l;
      appdata.setParserStartToken("POSTLOAD");

      delete m_flexer;
      m_flexer = new Flexer( &ifs, appdata.ParserStartToken() );
      m_flexer -> setCurrentFilename( filename );
    }

    GuiManager::Instance().dispatchPendingEvents();
    int y;

    if((y=yyparse())!=0){
      assert( m_flexer != 0);
      fprintf( yyerfp,"\n%s aborted.\n", appdata.ProgName().c_str() );
      return false;
    }

    return true;
  }
  catch(...) {
    std::cerr << "catched parser exception\n"<<std::flush;
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* parse --                                                                    */
/* --------------------------------------------------------------------------- */

bool App::parse( const std::string& parsestring ) {
  bool ret(false);

  if ( parsestring.size() == 0) {
    return false; // nothing to parse
  }
  PAlineno = 0; // reset line number
  //  DataPoolIntens::Instance().getDataPool().commitAllTransactions(); // needed for new datapool sections
  AppData &appdata = AppData::Instance();
  std::istream *is= 0;

  std::vector<std::string> parserModes;
  parserModes.push_back("POSTLOAD");
  parserModes.push_back("DESCRIPTION");
  std::vector<std::string>::iterator itMode = parserModes.begin();
  for (; itMode != parserModes.end(); ++itMode) {
    try {
      if (is) delete is;
      is = new std::istringstream( parsestring );
      delete m_flexer;
      appdata.setParserStartToken(itMode->c_str());
      m_flexer = new Flexer( is, appdata.ParserStartToken() );
      m_flexer -> setCurrentFilename( "" );

      GuiManager::Instance().dispatchPendingEvents();
      int y;

      if((y=yyparse())!=0){
        assert( m_flexer != 0);
        fprintf( yyerfp,"\n%s aborted.\n", appdata.ProgName().c_str() );
        continue;
      }

      ret = true;
      break;
    }
    //    catch(...) {
    catch(const std::exception& e) {
      std::cerr << "catched parser exception\n"<<std::flush;
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
  // must close file descriptor
  delete is;
  return ret;
}

/* --------------------------------------------------------------------------- */
/* parse --                                                                    */
/* --------------------------------------------------------------------------- */

bool App::parse( int &argc, char ** argv ){
  AppData &appdata = AppData::Instance();

  if( appdata.DisplayVersion() || appdata.DisplayHelp() )
    return true;

  if( appdata.createResFile() && appdata.DesFile().empty() ){
    return true;
  }

  if( appdata.ListFonts() ){
    return true;
  }

  if( appdata.DisplayQtGuiStyles() ){
    return true;
  }

  if( appdata.DisplayWhichGui() ){
    return true;
  }

  char* desFile = NULL;
  desFile = (char*)appdata.DesFile().c_str();

  std::istream *is= 0;
  std::string descFilePath;

//   if ( argc > 1 ){
  if ( desFile && strlen(desFile) ){
    int l;
    is = new std::ifstream( desFile );
    if( is->fail() ){
      perror( desFile );
      return false;
    }
    m_flexer = new Flexer( is, appdata.ParserStartToken() );

    std::string d=appdata.DesFile();
    std::string::size_type fpos = d.rfind( '/' );
    if( fpos == std::string::npos ){
      fpos=0;
    }
    else {
      fpos++;
    }
    std::string::size_type lpos = d.rfind( '.' );
    if( lpos == std::string::npos )
      lpos = d.size();

    if( fpos > 0 ){
      descFilePath= d.substr(0, fpos) ;
      m_flexer -> setCurrentFilename( d );
    }
    appdata.setLocaleDir( appdata.AppHome()+"/share/locale" );
    if (appdata.LocaleDomainName().size() > 0) { // use start option localdomain
      appdata.setLocaleDomain( appdata.LocaleDomainName() );
    }
    else {
      appdata.setLocaleDomain( d.substr( fpos, lpos-fpos ) );
    }
  }

  BUG_DEBUG(  "App::parse domain " << appdata.LocaleDomain() );

  GuiManager::Instance().dispatchPendingEvents();
  int y;

  if((y=yyparse())!=0){
    assert( m_flexer != 0);
    fprintf( yyerfp,"\n%s aborted.\n", appdata.ProgName().c_str() );
    return false;
  }

  // must close file descriptor
  delete is;

  if( AppData::Instance().OutputFormat() == "jsb" )
    return false;

  // print unimplemented functions
  if( appdata.PersistItemsFilename().empty()
      && appdata.ReflistFilename().empty()
      ) {
    JobManager::Instance().printUnimplementedFunctions( std::cerr );
  }

  if( appdata.PersistItemsFilename().empty()
      && appdata.ReflistFilename().empty() ){
    GuiManager::Instance().dispatchPendingEvents();
    StreamManager::Instance().fixupAllItemStreams();
    // parse MessageQueueReply.inc
    MessageQueue::parseIncludeFile();
    // parse UnitManager.inc
    UnitManager::Instance().parseIncludeFile();
  }
#ifndef _PARSER_ONLY

  // Start application and show Main-Form.
  if( appdata.PersistItemsFilename().empty()
      && appdata.ReflistFilename().empty() ){
    UImanager::Instance().createApplication();
  }
  // Initialize datapool with init file values
  if( !appdata.InitfileName().empty() ){
    StreamManager::Instance().readInitFile( appdata.InitfileName() );
  }

  FileStream::setTimestamp();

#endif
  return true;
}

/* --------------------------------------------------------------------------- */
/* getConfigurator                                                             */
/* --------------------------------------------------------------------------- */

Configurator *App::getConfigurator(){
  return m_configurator;
}

/* --------------------------------------------------------------------------- */
/* run --                                                                      */
/* --------------------------------------------------------------------------- */

int App::run(){
  AppData &appdata = AppData::Instance();

  if( !appdata.PersistItemsFilename().empty() ){
    DataPoolIntens::Instance().createDataPool();
    std::ofstream o( appdata.PersistItemsFilename().c_str() );
    if( o ){
      DataPoolIntens::Instance().printPersistentItems( o, appdata.PersistItemsForRest() );
    }
    else {
      std::cerr << "Intens: cannot open "
		<< appdata.PersistItemsFilename() << std::endl;
      return -1;
    }
    return  0 ;
  }
  if( !appdata.ReflistFilename().empty()){
    DataPoolIntens::Instance().createDataPool();
    std::ofstream o( appdata.ReflistFilename().c_str() );
    if( o ){
      createReflist( o );
    }
    else {
      std::cerr << "Intens: cannot open "
		<< appdata.ReflistFilename() << std::endl;
      return -1;
    }
    return  0 ;
  }

  if( appdata.DisplayVersion() ){
    return appdata.displayVersion() ? 0 : -1;
  }
  if( appdata.DisplayHelp() ){
    appdata.displayHelp();
    return 0;
  }
  if( appdata.createResFile() && appdata.DesFile().empty() ){
    GuiManager::writeResourceFile();
    return 0;
  }
  if( appdata.ListFonts() ){
    appdata.listFonts();
    return 0;
  }
  if( appdata.DisplayQtGuiStyles() ){
    appdata.displayQtGuiStyles();
    return 0;
  }
  if( appdata.DisplayWhichGui() ){
    appdata.whichGui();
    return 0;
  }

  if (appdata.LspWorker()) {
    std::cout << "<DICT>" << std::endl;
    DataPoolIntens::Instance().getDataPool().GetRootDict()->writeDD( std::cout );
    GuiElement::lspWrite( std::cout );
    GuiFolderGroup::lspWrite( std::cout );
    DataPoolIntens::Instance().lspWrite( std::cout );
    StreamManager::Instance().lspWrite( std::cout );
    MessageQueue::Instance().lspWrite( std::cout );
    JobManager::Instance().serializeFunctions( std::cout, AppData::serialize_XML);
    std::cout << "</DICT>" << std::endl;
    return 0;
  }

  MessageQueue::initialise();

  UImanager::Instance().startApplication(); // End of Program
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createReflist --                                                            */
/* --------------------------------------------------------------------------- */

void App::createReflist( std::ostream &os ){
  os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  os << "<?xml-stylesheet type=\"text/xsl\" href=\"ref.xsl\"?>" << std::endl;
  os << "<ref>" << std::endl;

  XMLStreamParameter datapool(0, false, false );
  datapool.addAttribute("function");
  datapool.write( os );

  JobManager::Instance().serializeFunctions( os, AppData::serialize_XML );
  ProcessGroup::serializeAll( os );

  IntensServerSocket::serialize( os, AppData::serialize_XML );
  os << "</ref>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* log --                                                                      */
/* --------------------------------------------------------------------------- */

void App::log( const std::string &s ){
  if( m_log.size() > 99 ){
    m_log.pop_front();
  }
  m_log.push_back( s );
}

/* --------------------------------------------------------------------------- */
/* printLog --                                                                 */
/* --------------------------------------------------------------------------- */

void App::printLog(){
  std::list<std::string>::iterator iter;
  for( iter = m_log.begin();iter != m_log.end(); ++iter ){
    std::cerr << *iter << std::endl;
  }
}
