
#if !defined(JOB_CODE_EXECUTE_INCLUDED_H)
#define JOB_CODE_EXECUTE_INCLUDED_H

#include "app/ReportGen.h"
#include "job/JobCodeExec.h"

class JobEngine;
class JobFunction;
class JobAction;
class Plugin;
class WorkerListener;
class ProcessGroup;

/** Execute a JobAction
 */
class JobCodeExecuteAction : public JobCodeExec
{
public:
  JobCodeExecuteAction( JobAction *proc ): m_action( proc ){}
  virtual ~JobCodeExecuteAction(){}
  /** Diese Funktion ruft die JobAction auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );

  virtual void serializeXML(std::ostream &os, bool recursive = false){
    m_action->serializeXML(os, recursive);
  }

private:
  JobAction    *m_action;
};

class JobCodePluginInitialise : public JobCodeExec
{
public:
  JobCodePluginInitialise( Plugin *plugin ): m_plugin( plugin ){}
  virtual ~JobCodePluginInitialise(){}
  /** Diese Funktion initialisiert das Plugin für den kommenden Aufruf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  Plugin    *m_plugin;
};

/** Pop a Plugin Parameter from Stack
 */
class JobCodePluginParameter : public JobCodeExec
{
public:
  JobCodePluginParameter( Plugin *plugin ): m_plugin( plugin ){}
  virtual ~JobCodePluginParameter(){}
  /** Diese Funktion holt ein StackData vom Stack und übergibt es
      einem Plugin.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  Plugin    *m_plugin;
};

class JobCodeActionResult : public JobCodeExec
{
public:
  JobCodeActionResult(){}
  virtual ~JobCodeActionResult(){}
  /** Diese Funktion analysiert das Resultat einer JobAction.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Execute a Function
 */
class JobCodeExecuteFunction : public JobCodeExec
{
public:
 JobCodeExecuteFunction( JobAction *func )
   : m_action( func ), m_xfer(0) {}
 JobCodeExecuteFunction( XferDataItem* xfer )
   : m_action(0), m_xfer( xfer ) {}
  virtual ~JobCodeExecuteFunction(){}
  /** Diese Funktion ruft die Function m_action auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );

  virtual void serializeXML(std::ostream &os, bool recursive = false ){
    if (m_action)
      m_action->serializeXML(os, recursive);
  }
private:
  JobAction    *m_action;
  XferDataItem *m_xfer;
};

/** Set Hardcopy in DialogHardcopy
 */
class JobCodeSetHardcopy : public JobCodeExec
{
public:
  JobCodeSetHardcopy(const std::string &reportId) :m_reportId( reportId ) {}
  virtual ~JobCodeSetHardcopy(){}
  /** Diese Funktion setzt im Hardcopy-Dialog den entsprechenden Eintrag.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string   m_reportId;
};

/** Start Hardcopy in DialogHardcopy
 */
class JobCodeStartHardcopy : public JobCodeExec
{
public:
 JobCodeStartHardcopy( ReportGen::Mode mode, HardCopyListener *hcl )
   : m_mode( mode ), m_hcl(hcl) {}
  virtual ~JobCodeStartHardcopy(){}
  /** Diese Funktion startet den Hardcopy-Dialog mit dem entsprechenden Eintrag.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  ReportGen::Mode   m_mode;
  HardCopyListener *m_hcl;
};

#endif
