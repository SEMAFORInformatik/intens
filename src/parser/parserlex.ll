%option noyywrap
%{
#include <string>
#include <sstream>
#include <fstream>

#include "utils/gettext.h"
#include "utils/NumLim.h"
#include "app/AppData.h"

#include "parser/IdManager.h"
#include "app/App.h"
#include "job/JobCodeStandards.h"

// only needed to prevent undeclared compile errors with QT
#ifdef HAVE_QT
typedef short Position;
#endif
using namespace std;

// FIX IT!
#include "job/JobFunction.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataPool.h"
#include "gui/GuiElement.h"
#include "gui/GuiListenerController.h"
#include "parser/Flexer.h"

class GuiPlotDataItem;
class DataReference;

#if defined(BISON_USE_PARSER_H_EXTENSION)
#include "parseryacc.h"
#else
#include "parseryacc.hh"
#endif

#define LAST(v) (v-1 + sizeof v / sizeof v[0])

bool IncludeFile( const std::string & );
bool endOfFile();
int reserved_word_type();
void setCurrentFilename( const char *filename );

int
  PAlineno = 1;

std::string sqlString;

%}

/* the "incl" state is used for picking up the name
 * of an include file
 */
%x incl
%x sql
%x sql_st_pr

/* ---------------------------------------------------------------- */
/*                            rules section                         */
/* ---------------------------------------------------------------- */

sql_declare     [Dd][Ee][Cc][Ll][Aa][Rr][Ee]
sql_begin       [Bb][Ee][Gg][Ii][Nn]
sql_select      [Ss][Ee][Ll][Ee][Cc][Tt]
sql_insert      [Ii][Nn][Ss][Ee][Rr][Tt]
sql_delete      delete
sql_update      [Uu][Pp][Dd][Aa][Tt][Ee]
sql_end         [Ee][Nn][Dd][ ]*[;]
file_include    INCLUDE
const_char      ['].[']
letter          [a-zA-Z]
digit           [0-9]
letter_or_digit [a-zA-Z_0-9#]
exp             [EeD][+-]?{digit}+
white_space     [ \t\r]
new_line        [\\][n]
quote           ["]
i18nbeg         [_][(]{white_space}*["]
i18nend         ["]{white_space}*[)]
res_beg         RESOURCE[(]{white_space}*["]
res_end         ["]{white_space}*[)]
dot		[.]
no_backslash    [.][^\\]
semicolon       [;]
other           .

%%
%{
  int t=App::Instance().getFlexer()->startToken();
  if ( t ){
    return t;
  }
%}

<INITIAL>{file_include}{white_space}+           BEGIN(incl);
<incl>[^{ \t\n\r}]+				{   std::string inclfile = YYText();
	  					    /* if inclfile is a RESOURCE */
						    if (inclfile.find("RESOURCE") == 0) {
						       std::string::size_type posB = inclfile.find_first_of("\"");
						       std::string::size_type posE = inclfile.find_last_of("\"");
						       if (posB != posE && posB !=  string::npos && posE !=  string::npos) {
						          std::string retStr;
						          std::string key = inclfile.substr(posB+1, posE-posB-1);
						          int ret = AppData::Instance().getResource(key, retStr, yylval.val_real, yylval.val_integer);
						          if (ret == tSTRING_CONSTANT) /* found a resource */
      						       	    inclfile = retStr;
  						          else
	   						    inclfile = ""; /* ignore this include */
						       }
						    }
                                                    if( inclfile.size() && IncludeFile( inclfile ) ){
							PAlineno = 1;
							}
						    BEGIN(0);
						}

<INITIAL>{sql_begin} |
         {sql_declare}     { BEGIN( sql_st_pr );
      			     yylval.val_string = 0;
			     sqlString = "";
			     sqlString += YYText(); }
<INITIAL>{sql_select} |
	 {sql_insert} |
	 {sql_delete} |
	 {sql_update}      { BEGIN( sql );
      			     yylval.val_string = 0;
			     sqlString = "";
			     sqlString += YYText(); }

<sql,sql_st_pr>\n          { sqlString += '\n';
			     ++PAlineno; }

<sql_st_pr>{sql_end}       { sqlString += YYText();
			     yylval.val_string = new std::string( sqlString );
			     BEGIN( 0 );
		             return tSQL_STRING; }

<sql_st_pr>{other}         { sqlString += YYText(); }

<sql>[^;]                  { sqlString += YYText(); }

<sql>{semicolon}           { yylval.val_string = new std::string( sqlString );
			     BEGIN( 0 );
		             return tSQL_STRING; }

<INITIAL>{letter}{letter_or_digit}* { /* identifier */
  return reserved_word_type();
}

<INITIAL>{digit}+{dot}{digit}*({exp})? |
<INITIAL>{digit}*{dot}{digit}+({exp})? |
<INITIAL>{digit}+{exp}                 { /* real */
/*  if( sscanf( YYText(), "%lf", &yylval.val_real ) != 1 ){ */
  yylval.val_real = NumLim::MAX_DOUBLE;
  std::istringstream is( YYText());
  is.imbue( std::locale::classic() );
  is >> yylval.val_real;
  if( yylval.val_real == NumLim::MAX_DOUBLE ){
    return tREAL_INVALID;
  }
  return tREAL_CONSTANT;
}

<INITIAL>{digit}+ { /* integer */
  sscanf( YYText(), "%d", &yylval.val_integer );
  return tINT_CONSTANT;
}

<INITIAL>{i18nbeg}[^"]*{i18nend} { /* string */
	/* Simon suggests to fix backslash problems in strings */
    const char *domain=AppData::Instance().LocaleDomain().c_str();
    /* this is to get rid of the surrounding quotes and parentheses: */
    const char *cbeg=YYText()+2;
    for( ;*cbeg!='\"'; ++cbeg );
    cbeg++;
    std::string::size_type l=yyleng-2;
    for( ; cbeg[l]!='\"'; --l )
      ;
    std::string s(cbeg, 0, l);
    yylval.val_string = new std::string( dgettext( domain,s.c_str() ) );

    for(size_t i=0,n=yylval.val_string->length();i<n;i++){
      char c = (*yylval.val_string)[i];
      if( c == '\n' )
        PAlineno++;
    }
    for( l=yylval.val_string->find("\\n"); l!=std::string::npos; l=yylval.val_string->find("\\n",l+1) ){
      yylval.val_string->replace( l, 2, "\n" );
    }
    return tSTRING_CONSTANT;
}

<INITIAL>{res_beg}[^"]*{res_end} { /* resource */
    const char *domain=AppData::Instance().LocaleDomain().c_str();
    /* this is to get rid of the surrounding quotes and parentheses: */
    const char *cbeg=YYText()+9;
    for( ;*cbeg!='\"'; ++cbeg );
    cbeg++;
    std::string::size_type l=yyleng-9;
    for( ; cbeg[l]!='\"'; --l )
      ;
    std::string s(cbeg, 0, l);
    for( l=s.find("\\n"); l!=std::string::npos; l=s.find("\\n",l+1) ){
      s.replace( l, 2, "\n" );
    }
    std::string retStr;
    int ret = AppData::Instance().getResource(s, retStr, yylval.val_real, yylval.val_integer);
    if (ret == tSTRING_CONSTANT)
      yylval.val_string = new std::string(retStr);
    return ret;
}

<INITIAL>{quote}((\\\\)*\\\")*([^\\](\\\\)*\\\"|[^"])*{quote} { /* string */
  int c;
  int      len;
  int      c2 = 0;

  yylval.val_string = new std::string(YYText(), 1, yyleng-2);
  if( !yylval.val_string )
    return tSTRING_CONSTANT;

  size_t j = 0;
  for(size_t i=0,n=yylval.val_string->length();i<n;i++,j++){
    char c = (*yylval.val_string)[i];
    if( c == '\n' )
      PAlineno++;
    if( c == '\\' ){
      if( ++i >= n )
        break;
      c = (*yylval.val_string)[i];
      switch(c){
        case '\n': PAlineno++; break;
        case '\\': c = '\\'; j++; break;
        case  '"': c =  '"'; break;
        default: c = '\\'; --i; break;
      }
    }
    (*yylval.val_string)[j] = c;
  }
  yylval.val_string->resize(j);

  return tSTRING_CONSTANT;
}

<INITIAL>{const_char} { /* char */
  yylval.val_char = YYText()[1];
  return tCHAR_CONSTANT;
}

<INITIAL>{new_line} {
  return tEOLN;
}

<INITIAL>^[#][ ]{digit}+.*\n {
	char buf[BUFSIZ];
  sscanf( YYText(),"# %d \"%s", &PAlineno, buf );
	setCurrentFilename( buf );
}

<INITIAL>">="                         { return tGEQ; }
<INITIAL>"<="                         { return tLEQ; }
<INITIAL>"=="                         { return tEQL; }
<INITIAL>"!="                         { return tNEQ; }
<INITIAL>"!"                          { return tNOT; }
<INITIAL>"||"                         { return tOR;  }
<INITIAL>"&&"                         { return tAND; }
<INITIAL>"++"                         { return tINCR; }
<INITIAL>"--"                         { return tDECR; }
<INITIAL>"+="                         { return tACCUMULATE; }
<INITIAL>[/][/].*                     ;
<INITIAL>{white_space}+               ;
<INITIAL>\n                           PAlineno++;

<INITIAL>{other}                      { return YYText()[0];                  }

     <<EOF>> {  if( endOfFile() )
                   yyterminate();
             }
%%

/* --------------------------------------------------------------------------- */
/* reserved_word_type --                                                       */
/* --------------------------------------------------------------------------- */

int reserved_word_type(){
  return App::Instance().getFlexer()->reserved_word_type();
}

/* --------------------------------------------------------------------------- */
/* IncludeFile --                                                              */
/* --------------------------------------------------------------------------- */

bool IncludeFile(const std::string &includeFilename ){
  return App::Instance().getFlexer()->IncludeFile( includeFilename, PAlineno, YY_BUF_SIZE );
}

/* --------------------------------------------------------------------------- */
/* endOfFile --                                                                */
/* --------------------------------------------------------------------------- */

bool endOfFile(){
  return App::Instance().getFlexer()->endOfFile( PAlineno );
}
/* --------------------------------------------------------------------------- */
/* setCurrentFilename --                                                       */
/* --------------------------------------------------------------------------- */

void setCurrentFilename(const char *filename){
  App::Instance().getFlexer()->setCurrentFilename( filename );
}

/* --------------------------------------------------------------------------- */
/* yylex --                                                                    */
/* --------------------------------------------------------------------------- */

int yylex(){
  return  App::Instance().getFlexer()->yylex();
}
