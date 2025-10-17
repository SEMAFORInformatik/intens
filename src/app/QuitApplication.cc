#include <string>

#include <qapplication.h>

#include "operator/IntensServerSocket.h"
#include "operator/MessageQueue.h"
#include "job/JobManager.h"
#include "utils/utils.h"
#include "app/QuitApplication.h"
#include "app/UiManager.h"
#include "app/AppData.h"
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "app/ReportGen.h"
#include "app/ModuleFactory.h"
#include "streamer/StreamManager.h"

QuitApplication *QuitApplication::s_instance = 0;

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

QuitApplication *QuitApplication::Instance( GuiDialog * dialog ){
  if( s_instance == 0 ){
    assert( dialog != 0 );
    s_instance = new QuitApplication( dialog );
  }
  else{
    assert( dialog == 0 );
  }
  return s_instance;
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void QuitApplication::ButtonPressed(){
  if ( MessageQueue::hasPendingRequestThreads() ) {
   GuiElement::ButtonType ret = GuiFactory::Instance()->showDialogConfirmation
      ( 0
	, _("Abort MessageQueue Connection")
	, _("A Message Queue Connection is still open.\n\nDo you want to close the Message Queue Connection?" )
	, 0 );
   if (ret == GuiElement::button_Yes) {
     MessageQueue::terminateAllRequestThreads();
    }
  }
  ask();
}

/* --------------------------------------------------------------------------- */
/* deleteWindowCallback --                                                     */
/* --------------------------------------------------------------------------- */
#ifndef HAVE_QT
void QuitApplication::deleteWindowCallback( Widget w, XtPointer q, XtPointer ){
  QuitApplication *quit = (QuitApplication *)q;
  quit->ask();
}
#endif

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void QuitApplication::confirmNoButtonPressed(){
//   IntensServerSocket::startAll();
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void QuitApplication::confirmYesButtonPressed(){
  if (AppData::Instance().hasTestModeFunc() || AppData::Instance().TestMode()) {
    std::cerr << "TestMode Exit function was called => exit(0)" << std::endl << std::flush;
    exit(0);
  }
  JobFunction *func = JobManager::Instance().getFunction( "QUIT" );
  if( func == 0 ){
    func = JobManager::Instance().getTask( "QUIT" );
    if( func == 0 ){
      exitApplication();
      return;
    }
  }
  func->setReason( JobElement::cll_Function );

  Trigger *trigger = new Trigger( this, func );
  trigger->startJob();
  // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
}

/* --------------------------------------------------------------------------- */
/* exitApplication --                                                          */
/* --------------------------------------------------------------------------- */

void QuitApplication::exitApplication(){
  UImanager::Instance().closeDescriptionFunction();
  lastActions();

  qApp->exit( 0 );
  delete this;
}

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* ask --                                                                      */
/* --------------------------------------------------------------------------- */

void QuitApplication::ask(){
  if( m_dialog != 0 ){
    if( m_dialog->isWaiting() ){
      return;
    }
  }

//   IntensServerSocket::stopAll();

  GuiFactory::Instance()->showDialogConfirmation
    ( 0
      , _("Exit")
      ,  compose( _("Do you really want to leave %1 ?" ), AppData::Instance().Title() )
      , this );

}

/* --------------------------------------------------------------------------- */
/* lastActions --                                                              */
/* --------------------------------------------------------------------------- */

void QuitApplication::lastActions(){
  IntensServerSocket::stopAll();
  if( AppData::Instance().createResFile() ){
    std::cout<<"QuitApplication::lastActions()  creating new resource file...."<<std::endl;
    GuiManager::writeResourceFile();
  }
  GuiManager::Instance().closeApplication();

  ReportGen::Instance().deleteFiles();

  // 2019-08-22 wegen Absturz in msys2 msys-ltdl-7.dll
  //  ModuleFactory::Instance().closeAll();
  ModuleFactory::closeAllStatic();
  StreamManager::Instance().deleteFiles();
  DataPoolIntens::Destroy();
}

/* --------------------------------------------------------------------------- */
/* Trigger::backFromJobStarter --                                               */
/* --------------------------------------------------------------------------- */

void QuitApplication::Trigger::backFromJobStarter( JobAction::JobResult jobResult ){
  if( jobResult != JobAction::job_Ok ){
    GuiFactory::Instance()->showDialogConfirmation( 0
						    , _("Exit function failed")
						    ,  compose( _("Do you want to leave %1 anyway?" ), AppData::Instance().Title() )
						    , this
						    );
  }
  else{
    m_quit->exitApplication();
  }
}

/* --------------------------------------------------------------------------- */
/* Trigger::confirmYesButtonPressed --                                         */
/* --------------------------------------------------------------------------- */

void QuitApplication::Trigger::confirmYesButtonPressed(){
  m_quit->exitApplication();
}

/* --------------------------------------------------------------------------- */
/* Trigger::confirmNoButtonPressed --                                          */
/* --------------------------------------------------------------------------- */

void QuitApplication::Trigger::confirmNoButtonPressed(){
//    IntensServerSocket::startAll();
}
