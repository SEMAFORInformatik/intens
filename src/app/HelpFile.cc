
#include "app/log.h"
#include "app/HelpFile.h"
#include "app/HelpManager.h"
#include "app/UiManager.h"
#include "app/AppData.h"
#include "utils/gettext.h"



HelpFile::HelpFile( HelpManager *helpManager )
  : m_Manager( helpManager )
  , m_MenuHidden( false ){
}

HelpFile::~HelpFile(){
}

/* --------------------------------------------------------------------------- */
/* setMenuTitle --                                                             */
/* --------------------------------------------------------------------------- */

void HelpFile::setMenuTitle( const std::string &title ){
  m_Title = title;
}

/* --------------------------------------------------------------------------- */
/* setFileName --                                                              */
/* --------------------------------------------------------------------------- */

void HelpFile::setFileName( const std::string &name ){
//   if( name.empty() )
//     return;

//   if( name[0] != '/' ){
//     std::string    dir = AppData::Instance().HelpFileDirectory();
//     m_FileName = dir + name;
//   }
//   else {
    m_FileName = name;
//   }

}

/* --------------------------------------------------------------------------- */
/* hideHelpMenu --                                                             */
/* --------------------------------------------------------------------------- */

void HelpFile::hideHelpMenu(){
  m_MenuHidden = true;
}

/* --------------------------------------------------------------------------- */
/* installHelpKey --                                                           */
/* --------------------------------------------------------------------------- */

void HelpFile::installHelpKey( const std::string &keyName, int chapterNo ){
  m_Manager->installHelpKey( this, keyName, chapterNo );
}

/* --------------------------------------------------------------------------- */
/* installHelpCallback --                                                      */
/* --------------------------------------------------------------------------- */

void HelpFile::installHelpCallback(){
  if( !m_MenuHidden ){
    std::string title = m_Title;

    if( title.empty() ){
      title = m_FileName;
    }

    UImanager &ui = UImanager::Instance();
    ui.addHelp( title, this );

    if( false ){
      Llog( LogWarning, "HPinstallHelpFile"
	    , _("Help title '%s' already specified.")
	    , title.c_str() );
      return;
    }
  }
  Llog( LogDebug, "HPinstalledHelpfile"
       , "Helpfile '%s' installed\n"
       , m_FileName.c_str() );
}

/* --------------------------------------------------------------------------- */
/* help --                                                                     */
/* --------------------------------------------------------------------------- */

void HelpFile::help(const std::string &keyName, const int chapterNo){
  m_Manager->helpBrowser( this, keyName );
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                                     */
/* --------------------------------------------------------------------------- */

void HelpFile::ButtonPressed(){
  help();
}
