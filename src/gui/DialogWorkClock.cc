
#include "time.h"
#include "utils/Debugger.h"
#include "gui/Timer.h"
#include "gui/GuiFactory.h"
#include "job/JobController.h"
#include "gui/DialogWorkClock.h"

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
DialogWorkClock::DialogWorkClock( DialogWorkClockListener *listener )
  : m_listener( listener )
  , m_elapsed_time( 0 )
  , m_start_time( 0 ){
  BUG(BugGuiWorker,"DialogWorkClock::DialogWorkClock" );
  m_timer=GuiFactory::Instance()->createTimer( CLOCK_TIMER_INTERVAL, true );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

DialogWorkClock::~DialogWorkClock(){
  BUG(BugGuiWorker,"DialogWorkClock::~DialogWorkClock" );
  if (m_timer) delete m_timer;
  if (m_task)  delete m_task;
}

/* --------------------------------------------------------------------------- */
/* removeClockTimer --                                                         */
/* --------------------------------------------------------------------------- */
void DialogWorkClock::removeClockTimer(){
  BUG(BugGuiWorker,"DialogWorkClock::removeClockTimer" );
  m_timer->stop();
}

/* --------------------------------------------------------------------------- */
/* cancelButtonPressed --                                                      */
/* --------------------------------------------------------------------------- */

void DialogWorkClock::cancelButtonPressed(){
  if (m_listener)
    m_listener->cancelButtonPressed();
}

/* --------------------------------------------------------------------------- */
/* setStartTime --                                                             */
/* --------------------------------------------------------------------------- */

void DialogWorkClock::setStartTime(){
  m_start_time = time(0);
}

/* --------------------------------------------------------------------------- */
/* elapsedTime --                                                              */
/* --------------------------------------------------------------------------- */

time_t DialogWorkClock::elapsedTime(){
  return m_elapsed_time;
}

/* --------------------------------------------------------------------------- */
/* setElapsedTime --                                                           */
/* --------------------------------------------------------------------------- */

void DialogWorkClock::setElapsedTime(){
  m_elapsed_time = time(0) - m_start_time;
}
