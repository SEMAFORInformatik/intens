
#if !defined(JOB_CODE_PUSH_INCLUDED_H)
#define JOB_CODE_PUSH_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;
class JobDataReference;
class JobVarDataReference;
class GuiIndex;

/// Pop and delete
class JobCodePopAndDelete : public JobCodeExec
{
public:
  JobCodePopAndDelete() {}
  virtual ~JobCodePopAndDelete() {}
  /** Die Funktion löscht einen Wert auf dem Stack.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};


/// PushInvalid
class JobCodePushInvalid : public JobCodeExec
{
public:
  JobCodePushInvalid(){}
  virtual ~JobCodePushInvalid(){}
  /** Die Funktion legt einen ungültigen Wert auf dem Stack ab.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushReal
class JobCodePushReal : public JobCodeExec
{
public:
  JobCodePushReal( double d ) :m_value( d ){}
  virtual ~JobCodePushReal(){}
  /** Die Funktion legt einen konstanten Wert (double) auf dem Stack ab.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  double   m_value;
};

/// PushInteger
class JobCodePushInteger : public JobCodeExec
{
public:
  JobCodePushInteger( int i ) :m_value( i ){}
  virtual ~JobCodePushInteger(){}
  /** Die Funktion legt einen konstanten Wert (int) auf dem Stack ab.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  int   m_value;
};

/// PushInteger
class JobCodePushSortOrder : public JobCodeExec
{
public:
  JobCodePushSortOrder( const std::string &listId ) :m_listId( listId ){}
  virtual ~JobCodePushSortOrder(){}
  /** Die Funktion legt einen konstanten Wert (int) auf dem Stack ab.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string   m_listId;
};

/// PushString
class JobCodePushString : public JobCodeExec
{
public:
  JobCodePushString( const std::string &s ) :m_value( s ){}
  virtual ~JobCodePushString(){}
  /** Die Funktion legt einen konstanten Wert (string) auf dem Stack ab.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string   m_value;
};

/// PushEndOfLine
class JobCodePushEndOfLine : public JobCodeExec
{
public:
  JobCodePushEndOfLine(){}
  virtual ~JobCodePushEndOfLine(){}
  /** Die Funktion legt ein 'EndOfLine' auf dem Stack ab.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushReason
class JobCodePushReason : public JobCodeExec
{
public:
  JobCodePushReason(CallReason r ) :m_reason( r ){}
  virtual ~JobCodePushReason(){}
  /** Die Funktion legt auf dem Stack ab, ob der über das GUI eingegebene
      Grund des Aufrufs richtig ist (0 oder 1).
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  CallReason  m_reason;
};

/// PushNewValue
class JobCodePushNewValue : public JobCodeExec
{
public:
  JobCodePushNewValue(){}
  virtual ~JobCodePushNewValue(){}
  /** Die Funktion legt den über das GUI eingegebenen neuen Wert auf dem
      Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert ungültig.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushNewValid
class JobCodePushNewValid : public JobCodeExec
{
public:
  JobCodePushNewValid(){}
  virtual ~JobCodePushNewValid(){}
  /** Die Funktion legt auf dem Stack ab, ob der über das GUI eingegebene
      neue Wert gültig ist (0 oder 1).
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer 0.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushOldValue
class JobCodePushOldValue : public JobCodeExec
{
public:
  JobCodePushOldValue(){}
  virtual ~JobCodePushOldValue(){}
  /** Die Funktion legt den im GUI überschriebenen alten Wert auf dem
      Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert ungültig.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushOldValid
class JobCodePushOldValid : public JobCodeExec
{
public:
  JobCodePushOldValid(){}
  virtual ~JobCodePushOldValid(){}
  /** Die Funktion legt auf dem Stack ab, ob der über das GUI überschriebene
      alte Wert gültig war (0 oder 1).
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer 0.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushColIndex
class JobCodePushColIndex : public JobCodeExec
{
public:
  JobCodePushColIndex(){}
  virtual ~JobCodePushColIndex(){}
  /** Die Funktion legt den Column-Index der über das GUI bearbeiteten
      Datenvariablen auf dem Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer 0.
      Diese Operator existiert nur aus Gründen der Kompatibilität zum
      alten Intens. Jede Variable war dort eine Matrix.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushRowIndex
class JobCodePushRowIndex : public JobCodeExec
{
public:
  JobCodePushRowIndex(){}
  virtual ~JobCodePushRowIndex(){}
  /** Die Funktion legt den Row-Index der über das GUI bearbeiteten
      Datenvariablen auf dem Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer 0.
      Diese Operator existiert nur aus Gründen der Kompatibilität zum
      alten Intens. Jede Variable war dort eine Matrix.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushRowDiagramXPos
class JobCodePushDiagramXPos : public JobCodeExec
{
public:
  JobCodePushDiagramXPos(){}
  virtual ~JobCodePushDiagramXPos(){}
  /** Die Funktion legt den DiagramXPos der über das GUI bearbeiteten
      Datenvariablen auf dem Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer 0.
      Diese Operator existiert nur aus Gründen der Kompatibilität zum
      alten Intens. Jede Variable war dort eine Matrix.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushRowDiagramYPos
class JobCodePushDiagramYPos : public JobCodeExec
{
public:
  JobCodePushDiagramYPos(){}
  virtual ~JobCodePushDiagramYPos(){}
  /** Die Funktion legt den DiagramYPos der über das GUI bearbeiteten
      Datenvariablen auf dem Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer 0.
      Diese Operator existiert nur aus Gründen der Kompatibilität zum
      alten Intens. Jede Variable war dort eine Matrix.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushSortCriteria
class JobCodePushSortCriteria : public JobCodeExec
{
public:
  JobCodePushSortCriteria(){}
  virtual ~JobCodePushSortCriteria(){}
  /** Die Funktion legt den SortCriteria der über das GUI bearbeiteten
      Datenvariablen auf dem Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist, sonst ist der Wert immer invalid.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine *eng );
};

/// PushIndex
class JobCodePushIndex : public JobCodeExec
{
public:
  JobCodePushIndex(){}
  virtual ~JobCodePushIndex(){}
  /** Die Funktion legt den gewünschten Index der über das GUI bearbeiteten
      Datenvariablen auf dem Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer invalid.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushUnits
class JobCodePushUnits : public JobCodeExec
{
public:
  JobCodePushUnits(){}
  virtual ~JobCodePushUnits(){}
  virtual OpStatus execute( JobEngine * );
};

/// PushLabel
class JobCodePushLabel : public JobCodeExec
{
public:
  JobCodePushLabel(){}
  virtual ~JobCodePushLabel(){}
  virtual OpStatus execute( JobEngine * );
};

/// PushIndexOfLevel
class JobCodePushIndexOfLevel : public JobCodeExec
{
public:
  JobCodePushIndexOfLevel(){}
  virtual ~JobCodePushIndexOfLevel(){}
  /** Die Funktion legt den gewünschten Index der über das GUI bearbeiteten
      Datenvariablen auf dem Stack ab.
      Dies ist nur möglich, wenn die aufgerufene Funktion mit einer
      Datenvariablen verbunden ist. sonst ist der Wert immer invalid.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};


/// PushIndexNumber
class JobCodePushIndexNumber : public JobCodeExec
{
public:
  JobCodePushIndexNumber(){}
  virtual ~JobCodePushIndexNumber(){}
  /** Die Funktion legt die gewünschte IndexNummer auf dem Stack ab.
      Ist die Funktion mit einer Datenvariablen verbunden, so entspricht
      das Ergebnis einem Aufruf von JobCodePushColIndex::execute().
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushError
class JobCodePushError : public JobCodeExec
{
public:
  JobCodePushError(){}
  virtual ~JobCodePushError(){}
  /** Die Funktion legt einen Wert (0 oder 1) auf dem Stack ab der
      anzeigt, ob mit SET_ERROR ein Fehler raportiert wurde.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// PushVariable
class JobCodePushVariable : public JobCodeExec
{
public:
  JobCodePushVariable( JobDataReference *data ) : m_data( data ){}
  virtual ~JobCodePushVariable(){}
  /** Die Funktion holt alle Indizes einer Variablen vom Stack und setzt
      sie in der DataReference ein, Anschliessend wird die indexierte Variable
      auf dem Stack abgelegt.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  JobDataReference *m_data;
};

/// JobCodeSetCycleNumber
class JobCodeSetCycleNumber : public JobCodeExec
{
public:
  JobCodeSetCycleNumber(){}
  virtual ~JobCodeSetCycleNumber(){}
  /** Die Funktion holt eine Variable vom Stack und setzt für den Zugriff
      die Cyclenummer neu.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/// SetThis
class JobCodeSetThis : public JobCodeExec
{
public:
  JobCodeSetThis( JobDataReference *data ) : m_data( data ){}
  virtual ~JobCodeSetThis(){}
  /** Die Funktion initialisiert das JobVarDataReference-Objekt
      mit der DataReference des Inputs.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  JobDataReference *m_data;
};

/// SetBase
class JobCodeSetBase : public JobCodeExec
{
public:
  JobCodeSetBase( JobDataReference *data ) : m_data( data ){}
  virtual ~JobCodeSetBase(){}
  /** Die Funktion initialisiert das JobVarDataReference-Objekt
      mit der Base DataReference des Inputs.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  JobDataReference *m_data;
};

/// SetSource
class JobCodeSetSource : public JobCodeExec
{
public:
  JobCodeSetSource( JobDataReference *data, JobDataReference *data_idx )
    : m_data( data ), m_dataIdx( data_idx ){}
  virtual ~JobCodeSetSource(){}
  /** Die Funktion initialisiert das JobVarDataReference-Objekt
      mit der DataReference des Inputs.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  JobDataReference *m_data;
  JobDataReference *m_dataIdx;
};

/// SetSource2
class JobCodeSetSource2 : public JobCodeExec
{
public:
  JobCodeSetSource2( JobDataReference *data ) : m_data( data ){}
  virtual ~JobCodeSetSource2(){}
  /** Die Funktion initialisiert das JobVarDataReference-Objekt
      mit der DataReference des Inputs.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  JobDataReference *m_data;
};

/// PushIndex
class JobCodePushGuiIndex : public JobCodeExec
{
public:
  JobCodePushGuiIndex( GuiIndex *index ) : m_index( index ){}
  virtual ~JobCodePushGuiIndex(){}
  /** Die Funktion execute() nimmt den wert des GuiIndex und pushed diesen
      auf den Stack.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiIndex *m_index;
};


#endif
