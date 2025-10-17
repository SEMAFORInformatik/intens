#if !defined(JOB_CODE_ITEM_INCLUDED_H)
#define JOB_CODE_ITEM_INCLUDED_H

#include "job/JobElement.h"

class JobEngine;

/** Alle Code-Objekte werden von JobCodeItem abgeleitet.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobCodeItem.h,v 1.3 2005/04/07 13:26:19 ked Exp $
*/
class JobCodeItem : public JobElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobCodeItem(){}
  virtual ~JobCodeItem(){}

/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  /** Diese Funktion soll den Typ eines Code-Objects liefern.
      @return Typ des Objekts (siehe JobElement.h)
   */
  virtual CodeType Type() = 0;

  /** Code-Objekte vom Typ code_Operation können mit execute() ausgeführt werden.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine *eng ) = 0;

  /** Bei einem vorzeitigen Abbruch der Verarbeitung wird diese Funktion vom
      JobEngine-Objekt für das nächste folgende JobCodeItem aufgerufen für
      allfällige Aufräumarbeiten. Dies ist nur für JobCode-Objekte mit einer
      Pause-Funktion (op_Pause) interessant, z.B. bei noch laufenden Threads.
      \return Status der Operation für nicht vorhersehbare Abstürze
  */
  virtual OpStatus cancel( JobEngine *eng ){ return op_FatalError; }

  virtual void serializeXML(std::ostream &, bool recursive = false){}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};

#endif
