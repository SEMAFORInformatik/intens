
#ifndef _ERRORHANDLER_H
#define _ERRORHANDLER_H

#include <string>

/* extern void IPrintMessage( char *, int, const char *, ... ); */
/* #define IPrintMessage0( msg ) \ */
/*   IPrintMessage( __FILE__, __LINE__, (msg) ); */
/* #define IPrintMessage1( msg, arg1 ) \ */
/*   IPrintMessage( __FILE__, __LINE__, (msg), (arg1) ); */
/* #define IPrintMessage2( msg, arg1, arg2 ) \ */
/*   IPrintMessage( __FILE__, __LINE__, (msg), (arg1), (arg2) ); */
extern void IPrintMessage( const std::string & );

extern void ParserError( const std::string &s );
/* extern void ParserError( const std::string & , ... ); */
/* extern void ParserError( const std::string &, const std::string * ); */
extern void ERparserErrorMessage( const char *, const char * );
extern void ERsetErrorItemName( const std::string & );
/* extern void ERErrorMessage( const std::string &, const char *text ); */

//extern void yywhere( void );
void yyerror( const char *s );

#endif
