
#include "job/JobIncludes.h"

#include "job/JobCodeEvalAssign.h"
#include "job/JobEngine.h"
#include "job/JobStackItem.h"
#include "job/JobStackData.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataString.h"
#include "job/JobStackDataVariable.h"
#include "job/JobStackDataStructure.h"
#include "job/JobController.h"
#include "gui/GuiIndex.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "app/DataPoolIntens.h"
#include "utils/Date.h"

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalValue::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalValue::execute");
  BUG_EXIT("do nothing");
  return op_Ok;

  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  JobStackDataPtr item;

  switch( v->getDataType() ){
  case DataDictionary::type_Integer:{
      int i;
      if( v->getIntegerValue( i ) ){
        item = new JobStackDataInteger( i );
        BUG_MSG("Integer Value " << i);
      }
    }
    break;

  case DataDictionary::type_Real:{
      double d;
      if( v->getRealValue( d ) ){
        item = new JobStackDataReal( d );
        BUG_MSG("Real Value " << d);
      }
    }
    break;

  case DataDictionary::type_Complex:{
      dComplex c;
      if( v->getComplexValue( c ) ){
        item = new JobStackDataComplex( c );
        BUG_MSG("Complex Value " << c);
      }
    }
    break;

  case DataDictionary::type_String:{
      std::string s;
      if( v->getStringValue( s ) ){
        item = new JobStackDataString( s );
        BUG_MSG("String Value " << s);
      }
  }
  break;

  case DataDictionary::type_StructVariable:{
    DataReference *ref = 0;
    if( v->getStructureValue( ref ) ){
      item = new JobStackDataStructure( ref );
      BUG_MSG("Struct Value " << ref->fullName( true ) );
    }
  }
  break;

  default:
    return op_FatalError;
  }

  if( item.isnt_valid() )
    eng->pushInvalid();
  else
    eng->pushData( item );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalReal::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalReal::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getRealValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalComplex::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalComplex::execute");

  JobStackDataPtr dat_imag( eng->pop() );
  if( dat_imag.isnt_valid() ) return op_FatalError;
  double dimag;
  bool valid_imag = false;

  switch( dat_imag->getDataType() ){
  case DataDictionary::type_Integer:
  case DataDictionary::type_Real:
    if( (valid_imag = dat_imag->getRealValue( dimag )) ){
      BUG_MSG("Imag Value " << dimag);
    }
    break;
  default:
    break;
  }

  JobStackDataPtr dat_real( eng->pop() );
  if( dat_real.isnt_valid() ) return op_FatalError;
  double dreal;
  bool valid_real = false;

  switch( dat_real->getDataType() ){
  case DataDictionary::type_Integer:
  case DataDictionary::type_Real:
    if( (valid_real = dat_real->getRealValue( dreal )) ){
      BUG_MSG("Real Value " << dreal);
    }
    break;
  default:
    break;
  }

  if( valid_real && valid_imag ){
    eng->push( new JobStackDataComplex( dComplex( dreal, dimag ) ) );
    BUG_EXIT("Value is " << dreal << ", " << dimag );
  }
  else {
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalImag::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalImag::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getImagValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalAbs::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalAbs::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getAbsValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalLength::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalLength::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  std::string s;
  if( !v->getStringValue( s ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataInteger( s.length() ) );
    BUG_EXIT("Value is " << s.length());
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalSin::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalSin::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getSinValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalCos::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalCos::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getCosValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}
/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalTan::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalTan::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getTanValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalASin::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalASin::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getASinValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalACos::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalACos::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getACosValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalATan::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalATan::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getATanValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalATan2::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalATan2::execute");
  JobStackDataPtr dat_x( eng->pop() );
  JobStackDataPtr dat_y( eng->pop() );
  if( dat_x.isnt_valid() || dat_y.isnt_valid() ) return op_FatalError;

  OpStatus status = op_Warning;
  // Ist bei einer aTan2-Funktion ein ungültiger Wert beteiligt, so ist das
  // Resultat auch ungültig.
  if( dat_x->isInvalid() || dat_y->isInvalid() ){
    eng->pushInvalid();
  }
  else{
    double dx, dy, angle;
    if( !dat_x->getRealValue( dx ) || !dat_y->getRealValue( dy ) ) {
      eng->pushInvalid();
      BUG_EXIT("NOT valid");
    }
    else{
      angle = atan2(dy, dx);
      eng->push( new JobStackDataReal( angle ) );
      status = op_Ok;
      BUG_EXIT("Value is " << angle);
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalLog::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalLog::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getLogValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalLog10::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalLog10::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getLog10Value( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalSqrt::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalSqrt::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getSqrtValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalArg::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalArg::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  double d;
  if( !v->getArgValue( d ) ){
    eng->pushInvalid();
    BUG_EXIT("NOT valid");
  }
  else{
    eng->push( new JobStackDataReal( d ) );
    BUG_EXIT("Value is " << d);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalValid::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalValid::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  eng->pushTrue( v->isValid() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssign::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAssign::execute");
  JobStackDataPtr d( eng->pop() );
  JobStackDataPtr v( eng->pop() );
  if( d.isnt_valid() || v.isnt_valid() ) return op_FatalError;

  return v->assign( eng, d );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssignCorr::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAssign::execute");
  JobStackDataPtr d( eng->pop() );
  JobStackDataPtr v( eng->pop() );
  if( d.isnt_valid() || v.isnt_valid() ) return op_FatalError;

  return v->assignCorresponding( eng, d );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAccumulate::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAccumulate::execute");
  JobStackDataPtr d( eng->pop() );
  JobStackDataPtr v( eng->pop() );
  if( d.isnt_valid() || v.isnt_valid() ) return op_FatalError;

  return v->accumulate( eng, d );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeIncrement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeIncrement::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  return v->increment( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeDecrement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDecrement::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  return v->decrement( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssignIndex::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAssignIndex::execute");
  JobStackDataPtr d( eng->pop() );
  if( d.isnt_valid() ) return op_FatalError;

  JobAction *action(0);
  int i = 0;
  if( !d->getIntegerValue( i ) ){
    BUG_EXIT( "Value not valid: Index not set" );
    action = m_index->getIndexAction();
  }
  if( action == 0 && !m_index->isIndexAccepted( i ) ){
    BUG_EXIT( "Value " << i << " is not accepted" );
    action = m_index->getIndexAction();
  }
  if( action == 0 ){
    action = m_index->getIndexAction( i );
  }
  assert( action != 0 );
  eng->getFunction()->startNextJobAction( action );
  return op_Wait;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeClearValues::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeClearValues::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  if( !v->clearValues() ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: clear in item '";
    v->printFullName( logmsg, true );
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    return op_Warning;
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEraseValues::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEraseValues::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  if( !v->eraseValues() ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: clear in item '";
    v->printFullName( logmsg, true );
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    return op_Warning;
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePackRowValues::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePackRowValues::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  if( !v->packValues( true ) ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: pack row in item '";
    v->printFullName( logmsg, true );
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    return op_Warning;
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePackColValues::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePackColValues::execute");
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = op_Ok;
  if( !v->packValues( false ) ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: pack column in item '";
    v->printFullName( logmsg, true );
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    return op_Warning;
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeBeginTransaction::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeBeginTransaction::execute");
  DataPoolIntens::Instance().BeginDataPoolTransaction( eng );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeCommitTransaction::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeCommitTransaction::execute");
  DataPoolIntens::Instance().CommitDataPoolTransaction( eng );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAbortTransaction::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAbortTransaction::execute");
  DataPoolIntens::Instance().RollbackDataPoolTransaction( eng );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEvalChanged::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEvalChanged::execute");
  GuiElement *elem = GuiElement::findElement( m_guiElemId );
  assert( elem != 0 );

  JobStackDataPtr ftData;
  if ( m_filter ) {
    // get filter DataRef which should be ancestor of updated DataRefs
    ftData = eng->pop();
  }

  // get timestamp
  JobStackDataPtr tsData( eng->pop() );

  if( tsData.isnt_valid() ) return op_FatalError;
  int timestamp = 0;
  tsData->getIntegerValue( timestamp );

  // increase timestamp ???
  // => if query for datapool modificactions
  // => equal timestamp of query and dataitems
  // => returns TRUE (Modification!!!)
  // ===> we don't like this
  // ===> we increase the timestamp
  ++timestamp;

  DataReference *ref = 0;
  if( ftData.is_valid() ) {
    ftData->getStructureValue( ref );
  }

  XferDataItem *xfer = ref ? new XferDataItem(ref) : 0;
  eng->pushTrue( elem->hasChanged(timestamp, xfer) );
  if (xfer) delete xfer;

  return op_Ok;
}
