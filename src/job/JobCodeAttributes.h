
#if !defined(JOB_CODE_ATTRIBUTES_INCLUDED_H)
#define JOB_CODE_ATTRIBUTES_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;

/** \file
    Die Datei beschreibt die JobCode-Klassen für das Mutieren
    von Attributen im Datapool.
*/
/** Attribute Editable
 */
class JobCodeAttrEditable : public JobCodeExec
{
public:
  JobCodeAttrEditable(){}
  virtual ~JobCodeAttrEditable(){}
  /** Diese Funktion setzt das Data-Attribute Editable.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Attribute Locked
 */
class JobCodeAttrLock : public JobCodeExec
{
public:
  JobCodeAttrLock(){}
  virtual ~JobCodeAttrLock(){}
  /** Diese Funktion setzt das Data-Attribute Locked.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Attribute Color
 */
class JobCodeAttrColor : public JobCodeExec
{
public:
  JobCodeAttrColor(){}
  virtual ~JobCodeAttrColor(){}
  /** Diese Funktion setzt ein Data-Attribute Color<n>. Die
      anderen Color-Bits werden zurück gesetzt.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Attribute ColorSetBit
 */
class JobCodeAttrColorSetBit : public JobCodeExec
{
public:
  JobCodeAttrColorSetBit(){}
  virtual ~JobCodeAttrColorSetBit(){}
  /** Diese Funktion setzt ein Data-Attribute Color<n>.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Attribute ColorUnsetBit
 */
class JobCodeAttrColorUnsetBit : public JobCodeExec
{
public:
  JobCodeAttrColorUnsetBit(){}
  virtual ~JobCodeAttrColorUnsetBit(){}
  /** Diese Funktion setzt ein Data-Attribute Color<n> zurück.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Mit dem JobCode-Objekt der Klasse JobCodeAttrSetDataTimestamp wird
    der DataTimestamp in DataElement-Objekten gesetzt.
 */
class JobCodeAttrSetDataTimestamp : public JobCodeExec
{
public:
  /** Konstruktor */
  JobCodeAttrSetDataTimestamp(){}
  /** Destruktor */
  virtual ~JobCodeAttrSetDataTimestamp(){}
  /** Die Funktion setzt den DataTimestamp von DataElement-Objekten.
      \param eng Pointer auf das aufrufende JobEngine Objekt.
      \return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Mit dem JobCode-Objekt der Klasse JobCodeAttrSetValueTimestamp wird
    der ValueTimestamp in DataElement-Objekten gesetzt.
 */
class JobCodeAttrSetValueTimestamp : public JobCodeExec
{
public:
  /** Konstruktor */
  JobCodeAttrSetValueTimestamp(){}
  /** Destruktor */
  virtual ~JobCodeAttrSetValueTimestamp(){}
  /** Die Funktion setzt den ValueTimestamp von DataElement-Objekten.
      \param eng Pointer auf das aufrufende JobEngine Objekt.
      \return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

#endif
