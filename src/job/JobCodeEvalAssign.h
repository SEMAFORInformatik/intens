
#if !defined(JOB_CODE_EVAL_ASSIGN_INCLUDED_H)
#define JOB_CODE_EVAL_ASSIGN_INCLUDED_H

#include "job/JobCodeExec.h"

class JobEngine;
class JobDataReference;
class GuiIndex;

/** Die Code-Funktion dieser Klasse weist einer Variablen einen Wert zu.
 */
class JobCodeAssign : public JobCodeExec
{
public:
  JobCodeAssign(){}
  virtual ~JobCodeAssign(){}
  /** Die Funktion execute() weist einer Variablen einen Wert zu.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse weist einer Struct-Variablen alle Werte
    der korrespondierenden Items zu.
 */
class JobCodeAssignCorr : public JobCodeExec
{
public:
  JobCodeAssignCorr(){}
  virtual ~JobCodeAssignCorr(){}
  /** Die Funktion execute() weist einer Variablen einen Wert zu.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse addiert einen Wert in eine Variable.
 */
class JobCodeAccumulate : public JobCodeExec
{
public:
  JobCodeAccumulate(){}
  virtual ~JobCodeAccumulate(){}
  /** Die Funktion execute() addiert einen Wert in eine Variable.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse erhöht den Wert einer Variablen um 1.
 */
class JobCodeIncrement : public JobCodeExec
{
public:
  JobCodeIncrement(){}
  virtual ~JobCodeIncrement(){}
  /** Die Funktion execute() erhöht den Wert einer Variablen um 1.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse erhöht den Wert einer Variablen um 1.
 */
class JobCodeDecrement : public JobCodeExec
{
public:
  JobCodeDecrement(){}
  virtual ~JobCodeDecrement(){}
  /** Die Funktion execute() reduziert den Wert einer Variablen um 1.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse weist einem GuiIndex einen Wert zu.
 */
class JobCodeAssignIndex : public JobCodeExec
{
public:
  JobCodeAssignIndex( GuiIndex *index ) : m_index( index ){}
  virtual ~JobCodeAssignIndex(){}
  /** Die Funktion execute() weist einem GuiIndex einen Wert zu.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiIndex *m_index;
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Wert einer Variablen.
*/
class JobCodeEvalValue : public JobCodeExec
{
public:
  JobCodeEvalValue(){}
  virtual ~JobCodeEvalValue(){}
  /** Die Funktion execute() holt eine Variable vom Stack und ermittelt den
      Wert. Das Resultat wird auf dem Stack abgelegt.
      @param engine Pointer auf das ausführende JobEngine-Objekt
      @return Status nach der Operation.
  */
  virtual OpStatus execute( JobEngine *engine );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Complex-Wert einer Variablen.
*/
class JobCodeEvalComplex : public JobCodeExec
{
public:
  JobCodeEvalComplex(){}
  virtual ~JobCodeEvalComplex(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Real-Teil einer Variablen.
*/
class JobCodeEvalReal : public JobCodeExec
{
public:
  JobCodeEvalReal(){}
  virtual ~JobCodeEvalReal(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Imaginär-Teil einer Variablen.
*/
class JobCodeEvalImag : public JobCodeExec
{
public:
  JobCodeEvalImag(){}
  virtual ~JobCodeEvalImag(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Betrag einer Variablen.
*/
class JobCodeEvalAbs : public JobCodeExec
{
public:
  JobCodeEvalAbs(){}
  virtual ~JobCodeEvalAbs(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Betrag einer Variablen.
*/
class JobCodeEvalLength : public JobCodeExec
{
public:
  JobCodeEvalLength(){}
  virtual ~JobCodeEvalLength(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Sinus einer Variablen.
*/
class JobCodeEvalSin : public JobCodeExec
{
public:
  JobCodeEvalSin(){}
  virtual ~JobCodeEvalSin(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    ArcusSinus einer Variablen.
*/
class JobCodeEvalASin : public JobCodeExec
{
public:
  JobCodeEvalASin(){}
  virtual ~JobCodeEvalASin(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Log10 einer Variablen.
*/
class JobCodeEvalLog10 : public JobCodeExec
{
public:
  JobCodeEvalLog10(){}
  virtual ~JobCodeEvalLog10(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Cosinus einer Variablen.
*/
class JobCodeEvalCos : public JobCodeExec
{
public:
  JobCodeEvalCos(){}
  virtual ~JobCodeEvalCos(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Tangens einer Variablen.
*/
class JobCodeEvalTan : public JobCodeExec
{
public:
  JobCodeEvalTan(){}
  virtual ~JobCodeEvalTan(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    ArcusCosinus einer Variablen.
*/
class JobCodeEvalACos : public JobCodeExec
{
public:
  JobCodeEvalACos(){}
  virtual ~JobCodeEvalACos(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    ArcusTangens einer Variablen.
*/
class JobCodeEvalATan : public JobCodeExec
{
public:
  JobCodeEvalATan(){}
  virtual ~JobCodeEvalATan(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    ArcusTangens2 zweier job_expressions.
*/
class JobCodeEvalATan2 : public JobCodeExec
{
public:
  JobCodeEvalATan2(){}
  virtual ~JobCodeEvalATan2(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Log einer Variablen.
*/
class JobCodeEvalLog : public JobCodeExec
{
public:
  JobCodeEvalLog(){}
  virtual ~JobCodeEvalLog(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt die
    Wurzel einer Variablen.
*/
class JobCodeEvalSqrt : public JobCodeExec
{
public:
  JobCodeEvalSqrt(){}
  virtual ~JobCodeEvalSqrt(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt den
    Winkel einer Variablen.
*/
class JobCodeEvalArg : public JobCodeExec
{
public:
  JobCodeEvalArg(){}
  virtual ~JobCodeEvalArg(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ermittelt die
    Gültigkeit einer Variablen.
*/
class JobCodeEvalValid : public JobCodeExec
{
public:
  JobCodeEvalValid(){}
  virtual ~JobCodeEvalValid(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeClearValues: public JobCodeExec
{
public:
  JobCodeClearValues(){}
  virtual ~JobCodeClearValues(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeEraseValues: public JobCodeExec
{
public:
  JobCodeEraseValues(){}
  virtual ~JobCodeEraseValues(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodePackRowValues: public JobCodeExec
{
public:
  JobCodePackRowValues(){}
  virtual ~JobCodePackRowValues(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodePackColValues: public JobCodeExec
{
public:
  JobCodePackColValues(){}
  virtual ~JobCodePackColValues(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeBeginTransaction: public JobCodeExec
{
public:
  JobCodeBeginTransaction(){}
  virtual ~JobCodeBeginTransaction(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeCommitTransaction: public JobCodeExec
{
public:
  JobCodeCommitTransaction(){}
  virtual ~JobCodeCommitTransaction(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeAbortTransaction: public JobCodeExec
{
public:
  JobCodeAbortTransaction(){}
  virtual ~JobCodeAbortTransaction(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse ueberprueft
    ob daten innerhalb eines GuiElements geaendert wurden
 */
class JobCodeEvalChanged : public JobCodeExec
{
public:
  JobCodeEvalChanged(const std::string &guiElemId, const bool filter)
   : m_guiElemId(guiElemId)
   , m_filter(filter) {}
  virtual ~JobCodeEvalChanged(){}
  virtual OpStatus execute( JobEngine * );
 private:
  std::string m_guiElemId;
  bool m_filter;
};

#endif
