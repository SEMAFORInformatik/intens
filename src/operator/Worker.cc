
#include "gui/GuiScrolledText.h"
#include "gui/Timer.h"
#include "gui/GuiFactory.h"
#include "utils/Debugger.h"

#include "operator/Worker.h"

Worker *Worker::s_to_delete=0;

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
Worker::Worker( WorkerController *controller, int intv2 )
  : m_controller( controller )
  , m_stopped( false )
  , m_normal_eot( false )
  , m_interval2(intv2) {
  BUG(BugGuiWorker, "Worker::Worker");
  BUG_MSG( this );

  m_timer=GuiFactory::Instance()->createTimer( WORK_TIMER_INTERVAL );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

Worker::~Worker(){
  BUG(BugGuiWorker, "Worker::~Worker (Destructor)" );
  BUG_MSG( this );

  if (m_timer) delete m_timer;
  if (m_task)  delete m_task;
}

/* --------------------------------------------------------------------------- */
/* start --                                                                    */
/* --------------------------------------------------------------------------- */
void Worker::start(){
  m_timer->start();
}

/* --------------------------------------------------------------------------- */
/* work --                                                                     */
/* --------------------------------------------------------------------------- */
void Worker::work(){
  if( s_to_delete ){
    delete s_to_delete;
    s_to_delete = 0;
  }
  BUG_SILENT(BugGuiWorker,"Worker::work");
  JobAction::JobResult rslt =  m_controller->work();
  switch( rslt ){
  case JobAction::job_Wait :
    m_timer->setInterval( m_interval2);
    start();
    break;
  case JobAction::job_Ok : // fertig und alles OK
  case JobAction::job_Canceled :
  case JobAction::job_Aborted :
    m_timer->stop();
    m_controller->endWorker( rslt );
    s_to_delete = this;
    break;
  default :
    assert( false );
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* endWorker --                                                                */
/* --------------------------------------------------------------------------- */
void WorkerController::endWorker( JobAction::JobResult rslt ){
  backFromWorker( rslt );
}
