#include <string>

#include "utils/Debugger.h"

#include "operator/MatlabInitProcess.h"
#include "operator/MatlabProcess.h"

#include "utils/gettext.h"

#include "app/AppData.h"

#include "gui/GuiFactory.h"



MatlabInitProcess *MatlabInitProcess::s_instance = 0;

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
MatlabInitProcess::MatlabInitProcess()
  : Process("MatlabInitProcess")
  , m_initialized( false )
  , m_terminated(true)
  , m_dialog( false )
  , m_silentMode( false )
  , m_thread(this) {
}

/* --------------------------------------------------------------------------- */
/* init --                                                                     */
/* --------------------------------------------------------------------------- */
void MatlabInitProcess::init(){
  BUG( BugOperator, "MatlabInitProcess::init()" );
  m_dialog = false;

  AppData &appdata = AppData::Instance();
  std::string start_cmd ("matlab -nodisplay -nojvm");
  if( !appdata.MatlabNode().empty() ){
#if defined( __hpux )
    start_cmd = "remsh -n ";
#else
    start_cmd = " rsh -n ";
#endif
    start_cmd += appdata.MatlabNode() + " matlab -display " + appdata.DisplayName();
  }
  if( !MatlabProcess::open( start_cmd ) ){
    m_thread.lock();
    setExitStatus(1);
    m_thread.unlock();
    return;
  }
  m_initialized = true;
  m_thread.lock();
  m_terminated=true;
  m_thread.unlock();
  BUG_MSG("MatlabInitProcess::init() done." );
}

/* --------------------------------------------------------------------------- */
/*  informOkButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */
void MatlabInitProcess::informOkButtonPressed(){
  m_thread.lock();
  m_terminated = true;
  m_thread.unlock();
}

/* --------------------------------------------------------------------------- */
/* start --                                                                    */
/* --------------------------------------------------------------------------- */
bool MatlabInitProcess::start(){
  BUG( BugOperator, "MatlabInitProcess::start()" );
  setExitStatus(0);
  if( m_initialized ){
    m_terminated=true;
    m_is_running = true;
  }
  else{
    m_terminated = false;
    m_is_running = true;
#if ( defined( linux ) || defined( __hpux ) ) || defined( __WIN32__ )
    init();
#else
    m_thread.start();
#endif
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* doInit --                                                                   */
/* --------------------------------------------------------------------------- */
void MatlabInitProcess::startThread() {
  BUG( BugOperator, "MatlabInitProcess::startThread" );
  init();
}

/* --------------------------------------------------------------------------- */
/* hasTerminated --                                                            */
/* --------------------------------------------------------------------------- */
bool MatlabInitProcess::hasTerminated(){
  BUG( BugOperator, "MatlabInitProcess::hasTerminated()" );
  if( getExitStatus() != 0 && !m_dialog ){
    if ( m_silentMode ) {
      informOkButtonPressed();
      m_dialog = true;
      return true;
    }
    GuiFactory::Instance() -> showDialogWarning
      ( 0
	, _("Error")
        ,_("Cannot start Matlab.\n")
	, this );
  }
  bool terminated;
  m_thread.lock();
  terminated=m_terminated;
  m_thread.unlock();

  if( !terminated ){
    BUG_MSG( "MatlabInitProcess Not yet Terminated" );
    return false;
  }
  m_is_running = false;
  return true;
}

/* --------------------------------------------------------------------------- */
/* stop --                                                                     */
/* --------------------------------------------------------------------------- */
bool MatlabInitProcess::stop(){
  BUG( BugOperator, "MatlabInitProcess::stop()" );
  m_thread.lock();
  if( !m_terminated )
    m_thread.terminate();
  m_terminated = true;
  m_thread.unlock();
  return true;
}

/* --------------------------------------------------------------------------- */
/* ConnectionCloseMenuButtonListener ButtonPressed  --                         */
/* --------------------------------------------------------------------------- */
void MatlabInitProcess::ConnectionCloseMenuButtonListener::ButtonPressed() {
  if (!MatlabInitProcess::s_instance) {
    std::cout << "  matlab not open!!\n";
    return;
  }
  if (!MatlabInitProcess::s_instance->initialized()) {
    std::cout << "  matlab not initialized!!\n";
    return;
  }
  MatlabProcess::close();
  MatlabInitProcess::MatlabInitProcess::s_instance->m_initialized = false;
  std::cout << "  matlab stopped!!\n";
};
