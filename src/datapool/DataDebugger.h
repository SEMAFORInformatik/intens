// ****************************************************************
// DataDebugger
//
// $Id: DataDebugger.h 1.5 2014/03/04 14:17:17Z bho Exp $
// ****************************************************************

#if !defined(__DP_DEBUGGER_H__)
#define __DP_DEBUGGER_H__

#include <string>
#include <map>
#include <vector>

#if !defined(__DP__STANDALONE_VS2013__)
#include "utils/gettext.h"
#include "utils/Debugger.h"
#else
// ================================================================ //
// Macros                                                           //
// ================================================================ //

#define BugCharPtr(p) ((p) == 0 ? "<Null>" : (p))
#define BugPtr(p)     (void *)(p)
#define BugBool(b) ((b) ? "true" : "false")

// ================================================================ //
// Macros für Memory-Checks                                         //
// ================================================================ //

#ifdef _DEBUGGER_
#define BUG_DECLARE_COUNT    static int s_bug_count
#define BUG_INIT_COUNT(OBJ)  int OBJ::s_bug_count = 0
#define BUG_INCR_COUNT       s_bug_count++
#define BUG_DECR_COUNT       --s_bug_count
#define BUG_DECL_SHOW_COUNT(OBJ) \
  static const std::string BUG_classname() { return OBJ; } \
  static int BUG_show_count( std::ostream &ostr, bool conditional ) \
    { if( conditional ? s_bug_count > 0 : true ){ \
        ostr << s_bug_count << " Object(s) of " \
             << BUG_classname() << " found" << std::endl; \
      } \
      return s_bug_count; \
    } \
  static int BUG_count() { return s_bug_count; }
#define BUG_SHOW_COUNT(OBJ,ostr,cond)  OBJ::BUG_show_count( ostr, cond )
#define BUG_INSERT_COUNT(OBJ,mem) mem.insert( OBJ::BUG_classname(), OBJ::BUG_count() )
#else
#define BUG_DECLARE_COUNT
#define BUG_INIT_COUNT(OBJ)
#define BUG_INCR_COUNT
#define BUG_DECR_COUNT
#define BUG_DECL_SHOW_COUNT(OBJ)
#define BUG_SHOW_COUNT(OBJ,ostr,cond) 0
#define BUG_INSERT_COUNT(OBJ,mem)
#endif

// ================================================================ //
// Macros für Trace-Feature                                         //
// ================================================================ //

#if (_DEBUGGER_)
#define BUG_LOGNAME(X)    Debugger::set_logname( X )
#define BUG_INDENT(X)     Debugger::set_log_indent( X )
#define BUG_INITIALIZE(X) DataDebugger::initialize( X )
#define BUG_CLOSE         Debugger::close_file()
#define BUG_TERMINATE     DataDebugger::terminate()
#define BUG(C,F)          DataDebugger __DEBUGGER__(C,__FILE__,__LINE__,F,true)
#define BUG_SILENT(C,F)   DataDebugger __DEBUGGER__(C,__FILE__,__LINE__,F,false,true)
#define BUG_PARA(C,F,P)   DataDebugger __DEBUGGER__(C,__FILE__,__LINE__,F,false); \
                          do { if(__DEBUGGER__.is_enabled()){ \
                                 __DEBUGGER__.enter_msg(__LINE__) << P << std::endl; \
                              } \
                          } while(0)
#define BUG_EXIT(P)       do { if(__DEBUGGER__.is_enabled()){ \
                                 __DEBUGGER__.exit_msg(__LINE__) << P << std::endl; \
                               } \
                          } while(0)
#define BUG_MSG(P)        do { if (__DEBUGGER__.is_enabled()){ \
                                 __DEBUGGER__.msg(__LINE__) << P << std::endl; \
                               } \
                          } while(0)
#define BUG_FATAL(Para)   BUG_MSG(Para)
#else
#define BUG_LOGNAME(X)
#define BUG_INDENT(X)
#define BUG_INITIALIZE(X)
#define BUG_CLOSE
#define BUG_TERMINATE
#define BUG(Category,Fun)
#define BUG_SILENT(Category,Fun)
#define BUG_PARA(Category,Fun,Para)
#define BUG_EXIT(Para)
#define BUG_MSG(Para)
#define BUG_FATAL(Para)
#endif

/** Beschreibung der Aufzaehlung BugCategory
    \todo Die Beschreibung der Aufzaehlung BugCategory fehlt
 */
enum BugCategory {
     BugMisc /** Beschreibung fehlt */  = 0
    ,BugRef /** Beschreibung fehlt */
    ,BugUndo /** Beschreibung fehlt */
    ,BugTTrail /** Beschreibung fehlt */
    ,BugData /** Beschreibung fehlt */
    ,BugNone           // need this for initialization /** Beschreibung fehlt */
    ,BugMax            // Maximum Debugging. Must be last in Enum
  };
/** \enum BugCategory
    \todo Die Beschreibung der Aufzaehlungswerte ist unvollstaendig
 */



///
class DataDebugger
{
// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //
public:
  ///
  DataDebugger( int category
               , const std::string &filename
               , int line
               , const std::string &fun
               , bool enter=false
               , bool msgonly=false );
  ///
  virtual ~DataDebugger();

private:

  /** Beschreibung der Funktion DataDebugger
      \param ref
      \todo Die Beschreibung der Funktion DataDebugger fehlt
   */
  DataDebugger( const DataDebugger &ref ); // unused

  /** Beschreibung der Funktion operator=
      \param ref
      \return
      \todo Die Beschreibung der Funktion operator= fehlt
   */
  DataDebugger &operator=( const DataDebugger &ref ); // unused

// ================================================================ //
// public member functions                                          //
// ================================================================ //
public:

  /** Beschreibung der Funktion set_logname
      \param name
      \return
      \todo Die Beschreibung der Funktion set_logname fehlt
   */
  static void set_logname( const std::string &name );

  /** Beschreibung der Funktion get_logname
      \return
      \todo Die Beschreibung der Funktion get_logname fehlt
   */
  static std::string &get_logname();

  /** Beschreibung der Funktion set_log_indent
      \param indent
      \return
      \todo Die Beschreibung der Funktion set_log_indent fehlt
   */
  static void set_log_indent( int indent );

  /** Beschreibung der Funktion get_log_indent
      \return
      \todo Die Beschreibung der Funktion get_log_indent fehlt
   */
  static int get_log_indent();

  /** Beschreibung der Funktion output_stream
      \return
      \todo Die Beschreibung der Funktion output_stream fehlt
   */
  static std::ostream &output_stream();

  /** Beschreibung der Funktion msg
      \param line
      \return
      \todo Die Beschreibung der Funktion msg fehlt
   */
  std::ostream &msg( int line );

  /** Beschreibung der Funktion enter_msg
      \param line
      \param implicit_call
      \return
      \todo Die Beschreibung der Funktion enter_msg fehlt
   */
  std::ostream &enter_msg( int line, bool implicit_call = false );

  /** Beschreibung der Funktion exit_msg
      \param line
      \param implicit_call
      \return
      \todo Die Beschreibung der Funktion exit_msg fehlt
   */
  std::ostream &exit_msg( int line, bool implicit_call = false );

  /** Beschreibung der Funktion terminate
      \return
      \todo Die Beschreibung der Funktion terminate fehlt
   */
  static void terminate();

  /** Beschreibung der Funktion initialize
      \param ostr
      \return
      \todo Die Beschreibung der Funktion initialize fehlt
   */
  static void initialize( std::ostream *ostr = 0 );

  /** Beschreibung der Funktion is_initialized
      \return
      \todo Die Beschreibung der Funktion is_initialized fehlt
   */
  static bool is_initialized();

  /** Beschreibung der Funktion open_file
      \return
      \todo Die Beschreibung der Funktion open_file fehlt
   */
  static void open_file();

  /** Beschreibung der Funktion close_file
      \return
      \todo Die Beschreibung der Funktion close_file fehlt
   */
  static void close_file();

  /** Beschreibung der Funktion is_enabled
      \return
      \todo Die Beschreibung der Funktion is_enabled fehlt
   */
  bool is_enabled() const;

  /** Beschreibung der Funktion isnt_enabled
      \return
      \todo Die Beschreibung der Funktion isnt_enabled fehlt
   */
  bool isnt_enabled() const;

  /** Beschreibung der Funktion is_enabled
      \param category
      \return
      \todo Die Beschreibung der Funktion is_enabled fehlt
   */
  static bool is_enabled( int category );

  /** Beschreibung der Funktion modify_flag
      \param name
      \param val
      \return
      \todo Die Beschreibung der Funktion modify_flag fehlt
   */
  static bool modify_flag( const std::string &name, bool val );

  /** Beschreibung der Funktion modify_flag
      \param cat
      \param val
      \return
      \todo Die Beschreibung der Funktion modify_flag fehlt
   */
  static bool modify_flag( BugCategory cat, bool val );

  /** Beschreibung der Funktion modify_all_flags
      \param val
      \return
      \todo Die Beschreibung der Funktion modify_all_flags fehlt
   */
  static void modify_all_flags( bool val );

// ================================================================ //
// protected member functions                                       //
// ================================================================ //
protected:

  /** Beschreibung der Funktion init_category_list
      \return
      \todo Die Beschreibung der Funktion init_category_list fehlt
   */
  static void init_category_list();

  /** Beschreibung der Funktion output
      \param line
      \return
      \todo Die Beschreibung der Funktion output fehlt
   */
  std::ostream &output( int line );

// ================================================================ //
// private Data                                                     //
// ================================================================ //
private:
  typedef std::map<std::string, int> CategoryMap;
  typedef std::vector<bool> CategoryStatus;

  static std::ostream   *s_out_stream;
  static std::fstream   *s_out_file;
  static std::string     s_file_name;
  static bool            s_file_open;
  static bool            s_initialized;
  static int             s_indent;
  static CategoryMap     s_category_list;
  static CategoryStatus  s_enable_status;

  std::string            m_funktion;
  std::string            m_filename;
  int                    m_lineno;
  int                    m_category;
  bool                   m_exit_has_been_shown;
};
#endif // __DP__STANDALONE_VS2013__

#endif
/** \file */
