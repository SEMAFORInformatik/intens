
#include "gui/GuiScrolledText.h"
#include "gui/DialogWorker.h"
#include "gui/Timer.h"
#include "gui/GuiFactory.h"

#include "utils/Debugger.h"

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
DialogWorker::DialogWorker(bool use_timer
			   , WorkerListener *listener )
  : m_interval( (1000*DIALOG_WORK_TIMER_INTERVAL) )
    , m_timer_used( use_timer )
    , m_listener( listener )
    , m_start_time( 0 )
    , m_elapsed_time( 0 )
    , m_stopped( false )
    , m_normal_eot( false ){
  BUG(BugGuiWorker, "DialogWorker::DialogWorker (Constructor)" );
  if( !m_timer_used )
    m_interval = 0;

  m_timer=GuiFactory::Instance()->createTimer( m_interval );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

DialogWorker::~DialogWorker(){
  BUG(BugGuiWorker, "DialogWorker::~DialogWorker (Destructor)" );
  if (m_timer) delete m_timer;
  if (m_task)  delete m_task;
}

/* --------------------------------------------------------------------------- */
/* continueWork --                                                             */
/* --------------------------------------------------------------------------- */
bool DialogWorker::continueWork(){
  // Die Arbeit geht weiter.
  if( interval() == 0 && !timer_used() ){
    // Der Xt-Workloop laeuft bereits. Es geht so weiter.
    return false;
  }
  // Die Arbeit muss neu aufgesetzt werden. Dies kann durch einen Timer
  // oder einen Xt-Workloop geschehen.
  startProcedure();

  // Falls ein laufender Xt-Workloop abgegrochen werden soll.
  return true;
}

/* --------------------------------------------------------------------------- */
/* startProcedure --                                                           */
/* --------------------------------------------------------------------------- */
void DialogWorker::startProcedure(){
  if( interval() > 0 )
    setTimerUsed();
  m_timer->start();
}

/* --------------------------------------------------------------------------- */
/* setTimerInterval --                                                         */
/* --------------------------------------------------------------------------- */
void DialogWorker::setTimerInterval( long t ) {
  m_timer->setInterval(t);
}

/* --------------------------------------------------------------------------- */
/* interval --                                                                 */
/* --------------------------------------------------------------------------- */
unsigned long DialogWorker::interval(){
  return m_timer->getInterval();
}

/* --------------------------------------------------------------------------- */
/* work --                                                                     */
/* --------------------------------------------------------------------------- */
bool DialogWorker::work(){
  static int s = 0;
  BUG_SILENT(BugGuiWorker,"DialogWorker::work");

  if( !stopped() ){
    // Aus unserer Sicht läuft alles flott. Wir rufen die Arbeit !
    if( listener()->work() ){
      // Es ist vollbracht. Die Arbeit ist beendet.
      setNormalEot( true );
      setElapsedTime();
      BUG_MSG("THIS=" << this << ": normal end" );
    }
  }
  else{
    // Der Prozess wurde manuell gestoppt. Bevor wir entgültig abbrechen,
    // fragen wir den Listener, ob der Stop durchgeführt werden kann.
    // Unter Umständen müssen zuerst untergeordnete Prozesse gestoppt
    // werden
    BUG_MSG("THIS=" << this << ": stop pending" );
    if( !listener()->stopped() ){
      setTimerInterval( DIALOG_WORK_TIMER_INTERVAL );
      return continueWork();
    }
    BUG_MSG("THIS=" << this << ": stop confirmed by listener");
  }

  if( stopped() || normalEot() ){
    m_timer->stop();
    BUG_MSG("THIS=" << this << ": stopped" );
    // Wir nehmen dem Listener den Dialog weg. Somit kann er keinen
    // Einfluss mehr nehmen auf den WorkDialog.
    if( listener()->getDialog() != 0 ){
      BUG_MSG("THIS=" << this << ": Dialog available");
      if( !(m_listener->getDialog()->isDialogInvisible()) ){
	BUG_MSG("THIS=" << this << ": Dialog visible");
	listener()->setDialog( 0 );
	popParentDialog();
      }
    }

    // Die Arbeit ist beendet.
    bool aborted = false;
    if( normalEot() ){
      BUG_MSG("THIS=" << this << ": normal eot");
      // Erst jetzt erfahren wir, ob alles wirklich ok ist.
      setNormalEot( listener()->EndOfWork() );
      if( !normalEot() ){
        BUG_MSG("THIS=" << this << ": but it isnt normal eot");
	// Jeder Listener muss bei einem Fehler selber schauen, wie
	// er dies der Aussenwelt beibringt.
	aborted = true;
      }
    }
    else{
      BUG_MSG("THIS=" << this << ": canceled");
      // Die Arbeit wurde gewaltsam mit 'Cancel' abgebrochen.
      listener()->workCancelButtonPressed();
    }
    unmanage();
    if( aborted ){
      // Der Listener hat nach dem Ende des Work-Dialogs die Chance
      // selber noch etwas zu unternehmen (z.B. MessageBox).
      BUG_MSG("THIS=" << this << ": call workFailed");
      listener()->workFailed();
    }

    listener()->workTerminated();
    // Die Arbeit ist beendet, mit welchem Erfolg auch immer.
    BUG_MSG("THIS=" << this << ": this was the last work()");
    GuiFactory::Instance()->createSimpleAction( new MyDelete( this ) );
    return true;
  }
  return continueWork();
};

/* --------------------------------------------------------------------------- */
/* writeLog --                                                                 */
/* --------------------------------------------------------------------------- */

void WorkerListener::writeLog( const std::string &msg ){
 GuiFactory::Instance()->getLogWindow()->writeText( msg );
}
