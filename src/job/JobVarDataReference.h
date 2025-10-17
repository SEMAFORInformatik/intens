
#if !defined(JOB_VAR_DATAREFERENCE_INCLUDED_H)
#define JOB_VAR_DATAREFERENCE_INCLUDED_H

#include "job/JobDataReference.h"

/** Die JobDataReference enthält eine DataReference. Sie enthält zusätzlich eine
    Liste aller Indizes, welche zur Laufzeit eines Jobs neu gesetzt werden.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: JobVarDataReference.h,v 1.3 2007/02/14 13:07:56 amg Exp $
*/
class JobVarDataReference : public JobDataReference
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobVarDataReference(bool isJobVar = false, bool isJobParentVar = false);
  virtual ~JobVarDataReference();

private:
  JobVarDataReference( JobDataReference &dataref );

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  static JobDataReference *convertToVar( JobDataReference *ref );

  virtual bool isVariable() { return true; }

  /** Mit dieser Funktion werden zur Laufzeit eines Jobs der Name und die Indizes
      neu gesetzt. Die Funktion holt sich vom Stack der JobEngine die nötigen Daten.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus updateDataReference( JobEngine *eng );

  /** Mit dieser Funktion kann die Root-DataReverence neu gesetzt werden.
      @param ref Pointer der neuen Root-DataReference.
  */
  virtual void setDataRoot( DataReference *ref );
/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
  /**
   Konvertiert die DataReference zur Parent DataReference.
   Wird verwendet beim Parent Job (z.B.: 'PARENT(THIS).name;' )
   */
  bool convertToParentDataReference( DataReference *ref );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  DataReference  *m_data_root;
  int             m_level_offset;
  bool            m_isJobVar;
  bool            m_isJobParentVar;
};

#endif
