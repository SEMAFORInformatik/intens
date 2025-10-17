
#if !defined(JOB_CODE_EXECUTABLE_INCLUDED_H)
#define JOB_CODE_EXECUTABLE_INCLUDED_H

#include "job/JobCodeItem.h"

/** Alle Code-Objekte, welche mit der Member-Funktion execute() ausgeführt werden
    können, werden von JobCodeExec abgeleitet.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobCodeExec.h,v 1.3 2000/08/28 12:55:58 ked Exp $
*/
class JobCodeExec : public JobCodeItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobCodeExec(){}
  virtual ~JobCodeExec(){}


/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  /** Diese Funktion liefert den Typ des Code-Objekts.
      @return Typ des Objekts (siehe JobElement.h)
  */
  virtual CodeType Type() { return code_Operation; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};

#endif
