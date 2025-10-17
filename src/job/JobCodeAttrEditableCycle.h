
#if !defined(JOB_CODE_ATTR_EDITABLE_CYCLE_INCLUDED_H)
#define JOB_CODE_ATTR_EDITABLE_CYCLE_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;

/** Attribute Editable Cycle
 */
class JobCodeAttrEditableCycle : public JobCodeExec
{
public:
  JobCodeAttrEditableCycle(){}
  virtual ~JobCodeAttrEditableCycle(){}
  /** Diese Funktion setzt das Data-Attribute Editable eines
      bestimmten Cycles.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

#endif
