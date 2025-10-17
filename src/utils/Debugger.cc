
#include <assert.h>
#include <sys/time.h>
#if !defined _WIN32
#include <sys/resource.h>
#endif

#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "utils/utils.h"

INIT_LOGGER();

using namespace std;

// ================================================================ //
// Initialisation of static variables                               //
// ================================================================ //

bool            Debugger::s_BugIsEnabled[BugMax];
bool            Debugger::s_BugInitialized = false;
int             Debugger::s_indent = 0;
Debugger::CategoryMap Debugger::s_categoryMap;

#if defined _WIN32
std::ostream&  Debugger::s_BugOutFile = std::cout;
#else
static std::ofstream* _BugOutFile = new std::ofstream("IntensBugFile");
std::ostream&  Debugger::s_BugOutFile = *_BugOutFile;
#endif

#if defined HAVE_LOG4CPLUS
Debugger::LoggerCategoryMap  Debugger::s_LoggerCategoryMap;
#endif

/*********************************************************************/
/* Debug_Destructor                                                  */
/* Die folgende Mini-Klasse dient dazu in iherem Destruktor die sta- */
/* tischen Variablen des Debuggers zu deallozieren. Das einzige, was */
/* sie tut ist, einen Destructor bereit zu halten.                   */
/*********************************************************************/
class Debug_Destructor{
  public:
  ~Debug_Destructor() {Debugger::TerminateDebugger(); }
  };

Debug_Destructor Debug_Destructor;

// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //

Debugger::Debugger(){}

Debugger::~Debugger(){}

// ================================================================ //
// Member Functions                                                 //
// ================================================================ //

// ---------------------------------------------------------------- //
// InitializeDebugger --                                            //
// ---------------------------------------------------------------- //

void Debugger::InitializeDebugger(){
  s_BugOutFile.precision(14);
  s_BugOutFile.setf( (std::_Ios_Fmtflags)0, ios::floatfield );

  ModifyAllDebugFlags( false );
  s_BugInitialized = true;

  s_categoryMap.insert( CategoryMap::value_type( "DescriptionFile", BugDescriptionFile ) );
  s_categoryMap.insert( CategoryMap::value_type( "PythonLogFile", BugPythonLogFile ) );
  s_categoryMap.insert( CategoryMap::value_type( "Data", BugData ) );
  s_categoryMap.insert( CategoryMap::value_type( "Ref", BugRef ) );
  s_categoryMap.insert( CategoryMap::value_type( "Dict", BugDict ) );
  s_categoryMap.insert( CategoryMap::value_type( "Undo", BugUndo ) );
  s_categoryMap.insert( CategoryMap::value_type( "TTrail", BugTTrail ) );
  s_categoryMap.insert( CategoryMap::value_type( "MLS", BugMLS ) );
  s_categoryMap.insert( CategoryMap::value_type( "MLSFile", BugMLSFile ) );
  s_categoryMap.insert( CategoryMap::value_type( "MLSInterface", BugMLSInterface ) );
  s_categoryMap.insert( CategoryMap::value_type( "Xfer", BugXfer ) );
  s_categoryMap.insert( CategoryMap::value_type( "Gui", BugGui ) );
  s_categoryMap.insert( CategoryMap::value_type( "GuiFld", BugGuiFld ) );
  s_categoryMap.insert( CategoryMap::value_type( "GuiTable", BugGuiTable ) );
  s_categoryMap.insert( CategoryMap::value_type( "GuiTrigger", BugGuiTrigger ) );
  s_categoryMap.insert( CategoryMap::value_type( "GuiIcon", BugGuiIcon ) );
  s_categoryMap.insert( CategoryMap::value_type( "GuiMgr", BugGuiMgr ) );
  s_categoryMap.insert( CategoryMap::value_type( "GuiWorker", BugGuiWorker ) );
  s_categoryMap.insert( CategoryMap::value_type( "GuiFolder", BugGuiFolder ) );
  s_categoryMap.insert( CategoryMap::value_type( "Utilities", BugUtilities ) );
  s_categoryMap.insert( CategoryMap::value_type( "Misc", BugMisc ) );
  s_categoryMap.insert( CategoryMap::value_type( "Operator", BugOperator ) );
  s_categoryMap.insert( CategoryMap::value_type( "DBManager", BugDBManager ) );
  s_categoryMap.insert( CategoryMap::value_type( "DBFilter", BugDBFilter ) );
  s_categoryMap.insert( CategoryMap::value_type( "DBCurator", BugDBCurator ) );
  s_categoryMap.insert( CategoryMap::value_type( "DBTimestamp", BugDBTimestamp ) );
  s_categoryMap.insert( CategoryMap::value_type( "Streamer", BugStreamer ) );
  s_categoryMap.insert( CategoryMap::value_type( "Targets", BugTargets ) );
  s_categoryMap.insert( CategoryMap::value_type( "Job", BugJob ) );
  s_categoryMap.insert( CategoryMap::value_type( "JobStart", BugJobStart ) );
  s_categoryMap.insert( CategoryMap::value_type( "JobCode", BugJobCode ) );
  s_categoryMap.insert( CategoryMap::value_type( "JobStack", BugJobStack ) );
  s_categoryMap.insert( CategoryMap::value_type( "Plot", BugPlot ) );
  s_categoryMap.insert( CategoryMap::value_type( "Plugin", BugPlugin ) );
  s_categoryMap.insert( CategoryMap::value_type( "None", BugNone ) );

#if HAVE_LOG4CPLUS
  // initialize LoggerCategoryMap
  CategoryMap::iterator it = s_categoryMap.begin();
  for (; it != s_categoryMap.end(); ++it) {
    std::string key = "org.semafor.intens."+ it->first;
    BUG_DEBUG("key: " << key.c_str() << " LoggerName: "
              << log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(key.c_str())).getName()
              << " LogLevel: " << log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(key.c_str())).getLogLevel());
    s_LoggerCategoryMap.insert( LoggerCategoryMap::value_type(it->second,
							      log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(key.c_str())))  );
    DebugFlagEnabled(it->second);
  }
#endif
}

// ---------------------------------------------------------------- //
// TerminateDebugger --                                             //
// ---------------------------------------------------------------- //

void Debugger::TerminateDebugger(){
  ModifyAllDebugFlags( false );
#if defined _WIN32
#else
    if( s_BugOutFile.good() ){
      dynamic_cast<std::ofstream&>(s_BugOutFile).close();
  }
#endif
}

// ---------------------------------------------------------------- //
// getTimestamp --                                                  //
// ---------------------------------------------------------------- //

double Debugger::getTimestamp() const{
  double ticks;
#if _WIN32 //defined(true)
  ticks = (double)clock() / CLOCKS_PER_SEC;
#else
  struct rusage usage;
  getrusage( RUSAGE_SELF,&usage );

  double usr_time = (double)usage.ru_utime.tv_sec +
                    (double)usage.ru_utime.tv_usec / 1000000.0;
  double sys_time = (double)usage.ru_stime.tv_sec +
                    (double)usage.ru_stime.tv_usec / 1000000.0;
  ticks = usr_time + sys_time;
#endif
  return ticks;
}

// ---------------------------------------------------------------- //
// elapsedSeconds --                                                //
// ---------------------------------------------------------------- //

double Debugger::elapsedSeconds( double ts_alt ) const{
  return runden( getTimestamp() - ts_alt, 5 );
}

// ---------------------------------------------------------------- //
// DebuggerOnline --                                                //
// ---------------------------------------------------------------- //

bool Debugger::DebuggerOnline() const{
  return s_BugInitialized;
}

// ---------------------------------------------------------------- //
// ModifyDebugFlag --                                               //
// ---------------------------------------------------------------- //

bool Debugger::ModifyDebugFlag( const std::string &cat, bool status ){
  if( s_categoryMap.find( cat ) != s_categoryMap.end() ){
    s_BugIsEnabled[ s_categoryMap[cat] ] = status;
#if HAVE_LOG4CPLUS
    s_LoggerCategoryMap[ s_categoryMap[cat] ].setLogLevel( status ?
							    log4cplus::DEBUG_LOG_LEVEL : log4cplus::NOT_SET_LOG_LEVEL );
#endif
    return true;
  }
  return false;
}

// ---------------------------------------------------------------- //
// ModifyAllDebugFlags --                                           //
// ---------------------------------------------------------------- //

void Debugger::ModifyAllDebugFlags( bool status ){
  for( int i = 0; i < BugMax; i++ ){
    s_BugIsEnabled[i] = status;
  }
}

// ---------------------------------------------------------------- //
// ModifyAllDebugFlags --                                           //
// ---------------------------------------------------------------- //

void Debugger::ModifyAllDebugFlags( bool *status ){
  for( int i = 0; i < BugMax; i++){
    s_BugIsEnabled[i] = status[i];
  }
}

// ---------------------------------------------------------------- //
// DebugFlagEnabled --                                              //
// ---------------------------------------------------------------- //

bool Debugger::DebugFlagEnabled( BugCategory cat ){
#if HAVE_LOG4CPLUS
   int pval = s_LoggerCategoryMap[cat].getLogLevel();
   if (pval == log4cplus::DEBUG_LOG_LEVEL) {
     s_BugIsEnabled[cat] = true;
   }
#endif
  return s_BugIsEnabled[cat];
}

// ---------------------------------------------------------------- //
// BugStream --                                                     //
// ---------------------------------------------------------------- //

std::ostream &Debugger::BugStream(){
  return s_BugOutFile;
}

#if HAVE_LOG4CPLUS
// ---------------------------------------------------------------- //
// castLogger --                                                    //
// ---------------------------------------------------------------- //

log4cplus::Logger* Debugger::castLogger(void* logger) {
  return static_cast<log4cplus::Logger*>(logger);
}

// ---------------------------------------------------------------- //
// getFileLoggerName --                                             //
// ---------------------------------------------------------------- //

const std::string Debugger::getFileLoggerName(const char* filename) {
  std::string afn;
  std::filesystem::path fn(filename);
  afn = fn.filename().string();
  while (fn.has_parent_path() &&
         fn.parent_path().filename().string() != "src" &&
         fn.parent_path().filename().string() != "intens") {
    fn = fn.parent_path();
    afn = fn.filename().string() + "." + afn;
  }
  return "org.semafor.intens." + afn;
 }
#endif
