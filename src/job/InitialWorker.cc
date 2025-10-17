#include "job/JobStarter.h"
#include "utils/Debugger.h"
#include "job/InitialWorker.h"

InitialWorker *InitialWorker::s_instance = 0;

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

InitialWorker &InitialWorker::Instance(){
  if( s_instance == 0 ){
    s_instance = new InitialWorker();
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */

InitialWorker::InitialWorker(){
}

InitialWorker::~InitialWorker(){
}

/* --------------------------------------------------------------------------- */
/* appendAction --                                                             */
/* --------------------------------------------------------------------------- */
void InitialWorker::appendAction( JobAction *action ){
  BUG( BugJobStart, "InitialWorker::appendAction" );

  m_actions.push_back( action );
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                             */
/* --------------------------------------------------------------------------- */
void InitialWorker::backFromJobStarter( JobAction::JobResult rslt ){
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */
void InitialWorker::startJobAction(JobElement::CallReason reason, JobStackData *old_data){
  BUG( BugJobStart, "InitialWorker::startJobAction" );

  ActionIterator iter;
  for( iter = m_actions.begin();
       iter!=m_actions.end();
       ++iter ){
    Trigger *trigger = new Trigger(*iter, this);
    if (reason < JobElement::cll_CallReasonEntries) {
      trigger->setReason(reason);
      if (old_data) trigger->setOldData(old_data);
    }
    trigger->startJob();
  }
  m_actions.clear();
}
