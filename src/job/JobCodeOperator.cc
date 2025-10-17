
#include "job/JobIncludes.h"

#include "job/JobCodeOperator.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "job/JobStackDataReal.h"

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpAdd::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpAdd::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  // Bei einer Addition mit ungültigen Werten bleibt immer der gültige Wert
  // übrig, unabhängig vom Typ.
  if( dat_first->isInvalid() ){
    eng->pushData( dat_second );
    return op_Ok;
  }
  if( dat_second->isInvalid() ){
    eng->pushData( dat_first );
    return op_Ok;
  }
  return dat_first->add( eng, dat_second ); // Addition
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpSubtract::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpSubtract::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  // Wird ein ungültiger Wert subtrahiert, so verbleibt der erste Wert auf
  // dem Stack, unabhängig vom Typ.
  if( dat_second->isInvalid() ){
    eng->pushData( dat_first );
    return op_Ok;
  }

  if( dat_first->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }
  return dat_first->subtract( eng, dat_second ); // Subtraction
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpMultiply::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpMultiply::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  // Ist bei einer Multiplikation ein ungültiger Wert beteiligt, so ist das
  // Resultat auch ungültig.
  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }
  return dat_first->multiply( eng, dat_second ); // Multiplikation
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpDivide::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpDivide::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  // Ist bei einer Division ein ungültiger Wert beteiligt, so ist das
  // Resultat auch ungültig.
  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }

  return dat_first->divide( eng, dat_second ); // Division
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpModulo::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpModulo::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  // Ist bei einer Division ein ungültiger Wert beteiligt, so ist das
  // Resultat auch ungültig.
  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }

  return dat_first->modulo( eng, dat_second ); // Modulo
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpPower::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpPower::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  // Ist bei einer Power-Funktion ein ungültiger Wert beteiligt, so ist das
  // Resultat auch ungültig.
  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }

  return dat_first->power( eng, dat_second ); // Power
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpNegate::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpNegate::execute");
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() ){
    return op_FatalError;
  }
  return dat_first->negate( eng ); // Negativ
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpGtr::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpGtr::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  OpStatus status = op_Warning;
  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }

  return dat_first->greater( eng, dat_second ); // Grösser
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpGeq::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpGeq::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  OpStatus status = op_Warning;
  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }

  return dat_first->greaterEqual( eng, dat_second ); // Grösser gleich
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpLss::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpLss::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }

  return dat_first->less( eng, dat_second ); // Kleiner
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpLeq::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpLeq::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushInvalid();
    return op_Warning;
  }

  return dat_first->lessEqual( eng, dat_second ); // Kleiner Gleich
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpEql::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpEql::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushTrue( dat_first->isInvalid() && dat_second->isInvalid() );
    return op_Warning;
  }

  return dat_first->equal( eng, dat_second ); // Gleich
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpNeq::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpNeq::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  if( dat_first->isInvalid() || dat_second->isInvalid() ){
    eng->pushTrue( dat_first->isValid() || dat_second->isValid() );
    return op_Warning;
  }

  return dat_first->notEqual( eng, dat_second ); // Nicht Gleich
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpAnd::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpAnd::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  eng->pushTrue( dat_first->isTrue() && dat_second->isTrue() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpOr::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpOr::execute");
  JobStackDataPtr dat_second( eng->pop() );
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() || dat_second.isnt_valid() ) return op_FatalError;

  eng->pushTrue( dat_first->isTrue() || dat_second->isTrue() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpNot::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpNot::execute");
  JobStackDataPtr dat_first( eng->pop() );
  if( dat_first.isnt_valid() ) return op_FatalError;

  eng->pushTrue( !dat_first->isTrue() );
  return op_Ok;
}
