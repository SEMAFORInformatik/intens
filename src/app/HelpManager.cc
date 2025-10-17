
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "app/HelpBrowser.h"

#include "app/log.h"
#include "app/HelpManager.h"
#include "app/AppData.h"
#include "gui/GuiFactory.h"
#include "gui/Timer.h"
#include "app/UiManager.h"
#include "app/HelpText.h"
#include "app/HelpHTML.h"
#include "app/HelpURL.h"
#include "app/HelpFile.h"
#include "job/JobWebApiResponse.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

static Process *s_currentProcess;

class HelpCall {
public:
  HelpCall( HelpFile *helpFile, std::string keyName )
    : m_HelpFile( helpFile )
  , m_KeyName( keyName ){
  }
  virtual ~HelpCall(){}

  HelpFile  *getHelpFile() const {return m_HelpFile;}
  std::string     getKeyName() const {return m_KeyName;}

private:
  HelpFile  *m_HelpFile;
  std::string     m_KeyName;
};

/*******************************************************************************/
/* initialize static variable                                                  */
/*******************************************************************************/

HelpManager *HelpManager::s_Instance = 0;

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

HelpManager &HelpManager::Instance(){
  if( s_Instance == 0 ){
    s_Instance = new HelpManager;
  }
  return *s_Instance;
}

HelpManager::HelpManager()
  : m_helpCall(0){
  s_currentProcess = &m_remoteBrowserProcess;
  m_timer=GuiFactory::Instance()->createTimer( 0 );
  m_timer->addTask( this );
}

HelpManager::~HelpManager(){
  if (m_timer) delete m_timer;
}


/* --------------------------------------------------------------------------- */
/* getHelpKey --                                                               */
/* --------------------------------------------------------------------------- */

HelpManager::HelpKey * HelpManager::getHelpKey( const std::string &name ) const{
  HelpKeyList::iterator iter = m_HelpKeyList.find( name );
  if( iter == m_HelpKeyList.end() ){
    return 0;
  }
  return &(iter->second);
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* addHelpText --                                                              */
/* --------------------------------------------------------------------------- */

HelpText *HelpManager::addHelpText( const std::string &name ){
  HelpFileList::iterator iter = m_HelpFiles.find( name );
  if( iter != m_HelpFiles.end() ){
    return 0;
  }
  HelpText *helpfile = new HelpText(this);
  helpfile->setFileName( name );
  return helpfile;
}

/* --------------------------------------------------------------------------- */
/* addHelpHTML --                                                              */
/* --------------------------------------------------------------------------- */

HelpFile *HelpManager::addHelpHTML( const std::string &name ){
  HelpFileList::iterator iter = m_HelpFiles.find( name );
  if( iter != m_HelpFiles.end() ){
    return 0;
  }
  HelpHTML *helpfile = new HelpHTML(this);
  helpfile->setFileName( name );
  return helpfile;
}

/* --------------------------------------------------------------------------- */
/* addHelpURL --                                                              */
/* --------------------------------------------------------------------------- */

HelpFile *HelpManager::addHelpURL( const std::string &name ){
  HelpFileList::iterator iter = m_HelpFiles.find( name );
  if( iter != m_HelpFiles.end() ){
    return 0;
  }
  HelpURL *helpfile = new HelpURL(this);
    helpfile->setFileName( name );
  return helpfile;
}

/* --------------------------------------------------------------------------- */
/* installHelpKey --                                                           */
/* --------------------------------------------------------------------------- */

void HelpManager::installHelpKey( HelpFile *const helpFile, const std::string &name, int chapterNo ){
  assert(helpFile!=0);
  if ( AppData::Instance().Debug() )
    std::cerr << "Key is " << name << std::endl;
  HelpKeyList::iterator hk = m_HelpKeyList.find( name );
  if( hk != m_HelpKeyList.end() ){
    std::cerr << "Helpkey " << name << " is already installed" << std::endl;
    return;
  }

  HelpKey key( name, helpFile, chapterNo);
  m_HelpKeyList.insert( HelpKeyList::value_type( name, key ) );
}

/* --------------------------------------------------------------------------- */
/* getFileName --                                                              */
/* --------------------------------------------------------------------------- */

std::string HelpManager::getFileName( const std::string &name ) const{
  std::ifstream  file( name.c_str() );
  std::string    fileName;
  std::string    dir = AppData::Instance().HelpFileDirectory();

  if (file.fail()) {
    if ( !dir.empty() ) {
      if ( name[0] == '/' ){
        fileName = name;
      }
      else
      if( name.size() > 2 ){
        if ( name[0] == '.' && name[1] == '/') {
	  fileName = dir + name.substr(2);
	}
        else{
          fileName = dir + name;
	}
      }
      else{
        fileName = dir + name;
      }
    }
    else{
      fileName = name;
    }
  }
  else{
    fileName = name;
  }
  if( file.is_open() )
    file.close();
  return fileName;
}

/* --------------------------------------------------------------------------- */
/* installHelpText --                                                          */
/* --------------------------------------------------------------------------- */

HelpFile *HelpManager::installHelpText( const std::string &name ){
  HelpText *helpFile;
  std::string    fileName;
//   MultiLanguage &mls = MultiLanguage::Instance();

  fileName=getFileName(name);

  if(( helpFile=(addHelpText( fileName ))) == NULL ){
    Llog( LogWarning, "HPinstallHelpfile"
// 	  ,mls.getString( "ML_HM_HELPFILE_IS_INST", ML_CONST_STRING ).c_str()
	  , _("Helpfile '%s' is already installed")
	  ,fileName.c_str() );
    return 0;
  }
  m_HelpFiles.insert( HelpFileList::value_type( fileName, helpFile ) );

  if( helpFile->openHelpFile() ) {
    return 0;
  }
  if ( helpFile->inspectHelpText() ) {
    helpFile->closeHelpFile();
    return 0;
  }

  helpFile->closeHelpFile();

  return static_cast<HelpFile *>(helpFile);
}

/* --------------------------------------------------------------------------- */
/* installHelpHTML --                                                          */
/* --------------------------------------------------------------------------- */

HelpFile *HelpManager::installHelpHTML( const std::string &name ){
  HelpFile *helpFile;

  if( (helpFile=addHelpHTML( name )) == NULL ){
    Llog( LogWarning, "HPinstallHelpfile"
// 	  , MultiLanguage::Instance().getString( "ML_HM_HELPFILE_IS_INST", ML_CONST_STRING ).c_str()
	  , _("Helpfile '%s' is already installed")
	  , name.c_str() );

    HelpFileList::iterator iter = m_HelpFiles.find( name );
    if( iter == m_HelpFiles.end() ){
      return 0;
    }
    helpFile = static_cast<HelpHTML *>(iter->second);
  }
  return static_cast<HelpFile *>(helpFile);
}

/* --------------------------------------------------------------------------- */
/* installHelpURL --                                                           */
/* --------------------------------------------------------------------------- */

HelpFile *HelpManager::installHelpURL( const std::string &name ){
  HelpFile *helpFile;

  if( (helpFile=addHelpURL( name )) == NULL ){
    Llog( LogWarning, "HPinstallHelpfile"
// 	  , MultiLanguage::Instance().getString( "ML_HM_HELPFILE_IS_INST", ML_CONST_STRING ).c_str()
	  , _("Helpfile '%s' is already installed")
	  , name.c_str() );

    HelpFileList::iterator hf = m_HelpFiles.find( name );
    if( hf == m_HelpFiles.end() ){
      return 0;
    }
    helpFile = static_cast<HelpURL *>(hf->second);
  }
  return static_cast<HelpFile *>(helpFile);
}


/* --------------------------------------------------------------------------- */
/* helpBrowser --                                                              */
/* --------------------------------------------------------------------------- */

void HelpManager::helpBrowser( HelpFile *const helpFile, const std::string &keyName ){
  std::string cmd;

  // first try with remote
  helpFile->helpBrowserCmd(cmd, keyName, true );
  if( cmd.empty() ){
    return;
  }
  if(AppData::Instance().HeadlessWebMode()) {
    trim(cmd, "\"");
    JobWebApiResponse::setNextOpenURL(cmd);
    return;
  }

  m_remoteBrowserProcess.setExecCmd( cmd );
  assert( m_helpCall == 0 );
  m_helpCall = new HelpCall(helpFile, keyName);
  m_remoteBrowserProcess.start();
  m_timer->start();
}

/* --------------------------------------------------------------------------- */
/* tick --                                                                     */
/* --------------------------------------------------------------------------- */

void HelpManager::tick() {
  assert( m_helpCall != 0 );
  HelpManager::Instance().manageStartBrowser( m_helpCall );
  delete m_helpCall;
  m_helpCall = 0;
}

/* --------------------------------------------------------------------------- */
/* manageStartBrowser --                                                       */
/* --------------------------------------------------------------------------- */

bool HelpManager::manageStartBrowser( HelpCall *hc ){
  if( !s_currentProcess->hasTerminated() ){
    return false; /* still starting */
  }

  /* second try to start Netscape */

  if(s_currentProcess->getExitStatus()==1 && s_currentProcess != &m_browserProcess){
    std::string cmd;
    hc->getHelpFile()->helpBrowserCmd( cmd
				       , hc->getKeyName()
				       , false );
    s_currentProcess = &m_browserProcess;
    s_currentProcess->setExecCmd(cmd);
    s_currentProcess->start();
    return false;
  }
  /* finally we have to delete the HelpCallStruct */
  s_currentProcess = &m_remoteBrowserProcess;
//   delete hc;
  return true; /* that's the end */
}

/* --------------------------------------------------------------------------- */
/* createAllHelpText --                                                        */
/* --------------------------------------------------------------------------- */

void HelpManager::createAllHelpText() {
  HelpFileList::iterator hf = m_HelpFiles.begin();
  for(; hf != m_HelpFiles.end(); ++hf ){
    HelpText *ht= dynamic_cast<HelpText*>(hf->second);
    if (ht) {
      ht->create();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* HelpKey::show --                                                            */
/* --------------------------------------------------------------------------- */

void HelpManager::HelpKey::show(){
  m_HelpFile->help( m_Name, m_ChapterNo );
}
