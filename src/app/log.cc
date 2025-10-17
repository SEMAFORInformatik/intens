
#if defined __MINGW32__ && defined HAVE_QT
// #include <log.hh>
#include <iostream>
typedef enum {
    LogDebug    /* debug messages                        */
  , LogInfo     /* informational messages                */
  , LogNotice   /* conditions that need special handling */
  , LogWarning  /* warning messages                      */
  , LogError    /* errors                                */
  , LogFatal    /* conditions that should be corrected   */
} SeverityLevel;
void Llog( const SeverityLevel level, const char *who, const char *what, ... ) {
//   std::cerr << " WARNING  Llog not implemented!!!" << std::endl << std::flush;
}
#else
#include <stdarg.h>

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>

#include "app/log.h"
#include <string.h>

#define FACILITY        LOG_USER  /* LOG_LOCAL0-7 */
/* This is the syslog facility to be assigned to all system log messages.

   Chose either:

     LOG_USER
     LOG_LOCAL0
     LOG_LOCAL1
     LOG_LOCAL2
     LOG_LOCAL3
     LOG_LOCAL4
     LOG_LOCAL5
     LOG_LOCAL6
     LOG_LOCAL7
   */
static char * getdatetime( void );
static int    sys_priority( SeverityLevel level );

static SeverityLevel  current_level = LogInfo;
static FILE *         logfile       = stderr;

/*--------------------------------------------------------------------------
  LopenLog --

  opens the logfile (stderr is default)

 */
void LopenLog( const char *ident, const int local )
{
  if( (logfile != stderr) && (logfile != NULL) )
    LcloseLog();

  if( local ){ /* don't use the system logger */
    if( ident != NULL ){
      if( ident[0] != '\0' ){

	if( logfile == NULL )
	  closelog(); /* closes the system log */

	if( (logfile=fopen( ident, "a" )) != NULL )
	  return;

	logfile = stderr;
	Llog( LogWarning, "LopenLog", "%s: %s", ident, strerror(errno) );
	return;
      }
    }
    logfile = stderr;
    return;
  }
  /* We use the system logger:
     syslog() passes message to syslogd, which logs it  in  an
     appropriate  system  log,  writes  it to the system console,
     forwards it to a list of users, or forwards it to  the  sys-
     logd  on  another  host  over  the  network.  The message is
     tagged with a priority of priority.  The message looks  like
     a  printf  string  except  that  %m  is  replaced by the
     current error message (collected from  errno).   A  trailing
     NEWLINE is added if needed.
     */
  openlog( ident, LOG_PID, FACILITY );
  logfile = 0;
}

/*--------------------------------------------------------------------------
  LsetLogMask --

  all messages with a severity level higher than or equal to "level" will be
  written to the logfile
  */
void LsetLogMask( const SeverityLevel level )
{
  int pri;

  current_level = level;
  if( logfile != NULL )
    return;

  pri = sys_priority(level);
  setlogmask( LOG_UPTO(pri) );
}
/*--------------------------------------------------------------------------
  Llog --

  writes the log message to the logfile if "level" is equal or higher than
  "current_level"
  */
void Llog( const SeverityLevel level
	   , const char *who, const char *what, ... )
{
  va_list ap;

  if( level < current_level )
    return;

  va_start( ap, what );
  if( logfile == NULL ){/* inform the system logger: */
    char buf[BUFSIZ];
    vsprintf( buf, what, ap );
    syslog( sys_priority( level ), buf, 1 );  // 1: dummy format argument to avoid compiler warning
  }
  else{ /* use the log file (or stderr) */
    fputs( getdatetime(), logfile );
    fputs( " ", logfile );
    fputs( who, logfile );
    fputs( ": ", logfile );
    vfprintf( logfile, what, ap );
    fputc( '\n', logfile );
  }
  va_end( ap );
  return;
}
/*--------------------------------------------------------------------------
  closelog --

  closes the logfile.
  */
void LcloseLog( )
{
  if( logfile == NULL )
    closelog();
  else{
    if( logfile != stderr ){
      Llog( LogInfo, "LcloseLog", "closing logfile" );
      fclose( logfile );
    }
  }
  logfile = stderr;
}
/*--------------------------------------------------------------------------
  sys_priority --

  maps the SeverityLevel to the system priorities
  */
static int sys_priority( SeverityLevel level )
{
  switch( level ){
  case LogDebug:
    return LOG_DEBUG;
  case LogInfo:
    return LOG_INFO;
  case LogNotice:
    return LOG_NOTICE;
  case LogWarning:
    return LOG_WARNING;
  case LogError:
    return LOG_ERR;
  default:
    return LOG_CRIT;
  }
}
/*--------------------------------------------------------------------------
  getdatetime --

  returns the local date and time using the format yyyy/mm/dd hh:mm:ss
  */
static char *getdatetime(void )
{
  static char timebuf[100];
  time_t now;

  (void) time( &now );
  strftime( timebuf, sizeof timebuf, "%Y/%m/%d %H:%M:%S", localtime( &now ) );
  return timebuf;
}

#endif
