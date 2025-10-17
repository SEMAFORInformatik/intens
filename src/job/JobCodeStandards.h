
#if !defined(JOB_CODE_STANDARDS_INCLUDED_H)
#define JOB_CODE_STANDARDS_INCLUDED_H

#include <string>

#include "job/JobCodeExec.h"

class JobEngine;
class JobDataReference;

class JobCodeOpIf : public JobCodeExec
{
public:
  JobCodeOpIf(): m_else( 0 ), m_next( 0 ){}
  virtual ~JobCodeOpIf(){}
  virtual OpStatus execute( JobEngine * );
  void setElseAddress( int addr ) { m_else = addr; }
  void setNextAddress( int addr ) { m_next = addr; }
private:
  int m_else;
  int m_next;
};

class JobCodeOpWhile : public JobCodeExec
{
public:
  JobCodeOpWhile(): m_next( 0 ){}
  virtual ~JobCodeOpWhile(){}
  virtual OpStatus execute( JobEngine * );
  void setNextAddress( int addr ) { m_next = addr; }
private:
  int m_next;
};

class JobCodeBranchWithAddr : public JobCodeExec
{
public:
  JobCodeBranchWithAddr( int addr ): m_addr( addr ){}
  virtual ~JobCodeBranchWithAddr(){}
  virtual OpStatus execute( JobEngine * );
private:
  int   m_addr;
};

class JobCodeBranch : public JobCodeExec
{
public:
  JobCodeBranch(){}
  virtual ~JobCodeBranch(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeAbort : public JobCodeExec
{
public:
  JobCodeAbort(){}
  virtual ~JobCodeAbort(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeExitIntens : public JobCodeExec
{
public:
  JobCodeExitIntens(bool force=false)
    : m_force(force){}
  virtual ~JobCodeExitIntens(){}
  virtual OpStatus execute( JobEngine * );
private:
  bool m_force;
};

class JobCodeBeep : public JobCodeExec
{
public:
  JobCodeBeep(){}
  virtual ~JobCodeBeep(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeEndOfFunction : public JobCodeExec
{
public:
  JobCodeEndOfFunction(){}
  virtual ~JobCodeEndOfFunction(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeNop : public JobCodeExec
{
public:
  JobCodeNop( const std::string &msg ): m_msg( msg ){}
  virtual ~JobCodeNop(){}
  virtual OpStatus execute( JobEngine * );
private:
  std::string   m_msg;
};

class JobCodeRound : public JobCodeExec
{
public:
  JobCodeRound(){}
  virtual ~JobCodeRound(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeRound5 : public JobCodeExec
{
public:
  JobCodeRound5(){}
  virtual ~JobCodeRound5(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeRound10 : public JobCodeExec
{
public:
  JobCodeRound10(){}
  virtual ~JobCodeRound10(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeSize : public JobCodeExec
{
public:
  JobCodeSize(){}
  virtual ~JobCodeSize(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeDataSize : public JobCodeExec
{
public:
  JobCodeDataSize(){}
  virtual ~JobCodeDataSize(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeIndex : public JobCodeExec
{
public:
  JobCodeIndex(){}
  virtual ~JobCodeIndex(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeIcon : public JobCodeExec
{
public:
  JobCodeIcon(){}
  virtual ~JobCodeIcon(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeCompare : public JobCodeExec
{
public:
  JobCodeCompare(bool bCycle) : m_bCycle(bCycle) {}
  virtual ~JobCodeCompare(){}
  virtual OpStatus execute( JobEngine * );
 private:
  int m_bCycle;
};

class JobCodeAssignConsistency : public JobCodeExec
{
public:
  JobCodeAssignConsistency() {}
  virtual ~JobCodeAssignConsistency(){}
  virtual OpStatus execute( JobEngine * );
};

class JobCodeClass : public JobCodeExec
{
public:
  JobCodeClass(GuiElement *gui_el, const std::string &className):
    m_guiElement(gui_el), m_className(className) {
  }
  virtual ~JobCodeClass(){}
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement *m_guiElement;
  std::string m_className;
};

class JobCodeSetResource : public JobCodeExec
{
public:
 JobCodeSetResource(const std::string &key, const std::string &value, bool useValue)
   : m_key(key), m_value(value), m_useValue(useValue) {}
  virtual ~JobCodeSetResource(){}
  virtual OpStatus execute( JobEngine * );
 private:
  std::string m_key;
  std::string m_value;
  bool        m_useValue;
};

/** Die Code-Funktion dieser Klasse setzt die
    THIS datenvariable
 */
class JobCodeAssignThis : public JobCodeExec
{
public:
  JobCodeAssignThis(){}
  virtual ~JobCodeAssignThis(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse setzt die
    INDEX der JobFunction gesetzt
 */
class JobCodeAssignFuncIndex : public JobCodeExec
{
public:
  JobCodeAssignFuncIndex(){}
  virtual ~JobCodeAssignFuncIndex(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse setzt die
    REASON der JobFunction gesetzt
 */
class JobCodeAssignFuncReason : public JobCodeExec
{
public:
  JobCodeAssignFuncReason(){}
  virtual ~JobCodeAssignFuncReason(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse setzt die
    Argumente der JobFunction
 */
class JobCodeAssignFuncArguments : public JobCodeExec
{
public:
  JobCodeAssignFuncArguments(){}
  virtual ~JobCodeAssignFuncArguments(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse gibt den
    Timestamp der ersten datenvariablen zurueck
 */
class JobCodeTimestamp : public JobCodeExec
{
public:
  JobCodeTimestamp(){}
  virtual ~JobCodeTimestamp(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse untersucht, ob
    ein Database-Item modifiziert wurde.
 */
class JobCodeDbModified : public JobCodeExec
{
public:
  JobCodeDbModified(){}
  virtual ~JobCodeDbModified(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse setzt den db Timestamp
    eins Database-Items (so dass es nacher als nicht modifiziert
    erkannt wird).
 */
class JobCodeSetDbTimestamp : public JobCodeExec
{
public:
  JobCodeSetDbTimestamp(){}
  virtual ~JobCodeSetDbTimestamp(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse gibt den
    Classname der datenvariablen zurueck
 */
class JobCodeClassname: public JobCodeExec
{
public:
  JobCodeClassname(){}
  virtual ~JobCodeClassname(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse gibt den
    Nodename der datenvariablen zurueck
 */
class JobCodeNodename: public JobCodeExec
{
public:
  JobCodeNodename(){}
  virtual ~JobCodeNodename(){}
  virtual OpStatus execute( JobEngine * );
};

#endif
