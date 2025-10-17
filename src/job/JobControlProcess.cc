
#include "job/JobIncludes.h"
#include "job/JobEngine.h"
#include "job/JobControlProcess.h"
#include "gui/GuiManager.h"

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* executeJobController --                                                     */
/* --------------------------------------------------------------------------- */

bool JobControlProcess::executeJobController(){
  BUG(BugJobStart,"JobControlProcess::executeJobController");
  assert( m_next_action != 0 );
  assert( m_engine != 0 );
  // Nun starten wir die Processgroup.
  // Das GuiElement als Parent für den WorkerDialog folgt später
  m_next_action->setJobController( this );
  m_next_action->startJobAction();
  return true;
}

/* --------------------------------------------------------------------------- */
/* stopJobController --                                                        */
/* --------------------------------------------------------------------------- */

bool JobControlProcess::stopJobController(){
  BUG(BugJobStart,"JobControlProcess::stopJobController" );
  return m_next_action->stopJobAction();
}

/* --------------------------------------------------------------------------- */
/* backFromJobAction --                                                        */
/* --------------------------------------------------------------------------- */

void JobControlProcess::backFromJobAction( JobResult rslt ){
  BUG_PARA(BugJobStart,"JobControlProcess::backFromJobAction", rslt );
  m_engine->pushTrue( rslt == job_Ok );
}
