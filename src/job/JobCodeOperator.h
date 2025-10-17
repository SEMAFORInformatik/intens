#if !defined(JOB_CODE_OPERATOR_INCLUDED_H)
#define JOB_CODE_OPERATOR_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;

/// Add
class JobCodeOpAdd : public JobCodeExec
{
public:
  JobCodeOpAdd(){}
  virtual ~JobCodeOpAdd(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und addiert sie.
      Das Resultat wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Subtract
class JobCodeOpSubtract : public JobCodeExec
{
public:
  JobCodeOpSubtract(){}
  virtual ~JobCodeOpSubtract(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und subtrahiert das
      erste Element vom Zweiten.
      Das Resultat von (e2 - e1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Multiply
class JobCodeOpMultiply : public JobCodeExec
{
public:
  JobCodeOpMultiply(){}
  virtual ~JobCodeOpMultiply(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und multipliziert sie.
      Das Resultat von (e1 * e2) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Divide
class JobCodeOpDivide : public JobCodeExec
{
public:
  JobCodeOpDivide(){}
  virtual ~JobCodeOpDivide(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und dividiert das
      zweite Element durch das Erste.
      Das Resultat von (e2 / e1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Modulo
class JobCodeOpModulo : public JobCodeExec
{
public:
  JobCodeOpModulo(){}
  virtual ~JobCodeOpModulo(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und führt eine
      modulo Operation durch
      zweite Element modulo das Erste.
      Das Resultat von (e2 % e1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Power
class JobCodeOpPower : public JobCodeExec
{
public:
  JobCodeOpPower(){}
  virtual ~JobCodeOpPower(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und potenziert das
      zweite Element mit dem Ersten.
      Das Resultat von (e2 ** e1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Negate
class JobCodeOpNegate : public JobCodeExec
{
public:
  JobCodeOpNegate(){}
  virtual ~JobCodeOpNegate(){}
  /** Die Funktion holt sich ein Element vom Stack und ändert das Vorzeichen.
      Das Resultat von (-e1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Greater
class JobCodeOpGtr : public JobCodeExec
{
public:
  JobCodeOpGtr(){}
  virtual ~JobCodeOpGtr(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 > e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// GreaterEqual
class JobCodeOpGeq : public JobCodeExec
{
public:
  JobCodeOpGeq(){}
  virtual ~JobCodeOpGeq(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 >= e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Less
class JobCodeOpLss : public JobCodeExec
{
public:
  JobCodeOpLss(){}
  virtual ~JobCodeOpLss(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 < e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// LessEqual
class JobCodeOpLeq : public JobCodeExec
{
public:
  JobCodeOpLeq(){}
  virtual ~JobCodeOpLeq(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 <= e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Equal
class JobCodeOpEql : public JobCodeExec
{
public:
  JobCodeOpEql(){}
  virtual ~JobCodeOpEql(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 == e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// NotEqual
class JobCodeOpNeq : public JobCodeExec
{
public:
  JobCodeOpNeq(){}
  virtual ~JobCodeOpNeq(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 != e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

// And
class JobCodeOpAnd : public JobCodeExec
{
public:
  JobCodeOpAnd(){}
  virtual ~JobCodeOpAnd(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 && e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Or
class JobCodeOpOr : public JobCodeExec
{
public:
  JobCodeOpOr(){}
  virtual ~JobCodeOpOr(){}
  /** Die Funktion holt sich zwei Elemente vom Stack und evaliert (e2 || e1).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// Not
class JobCodeOpNot : public JobCodeExec
{
public:
  JobCodeOpNot(){}
  virtual ~JobCodeOpNot(){}
  /** Die Funktion holt sich ein Element vom Stack und evaliert (e1 != 0).
      Das Resultat (0 oder 1) wird wieder auf dem Stack plaziert.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

#endif
