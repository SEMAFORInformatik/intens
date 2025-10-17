
#include "job/JobActionSimple.h"
#include "utils/Debugger.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobActionSimple::JobActionSimple( const std::string &name )
  : JobAction( name ){
  BUG_PARA( BugJob, "Constructor JobActionSimple", "[" << Name() << "]" );
  setSilent();
}

JobActionSimple::~JobActionSimple() {
  BUG_PARA( BugJob, "Destructor JobActionSimple", "[" << Name() << "]" );
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobActionSimple::startJobAction(){
  BUG_PARA( BugJobStart, "startJobAction", "[" << Name() << "]" );
  executeSimpleAction();
  // Eine SimpleAction ist per Definition immer ok.
  endJobAction( job_Ok );
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobActionSimple::stopJobAction(){
  // unmöglich
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobActionSimple::backFromJobController( JobResult rslt ){
  // unmöglich
  assert( false );
}
