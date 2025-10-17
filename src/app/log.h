#ifndef LOG_H
#define LOG_H

#ifndef NO_DEBUG
#define LOGASSERT( ex ) if( (!ex) ){ \
   Llog( LogFatal, __FILE__, "%d: %s", __LINE__, #ex ); abort(); }
#else
#define ASSERT( ex )
#endif

typedef enum {
    LogDebug    /* debug messages                        */
  , LogInfo     /* informational messages                */
  , LogNotice   /* conditions that need special handling */
  , LogWarning  /* warning messages                      */
  , LogError    /* errors                                */
  , LogFatal    /* conditions that should be corrected   */
} SeverityLevel;

void LopenLog( const char *name, const int local );
/*
  opens the log file if local is 1. Otherwise the messages will be
  passed to the system log using the name as "ident".
  In local mode, it uses stderr if "name" is NULL or if the file
  can't be opened. Can be called more than once.
  */

void LsetLogMask( const SeverityLevel level );
/*
  Sets the severity level.
  All messages with a severity level lower than "level" will be ignored.
  Default is LogInfo.
  */

void Llog( const SeverityLevel level, const char *who, const char *what, ... );
/*
  Logs the message to the log file (stderr is default)
  */

void LcloseLog( );
/*
  closes the log file
  */
#endif
