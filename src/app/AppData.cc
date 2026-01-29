#include <iostream>
#include <assert.h>

#include <stdlib.h> // atoi

#include <string>
#include <iomanip>
#include <libintl.h>
#include <getopt.h>

#include "app/AppData.h"
#include "app/App.h"
#ifdef HAVE_OAUTH
#include "app/oauthclient.h"
#endif
#include "utils/utils.h"

#include "gui/GuiElement.h"
#include "gui/GuiPlotDataItem.h"
#include "gui/GuiListenerController.h"
#include "job/JobCodeStandards.h"

#if defined(BISON_USE_PARSER_H_EXTENSION)
#include "parser/parseryacc.h"
#else
#include "parser/parseryacc.hh"
#endif
#include "intens_revision.h"

#include "gui/GuiManager.h"
//#ifdef HAVE_QT
#if defined HAVE_QT
#if !defined HAVE_HEADLESS
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQtManager.h"
#endif
// Windows must be undefined to prevent
// strange compile errors in qsettings.h (line 238)
// ..qsettings.h:238: error: expected identifier before ‘false’
#undef Windows
#include <QSettings>
#include <QTextStream>
#include <qstylefactory.h>
#endif

INIT_LOGGER();


/*******************************************************************************/
/* static variables                                                            */
/*******************************************************************************/

// #ifdef HAVE_QT
static struct option long_options[] = {
   {"geometry",     required_argument, 0,  0}
  ,{"name",         required_argument, 0,  0}
  ,{"display",      required_argument, 0,  0}
  ,{"matlabnode",   required_argument, 0,  0}
  ,{"fontname",     required_argument, 0,  0}
  ,{"background",   required_argument, 0,  0}
  ,{"xfontlist",    required_argument, 0,  0}
  ,{"listFonts",    no_argument,       0,  0}
  ,{"init",         required_argument, 0,  0}
  ,{"notitle",      no_argument,       0,  0}
  ,{"shortMainTitle", no_argument,     0,  0}
  ,{"toolbar",      no_argument,       0,  0}
  ,{"undo",         no_argument,       0,  0}
  ,{"helpmsg",      no_argument,       0,  0}
  ,{"resfile",      required_argument, 0,  0}
  ,{"createRes",    required_argument, 0,  0}
  ,{"qtGuiStyle",   required_argument, 0,  0}
  ,{"listQtGuiStyles",  no_argument,   0,  0}
  ,{"localeDomain", required_argument, 0,  0}
  ,{"maxoptions",   required_argument, 0,  0}
  ,{"maxlines",     required_argument, 0,  0}
  ,{"toolTipDuration", required_argument, 0,  0}
  ,{"leftIcon",     required_argument, 0,  0}
  ,{"rightIcon",    required_argument, 0,  0}
  ,{"startupImage", required_argument, 0,  0}
  ,{"rolefile",     required_argument, 0,  0}
  ,{"includePath",  required_argument, 0,  0}
  ,{"pspreviewer",  required_argument, 0,  0}
  ,{"disableSVGSupport", no_argument,       0,  0}
  ,{"printerConfig",required_argument, 0,  0}
  ,{"jsb",          required_argument, 0,  0}
  ,{"xml",          required_argument, 0,  0}
  ,{"xmlPath",      required_argument, 0,  0}
  ,{"apprunPath",   required_argument, 0,  0}
  ,{"dbdriver",     required_argument, 0,  0}
  ,{"dbautologon",  no_argument,       0,  0}
  ,{"debug",        required_argument, 0,  0}
  ,{"persistfile",           required_argument, 0,  0}
  ,{"persistfileREST",       required_argument, 0,  0}
  ,{"logconfig",    required_argument, 0,  0}
  ,{"log4cplusPropertiesFile", required_argument, 0,  0}
  ,{"startToken",   required_argument, 0,  0}
  ,{"reflistfile",  required_argument, 0,  0}
  ,{"helpdir",      required_argument, 0,  0}
  ,{"version",      no_argument,       0,  0}
  ,{"help",         no_argument,       0,  0}
  ,{"withTargetStreamInfo", no_argument,  0,  0}
  ,{"whichGui",     no_argument,       0,  0}
  ,{"withWheelEvent", no_argument,  0,  0}
  ,{"withInputStructFunc", no_argument,  0,  0}
  ,{"withoutArrowKeys", no_argument,  0,  0}
  ,{"withKeypadDecimalPoint", no_argument,  0,  0}
  ,{"defaultScaleFactor1", no_argument,  0,  0}
  ,{"withoutEditableComboBox", no_argument,  0,  0}
  ,{"withoutTextPopupMenu", no_argument,  0,  0}
  ,{"withoutRangeCheck", no_argument,  0,  0}
  ,{"test",         required_argument,       0,  0}
  ,{"testmode", no_argument, 0, 0}
  ,{"noInitFunc", no_argument, 0,  0}
  ,{"replyPort",    required_argument,       0,  0}
  ,{"sendMessageQueueWithMetadata", no_argument,  0,  0}
  ,{"defaultMessageQueueDependencies", required_argument,  0,  0}
#ifdef HAVE_OAUTH
  ,{"oauth",     required_argument, 0,  0}
  ,{"oauthAccessTokenUrl",     required_argument, 0,  0}
  ,{"oauthScopes",     required_argument, 0,  0}
#endif
  ,{"opentelemetryMetadata", no_argument,  0,  0}
  ,{"lspWorker", no_argument,  0,  0}
  ,{"unitManager", optional_argument,  0,  0}
  // Do not ever remove this last line. If you do, then something very horrible will happen!
  ,{0,              0,                 0,  0}
};
//#endif


/*******************************************************************************/
/* initialize static variable                                                  */
/*******************************************************************************/

AppData *AppData::s_instance = 0;

/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/

AppData::AppData()
  : m_debug( 0 )
  , m_mlString( false )
  , m_app_titlebar( false )
  , m_app_shortMainTitle( false )
  , m_app_toolbar( false )
  , m_undo( false )
  , m_helpmessages( NoneType )
  , m_maxoptions( 0 )
  , m_maxlines( 0 )
  , m_toolTipDuration( -1 )
  , m_marginOffset(0)
  , m_featureMatlab(false)
  , m_disableFeatureSVG(false)
  , m_resCreate(false)
  , m_dbAutologon( false )
  , m_persistItemsForRest( false )
  , m_displayVersion( false )
  , m_displayHelp( false )
  , m_listFonts( false )
  , m_withTargetStreamInfo( false )
  , m_displayWhichGui( true )
  , m_withGuiWheelEvent( false )
  , m_withArrowKeys( false )
  , m_withKeypadDecimalPoint( false )
  , m_defaultScaleFactor1( false )
  , m_withInputStructFunc( false )
  , m_comboBoxEditable( false )
  , m_withTextPopupMenu( false )
  , m_withRangeCheck( true )
  , m_parserStartToken( 0 )
  , m_testModeExitCode( successExitCode )
  , m_noInitFunc( false )
  , m_replyPort( 0 )
  , m_sendMessageQueueWithMetadata(false)
  , m_defaultMessageQueueDependencies(true)
  , m_pyLogMode( false )
  , m_oauthClient(0)
  , m_opentelemetry_metadata(false)
  , m_lspWorker(false)
  , m_unitManagerFeature(unitManagerFeature_none)
  , m_testMode(false)
{
  const char *a = getenv("APPHOME");
  if( a ){
    m_appHome = a;
  }
}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

AppData &AppData::Instance(){
  if( s_instance == 0 ){
    s_instance = new AppData;
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* getApplicationResources --                                                  */
/* --------------------------------------------------------------------------- */

void AppData::getApplicationResources(){

  // standard previewer
#if defined HAVE_QT && !defined HAVE_HEADLESS
  if( GuiQtManager::Instance().Settings() ) {
    m_psPreviewer = GuiQtManager::Instance().Settings()->value
 					   ( "Intens/Previewer", "").toString().toStdString();
  }
#endif
  if (m_psPreviewer.empty()) {
#if  __MINGW32__
    m_psPreviewer = "";
#elif  __linux__
    m_psPreviewer = "acroread";
#else
    m_psPreviewer = "xdg-open";
#endif
  }

  // High DPI Display Migrate Existing Qt Applications
  mysetenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
  char *screen_scale = getenv("QT_AUTO_SCREEN_SCALE_FACTOR");
  char *scale_factor = getenv("QT_SCALE_FACTOR");
  char *screen_scale_factors = getenv("QT_SCREEN_SCALE_FACTORS");
  if (scale_factor) {
    BUG_INFO("QT_SCALE_FACTOR: " << scale_factor);
  }
  if (screen_scale_factors) {
    BUG_INFO("QT_SCREEN_SCALE_FACTORS: " << screen_scale_factors);
  }
  BUG_INFO("QT_AUTO_SCREEN_SCALE_FACTOR: " << screen_scale);

}

/* --------------------------------------------------------------------------- */
/* setResource --                                                              */
/* --------------------------------------------------------------------------- */

void AppData::setResource(const std::string& key, std::string& stringValue){
  BUG( BugMLS, "AppData:setResource" );
#if defined HAVE_QT && !defined HAVE_HEADLESS
  // set resource in resource file
  if( GuiQtManager::Settings() ) {
    GuiQtManager::Settings()->setValue( QString::fromStdString("Resource/"+key), QString::fromStdString(stringValue));
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* setResource --                                                              */
/* --------------------------------------------------------------------------- */

void AppData::setResource(const std::string& key, double doubleValue){
  BUG( BugMLS, "AppData:setResource" );
#if defined HAVE_QT && !defined HAVE_HEADLESS
  // set resource in resource file
  if( GuiQtManager::Settings() ) {
    GuiQtManager::Settings()->setValue( QString::fromStdString("Resource/"+key), doubleValue);
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* setResource --                                                              */
/* --------------------------------------------------------------------------- */

void AppData::setResource(const std::string& key, int intValue){
  BUG( BugMLS, "AppData:setResource" );
#if defined HAVE_QT && !defined HAVE_HEADLESS
  // set resource in resource file
  if( GuiQtManager::Settings() ) {
    GuiQtManager::Settings()->setValue( QString::fromStdString("Resource/"+key), intValue);
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* getResource --                                                              */
/* --------------------------------------------------------------------------- */

int AppData::getResource(const std::string& key,
                         std::string& stringValue, double& doubleValue, int& intValue){
  BUG( BugMLS, "AppData:getResource" );
#if defined HAVE_QT && !defined HAVE_HEADLESS
  QVariant val;

  if (AppData::Instance().LspWorker()) {
    intValue = 2048;
    return tINT_CONSTANT;
  }

  // if not, get resource from enviroment variable
  if (!val.isValid()) {
    const char *s = getenv(key.c_str());
    if (s) {
      val = QVariant(s);
    }
  }

  // get resource from resource file
  if( GuiQtManager::Settings() && !val.isValid()) {
    val = GuiQtManager::Settings()->value( QString::fromStdString("Resource/"+key));
  }

  // IS_WEBTENS, REST_SERVICE_BASE, KUBERNETES_SERVICE_HOST and ReplyPort ckeck
  // no parser error if undefined
  if (!val.isValid() && AppData::ReplyPort() > 0){
    if (key == "IS_WEBTENS"){
      val = 0;
    }
    if (key == "REST_SERVICE_BASE" ||
        key == "KUBERNETES_SERVICE_HOST"){
      val = "";
    }
  }

  // if not and a TestResource return an empty string else Return RESOURCE_UNKNOWN
  if (!val.isValid()) {
    if (hasTestModeFunc() ||
        (!hasTestModeFunc() && QString::fromStdString(key).startsWith("TEST_"))) {
      //      std::cerr << "resource unknown: '"<< key << "'" <<std::endl;
      stringValue = "";
      return tSTRING_CONSTANT;
    }
    ///    val = QVariant(QString::fromStdString(key));
    return tRESOURCE_UNKNOWN;
  }

  // detect type of value
  if (val.isValid()) {
    bool okInt(false), okReal(false);
    int i = val.toInt(&okInt);
    if (okInt) {
      intValue = i;
      BUG_EXIT("Result type Integer Value '"<< intValue << "'");
      return tINT_CONSTANT;
    } else {
      double d = val.toReal(&okReal);
      if (okReal) {
        doubleValue = d;
        BUG_EXIT("Result type Integer Value '"<< doubleValue << "'");
        return tREAL_CONSTANT;
      }
    }

    stringValue = val.toString().toStdString();
    BUG_EXIT("Result type String Value '"<< stringValue << "'");
    return tSTRING_CONSTANT;
  } else {
    // should never happen
    stringValue = key;
    return tSTRING_CONSTANT;
  }
#endif
  return tRESOURCE_UNKNOWN;
}

/* --------------------------------------------------------------------------- */
/* set<Functions> --                                                           */
/* --------------------------------------------------------------------------- */

void AppData::setDebug( int level )                      { m_debug = level; }
void AppData::setProgName( const std::string &p )        { m_progname = p; }
void AppData::setClassName( const std::string &c )       { m_classname = c; }
void AppData::setVersion( const std::string &v )         { m_version = v; }
void AppData::setUserName( const std::string &n )        { m_username = n; }
void AppData::setRealName( const std::string &n )        { m_realname = n; }
void AppData::setPassword( const std::string &p )        { m_password = p; }
void AppData::setHostName( const std::string &h )        { m_hostname = h; }
void AppData::setIntensHome( const std::string &h )      { m_intensHome = h; }
void AppData::setLocaleDir( const std::string &d )       { m_localeDir = d; }
void AppData::setLocaleDomain( const std::string &d )    {
  m_localeDomain = d;
  bindtextdomain(d.c_str(), LocaleDir().c_str() );
  bind_textdomain_codeset(d.c_str(), "UTF-8");
}
void AppData::setLocaleDomainName( const std::string &d )    {
  m_localeDomainName = d;
}
void AppData::setLogConfig( const std::string &l )       { m_logconfig = l; }
void AppData::setPyLogMode( bool f )                     { m_pyLogMode = f; }
void AppData::setRolefileName( const std::string &r )    { m_roleFile = r; }
void AppData::setOrganization( const std::string &o )    { m_organization = o; }
void AppData::setDivision( const std::string &d )        { m_division = d; }
void AppData::setSite( const std::string &s )            { m_site = s; }
void AppData::setLicenseKey( const std::string &k )      { m_licenseKey = k; }
void AppData::setMatlabnode( const std::string &n )      { m_matlabnode = n; }
void AppData::setTitle( const std::string &t )           { if ( m_title.empty() ) m_title = t; }
void AppData::setMaxOptions( int i )                     { m_maxoptions = i; }
void AppData::setAppToolbar( bool b )                    { m_app_toolbar = b; }
void AppData::setAppTitlebar( bool b )                   { m_app_titlebar = b; }
void AppData::setAppShortMainTitle( bool b )             { m_app_shortMainTitle = b; }
void AppData::setUndo()                                  { m_undo = true; }
void AppData::setHelpmessages( AppData::HelpMessageType type ) { m_helpmessages = type; }
void AppData::setMarginOffset( int m )                   { m_marginOffset = m; }
void AppData::setDisplayName( const std::string &d )     { m_displayname = d; }
void AppData::setStartupImage( const std::string &i )    { m_startupImage = i; }
void AppData::setLeftTitleIcon( const std::string &l )   { m_leftTitleIcon = l; }
void AppData::setRightTitleIcon( const std::string &r )  { m_rightTitleIcon = r; }
void AppData::setPsPreviewer( const std::string &p )     { m_psPreviewer = p; }
void AppData::setPrinterConfig( const std::string &p )   { m_printerConfig = p; }
void AppData::setFeatureMatlab( bool f )                 { m_featureMatlab = f; }
void AppData::setDisableFeatureSVG( bool f )             { m_disableFeatureSVG = f; }
void AppData::setOAuth( const std::string &s )           { m_oauth = s; }
void AppData::setOAuthAccessTokenUrl( const std::string &s )           { m_oauthAccessTokenUrl = s; }
void AppData::setOAuthScopes( const std::string &s )     { m_oauthScopes = s; }

void AppData::setPersistItemsFilename( const char *filename, bool restdb ){
  m_persistItemsFilename = filename;
  m_persistItemsForRest = restdb;
}

void AppData::setLog4cplusPropertiesFilename( const char *filename ){
  m_log4cplusPropertiesFilename = filename;
}

void AppData::setParserStartToken( const char *token ){
  if( std::string(token)=="DATAPOOL")
    m_parserStartToken = START_DATAPOOL;
  else
    if( std::string(token)=="POSTLOAD")
      m_parserStartToken = START_POSTLOAD;
    else
      m_parserStartToken = 0;
}

void AppData::setReflistFilename( const char *filename ){
  m_reflistFilename = filename;
}

void AppData::setHelpDirectory( const char *dirname ){
  m_helpdir = dirname;
  if( m_helpdir.size()>0 ){
    if( m_helpdir[ m_helpdir.size()-1 ]!='/' )
      m_helpdir += "/";
  }
}
void AppData::setFontname( const std::string &n )        { m_fontname = n;}
void AppData::setInitfileName( const std::string &n )    { m_initfile = n;}
void AppData::setMaxLines( int l )                       { m_maxlines = l; }
void AppData::setToolTipDuration( int sec )              { m_toolTipDuration = sec; }
void AppData::setIncludePath( const std::string &n )     { m_includePath = n; }

void AppData::setResfile( const std::string &n )         { m_resfile = n;}
void AppData::setXmlPath( const std::string &n )         { m_xmlPath = n; }
void AppData::setApprunPath( const std::string &n )      { m_apprunPath = n; }
void AppData::setOutputFile( const std::string &n )      { m_outputFile = n; }
void AppData::setDBdriver( const std::string &n )        { m_dbdriver = n; }
void AppData::setDBAutologon( bool d )                   { m_dbAutologon = d; }

void AppData::setXFontList( const std::string &n )       { m_xfontlist = n; }
void AppData::setDesFile( const std::string &n )         { m_desFile = n;}

void AppData::setResCreate( bool f )                     { m_resCreate = f;}
void AppData::setNewResFileName( const std::string &f)   { m_newResFile= f; }

void AppData::setDisplayVersion( bool d )                { m_displayVersion = d;}
void AppData::setDisplayHelp( bool d )                   { m_displayHelp    = d;}

void AppData::setListFonts( bool d )                     { m_listFonts = d;}
void AppData::setQtGuiStyle( const std::string &n )      { m_qtGuiStyle = n;}
void AppData::setDisplayQtGuiStyles( bool d )            { m_displayQtGuiStyles = d;}
void AppData::setTargetStreamInfo( bool d )              { m_withTargetStreamInfo = d;}
void AppData::setDisplayWhichGui( bool d )               { m_displayWhichGui = d;}
void AppData::setGuiWheelEvent( bool d )                 { m_withGuiWheelEvent = d;}
void AppData::setArrowKeys( bool d )                     { m_withArrowKeys = d;}
void AppData::setKeypadDecimalPoint( bool d )            { m_withKeypadDecimalPoint = d;}
void AppData::setDefaultScaleFactor1( bool d )           { m_defaultScaleFactor1 = d;}
void AppData::setInputStructFunc( bool d )               { m_withInputStructFunc = d;}
void AppData::setGuiComboBoxEditable( bool d )           { m_comboBoxEditable = d;}
void AppData::setGuiTextPopupMenu( bool d )              { m_withTextPopupMenu = d;}
void AppData::setGuiRangeCheck( bool d )                 { m_withRangeCheck = d;}
void AppData::setTestModeFunc( const std::string &f )    { m_testModeFunc = f; }
void AppData::setReplyPort( int port )                   { m_replyPort = port; }
void AppData::setSendMessageQueueWithMetadata(bool b)    { m_sendMessageQueueWithMetadata = b; }
void AppData::setDefaultMessageQueueDependencies(bool b) {
  m_defaultMessageQueueDependencies = b;
}
void AppData::setOpenTelemetryMetadata()                 { m_opentelemetry_metadata= true; }
void AppData::setLspWorker()                             { m_lspWorker = true; }

/* --------------------------------------------------------------------------- */
/* addUserGroup --                                                             */
/* --------------------------------------------------------------------------- */

void AppData::addUserGroup( const std::string &group ){
  UserGroupList::iterator ui;
  for( ui = m_grouplist.begin(); ui != m_grouplist.end(); ++ui ){
    if( (*ui) == group ){
      return;
    }
  }
  m_grouplist.push_back( group );
}

/* --------------------------------------------------------------------------- */
/* checkUserGroup --                                                           */
/* --------------------------------------------------------------------------- */

bool AppData::checkUserGroup( const std::string &group ){
  UserGroupList::iterator ui;
  for( ui = m_grouplist.begin(); ui != m_grouplist.end(); ++ui ){
    if( (*ui) == group ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* addUserGroupsGroup --                                                       */
/* --------------------------------------------------------------------------- */

void AppData::addUserGroupsGroup( const std::string &name, bool ok ){
  UserGroupGroups::iterator g = m_usergroups.find( name );
  if( g != m_usergroups.end() ){
    assert( false );
  }
  m_usergroups.insert( UserGroupGroups::value_type( name, ok ) );
}

/* --------------------------------------------------------------------------- */
/* checkUserGroupsGroup --                                                     */
/* --------------------------------------------------------------------------- */

bool AppData::checkUserGroupsGroup( const std::string &name ){
  UserGroupGroups::iterator g = m_usergroups.find( name );
  if( g == m_usergroups.end() ){
    assert( false );
  }
  return (*g).second;
}

static std::vector<std::string> split( std::string s, std::string delimiter ){
  size_t pos = 0;
  std::vector<std::string> result;
  while ( (pos = s.find(delimiter)) != std::string::npos ){
    result.push_back( s.substr(0, pos) );
    s.erase( 0, pos + delimiter.length() );
  }
  if(!s.empty()) {
    result.push_back(s);
  }
  return result;
}

/* --------------------------------------------------------------------------- */
/* get<Functions> --                                                           */
/* --------------------------------------------------------------------------- */

int AppData::Debug()                            { return m_debug; }
bool AppData::isWindows() {
#if defined Q_OS_WIN || defined Q_OS_CYGWIN
  return true;
#endif
  return false;
}
const std::string &AppData::ProgName()          { return m_progname; }
const std::string &AppData::ClassName()         { return m_classname; }
const std::string &AppData::Version()           { return m_version; }
const std::string AppData::VersionString()      {
  std::vector<std::string> list = split(m_version,".");
  std::stringstream stream;
  for (int i = 0; i < list.size(); ++i) {
    int num;
    size_t pos = 0;
    if ((pos = list[i].find("-")) == std::string::npos) {
      sscanf(list[i].c_str(),"%d",&num);
      stream << std::setfill('0') << std::setw(3) << num;
      if (i < list.size() - 1) {
        stream << std::setw(1) << ".";
      }
    } else {
      std::vector<std::string> list2 = split(list[i],"-");
      if (list2.size() == 2) {
	sscanf(list2[0].c_str(),"%d",&num);
	stream << std::setfill('0') << std::setw(3) << num;
	stream << std::setw(1) << "-" << list2[1];
      } else
	stream << list[i];
    }
  }
  return stream.str();
}
const std::string &AppData::Revision()          {
  if (m_revision.empty()) {
    m_revision = RevisionString;
  }
  return m_revision;
}
const std::string &AppData::RevisionDate()      {
  if (!m_revisionDate.size()) {
    m_revisionDate = RevisionDateString;
  }
  return m_revisionDate;
}
const std::string &AppData::UserName()          { return m_username; }
const std::string &AppData::RealName()          { return m_realname; }
const std::string &AppData::Password()          { return m_password; }
const std::string &AppData::HostName()          { return m_hostname; }
const std::string &AppData::IntensHome()        { return m_intensHome; }
const std::string &AppData::AppHome()           { return m_appHome; }
const std::string &AppData::LocaleDir()         { return m_localeDir; }
const std::string &AppData::LocaleDomain()      { return m_localeDomain; }
const std::string &AppData::LocaleDomainName()  { return m_localeDomainName; }
const std::string &AppData::LogConfig()         { return m_logconfig; }
const bool AppData::PyLogMode()                 { return m_pyLogMode; }
const std::string &AppData::RolefileName()      { return m_roleFile; }
const std::string &AppData::Organization()      { return m_organization; }
const std::string &AppData::Division()          { return m_division; }
const std::string &AppData::Site()              { return m_site; }
const std::string &AppData::LicenseKey()        { return m_licenseKey; }
const std::string &AppData::InitfileName()      { return m_initfile; }
const std::string &AppData::DisplayName() const { return m_displayname; }
const std::string &AppData::MatlabNode() const  { return m_matlabnode; }
bool AppData::MlString()                        { return m_mlString; }
bool AppData::AppTitlebar()                     { return m_app_titlebar; }
bool AppData::AppShortMainTitle()               { return m_app_shortMainTitle; }
bool AppData::AppToolbar()                      { return m_app_toolbar; }
bool AppData::Undo()                            { return m_undo; }
AppData::HelpMessageType AppData::Helpmessages(){ return m_helpmessages; }
const std::string &AppData::Title()             { return m_title; }
int AppData::MaxOptions()                       { return m_maxoptions; }
int AppData::MaxLines()                         { return m_maxlines; }
int AppData::ToolTipDuration()                  {
  return m_toolTipDuration > 0 ? 1000*m_toolTipDuration : m_toolTipDuration;
}
const std::string &AppData::FontName()          { return m_fontname; }
const std::string &AppData::XFontList()         { return m_xfontlist; }
const std::string &AppData::ResourceFile()      { return m_resfile; }
int AppData::MarginOffset()                     { return m_marginOffset; }
const std::string &AppData::StartupImage() const  { return m_startupImage; }
const std::string &AppData::LeftTitleIcon() const { return m_leftTitleIcon; }
const std::string &AppData::RightTitleIcon()const { return m_rightTitleIcon; }
const std::string &AppData::IncludePath()const    { return m_includePath; }
const std::string &AppData::PrinterConfig()const  { return m_printerConfig; }
const std::string &AppData::OutputFormat()const   { return m_outputFormat; }
const std::string &AppData::OutputFile()const     { return m_outputFile; }
const std::string &AppData::PsPreviewer()const    { return m_psPreviewer; }
const bool  AppData::FeatureMatlab( )             { return m_featureMatlab; }
const bool  AppData::disableFeatureSVG() const    { return m_disableFeatureSVG; }
const std::string &AppData::XmlPath()const        { return m_xmlPath; }
const std::string &AppData::ApprunPath()const     { return m_apprunPath; }
const std::string &AppData::DBdriver()const       { return m_dbdriver; }
const bool AppData::DBAutologon()const            { return m_dbAutologon; }
const std::string &AppData::DesFile()const        { return m_desFile; }
const std::string& AppData::TestModeFunc()        { return m_testModeFunc; }
const std::string &AppData::OAuth()               { return m_oauth; }
const std::string &AppData::OAuthAccessTokenUrl()               { return m_oauthAccessTokenUrl; }
const std::string &AppData::OAuthScopes()               { return m_oauthScopes; }
void AppData::runOAuthClient(UserPasswordListener* listener) {
  if (!OAuth().size()) return;

#ifdef HAVE_OAUTH
  if (!m_oauthClient && OAuth().size()) {
    m_oauthClient = new OAuthClient(QString::fromStdString(OAuth()),
                                    QString::fromStdString(OAuthAccessTokenUrl()));
  }
  m_oauthClient->setListener(listener);
  m_oauthClient->grant();
#endif
}

std::string AppData::OAuthToken()  {
#ifdef HAVE_OAUTH
  return m_oauthClient ? m_oauthClient->token() : "";
#else
  return "";
#endif
}
bool AppData::OpenTelemetryMetadata() const       { return m_opentelemetry_metadata; }
bool AppData::LspWorker() const                   { return m_lspWorker; }
bool AppData::hasUnitManagerFeature() const       {
  return m_unitManagerFeature != unitManagerFeature_none;
}
AppData::UnitManagerFeature AppData::getUnitManagerFeature() const { return m_unitManagerFeature; }
void AppData::setUnitManagerFeature(UnitManagerFeature featureCB){
  m_unitManagerFeature = featureCB;
}
const bool AppData::hasTestModeFunc() const       { return (m_testModeFunc.size() > 0); }
const bool AppData::TestMode() const              { return m_testMode; }
const bool AppData::NoInitFunc() const            { return m_noInitFunc; }
int AppData::ReplyPort()                          { return m_replyPort; }
bool AppData::HeadlessWebMode() {
  char *c = getenv("IS_WEBTENS");
  if (!c)
    c = getenv("KUBERNETES_SERVICE_HOST");
  return AppData::ReplyPort() > 0 && c && *c == '1';
}
bool AppData::sendMessageQueueWithMetadata()      { return m_sendMessageQueueWithMetadata; }
bool AppData::getMessageQueueDependencies(int option) {
  if (option == 0) return false;
  if (option == 1) return true;
  if (option == -1) {
    return m_defaultMessageQueueDependencies;
  }
  return false;
}
AppData::TestModeExitCode AppData::getTestModeExitCode() {
  return m_testModeExitCode;
}
void AppData::setTestModeExitCode(TestModeExitCode exitCode) {
  // only override if major exitcode
  if (m_testModeExitCode <  exitCode)
    m_testModeExitCode = exitCode;
}
const std::string &AppData::PersistItemsFilename() const{
  return m_persistItemsFilename;
}
bool AppData::PersistItemsForRest() const{
  return m_persistItemsForRest;
}
const std::string &AppData::Log4cplusPropertiesFilename() const{
  return m_log4cplusPropertiesFilename;
}
const int AppData::ParserStartToken() const{ return m_parserStartToken; }

const std::string &AppData::ReflistFilename() const{
  return m_reflistFilename;
}

bool AppData::createResFile()              { return m_resCreate;}
const std::string &AppData::newResFileName() const { return m_newResFile; }
bool AppData::DisplayVersion() { return m_displayVersion; }
bool AppData::DisplayHelp() { return m_displayHelp; }
bool AppData::ListFonts()   { return m_listFonts;}
const std::string &AppData::QtGuiStyle()const { return m_qtGuiStyle; }
bool AppData::DisplayQtGuiStyles() { return m_displayQtGuiStyles; }
bool AppData::GuiWheelEvent() const  { return m_withGuiWheelEvent;}
bool AppData::ArrowKeys() const      { return m_withArrowKeys;}
bool AppData::KeypadDecimalPoint() const  { return m_withKeypadDecimalPoint;}
bool AppData::DefaultScaleFactor1() const { return m_defaultScaleFactor1;}
bool AppData::InputStructFunc() const     { return m_withInputStructFunc;}
bool AppData::GuiComboBoxEditable() const { return m_comboBoxEditable; }
bool AppData::GuiWithTextPopuMenu() const { return m_withTextPopupMenu; }
bool AppData::GuiWithRangeCheck() const { return m_withRangeCheck; }
bool AppData::TargetStreamInfo() const { return m_withTargetStreamInfo; }
bool AppData::DisplayWhichGui() { return m_displayWhichGui;}


/* --------------------------------------------------------------------------- */
/* HelpFileDirectory --                                                        */
/* --------------------------------------------------------------------------- */
const std::string AppData::HelpFileDirectory(){
  BUG( BugMLSInterface, "AppData::HelpFileDirectory" );
  if( m_helpdir.size() >0 )
    return m_helpdir;
  else
    return  FileUtilities::GetCurrentWorkDirectory();
}

/* --------------------------------------------------------------------------- */
/* setDefaultOpts --                                                           */
/* --------------------------------------------------------------------------- */
void AppData::setDefaultOpts(){
  setMatlabnode( "" );
  setFontname( "Courier,Courier-Bold" );
  setLogConfig( "" );
  setInitfileName( "" );
  setAppTitlebar( true );
  setAppShortMainTitle( false );
  setHelpmessages( NoneType );
#ifndef HAVE_QT
  setResfile( "./intensrc" );
#endif
  setMaxOptions( 25 );
  setMaxLines( 500 );
  setToolTipDuration( -1 );
  setLeftTitleIcon( "semafor" );
  setRightTitleIcon( "" );
  setStartupImage( "" );
  setRolefileName( "" );
  setIncludePath( "" );
#ifdef __linux__
  setPsPreviewer( "xdg-open" ); // acroread
#else
  setPsPreviewer( "gv -antialias" ); // "acroread"
#endif
//   m_psPreviewer = "gv";
  setPrinterConfig( "printers.conf" );
  setXmlPath( "" );
  setApprunPath( "" );
  setXFontList( "-*-clean-medium-r-*,-*-clean-bold-r-*" );
  setDisplayVersion( false );
  setDisplayHelp( false );
  setListFonts( false );
  setDisplayQtGuiStyles( false );
  setTargetStreamInfo( false );
  setDisplayWhichGui( false );
  setGuiWheelEvent( false );
  setArrowKeys( true );
  setKeypadDecimalPoint( false );
  setDefaultScaleFactor1( false );
  setInputStructFunc( false );
  setGuiComboBoxEditable( true );
  setGuiTextPopupMenu( true );
  setDBdriver( "dbora.so" );
  setDBAutologon( false );
  setSendMessageQueueWithMetadata( false );
  setOAuth( "" );
  setOAuthAccessTokenUrl( "" );
  setOAuthScopes( "" );

#if HAVE_LOG4CPLUS
  std::string fn = (m_appHome.empty() ? "." : m_appHome) + "/config/log4cplus.properties";
  setLog4cplusPropertiesFilename(fn.c_str());
#endif
}


void AppData::writeSettings() {
#if defined HAVE_QT && !defined HAVE_HEADLESS
  if( GuiQtManager::Instance().Settings() ) {
    GuiQtManager::Instance().Settings()->setValue
      ( "Intens/HelpMessageType", Helpmessages());
  }
#endif
}
/* --------------------------------------------------------------------------- */
/* getOpt --                                                                   */
/* --------------------------------------------------------------------------- */
void AppData::getOpt(int &argc, char **argv){
  setDefaultOpts();

  int c;

  while ( argc > 1) {
#if  __MINGW32__
    // --help: MinGW says help is ambiguous (helpmsg, helpdir)
    // msys2 does it correct, so this code is to be removed again
    // in some years ;)
    if(optind < argc && strcmp(argv[optind], "--help") == 0) {
      setDisplayHelp();
      ++optind;
      continue;
    }
#endif

    int option_index = 0;
    c = getopt_long (argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    if (c == 0){
      const char* optName = long_options[option_index].name;
      // if( strcmp( optName, "geometry")==0){
      //   ...
      // }
      // if( strcmp( optName, "name")==0){
      //   ...
      // }
      // if( strcmp( optName, "display")==0){
      //   ...
      // }
      if( strcmp( optName, "matlabnode")==0){
        if (optarg) setMatlabnode( optarg );
      }
      else if( strcmp( optName, "fontname")==0){
        if (optarg) setFontname( optarg );
      }
      // else if( strcmp( optName, "background")==0){
      //   ...
      // }
      else if( strcmp( optName, "xfontlist")==0){
        if (optarg) setXFontList( optarg );
      }
      else if( strcmp( optName, "listFonts")==0){
        setListFonts( true );
      }
      else if( strcmp( optName, "init")==0){
        if (optarg) setInitfileName( optarg );
      }
      else if( strcmp( optName, "notitle")==0){
        setAppTitlebar( false );
      }
      else if( strcmp( optName, "shortMainTitle")==0){
        setAppShortMainTitle( true );
      }
      // else if( strcmp( optName, "toolbar")==0){
      //   std::cerr<<"Option \"toolbar\" ?"<<std::endl;
      // }
      else if( strcmp( optName, "undo")==0){
        setUndo();
      }
      else if( strcmp( optName, "helpmsg")==0){
        HelpMessageType helpMsgType = StatusBarType;
        setHelpmessages( helpMsgType );
      }
      else if( strcmp( optName, "resfile")==0){
        if (optarg){
#ifndef HAVE_QT
          if( ResourceFile().empty() ){
            setResfile( optarg );
            GuiManager::Instance().loadResourceFile( ResourceFile() );
            getOpt(argc, argv);
            return;
          }
#endif
          setResfile( optarg );
        }
      }
      else if( strcmp( optName, "createRes")==0){
        setResCreate( true );
        if (optarg) setNewResFileName(optarg);
      }
      else if( strcmp( optName, "qtGuiStyle")==0){
        if (optarg) setQtGuiStyle( optarg );
      }
      else if( strcmp( optName, "listQtGuiStyles")==0){
        setDisplayQtGuiStyles( true );
      }
      else if( strcmp( optName, "localeDomain")==0){
        if (optarg) setLocaleDomainName( optarg );
      }
      else if( strcmp( optName, "maxoptions")==0){
        if (optarg) setMaxOptions( atoi(optarg) );
      }
      else if( strcmp( optName, "maxlines")==0){
        if (optarg) setMaxLines( atoi(optarg) );
      }
      else if( strcmp( optName, "toolTipDuration")==0){
#if QT_VERSION > 0x050200
        if (optarg) setToolTipDuration( atoi(optarg) );
#else
        std::cerr<<"Ignoring option toolTipDuration (needs Qt Version 5.2)!"<<std::endl;
#endif
      }
      else if( strcmp( optName, "leftIcon")==0){
        if (optarg) setLeftTitleIcon( optarg );
      }
      else if( strcmp( optName, "rightIcon")==0){
        if (optarg) setRightTitleIcon( optarg );
      }
      else if( strcmp( optName, "startupImage")==0){
        if (optarg) setStartupImage( optarg );
      }
      else if( strcmp( optName, "rolefile")==0){
        if (optarg) setRolefileName( optarg );
      }
      else if( strcmp( optName, "includePath")==0){
        if (optarg) setIncludePath( optarg );
      }
      else if( strcmp( optName, "pspreviewer")==0){
        if (optarg) setPsPreviewer( optarg );
      }
      else if( strcmp( optName, "disableSVGSupport")==0){
        setDisableFeatureSVG( true );
      }
      else if( strcmp( optName, "printerConfig")==0){
        if (optarg) setPrinterConfig( optarg );
      }
      else if( strcmp( optName, "jsb")==0){
        m_outputFormat = "jsb";
        if (optarg) setOutputFile( optarg );
      }
      else if( strcmp( optName, "xml")==0){
        m_outputFormat = "xml";
        if (optarg) setOutputFile( optarg );
      }
      else if( strcmp( optName, "xmlPath")==0){
        if (optarg) setXmlPath( optarg );
      }
      else if( strcmp( optName, "apprunPath")==0){
        if (optarg) setApprunPath( optarg );
      }
      else if( strcmp( optName, "dbdriver")==0){
        if (optarg) setDBdriver( optarg );
      }
      else if( strcmp( optName, "dbautologon")==0){
        setDBAutologon( true );
      }
      else if( strcmp( optName, "debug")==0){
        if (optarg)
          if( !Debugger::ModifyDebugFlag( optarg, true ) )
            std::cerr << " Invalid debug category '" << optarg << "'\n";
      }
      else if( strcmp( optName, "persistfile")==0){
        if (optarg) setPersistItemsFilename( optarg );
      }
      else if( strcmp( optName, "persistfileREST")==0){
        if (optarg) setPersistItemsFilename( optarg, true );
      }
      else if( strcmp( optName, "logconfig")==0){
        if (optarg) setLogConfig( optarg );
      }
      else if( strcmp( optName, "log4cplusPropertiesFile")==0){
        if (optarg) setLog4cplusPropertiesFilename( optarg );
      }
      else if( strcmp( optName, "startToken")==0){
        if (optarg) setParserStartToken( optarg );
      }
      else if( strcmp( optName, "reflistfile")==0){
        if (optarg) setReflistFilename( optarg );
      }
      else if( strcmp( optName, "helpdir")==0){
        if (optarg) setHelpDirectory( optarg );
      }
      else if( strcmp( optName, "version")==0){
        setDisplayVersion();
      }
      else if( strcmp( optName, "help")==0){
        setDisplayHelp();
      }
      else if( strcmp( optName, "withTargetStreamInfo")==0){
        setTargetStreamInfo( true );
      }
      else if( strcmp( optName, "whichGui")==0){
        setDisplayWhichGui( true );
      }
      else if( strcmp( optName, "withWheelEvent")==0){
        setGuiWheelEvent( true );
      }
      else if( strcmp( optName, "withInputStructFunc")==0){
        setInputStructFunc( true );
      }
      else if( strcmp( optName, "withoutArrowKeys")==0){
        setArrowKeys( false );
      }
      else if( strcmp( optName, "withKeypadDecimalPoint")==0){
        setKeypadDecimalPoint( true );
      }
      else if( strcmp( optName, "defaultScaleFactor1")==0){
        setDefaultScaleFactor1( true );
      }
      else if( strcmp( optName, "withoutEditableComboBox")==0){
        setGuiComboBoxEditable( false );
      }
      else if( strcmp( optName, "withoutTextPopupMenu")==0){
        setGuiTextPopupMenu( false );
      }
      else if( strcmp( optName, "withoutRangeCheck")==0){
        setGuiRangeCheck( false );
      }
      else if( strcmp( optName, "test")==0){
        if (optarg) setTestModeFunc( optarg );
      }
      else if( strcmp( optName, "testmode")==0){
        m_testMode = true;
      }
      else if( strcmp( optName, "noInitFunc")==0){
        m_noInitFunc = true;
      }
      else if( strcmp( optName, "replyPort")==0){
        if (optarg) setReplyPort( atoi(optarg) );
      }
      else if( strcmp( optName, "sendMessageQueueWithMetadata")==0){
        setSendMessageQueueWithMetadata( true );
      }
      else if( strcmp( optName, "defaultMessageQueueDependencies")==0){
        if (optarg) {
          bool b(false);
          if (strcmp(optarg, "true") == 0 ||
              strcmp(optarg, "1") == 0) {
            b = true;
          }
          setDefaultMessageQueueDependencies( b );
        }
      }
#ifdef HAVE_OAUTH
      else if( strcmp( optName, "oauth")==0){
        if (optarg) {
           setOAuth(optarg);
        }
      }
      else if( strcmp( optName, "oauthAccessTokenUrl")==0){
        if (optarg) setOAuthAccessTokenUrl(optarg);
      }
      else if( strcmp( optName, "oauthScopes")==0){
        if (optarg) setOAuthScopes(optarg);
      }
#endif
      else if( strcmp( optName, "opentelemetryMetadata")==0){
        setOpenTelemetryMetadata();
      }
      else if( strcmp( optName, "lspWorker")==0){
        setLspWorker();
      }
      else if( strcmp( optName, "unitManager")==0){
        UnitManagerFeature featureComboBox(unitManagerFeature_comboBox_hidden);
        if (optarg){
          if (strcmp(optarg, "comboBox_always") == 0 || strcmp(optarg, "1") == 0) {
            featureComboBox = unitManagerFeature_comboBox_always;
          } else
          if (strcmp(optarg, "comboBox_hide_single") == 0 ||
              strcmp(optarg, "comboBox_if_choice") == 0 ||
              strcmp(optarg, "comboBox_if_selection") == 0 ||
              strcmp(optarg, "0") == 0) {
            featureComboBox = unitManagerFeature_comboBox_onlyMultiple;
          } else
            std::cerr << "unknown argument: " << optarg << ", default 'comboBox hidden' option will be used, " << std::endl;
        }
        setUnitManagerFeature(featureComboBox);
      }
      else{
        std::cerr << "Option '" << optName;
        if (optarg)
          std::cerr << " " << optarg;
        std::cerr << "' is ignored" << std::endl;
      }
    }
  }

  // argv[optind++] is the filename of the description file
  // the check that this is the name of a readable file is done
  // in App::parse (App.cc)
  if (optind < argc){
    setDesFile( argv[optind++] );
  }

  while (optind < argc){
    std::string s( argv[optind++] );
    std::cerr << compose(_("Commandline-option '%1' ignored."),s) <<std::endl;
  }

  // convert args for Xt ------------------------------------
  for(int i=1 ; i<argc; ++i ){
    // remove first '-' =======================
    if( argv[i][0]=='-' &&  argv[i][1]=='-' ){
      for(int i2=0 ; i2<(strlen(argv[i])); ++i2 ){
	argv[i][i2]=argv[i][i2+1];
// 	std::cout<<i2<<": ["<<argv[i][i2]<<"]"<<std::endl;
      }
    }
#ifdef __MINGW32__
    // remove first und last '"'
    int last = strlen(argv[i])-1;
    if( last > 0 && argv[i][0]=='"' &&  argv[i][last]=='"' ) {
      strncpy(argv[i], &(argv[i][1]), last-1);
      argv[i][last-1] = '\0';
    }
#endif
  }
  return;
}

/* --------------------------------------------------------------------------- */
/* displayHelp --                                                              */
/* --------------------------------------------------------------------------- */
void AppData::displayHelp(){
  std::cout << std::endl << std::flush;
  std::cout<< compose(_("This is %1 version %2 %3")," I N T E N S ", Version(), "HELP") <<std::flush;
  std::cout << std::endl << std::flush;
  std::cout << std::endl << std::flush;
  std::cout<< _("Commandline options:") << std::endl <<std::flush;
  int i=0;
  while( long_options[i].name ) {
    if( long_options[i].name != 0 ){
      std::cout <<"  ";
      std::cout <<"--" << long_options[i].name;
      if(long_options[i].has_arg==2){
        std::cout << "[=<argument>]";
        if (long_options[i].name == "unitManager")
          std::cout << " e.g. [comboBox_if_choice]";
      } else {
        std::cout <<" " << (long_options[i].has_arg==1? "<argument>":"");
      }
      std::cout << std::endl << std::flush;
    }
    ++i;
    if(i>100) break;
  }
  std::cout << std::endl << std::flush;
}
/* --------------------------------------------------------------------------- */
/* displayVersion --                                                           */
/* --------------------------------------------------------------------------- */
bool AppData::displayVersion(){
  //  std::cout << std::endl << std::flush;
  std::cout<< compose(_("This is %1 version %2")," I N T E N S ", Version()) <<std::endl;
  if (AppData::Instance().Revision().size())
    std::cout << compose(_("Revision: %1"), AppData::Instance().Revision()) <<std::endl;
  if (AppData::Instance().RevisionDate().size())
    std::cout << compose(_("Date: %1"), AppData::Instance().RevisionDate()) <<std::endl;

  return true;
}

/* --------------------------------------------------------------------------- */
/* listFonts --                                                           */
/* --------------------------------------------------------------------------- */
void AppData::listFonts(){

  displayVersion();

#if defined HAVE_QT && !defined HAVE_HEADLESS
  QtMultiFontString::listFonts();
#endif

}

/* --------------------------------------------------------------------------- */
/* displayQtGuiStyles --                                                       */
/* --------------------------------------------------------------------------- */
void AppData::displayQtGuiStyles(){
#if defined HAVE_QT && !defined HAVE_HEADLESS
  std::cout << std::endl << std::flush;
  std::cout << _("Available Qt Styles:") << std::endl << std::flush;
  QStringList styles = QStyleFactory::keys();
  styles.sort();
  for(QStringList::Iterator it = styles.begin(); it != styles.end(); ++it) {
    std::cout << "  " << (*it).toStdString() << std::endl << std::flush;
  }
  std::cout << std::endl << std::flush;
#endif
}

/* --------------------------------------------------------------------------- */
/* whichGui --                                                           */
/* --------------------------------------------------------------------------- */
void AppData::whichGui(){

#ifdef HAVE_HEADLESS
  std::cout << "Headless" << std::endl << std::flush;
  return;
#endif
#ifdef HAVE_QT
  std::cout << "QT" << std::endl << std::flush;
  return;
#endif

  std::cout << "NONE" << std::endl << std::flush;
  return;

}
