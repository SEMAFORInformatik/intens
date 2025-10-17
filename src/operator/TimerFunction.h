
#ifndef TIMER_FUNCTION_H
#define TIMER_FUNCTION_H

#include <vector>
#include <map>

#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "gui/TimerTask.h"

class JobFunction;
class QTimerEvent;

class TimerFunction {
// -----------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------
public:
  TimerFunction( const std::string &, JobFunction *, int);
  virtual ~TimerFunction();

private:
  TimerFunction();
 public:
  //-------------------------------------------------
  // private TimerTask class with tick method
  //-------------------------------------------------
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(TimerFunction* timer)
      : m_timer(timer){}
    void tick() { m_timer->startFunction();  }
  private:
    TimerFunction* m_timer;
  };
// -----------------------------------------------------
// private definitions
// -----------------------------------------------------
  class Trigger : public JobStarter
  {
  public:
    Trigger( TimerFunction *tf, JobFunction *f )
      : JobStarter( f )
      , m_timerFunc( tf ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    TimerFunction *m_timerFunc;
  };

// -----------------------------------------------------
// public member functions
// -----------------------------------------------------
public:
  static TimerFunction *getTimer( const std::string &id );
  void start(double sec_period, double sec_delay);
  void startFunction();
  void stop();
  void backFromJobStarter( JobAction::JobResult rslt );
  const std::string &getName(){ return m_name; }
  JobFunction *getFunction();

 private:
  static std::vector<TimerFunction*> s_timers;

  std::string                             m_name;
  JobFunction*                            m_function;
  int                                     m_maxPendingFunctions;
  MyTimerTask                            *m_task;
  Timer                                  *m_timer;
  int                                     m_period;
  int                                     m_delay;
};

#endif
