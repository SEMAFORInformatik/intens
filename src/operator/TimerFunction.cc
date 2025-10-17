
#include "operator/TimerFunction.h"
#include "gui/GuiFactory.h"
#include "gui/Timer.h"

std::vector<TimerFunction*> TimerFunction::s_timers;

// -----------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------
TimerFunction::TimerFunction( const std::string &id
			      , JobFunction *func
			      , int maxPendingFunctions )
  : m_name( id )
  , m_function( func )
  , m_maxPendingFunctions(maxPendingFunctions)
  , m_period(0)
  , m_delay(0)
  , m_timer(0)
  , m_task(0)
{
  s_timers.push_back( this );

  m_timer=GuiFactory::Instance()->createTimer( m_period, true );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

TimerFunction::~TimerFunction(){
}

// -----------------------------------------------------
// getFunction
// -----------------------------------------------------
JobFunction* TimerFunction::getFunction(){
  return m_function;
}

// -----------------------------------------------------
// getServer
// -----------------------------------------------------
TimerFunction *TimerFunction::getTimer( const std::string &id ){
  std::vector<TimerFunction*>::iterator iter;
  for( iter=s_timers.begin(); iter!=s_timers.end(); ++iter ){
    if( (*iter)->m_name==id ){
      return *iter;
    }
  }
  return 0;
}


// -----------------------------------------------------
// backFromJobStarter
// -----------------------------------------------------
void TimerFunction::backFromJobStarter( JobAction::JobResult rslt  ){
  //std::cout << "<TimerFunction><backFromJobStarter>\n"<<std::flush;
}

// -----------------------------------------------------
// backFromJobStarter
// -----------------------------------------------------
void TimerFunction::Trigger::backFromJobStarter( JobAction::JobResult rslt  ){
  if ( m_timerFunc)
    m_timerFunc->backFromJobStarter( rslt );
}

// -----------------------------------------------------
// start
// -----------------------------------------------------
void TimerFunction::start(double sec_period, double sec_delay){
  m_period = sec_period;
  m_delay = sec_delay;

  // time sec to msec
  m_timer->setInterval(sec_period*1000);
  m_timer->setDelay(sec_delay*1000);
  m_timer->start();
}

// -----------------------------------------------------
// stop
// -----------------------------------------------------
void TimerFunction::stop(){
  m_timer->stop();
}

// -----------------------------------------------------
// startFunction
// -----------------------------------------------------
void TimerFunction::startFunction(){
  // avoid filling jobqueue, if wanted
  if(m_maxPendingFunctions > 0 &&
     JobStarter::nPendingFunctions() + 1 > m_maxPendingFunctions) {  // + 1: the function to be started
    return;
  }

  if( m_function ){
    Trigger *trigger = new Trigger( this, m_function );
    trigger->startJob();
  }
}
