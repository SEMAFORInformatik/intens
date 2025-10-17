
#include <queue>

#include "utils/Debugger.h"
#include "utils/Date.h"
#include "job/JobController.h"
#include "job/JobAction.h"
#include "job/JobManager.h"
#include "gui/GuiEventData.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiManager.h"
#include "gui/DialogWorkClock.h"
#include "gui/DialogProgressBar.h"
#include "app/DataPoolIntens.h"

INIT_LOGGER();

int JobController::s_objectCounter=0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobController::JobController( JobAction *action,JobAction *callingAction, bool silent )
  : m_action( action )
  , m_callingAction( callingAction )
  , m_loopcontrol( 0 )
  , m_dialog( 0 )
  , m_silent( silent )
  , m_stopped( false )
  , m_block_undo( action->isBlockUndo() )
  , m_transactionStarted(false){
  ++s_objectCounter;
}

JobController::~JobController(){
  if (m_dialog)
    m_dialog->unsetListener();
  assert( s_objectCounter >= 0 );
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* writeLog --                                                                 */
/* --------------------------------------------------------------------------- */

void JobController::writeLog( const std::string &what ){
  if( m_action->isLogOn() ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << " : " << what << " : ";
    m_action->printLogTitle( logmsg );
    logmsg << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }
}

/* --------------------------------------------------------------------------- */
/* start --                                                                    */
/* --------------------------------------------------------------------------- */

void JobController::start(){
  assert( m_action != 0 );
  BUG_DEBUG("start name=" << m_action->Name() << ", this=" << this);

  if( s_objectCounter == 1 ){
    BUG_DEBUG("first object");
    JobManager::Instance().setJobGuiElement( m_action->getGuiElement() );
    JobManager::Instance().setActiveJobController(this);
    GuiFactory::Instance()->getDialogProgressBar()->setJobController(this);

    if( !m_action->isSilent() ){
      if( !m_silent ){
        createDialog();
      }
      else{
        BUG_DEBUG("Controller is silent");
      }
    }
    else{
      BUG_DEBUG("Action is silent");
    }

    // Mit dem folgenden Listener behalten wir die Kontrolle ueber den EventLoop
    // bis die Funktion zu Ende ist. GrabButton wird aktiviert.
    bool grabButton( true );

    // Activate Wait Cursor
    if( !m_action->isSilentFunction() && !m_callingAction->isSilentFunction() ){
      GuiFactory::Instance()->installDialogsWaitCursor();
      //      JobManager::Instance().printMessage( "start", GuiElement::msg_Information );
    }
    else{
      grabButton = false;
    }

    m_loopcontrol = new GuiEventLoopListener( grabButton );
    GuiManager::Instance().attachEventLoopListener( m_loopcontrol );
  }

  // Begin of Process
  if( m_action->isTask() ){
    BUG_DEBUG("begin of a task");
    writeLog( "BEGIN" );
  }
  if( m_action->isBlockUndo() ){
    BUG_DEBUG("action: undo is blocked");
    m_block_undo = true;
  }
  if( m_block_undo ){
    BUG_DEBUG("myself: undo is blocked");
  }

  if( m_transactionStarted ){
    BUG_DEBUG("transaction already started");
  }
  else{
    // Falls noch keine Datapool-Transaktion gestartet wurde, wird dies
    // nun veranlasst.
    startTransaction(); // kein block-undo
  }

  assert( m_action != 0 );
  m_action->setJobController( this );
  m_action->startJobAction();
}

/* --------------------------------------------------------------------------- */
/* startTransaction --                                                         */
/* --------------------------------------------------------------------------- */

void JobController::startTransaction(){
  BUG_DEBUG("startTransaction  undo blocked = " << m_block_undo);
  assert( !m_transactionStarted );

  if( m_block_undo ){
    BUG_DEBUG("block undo-feature for action " << m_action->Name());
    // DataPoolIntens::Instance().getDataPool().disableUndo();
    DataPoolIntens::Instance().getDataPool().disallowUndo();
  }
  DataPoolIntens::Instance().BeginDataPoolTransaction( this );
  m_transactionStarted = true;
}

/* --------------------------------------------------------------------------- */
/* commitTransaction --                                                        */
/* --------------------------------------------------------------------------- */

void JobController::commitTransaction(){
  BUG_DEBUG("commitTransaction");

  if( m_transactionStarted ){
    DataPoolIntens::Instance().CommitDataPoolTransaction( this );
    if( m_block_undo ){
      // DataPoolIntens::Instance().getDataPool().enableUndo();
      DataPoolIntens::Instance().getDataPool().allowUndo();
      m_block_undo = false;
      BUG_DEBUG("unblock undo-feature after end of action " << m_action->Name());
    }
    m_transactionStarted = false;
  }
}

/* --------------------------------------------------------------------------- */
/* abortTransaction --                                                         */
/* --------------------------------------------------------------------------- */

void JobController::abortTransaction(){
  BUG_DEBUG("abortTransaction");

  if( m_transactionStarted ){
    DataPoolIntens::Instance().RollbackDataPoolTransaction( this );
    if( m_block_undo ){
      // DataPoolIntens::Instance().getDataPool().enableUndo();
      DataPoolIntens::Instance().getDataPool().allowUndo();
      m_block_undo = false;
      BUG_DEBUG("unblock undo-feature after abort of action " << m_action->Name());
    }
    m_transactionStarted = false;
  }
}

/* --------------------------------------------------------------------------- */
/* stop --                                                                     */
/* --------------------------------------------------------------------------- */

void JobController::stop(){
  assert( m_action != 0 );
  BUG_DEBUG("stop name=" << m_action->Name() << ", this=" << this);

  if( m_stopped ){
    BUG_DEBUG("already stopped. Just wait ...");
    return;
  }
  m_stopped = true;

  // general case
  m_action->stopJobAction();
}

/* --------------------------------------------------------------------------- */
/* end --                                                                      */
/* --------------------------------------------------------------------------- */

void JobController::end( JobAction::JobResult rslt ){
  assert( m_action != 0 );
  BUG_DEBUG("end name=" << m_action->Name() <<
            ", this=" << this << ", result=" << rslt );

  assert( m_callingAction != 0 );
  if( rslt == JobAction::job_Ok        ||
      rslt == JobAction::job_Ok_ignore ||
      rslt == JobAction::job_Nok       ){
    commitTransaction();

    if( m_action->isTask() ){
      BUG_DEBUG( "normal end of task" );
      writeLog( "END" );
    }
  }
  else{
    if( rslt == JobAction::job_Canceled ){
      BUG_DEBUG( "job canceled" );
      writeLog( "CANCEL" );
    }
    else
    if( rslt == JobAction::job_Aborted ){
      BUG_DEBUG( "job aborted" );
      writeLog( "ABORT" );
    }
    else{
      BUG_DEBUG( "unknown fatal interrupt" );
      writeLog( "FATAL" );
    }
    abortTransaction();
  }

  BUG_DEBUG( "s_objectCounter " << s_objectCounter);
  if( s_objectCounter == 1 ){
    if( m_action->getUpdateForms() && m_callingAction->getUpdateForms()){
      GuiManager::Instance().update( GuiElement::reason_Process );
    }
    JobManager::Instance().setJobGuiElement( 0 );
    JobManager::Instance().setActiveJobController(0);
    GuiFactory::Instance()->getDialogProgressBar()->setJobController(0);
    // Deactivate Wait Cursor
    if( !m_action->isSilentFunction() && !m_callingAction->isSilentFunction() ){
      GuiFactory::Instance()->removeDialogsWaitCursor();
      //      JobManager::Instance().printMessage( "end", GuiElement::msg_Information );
    }

    // unmap progressbar
    BUG_DEBUG( "unmap Progressbar" );
    GuiFactory::Instance()->getDialogProgressBar()->getGuiElement()->unmap();
   }

  // endtransaction
  // log etc
  --s_objectCounter;

  if( m_loopcontrol != 0 ){
    m_loopcontrol->goAway();
  }

  if( m_dialog ){
    deleteDialog();
  }

  m_callingAction->endJobController( rslt );

  if( m_action->doDeleteAfterEndOfAction() ){
    BUG_DEBUG( "delete action" );
    delete m_action;
  }
  delete this;
}

/* --------------------------------------------------------------------------- */
/* cancelButtonPressed --                                                      */
/* --------------------------------------------------------------------------- */

void JobController::cancelButtonPressed(){
  BUG_DEBUG("cancelButtonPressed");
  stop();
}

/* --------------------------------------------------------------------------- */
/* createDialog --                                                             */
/* --------------------------------------------------------------------------- */

void JobController::createDialog(){
  BUG_DEBUG("createDialog");
  m_dialog=GuiFactory::Instance()->createDialogWorkClock(0,this,"Test","Test");
  m_dialog->manage();
}

/* --------------------------------------------------------------------------- */
/* deleteDialog --                                                             */
/* --------------------------------------------------------------------------- */

void JobController::deleteDialog(){
  BUG_DEBUG("deleteDialog");
  m_dialog->unmanage();
  delete m_dialog;
  m_dialog = 0;
}

/* --------------------------------------------------------------------------- */
/* setBlockUndo --                                                             */
/* --------------------------------------------------------------------------- */

void JobController::setBlockUndo( bool block_undo ){
  BUG_DEBUG("setBlockUndo block_undo=" << block_undo
            << ", action=" << m_action->Name() << ", @" << this);
  m_block_undo = block_undo;
}

/* --------------------------------------------------------------------------- */
/* ownerId --                                                                  */
/* --------------------------------------------------------------------------- */

std::string JobController::ownerId() const{
  std::ostringstream id;
  id << "JobController: " << this;
  if( m_action ){
    id << " Action=" << m_action->Name();
  }
  if( m_callingAction ){
    id << " callingAction=" << m_callingAction->Name();
  }
  return id.str();
}

/* --------------------------------------------------------------------------- */
/* CallingActionName --                                                        */
/* --------------------------------------------------------------------------- */

std::string JobController::CallingActionName() {
  return m_callingAction ? m_callingAction->Name() : "";
}

/* --------------------------------------------------------------------------- */
/* getBaseCallingAction --                                                     */
/* --------------------------------------------------------------------------- */

JobAction* JobController::getBaseCallingAction() {
  return  m_callingAction ? m_callingAction->getBaseCallingAction() : m_action;
}

/* --------------------------------------------------------------------------- */
/* getMessageQueueReplyThread --                                               */
/* --------------------------------------------------------------------------- */

MessageQueueReplyThread* JobController::getMessageQueueReplyThread() {
  return m_action->getMessageQueueReplyThread() ? m_action->getMessageQueueReplyThread() :
    m_callingAction->getBaseCallingAction() ?  m_callingAction->getBaseCallingAction()->getMessageQueueReplyThread() : 0;
}
