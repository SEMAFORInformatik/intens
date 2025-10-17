
#include <qobject.h>
#include <qfile.h>
#include <qurl.h>
#if QT_VERSION < 0x050000
#include <qhttp.h>
#endif
#include "gui/qt/HelpWindow.h"

#include "app/HelpURL.h"
#include "app/HelpManager.h"
#include "app/AppData.h"


/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/

HelpURL::HelpURL(HelpManager *helpManager)
  : HelpFile(helpManager){
}

static std::string escapeCmdArg( const std::string& cmd ){
  std::string res = "\"";
  for( std::string::const_iterator it = cmd.begin() ; it < cmd.end(); it++ ){
    switch( *it ){
      case '"':
      case '$':
      case '\\':
	res += '\\';
    }
    res += *it;
  }
  res += '\"';
  return res;
}

/* --------------------------------------------------------------------------- */
/* helpBrouserCmd --                                                           */
/* --------------------------------------------------------------------------- */

void HelpURL::helpBrowserCmd( std::string &cmd,  const std::string &keyName, bool remote ) const{

  if(!AppData::Instance().HeadlessWebMode()) {
#ifdef _WIN32
  cmd = getenv("BROWSER") ? getenv("BROWSER") : "start ";
#else
  cmd = getenv("BROWSER") ? getenv("BROWSER") : "xdg-open";
#endif
  }
  std::string fstr;
  if (m_FileName.find("://") == std::string::npos ) {
    std::string::size_type posA, posB;
    posA =  m_FileName.find_first_of('.');
    posB =  m_FileName.find_first_of('/');
    if (posA != std::string::npos && posB != std::string::npos &&
	posA > posB)
      fstr+= "file://";
    else
      fstr += "http://";
  } //else fstr += " ";
  fstr += m_FileName;
  if( !keyName.empty() ){
    fstr += "#";
    fstr += keyName;
  }
  cmd += " " + escapeCmdArg(fstr);
  if(!AppData::Instance().HeadlessWebMode()) {
    cmd += " &";
  }
  if ( AppData::Instance().Debug() ){
    std::cout << "helpBrowserCmd: '" << cmd << "'" << std::endl;
  }
}
