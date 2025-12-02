
#if !defined(_APPDATA_INCLUDED_H_)
#define _APPDATA_INCLUDED_H_

#include <stdio.h>
#include <string>
#include <list>
#include <map>

class OAuthClient;
class UserPasswordListener;

class AppData
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  virtual ~AppData(){}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  static AppData &Instance();

  void getApplicationResources();
  enum ResourceType {
    StringType,
    RealType,
    IntegerType
  };
  enum HelpMessageType {
    StatusBarType,
    ToolTipType,
    NoneType
  };
  enum SerializeType
  { serialize_XML
  , serialize_JSON
  , serialize_PROTO
  };
  // used as exit code in test mode
  enum TestModeExitCode {
    successExitCode = 0,
    messageExitCode = 0,
    failureExitCode = 2
  };
  // UnitManager FeatureComboBox
  enum UnitManagerFeature{
    unitManagerFeature_none = 0
    , unitManagerFeature_comboBox_hidden = 1
    , unitManagerFeature_comboBox_onlyMultiple = 2
    , unitManagerFeature_comboBox_always = 3
  };
  int getResource(const std::string& key,
			   std::string& stringValue, double& doubleValue, int& intValue);
  void setResource(const std::string& key, std::string& stringValue);
  void setResource(const std::string& key, double doubleValue);
  void setResource(const std::string& key, int intValue);

  void setDebug( int level );
  void setProgName( const std::string &p );
  void setClassName( const std::string &c );
  void setVersion( const std::string &v );
  void setUserName( const std::string &n );
  void setRealName( const std::string &n );
  void setPassword( const std::string &p );
  void setHostName( const std::string &h );
  void setIntensHome( const std::string &h );
  void setLocaleDir( const std::string &d );
  void setLocaleDomain( const std::string &d );
  void setLocaleDomainName( const std::string &d );
  void setLogConfig( const std::string &l );
  void setPyLogMode( bool f );
  void setRolefileName( const std::string &r );
  void setOrganization( const std::string &o );
  void setDivision( const std::string &d );
  void setSite( const std::string &s );
  void setLicenseKey( const std::string &o );
  void setMathnode( const std::string &n );
  void setMathport( int p );
  void setMatlabnode( const std::string &n );
  void setTitle( const std::string &t );
  void setMaxOptions( int i );
  void setAppToolbar( bool b );
  void setAppTitlebar( bool b );
  void setAppShortMainTitle( bool b );
  void setUndo();
  void setHelpmessages( HelpMessageType type );
  void setMarginOffset( int m );
  void setDisplayName( const std::string & );
  void setStartupImage( const std::string & );
  void setLeftTitleIcon( const std::string & );
  void setRightTitleIcon( const std::string & );
  void setPsPreviewer( const std::string & );
  void setDisableFeatureSVG( bool flag );
  void setPrinterConfig( const std::string & );
  void setFeatureMatlab( bool flag );
  void addUserGroup( const std::string & );
  bool checkUserGroup( const std::string & );
  void addUserGroupsGroup( const std::string &name, bool ok );
  bool checkUserGroupsGroup( const std::string &name );
  void setPersistItemsFilename( const char *filename, bool restdb=false );
  void setLog4cplusPropertiesFilename( const char *filename );
  void setParserStartToken( const char *token );
  void setReflistFilename( const char *filename );
  void setHelpDirectory( const char *dirname );
  void setFontname( const std::string &n );
  void setInitfileName( const std::string &n );
  void setDetailGrid( bool d );
  void setResfile( const std::string &n );
  void setMaxLines( int l );
  void setToolTipDuration( int sec );
  void setIncludePath( const std::string &n );
  void setXmlPath( const std::string &n );
  void setApprunPath( const std::string &n );
  void setOutputFile( const std::string &n );
  void setDBdriver( const std::string &n );
  void setDBAutologon( bool d );
  void setXFontList( const std::string &n );
  void setDesFile( const std::string &n );
  void setResCreate( bool f );
  void setNewResFileName( const std::string &f);
  void setDisplayVersion( bool d=true );
  void setDisplayHelp( bool d=true );
  void setListFonts( bool d=true );
  void setTargetStreamInfo( bool d=true );
  void setDisplayWhichGui( bool d=true );
  void displayHelp();
  bool displayVersion();
  void listFonts();
  void setQtGuiStyle( const std::string &f);
  void setDisplayQtGuiStyles( bool d=true );
  void displayQtGuiStyles();
  void setGuiWheelEvent( bool d=true );
  void setArrowKeys( bool d=true );
  void setKeypadDecimalPoint( bool d=true );
  void setDefaultScaleFactor1( bool d=true );
  void setInputStructFunc( bool d=true );
  void setGuiComboBoxEditable( bool d=true );
  void setGuiTextPopupMenu( bool d=true );
  void setGuiRangeCheck( bool d=true );
  void whichGui();
  void setTestModeFunc(const std::string &f);
  void setReplyPort(int port);
  void setSendMessageQueueWithMetadata(bool b);
  void setDefaultMessageQueueDependencies(bool b);
  void setOAuth( const std::string &n );
  void setOAuthAccessTokenUrl( const std::string &n );
  void setOpenTelemetryMetadata();
  void setLspWorker();
  UnitManagerFeature getUnitManagerFeature() const;
  void setUnitManagerFeature(UnitManagerFeature featureCB);

  int Debug();
  bool isWindows();
  const std::string &ProgName();
  const std::string &ClassName();
  const std::string &Version();
  const std::string VersionString();
  const std::string &Revision();
  const std::string &RevisionDate();
  const std::string &UserName();
  const std::string &RealName();
  const std::string &Password();
  const std::string &HostName();
  const std::string &IntensHome();
  const std::string &AppHome();
  const std::string &LocaleDir();
  const std::string &LocaleDomain();
  const std::string &LocaleDomainName();
  const std::string &LogConfig();
  const bool PyLogMode();
  const std::string &RolefileName();
  const bool FeatureMatlab();
  const bool disableFeatureSVG() const;
  const std::string &Organization();
  const std::string &Division();
  const std::string &Site();
  const std::string &LicenseKey();
  const std::string &InitfileName();
  const std::string &Mathnode();
  const std::string &MatlabNode() const;
  const std::string &DisplayName() const;
  const std::string &StartupImage() const;
  const std::string &LeftTitleIcon() const;
  const std::string &RightTitleIcon() const;
  const std::string &PsPreviewer() const;
  const std::string &PrinterConfig() const;
  const std::string &PersistItemsFilename() const;
  bool PersistItemsForRest() const;
  const std::string &Log4cplusPropertiesFilename() const;
  const int ParserStartToken() const;
  const std::string &ReflistFilename() const;
  int Mathport();
  bool MlString();
  bool AppTitlebar();
  bool AppShortMainTitle();
  bool AppToolbar();
  bool Undo();
  bool DetailGrid();
  HelpMessageType Helpmessages();
  const std::string &Title();
  int MaxOptions();
  int MaxLines();
  int ToolTipDuration();
  int MarginOffset();
  const std::string &FontName();
  const std::string &XFontList();
  const std::string &ResourceFile();
  const std::string &IncludePath() const;
  const std::string &OutputFormat() const;
  const std::string &XmlPath() const;
  const std::string &ApprunPath() const;
  const std::string &OutputFile() const;
  const std::string &DBdriver() const;
  const bool DBAutologon() const;
  const std::string &DesFile() const;
  bool createResFile();
  const std::string &newResFileName() const;
  bool DisplayVersion();
  bool DisplayHelp();
  bool ListFonts();
  const std::string &QtGuiStyle() const;
  bool DisplayQtGuiStyles();
  bool GuiWheelEvent() const;
  bool ArrowKeys() const;
  bool KeypadDecimalPoint() const;
  bool DefaultScaleFactor1() const;
  bool InputStructFunc() const;
  bool GuiComboBoxEditable() const;
  bool GuiWithTextPopuMenu() const;
  bool GuiWithRangeCheck() const;
  bool TargetStreamInfo() const;
  bool DisplayWhichGui();
  const std::string &TestModeFunc();
  const std::string &OAuth();
  const std::string &OAuthAccessTokenUrl();
  void runOAuthClient(UserPasswordListener* listener=0 );
  std::string OAuthToken();
  bool OpenTelemetryMetadata() const;
  bool LspWorker() const;
  bool hasUnitManagerFeature() const;
  const bool hasTestModeFunc() const;
  const bool TestMode() const;
  const bool NoInitFunc() const;
  int ReplyPort();
  bool HeadlessWebMode();
  bool sendMessageQueueWithMetadata();
  bool getMessageQueueDependencies(int option);
  TestModeExitCode getTestModeExitCode();
  void setTestModeExitCode(TestModeExitCode exitCode);
  /** write user settings to resource file
   */
  void writeSettings();

  const std::string HelpFileDirectory();

  void getOpt(int &argc, char **argv);
private:
  void setDefaultOpts();


/*******************************************************************************/
/* private Data                                                                */
/*******************************************************************************/
private:
  AppData();

  typedef std::list<std::string>        UserGroupList;
  typedef std::map<std::string, bool>   UserGroupGroups;

  static AppData   *s_instance;

  std::string       m_mathnode;    /* Hostname for math kernel */
  int               m_mathport;    /* port number for TCP communication */
  std::string       m_fontname;    /* Fontname for PostScript plots */
  std::string       m_xfontlist;   /* Fontnames for x window plots */
  int               m_debug;       /* Debug-Level: LogDebug, LogInfo, .. see log.h */
  std::string       m_matlabnode;  /* Hostname for matlab server*/
  std::string       m_displayname;  /* Name of display */
  std::string       m_progname;
  std::string       m_classname;
  std::string       m_hostname;
  std::string       m_username;
  std::string       m_realname;
  std::string       m_password;
  UserGroupList     m_grouplist;
  UserGroupGroups   m_usergroups;
  std::string       m_title;
  std::string       m_oauth;       /* Network Authorization */
  std::string       m_oauthAccessTokenUrl;       /* Network Authorization */
  std::string       m_organization;
  std::string       m_division;
  std::string       m_site;
  std::string       m_licenseKey;
  std::string       m_logconfig;
  std::string       m_initfile;
  std::string       m_resfile;
  std::string       m_version;
  std::string       m_revision;
  std::string       m_revisionDate;
  bool              m_pyLogMode;
  bool              m_mlString;
  bool              m_app_titlebar;
  bool              m_app_shortMainTitle;
  bool              m_app_toolbar;
  bool              m_undo;
  bool              m_detailGrid;
  HelpMessageType   m_helpmessages;
  int               m_maxoptions;   /* Number of buttons in one option pane */
  int               m_maxlines;     /* NUmber of lines in log window */
  int               m_toolTipDuration;     /* ToolTip Duration in seconds */
  std::string       m_intensHome;   /* Home directory of intens */
  std::string       m_appHome;   /* Application Home directory */
  std::string       m_localeDir;    /* locale directory */
  std::string       m_localeDomain; /* locale domain */
  std::string       m_localeDomainName; /* locale domain name*/
  int               m_marginOffset;
  std::string       m_roleFile;     /* filename of DB role definitions */
  std::string       m_leftTitleIcon; /* name of left title icon */
  std::string       m_rightTitleIcon; /* name of left title icon */
  std::string       m_startupImage;   /* name of startup image */
  std::string       m_includePath;   /* search include directories */
  std::string       m_psPreviewer;   /* pathname of postscript previewer */
  std::string       m_printerConfig; /* filename of printer config file */
  bool              m_featureMatlab; /* true if feature Matlab exists in license */
  bool              m_disableFeatureSVG; /* true if feature SVG should not be used  */
  std::string       m_outputFormat;    /* instructs Intens to create c++ source if not empty*/
  std::string       m_outputFile;    /* instructs Intens to create c++ source if not empty*/
  std::string       m_xmlPath;   /* directory to output xml-code by -create */
  std::string       m_apprunPath;   /* directory to output configuration-code by -create */

  std::string       m_dbdriver;  /* db driver to load: dbora or dbpost */
  bool              m_dbAutologon;  /* db driver to load: dbora or dbpost */
  std::string       m_persistItemsFilename;  /* parse input and create db include file */
  bool              m_persistItemsForRest;
  std::string       m_log4cplusPropertiesFilename;  /* log4cplus.properties filename (default: APPHOME/config/log4cplus.properties) */
  std::string       m_reflistFilename;  /* parse input and create xml file with referencies */
  std::string       m_helpdir;  /* base directory for relative help file paths */

  std::string       m_desFile;
  bool              m_resCreate;
  std::string       m_newResFile;

  int               m_parserStartToken;
  bool              m_displayVersion;
  bool              m_displayHelp;
  bool              m_listFonts;
  std::string       m_qtGuiStyle;
  bool              m_displayQtGuiStyles;
  std::string       m_licFeature;
  bool              m_displayWhichGui;
  bool              m_withGuiWheelEvent;
  bool              m_withArrowKeys;
  bool              m_withKeypadDecimalPoint;
  bool              m_defaultScaleFactor1;
  bool              m_withInputStructFunc;
  bool              m_comboBoxEditable;
  bool              m_withTextPopupMenu;
  bool              m_withRangeCheck;
  bool              m_withTargetStreamInfo;
  std::string       m_testModeFunc;
  bool              m_testMode;
  TestModeExitCode  m_testModeExitCode;
  bool              m_noInitFunc;
  int               m_replyPort;
  bool              m_sendMessageQueueWithMetadata;
  bool              m_defaultMessageQueueDependencies;
  bool              m_opentelemetry_metadata;
  bool              m_lspWorker;
  UnitManagerFeature m_unitManagerFeature;
  OAuthClient*      m_oauthClient;
};

#endif
