
#include "job/JobEngine.h"
#include "job/JobCodeTimer.h"
#include "operator/TimerFunction.h"
#include "xfer/XferDataItem.h"
#include "utils/Debugger.h"

/* --------------------------------------------------------------------------- */
/* Constructor / Destrctor --                                                  */
/* --------------------------------------------------------------------------- */

JobCodeTimer::JobCodeTimer( TimerFunction *timer, bool start,
		    double period, XferDataItem* periodXfer,
		    double delay , XferDataItem* delayXfer )
  : m_timer( timer )
  , m_start(start)
  , m_period(period)
  , m_periodXfer(periodXfer)
  , m_delay(delay)
  , m_delayXfer(delayXfer)
  {}

JobCodeTimer::~JobCodeTimer(){
}


/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeTimer::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeTimer::execute");
  double period(m_period), delay(m_delay);
  if (m_periodXfer) {
    m_periodXfer->getValue( period );
  }
  if (m_delayXfer) {
    m_delayXfer->getValue( delay );
  }

  // time: sec
  if (m_start)
    m_timer->start(period, delay);
  else
    m_timer->stop();
  return op_Ok;
}
