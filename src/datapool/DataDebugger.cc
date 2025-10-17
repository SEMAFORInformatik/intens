// ****************************************************************
// DataDebugger
//
// $Id: DataDebugger.cpp 1.8 2014/03/04 14:17:17Z bho Exp $
// ****************************************************************

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include "datapool/DataDebugger.h"
#include "utils/utils.h"

// ================================================================ //
// static variables                                                 //
// ================================================================ //

std::ostream *DataDebugger::s_out_stream = 0;
std::fstream *DataDebugger::s_out_file = 0;
std::string   DataDebugger::s_file_name( "DebuggerOutput.txt" );
bool          DataDebugger::s_file_open = false;
bool          DataDebugger::s_initialized = false;
int           DataDebugger::s_indent = 0;
DataDebugger::CategoryMap DataDebugger::s_category_list;
DataDebugger::CategoryStatus DataDebugger::s_enable_status;

// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //

DataDebugger::DataDebugger( int cat
                            , const std::string &filename
                            , int line
                            , const std::string &fun
                            , bool enter
                            , bool msgonly )
  : m_category( cat )
  , m_funktion( fun )
  , m_lineno( line )
  , m_exit_has_been_shown( msgonly ){
#if (_DEBUGGER_)
  std::string::size_type pos = filename.find_last_of( "/\\" );
  if( pos != std::string::npos ){
    m_filename = filename.substr( pos+1 );
  }
  else{
    m_filename = filename;
  }
  if( !is_initialized() ){
    initialize();
  }
  if( is_enabled() ){
    if( enter ){
      enter_msg( line, true );
    }
  }
#endif
}

DataDebugger::~DataDebugger(){
#if (_DEBUGGER_)
  if( is_enabled() && ! m_exit_has_been_shown ){
    exit_msg( 0, true );
  }
#endif
}

// ================================================================ //
// Member Functions                                                 //
// ================================================================ //

// ---------------------------------------------------------------- //
// init_category_list --                                            //
// ---------------------------------------------------------------- //

void DataDebugger::init_category_list(){
  s_category_list.insert( CategoryMap::value_type( "Misc"        , BugMisc ) );
  s_category_list.insert( CategoryMap::value_type( "BugRef"      , BugRef ) );
  s_category_list.insert( CategoryMap::value_type( "BugUndo"     , BugUndo ) );
  s_category_list.insert( CategoryMap::value_type( "BugTTrail"   , BugTTrail ) );
  s_category_list.insert( CategoryMap::value_type( "BugData"     , BugData ) );
}

// ---------------------------------------------------------------- //
// set_logname --                                                   //
// ---------------------------------------------------------------- //

void DataDebugger::set_logname( const std::string &name ){
  s_file_name = name;
}

// ---------------------------------------------------------------- //
// get_logname --                                                   //
// ---------------------------------------------------------------- //

std::string &DataDebugger::get_logname(){
  return s_file_name;
}

// ---------------------------------------------------------------- //
// set_log_indent --                                                //
// ---------------------------------------------------------------- //

void DataDebugger::set_log_indent( int indent ){
  s_indent = indent;
}

// ---------------------------------------------------------------- //
// get_log_indent --                                                //
// ---------------------------------------------------------------- //

int DataDebugger::get_log_indent(){
  return s_indent;
}

// ---------------------------------------------------------------- //
// initialize --                                                    //
// ---------------------------------------------------------------- //

void DataDebugger::initialize( std::ostream *ostr ){
#if (_DEBUGGER_)
  if( s_initialized ) return;

  s_out_stream = ostr;
  if( s_out_stream ){
    *s_out_stream << std::boolalpha;
    s_file_open = true;
  }

  modify_all_flags( false );
  s_initialized = true;

  init_category_list();

  int bug_max = 0;
  for( CategoryMap::iterator it = s_category_list.begin(); it != s_category_list.end(); ++it ){
    bug_max = maximum( it->second, bug_max );
  }
  bug_max++;
  s_enable_status.resize( bug_max, false );
#endif
}

// ---------------------------------------------------------------- //
// terminate --                                                     //
// ---------------------------------------------------------------- //

void DataDebugger::terminate(){
#if (_DEBUGGER_)
  modify_all_flags( false );
  
  if( s_file_open ){
    if( s_out_file != 0 ){
      s_out_file->close();
      delete s_out_file;
      s_out_file   = 0;
      s_out_stream = 0;
      s_file_open  = false;
    }
  }
#endif
}

// ---------------------------------------------------------------- //
// is_enabled --                                                    //
// ---------------------------------------------------------------- //

bool DataDebugger::is_enabled() const{
  return is_initialized() && is_enabled( m_category );
}

// ---------------------------------------------------------------- //
// isnt_enabled --                                                  //
// ---------------------------------------------------------------- //

bool DataDebugger::isnt_enabled() const{
  return !is_enabled();
}

// ---------------------------------------------------------------- //
// is_enabled --                                                    //
// ---------------------------------------------------------------- //

bool DataDebugger::is_enabled( int cat ){
#if (_DEBUGGER_)
  if( is_initialized() ){
    if( cat < (int)s_enable_status.size() ){
      return s_enable_status[cat];
    }
  }
#endif
  return false;
}

// ---------------------------------------------------------------- //
// is_initialized --                                                //
// ---------------------------------------------------------------- //

bool DataDebugger::is_initialized(){
  return s_initialized;
}

// ---------------------------------------------------------------- //
// open_file --                                                     //
// ---------------------------------------------------------------- //

void DataDebugger::open_file(){
#if (_DEBUGGER_)
  if( s_file_open ) return;

  s_out_file = new std::fstream;
  s_out_file->open( s_file_name.c_str(), std::ios::out | std::ios::app );

  s_out_file->setf( 0, std::ios::floatfield );
  s_out_file->precision(14);

  s_out_stream = s_out_file;
  s_file_open = true;

  *s_out_stream << "====> start tracing <=====" << std::endl;
#endif
}

// ---------------------------------------------------------------- //
// close_file --                                                    //
// ---------------------------------------------------------------- //

void DataDebugger::close_file(){
#if (_DEBUGGER_)
  if( !s_file_open ) return;

  if( s_out_stream != 0 ){
    *s_out_stream << "====> stop tracing <=====" << std::endl;
    s_out_stream = 0;
  }
  if( s_out_file != 0 ){
    delete s_out_file;
    s_out_file = 0;
  }
 s_file_open = false;
 s_file_name.clear();
#endif
}

// ---------------------------------------------------------------- //
// output --                                                        //
// ---------------------------------------------------------------- //

std::ostream &DataDebugger::output( int line ){
  open_file();

  if( s_out_stream == 0 ){
    if( line == 0 ){ 
      return std::cerr << "      ";
    }
    return std::cerr << std::setw(5) << line << " ";
  }
  if( line == 0 ){
    return *s_out_stream << "      ";
  }
  return *s_out_stream << std::setw(5) << line << " ";
}

// ---------------------------------------------------------------- //
// output_stream --                                                 //
// ---------------------------------------------------------------- //

std::ostream &DataDebugger::output_stream(){
  open_file();
  return *s_out_stream;
}

// ---------------------------------------------------------------- //
// enter_msg --                                                     //
// ---------------------------------------------------------------- //

std::ostream &DataDebugger::enter_msg( int line, bool implicit_call ){
  if( isnt_enabled() ){
    return output( line ) << "DataDebugger is disabled" << std::endl;
  }
  std::ostream &ostr = output( line ) << "-> ";
  for( int x = 0; x < s_indent; ++x ) ostr << ".";
  s_indent++;

  ostr << "[" << m_filename << "] " << m_funktion;
  if( implicit_call ){
    return ostr << std::endl << std::flush;
  }
  return ostr << " ";
}

// ---------------------------------------------------------------- //
// exit_msg --                                                      //
// ---------------------------------------------------------------- //

std::ostream & DataDebugger::exit_msg( int line, bool implicit_call ){
  if( isnt_enabled() ){
    return output( line ) << "DataDebugger is disabled" << std::endl;
  }

  s_indent--;
  std::ostream &ostr = output( line ) << "<- ";
  for( int x = 0; x < s_indent; ++x ) ostr << ".";

  m_exit_has_been_shown = true;
  ostr << "[" << m_filename << "] " << m_funktion;
  if( implicit_call ){
    return ostr << std::endl << std::flush;
  }
  return ostr << " ";
}

// ---------------------------------------------------------------- //
// msg --                                                           //
// ---------------------------------------------------------------- //

std::ostream &DataDebugger::msg( int line ){
  if( isnt_enabled() ){
    return output( line ) << "DataDebugger is disabled" << std::endl;
  }

  std::ostream &ostr = output( line ) << "   ";
  for( int x = 0; x < s_indent; ++x ) ostr << ".";

  if( m_exit_has_been_shown ){
    return ostr << "[" << m_filename << "] " << m_funktion << " ";
  }
  return ostr << "   ";
}

// ---------------------------------------------------------------- //
// modify_flag --                                                   //
// ---------------------------------------------------------------- //

bool DataDebugger::modify_flag( const std::string &name, bool val ){
#if (_DEBUGGER_)
  CategoryMap::iterator it = s_category_list.find( name );
  if( it != s_category_list.end() ){
    return modify_flag( (BugCategory)it->second, val );
  }
  if( name == "All" ){
    modify_all_flags( val );
    return true;
  }
#endif
  return false;
}

// ---------------------------------------------------------------- //
// modify_flag --                                                   //
// ---------------------------------------------------------------- //

bool DataDebugger::modify_flag( BugCategory cat, bool val ){
#if (_DEBUGGER_)
  if( (CategoryStatus::size_type)cat < s_enable_status.size() ){
    s_enable_status[ cat ] = val;
    return true;
  }
#endif
  return false;
}

// ---------------------------------------------------------------- //
// modify_all_flags --                                              //
// ---------------------------------------------------------------- //

void DataDebugger::modify_all_flags( bool val ){
#if (_DEBUGGER_)
  for( CategoryStatus::iterator it = s_enable_status.begin(); it != s_enable_status.end(); ++it ){
    *it = val;
  }
#endif
}
