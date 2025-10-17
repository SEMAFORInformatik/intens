
#include "utils/Separator.h"
#include "job/JobIncludes.h"
#include "job/JobManager.h"
#include "job/JobCodeCycle.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataString.h"
#include "job/InitialWorker.h"
#include "app/DataPoolIntens.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* run --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeCycle::run( const std::string &name, JobEngine *eng ){
  BUG( BugJobCode, "JobCodeCycle::run" );
  JobFunction *fun = JobManager::Instance().getFunction( name );
  if( fun == 0 ){
    fun = JobManager::Instance().getTask( name );
  }
  if( fun == 0 ){
    BUG_EXIT( "no Function '" << name << "'" );
    return op_Ok; // keine Funktion vorhanden
  }
  if( fun->notImplemented() ){
    BUG_EXIT( "Function '" << name << "' not implemented" );
    return op_Ok; // Funktion ist nicht implementiert !?
  }
  eng->callFunction( fun );
  BUG_EXIT( "Function '" << name << "called" );
  return op_Branch;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeNewCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeNewCycle::execute");
  // Auf dem Stack liegt der Name des neuen Cycle.
  JobStackDataPtr dat( eng->pop() );
  if( dat.isnt_valid() ) return op_FatalError;

  std::string s;
  dat->getStringValue( s );
  DataPoolIntens::Instance().newCycle( s );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeDeleteCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDeleteCycle::execute");
  DataPoolIntens &dp = DataPoolIntens::Instance();
  dp.removeCycle( dp.currentCycle() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeClearCycle::execute( JobEngine *eng ){
  BUG_DEBUG("Begin JobCodeClearCycle::execute");
  DataPoolIntens &dp = DataPoolIntens::Instance();
  dp.clearCycle( dp.currentCycle() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeFirstCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeFirstCycle::execute");
  DataPoolIntens::Instance().firstCycle();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeLastCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeLastCycle::execute");
  DataPoolIntens::Instance().lastCycle();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeNextCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeNextCycle::execute");
  DataPoolIntens::Instance().nextCycle();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGoCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGoCycle::execute");
  // Auf dem Stack liegt die Nummer des gewünschten Cycle.
  JobStackDataPtr dat( eng->pop() );
  if( dat.isnt_valid() ) return op_FatalError;

  int i;
  if( dat->getIntegerValue( i ) ){
    if( --i >= 0 ){
      DataPoolIntens &dp = DataPoolIntens::Instance();
      if( i != dp.currentCycle() ){
	if( i < dp.numCycles() ){
	  DataPoolIntens::Instance().goCycle( i );
	  return op_Ok;
	}
      }
    }
  }
  else{
    BUG_MSG("no cycle number available");
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetCycle::execute");
  int n = DataPoolIntens::Instance().currentCycle();
  eng->push( new JobStackDataInteger( n + 1 ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeMaxCycle::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMaxCycle::execute");
  int n = DataPoolIntens::Instance().numCycles();
  eng->push( new JobStackDataInteger( n ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetCycleName::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetCycleName::execute");
  // Auf dem Stack liegt die Nummer des gewünschten Cycle.
  JobStackDataPtr dat( eng->pop() );
  if( dat.isnt_valid() ) return op_FatalError;

  int i;
  DataPoolIntens &dp = DataPoolIntens::Instance();

  if( dat->getIntegerValue( i ) ){
    if( i < 1 || i > dp.numCycles() ){
      eng->pushInvalid();
      BUG_EXIT("#" << i << " is not a valid cycle number");
      return op_Ok;
    }
    i--;
  }
  else{
    i = dp.currentCycle();
    BUG_MSG("take current cycle number");
  }

  std::string name;
  if( !dp.getCycleName( i, name ) ){
    name = "<Cycle #" + std::to_string(i);
    name += ">";
  }

  eng->push( new JobStackDataString( name ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetCycleName::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetCycleName::execute");
  // Neuer Name des Cycles
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  // Auf dem Stack liegt die Nummer des gewünschten Cycle.
  JobStackDataPtr dat( eng->pop() );
  if( dat.isnt_valid() ) return op_FatalError;

  int i;
  DataPoolIntens &dp = DataPoolIntens::Instance();

  if( dat->getIntegerValue( i ) ){
    if( i < 1 || i > dp.numCycles() ){
      BUG_EXIT("#" << i << " is not a valid cycle number");
      return op_Warning;
    }
    i--;
  }
  else{
    i = dp.currentCycle();
    BUG_MSG("take current cycle number");
  }

  std::string name;
  if( !v->getStringValue( name ) ){
    name = "<no name>";
  }
  dp.setCycleName( i, name );
  return op_Ok;
}
