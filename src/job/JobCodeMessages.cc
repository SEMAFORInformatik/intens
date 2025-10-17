
#if HAVE_LOG4CPLUS
#if defined _WIN32
#undef UNICODE  // log4cplus wchar bug (see third-party/CMakeLists.txt UNICODE=OFF)
#endif
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

static log4cplus::Logger loggerDes = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("DescriptionFile"));
#define LOG_MSG(level, msg) \
  {\
    std::ostringstream __log_tmp_oss; \
    std::string __log_tmp_str; \
    __log_tmp_oss << msg; \
    __log_tmp_str = __log_tmp_oss.str(); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '\n', ' '); \
    std::replace(__log_tmp_str.begin(), __log_tmp_str.end(), '"', '\''); \
    if (level == "DEBUG") { \
      LOG4CPLUS_DEBUG(loggerDes, LOG4CPLUS_TEXT(__log_tmp_str)); \
    } \
    if (level == "INFO") { \
      LOG4CPLUS_INFO(loggerDes, LOG4CPLUS_TEXT(__log_tmp_str)); \
    }\
    if (level == "WARN") { \
      LOG4CPLUS_WARN(loggerDes, LOG4CPLUS_TEXT(__log_tmp_str)); \
    }\
    if (level == "ERROR") {\
      LOG4CPLUS_ERROR(loggerDes, LOG4CPLUS_TEXT(__log_tmp_str)); \
    }\
    if (level == "FATAL") {\
      LOG4CPLUS_FATAL(loggerDes, LOG4CPLUS_TEXT(__log_tmp_str)); \
    }\
  }
#endif

#include "job/JobIncludes.h"

#include "job/JobCodeMessages.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePrint::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePrint::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  if( v->isEndOfLine() ){
    eng->flushPrintStream();
  }
  else{
    v->print( eng->PrintStream() );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetErrorMsg::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetErrorMsg::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  if( v->isEndOfLine() ){
    eng->flushErrorStream();
  }
  else{
    v->print( eng->ErrorStream() );
  }
  eng->setError();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetError::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetError::execute");
  eng->setError();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeResetError::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeResetError::execute");
  eng->setError( false );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeLog::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetError::execute");

  std::ostringstream& os = eng->LogStream();
#if HAVE_LOG4CPLUS
  LOG_MSG(m_level, LOG4CPLUS_TEXT(os.str()))
#else
  BUG_MSG(os.str());
#endif
  eng->clearLogStream();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeLogMsg::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeLog::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;
  v->print( eng->LogStream() );
  return op_Ok;
}
