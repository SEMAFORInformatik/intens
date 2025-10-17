
/*
 * ISO language abbrevations
 *
 *  US-English         us
 *  GB-English         gb
 *  Deutsch            de
 *  Franzoesisch       fr
 *  Schweizerdeutsch   de_ch
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <iostream>

#include "app/AppData.h"
#include "app/App.h"
#include "utils/utils.h"
#include "utils/gettext.h"
#include "utils/ParserException.h"

#include "gui/GuiFactory.h"

#include "gui/GuiScrolledText.h"
#include "parser/Flexer.h"
#include "parser/errorhandler.h"

FILE * yyerfp = stderr;

//extern char *yytext;
extern int   yyleng;
extern int   PAlineno;
extern std::string ErrorItemName;

/*------------------------------------------------------------
  yywhere --

  ------------------------------------------------------------
*/
void yywhere( void )
{
  std::ostringstream os;
  Flexer *flexer = App::Instance().getFlexer();
  if (PAlineno > 0) {
    int lineNo = PAlineno -(*(flexer->YYText() ) == '\n' || ! *(flexer->YYText()));
    if (*(flexer->YYText())) {
      int i;
      for (i=0; i<20; i++)
        if (!(flexer->YYText())[i] || (flexer->YYText())[i] == '\n')
          break;
      if (i){
        os << (flexer->YYText());
      }
    }
    std::cerr << compose(_(":%1: at \"%2\": "),lineNo, os.str() );
  }

}
/*------------------------------------------------------------
  ERyyerror --

  ------------------------------------------------------------
*/
void ERyyerror(const std::string& s)
{
  std::cerr << App::Instance().getFlexer()->getCurrentFilename();
  yywhere();
  std::cerr << s <<std::endl;
}
/*------------------------------------------------------------
  yyerror --

  ------------------------------------------------------------
*/
void yyerror(const char *s)
{
  std::string filename = App::Instance().getFlexer()->getCurrentFilename();
  filename = filename.substr(0, filename.find_last_of('"'));
  std::cerr << filename;
  yywhere();
  std::cerr << _("Parser error.") << std::endl;
}
/*------------------------------------------------------------
  ERParserErrorMessage --

  ------------------------------------------------------------
*/
void ERparserErrorMessage( const char *err, const char *str )
{
  fprintf( stderr, "%s %s!\n", err, str );
  exit(1);
}

/* --------------------------------------------------------------------------- */
/* ParserError --                                                              */
/* --------------------------------------------------------------------------- */
void ParserError( const std::string &s ){
  ERyyerror( s );

  if (App::Instance().getFlexer()->getCurrentFilename().size()) {
    IPrintMessage(s);
    throw ch_semafor_intens::ParserException( "parser::ParserError",
					      "Failed to parse string\n" + s
                           );
  }
  exit(1);
}


void IPrintMessage( const std::string &errmsg ){
  AppData &appdata = AppData::Instance();
  GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : " + errmsg + "\n" );
}

/*--------------------------------------------------------------------
  ERsetErrorItemName --

  --------------------------------------------------------------------
*/
void ERsetErrorItemName( const std::string &tErrorItemName )
{
  ErrorItemName = tErrorItemName;
}
