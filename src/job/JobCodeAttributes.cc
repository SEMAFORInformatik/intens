
#include "job/JobIncludes.h"

#include "job/JobCodeAttributes.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "job/JobStackDataVariable.h"
#include "gui/GuiElement.h"
#include "gui/GuiManager.h"


/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrEditable::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrEditable::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setEditable( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrLock::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrLock::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setLocked( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrColor::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrColor::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setColor( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrColorSetBit::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrColorSetBit::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setColorBit( eng, b, false );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrColorUnsetBit::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrColorSetBit::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setColorBit( eng, b, true );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrSetDataTimestamp::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrTimestamp::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setDataTimestamp( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrSetValueTimestamp::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrTimestamp::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setValueTimestamp( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}
