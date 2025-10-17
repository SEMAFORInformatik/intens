
#if !defined(JOB_TIMER_FUNCTION)
#define JOB_TIMER_FUNCTION

#include "job/JobCodeExec.h"
#include "operator/TimerFunction.h"

class JobEngine;
class GuiElement;

class JobCodeTimer : public JobCodeExec
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  JobCodeTimer( TimerFunction *timer, bool start,
		double period=1000, XferDataItem* periodXfer=NULL,
		double delay=0 , XferDataItem* delayXfer=NULL);
  virtual ~JobCodeTimer();

  /*=============================================================================*/
  /* public Functions                                                            */
  /*=============================================================================*/
public:
  virtual OpStatus execute( JobEngine * );

  /*=============================================================================*/
  /* private Data                                                                */
  /*=============================================================================*/
private:
  TimerFunction *m_timer;
  bool           m_start;
  double         m_period;
  XferDataItem  *m_periodXfer;
  double         m_delay;
  XferDataItem  *m_delayXfer;
};

#endif
