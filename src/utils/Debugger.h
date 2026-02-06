
#if !defined(_INTENS_DEBUGGER_H_INCLUDED_)
#define _INTENS_DEBUGGER_H_INCLUDED_

#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <sstream>

#if defined  _DEBUG
  #define TRACE_MODE 1
#else
  #define TRACE_MODE 0
#endif

#if HAVE_LOG4CPLUS
#if defined _WIN32
#undef UNICODE  // log4cplus wchar bug (see third-party/CMakeLists.txt UNICODE=OFF)
#endif
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#else
#define std_endl std::endl
#define streamPrefix
#endif

// ================================================================ //
// Macros for Object-Counter                                        //
// ================================================================ //

#if (TRACE_MODE)

#define BUG_DECLARE_COUNT    static int s_bug_count
#define BUG_INIT_COUNT(OBJ)  int OBJ::s_bug_count = 0
#define BUG_INCR_COUNT       s_bug_count++
#define BUG_DECR_COUNT       --s_bug_count
#define BUG_DECL_SHOW_COUNT(OBJ) \
  static const std::string BUG_classname() { return OBJ; }     \
  static int BUG_show_count( std::ostream &ostr, bool conditional )	\
    { if( conditional ? s_bug_count > 0 : true ){ \
        ostr << s_bug_count << " Object(s) of " \
             << BUG_classname() << " found" << std::endl;	\
      } \
      return s_bug_count; \
    } \
  static int BUG_count() { return s_bug_count; }
#define BUG_SHOW_COUNT(OBJ,ostr,cond)  OBJ::BUG_show_count( ostr, cond )
#define BUG_INSERT_COUNT(OBJ,mem) mem.insert( OBJ::BUG_classname(), OBJ::BUG_count() )

#else

#define BUG_INITIALIZE(X)
#define BUG_DECLARE_COUNT
#define BUG_INIT_COUNT(OBJ)
#define BUG_INCR_COUNT
#define BUG_DECR_COUNT
#define BUG_DECL_SHOW_COUNT(OBJ)
#define BUG_SHOW_COUNT(OBJ,ostr,cond) 0
#define BUG_INSERT_COUNT(OBJ,mem)

#endif // TRACE_MODE

#define BugCharPtr(p) ((p) == 0 ? "<Null>" : (p))
#define BugPtr(p)     (void *)(p)
#define BugBool(b) ((b) ? "true" : "false")

// ================================================================ //
// global Definitions                                               //
// ================================================================ //

enum BugCategory {
  /****************************/
  /* Datapool                 */
  /****************************/
   BugData = 0       // Debug Datapool
  ,BugRef            // Debug DataReference
  ,BugDict           // Debug DataDictionary
  ,BugTTrail         // Debug TransactionTrail
  ,BugUndo           // Debug Undo/Redo-Feature

  /****************************/
  /* Multi-Language           */
  /****************************/
  ,BugMLS            // Debug MultiLanguage
  ,BugMLSFile        // Debug MultiLanguage Filehandling
  ,BugMLSInterface   // Debug Interface of MultiLanguage

  /****************************/
  /* Transfer Parameter       */
  /****************************/
  ,BugXfer           //

  /****************************/
  /* Gui                      */
  /****************************/
  ,BugGui            // Debug Gui
  ,BugGuiFld         // Debug Gui DataFields
  ,BugGuiTable       // Debug Gui Table
  ,BugGuiTrigger     // Debug Gui Function Calls
  ,BugGuiIcon        // Debug Gui Iconmanager
  ,BugGuiMgr         // Debug Gui Manager
  ,BugGuiWorker      // Debug Gui Worker
  ,BugGuiFolder      // Debug Gui Folders

  /****************************/
  /* Verschiedenes.           */
  /****************************/
  ,BugUtilities      // Debug Utility-Functions
  ,BugMisc           //

  /****************************/
  /* Operator                 */
  /****************************/
  ,BugOperator       //

  /****************************/
  /* Database                 */
  /****************************/
  ,BugDBManager      //
  ,BugDBFilter       //
  ,BugDBCurator      // Debug Curator Functions
  ,BugDBTimestamp

  /****************************/
  /* Streamer                 */
  /****************************/
  ,BugStreamer       //
  ,BugTargets        //

  /****************************/
  /* Job                      */
  /****************************/
  ,BugJob            // Debug general Functions
  ,BugJobStart       // Debug Job Control
  ,BugJobCode        // Debug Job Code Operators
  ,BugJobStack       // Debug Job Stack Items

  /****************************/
  /* Job                      */
  /****************************/
  ,BugPlot           //
  ,BugPlugin         //

  /****************************/
  /* DescriptionFile          */
  /****************************/
  ,BugDescriptionFile //

  /****************************/
  /* PythonLogFile            */
  /****************************/
  ,BugPythonLogFile //

  /****************************/
  /* Ende der Liste           */
  /****************************/
  ,BugNone           // need this for initialization
  ,BugMax            // Maximum Debugging. Must be last in Enum
};

// ================================================================ //
// Macros for Trace-Feature                                         //
// ================================================================ //
#include <libgen.h>
#define INIT_BASE_LOGGER(initFlag)       \
  static bool __debugLogger__   = false; \
  static bool __infoLogger__    = false; \
  static bool __warnLogger__ = false; \
  static bool __errorLogger__   = false; \
  static bool __notInitDebugLogger__ = initFlag; \
  static bool __notInitInfoLogger__ = initFlag; \
  static bool __notInitWarnLogger__ = initFlag; \
  static bool __notInitErrorLogger__ = initFlag;

#if HAVE_LOG4CPLUS
#define INIT_LOGGER() \
  static log4cplus::Logger __logger__ = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(Debugger::getFileLoggerName(__FILE__).c_str())); \
  INIT_BASE_LOGGER(true)
#else
#define INIT_LOGGER() \
  static int  __logger__ = 0;		 \
  INIT_BASE_LOGGER(false)
#endif

#define PYLOG_GETVALUE "intens.get_%1_value('%2')"
#define PYLOG_CALL_PROCESS "action='%1',"
#define PYLOG_SETVALUE "variable='%1',\nvalue=%2,"
#define PYLOG_SETVALUE_WITHFACTOR "variable='%1',\nvalue=%2 * %3,"
#define PYLOG_NEWCYCLE "# intens.new_cycle('%1')"
#define PYLOG_GOCYCLE "# intens.go_cycle('%1')"
#define PYLOG_FILEOPEN "# intens.file_open('%1', %2)"
#define PYLOG_MAPFOLDER "# intens.map_folder('%1')"
#define PYLOG_MAPFOLDER2 "# intens.map_folder('%1', %2)"
#define PYLOG_SETRESOURCE "# os.environ['%1'] = '%2'"

#if HAVE_LOG4CPLUS
#define INIT_DESLOGGER() \
  static log4cplus::Logger __desLogger__ = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(Debugger::getFileLoggerName("DescriptionTraceFile").c_str())); \
  static bool __debugDesLogger__   = false; \
  static bool __infoDesLogger__   = false; \
  static bool __notInitDebugDesLogger__ = true; \
  static bool __notInitInfoDesLogger__ = true;

#define INIT_PYTHONLOGGER() \
  static log4cplus::Logger __pyLogLogger__ = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(Debugger::getFileLoggerName("PythonTraceFile").c_str())); \
  static bool __debugPyLogLogger__   = false; \
  static bool __infoPyLogLogger__   = false; \
  static bool __notInitDebugPyLogLogger__ = true; \
  static bool __notInitInfoPyLogLogger__ = true;

#define DES_DEBUG(Str)             \
  if (__notInitDebugDesLogger__) { \
      __debugDesLogger__ = __desLogger__.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL); \
    __notInitDebugDesLogger__ = false; \
  } \
  if (__debugDesLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_DEBUG(__desLogger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }

#define DES_INFO(Str)	 \
  if (__notInitInfoDesLogger__) { \
      __infoDesLogger__ = __desLogger__.isEnabledFor(log4cplus::INFO_LOG_LEVEL); \
    __notInitInfoDesLogger__ = false; \
  } \
  if (__infoDesLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_INFO(__desLogger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }

#define PYLOG_DEBUG(Str)	 \
  if (__notInitDebugPyLogLogger__) { \
    __debugPyLogLogger__ = __pyLogLogger__.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL);  \
    __notInitDebugPyLogLogger__ = false; \
  } \
  if (__debugPyLogLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_DEBUG(__pyLogLogger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }

#define PYLOG_INFO(Str)	 \
  if (__notInitInfoPyLogLogger__) { \
      __infoPyLogLogger__ = __pyLogLogger__.isEnabledFor(log4cplus::INFO_LOG_LEVEL); \
    __notInitInfoPyLogLogger__ = false; \
  } \
  if (__infoPyLogLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_INFO(__pyLogLogger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }

#else
#define INIT_DESLOGGER() \
  static bool __desLogger__ =  false; \
  static bool __debugDesLogger__   = false; \
  static bool __infoDesLogger__   = false; \
  static bool __notInitDebugDesLogger__ = true; \
  static bool __notInitInfoDesLogger__ = true;
#define INIT_PYTHONLOGGER() \
  static bool __pyLogLogger__ =  false; \
  static bool __debugPyLogLogger__   = false; \
  static bool __infoPyLogLogger__   = false; \
  static bool __notInitDebugPyLogLogger__ = true; \
  static bool __notInitInfoPyLogLogger__ = true;

#define DES_DEBUG(STR) ""
#define DES_INFO(STR) ""
#define PYLOG_DEBUG(STR) ""
#define PYLOG_INFO(STR) ""
#endif

#if HAVE_LOG4CPLUS
#define BUG_DEBUG(Str)	 \
  if (__notInitDebugLogger__) { \
    __debugLogger__ = __logger__.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL); \
    __notInitDebugLogger__ = false; \
  } \
  if (__debugLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_DEBUG(__logger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }
#else
#define BUG_DEBUG(Str)
#endif

#if HAVE_LOG4CPLUS
#define BUG_INFO(Str) \
  if (__notInitInfoLogger__) { \
    __infoLogger__ = __logger__.isEnabledFor(log4cplus::INFO_LOG_LEVEL); \
    __notInitInfoLogger__ = false; \
  } \
  if (__infoLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_INFO(__logger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }
#else
#define BUG_INFO(Str)
#endif

#if HAVE_LOG4CPLUS
#define BUG_WARN(Str) \
  if (__notInitWarnLogger__) { \
    __warnLogger__ = __logger__.isEnabledFor(log4cplus::WARN_LOG_LEVEL); \
    __notInitWarnLogger__ = false; \
  } \
  if (__warnLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_WARN(__logger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }
#else
#define BUG_WARN(Str)
#endif

#if HAVE_LOG4CPLUS
#define BUG_ERROR(Str) \
  if (__notInitErrorLogger__) { \
    __errorLogger__ = __logger__.isEnabledFor(log4cplus::ERROR_LOG_LEVEL); \
    __notInitErrorLogger__ = false; \
  } \
  if (__errorLogger__) { \
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << Str; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    LOG4CPLUS_ERROR(__logger__, LOG4CPLUS_TEXT(__log_tmp_str)); \
  }
#else
#define BUG_ERROR(Str)
#endif

#if HAVE_LOG4CPLUS
#define BUG(Category,Fun) \
  void* __loggerL__ = Debugger::getLogger(Category); \
  if (__loggerL__) \
    LOG4CPLUS_DEBUG((*Debugger::castLogger(__loggerL__)),  LOG4CPLUS_TEXT(Fun))
#else
#define BUG(Category,Fun)
#endif

#if HAVE_LOG4CPLUS
#define BUG_SILENT(Category,Fun) \
  void* __loggerL__ = Debugger::getLogger(Category)
#else
#define BUG_SILENT(Category,Fun)
#endif

#if HAVE_LOG4CPLUS
#define BUG_PARA(Category,Fun,Para) \
  void* __loggerL__ = Debugger::getLogger(Category); \
  if (__loggerL__) \
    LOG4CPLUS_DEBUG((*Debugger::castLogger(__loggerL__)),  LOG4CPLUS_TEXT(Fun << "-" << Para))
#else
#define BUG_PARA(Category,Fun,Para)
#endif

#if HAVE_LOG4CPLUS
#define BUG_EXIT(Para) \
  if (__loggerL__) \
    LOG4CPLUS_DEBUG((*Debugger::castLogger(__loggerL__)),  LOG4CPLUS_TEXT(Para))
#else
#define BUG_EXIT(Para)
#endif

#if HAVE_LOG4CPLUS
#define BUG_MSG(Para) \
  if (__loggerL__) \
    LOG4CPLUS_DEBUG((*Debugger::castLogger(__loggerL__)),  LOG4CPLUS_TEXT(Para))
#else
#define BUG_MSG(Para)
#endif

#if HAVE_LOG4CPLUS
#define BUG_FATAL(Para) \
  if (__loggerL__) \
    LOG4CPLUS_DEBUG((*Debugger::castLogger(__loggerL__)),  LOG4CPLUS_TEXT(Para))
#else
#define BUG_FATAL(Para)
#endif

/** Die Debugger-Objekte werden verwendet für Logging und Trace-Messages.
 */
class Debugger
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  Debugger();

  virtual ~Debugger();

protected:
  /** we do not expect copy and assignment operations */
  Debugger(const Debugger &ref);
  Debugger& operator=(const Debugger &ref);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
#if HAVE_LOG4CPLUS
  static const std::string getFileLoggerName(const char* filename);
  inline static log4cplus::Logger* getLogger(BugCategory BugCat) {
    if( !s_BugIsEnabled[BugCat] ) return 0;
    return &s_LoggerCategoryMap[BugCat];
  }
  static log4cplus::Logger*  castLogger(void* logger);
#endif

  /** Die Funktion beendet den Debugger und schliesst den Output.
   */
  static void TerminateDebugger();

  /** Die Funktion initialisiert den Debugger. Sie öffnet die Ouputdatei und setzt
      die Default-Optionen.
   */
  static void InitializeDebugger();

  /** Die Funktion setzt die gewünschte Debug-Category auf den Wert status.
      @param cat Debug-Category
      @param status aktivieren oder deaktivieren der Debug-Category
      @return true => Die Category ist modifiziert worden.
   */
  static bool ModifyDebugFlag( const std::string &cat, bool status );

  /** Die Funktion setzt alle Debug-Categories auf den Wert status.
   * @param status aktivieren oder deaktivieren der Debug-Categories
   */
  static void ModifyAllDebugFlags( bool status );

  /** Die Funktion setzt alle Debug-Categories auf den jeweiligen Wert
      im Array status.
      @param status aktivieren oder deaktivieren der Debug-Categories
   */
  static void ModifyAllDebugFlags( bool *status );

  /** Die Funktion teilt mit, ob die Debug-Category aktiviert ist.
   */
  static bool DebugFlagEnabled(BugCategory Cat);

// ================================================================ //
// private member functions                                         //
// ================================================================ //
private:
  double getTimestamp() const;
  double elapsedSeconds( double ts ) const;

// ================================================================ //
// private Data                                                     //
// ================================================================ //
private:
  typedef std::map< std::string, BugCategory > CategoryMap;

#if HAVE_LOG4CPLUS
  typedef std::map< BugCategory, log4cplus::Logger> LoggerCategoryMap;
  static LoggerCategoryMap s_LoggerCategoryMap;
#endif
  static bool            s_BugIsEnabled[BugMax];
  static CategoryMap     s_categoryMap;
};

#endif // _INTENS_DEBUGGER_H_INCLUDED_
