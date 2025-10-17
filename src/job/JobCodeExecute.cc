
#include "job/JobIncludes.h"

#include "job/JobCodeExecute.h"
#include "job/JobEngine.h"
#include "job/JobTask.h"
#include "job/JobFunction.h"
#include "job/JobStackAddress.h"
#include "job/JobStackReturn.h"
#include "job/JobStackProcess.h"
#include "job/JobHardcopy.h"
#include "job/JobStackData.h"
#include "job/JobManager.h"
#include "app/Plugin.h"
#include "xfer/XferDataItem.h"
#include "operator/ProcessGroup.h"

#include "gui/GuiFactory.h"
#include "gui/GuiPrinterDialog.h"
#include "gui/GuiScrolledText.h"
#include "utils/Date.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeExecuteAction::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeExecuteAction::execute");
  eng->getFunction()->nextJobAction( m_action );
  BUG_EXIT("wait");
  return op_Wait;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePluginInitialise::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePluginInitialise::execute");
  m_plugin->clearArguments();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePluginParameter::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePluginParameter::execute");
  JobStackDataPtr dat( eng->pop() );
  std::string param;
  if( dat->getStringValue( param ) ){
    m_plugin->setArgument( param );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeActionResult::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeActionResult::execute");
  JobStackDataPtr dat( eng->pop() );
  assert( dat.is_valid() );
  if( !dat->isTrue() ){
    eng->setError();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeExecuteFunction::execute( JobEngine *eng ){
  std::string fname("<INVALID>");
  if( m_xfer && m_xfer->getValue( fname ) ){
    m_action = JobManager::Instance().getFunction(fname, false);
    if (!m_action) {
      m_action = JobManager::Instance().getTask(fname);
      if (dynamic_cast<JobTask*>(m_action) && AppData::Instance().HeadlessWebMode()) {
        std::stringstream logmsg;
        logmsg << DateAndTime() << " : Task " << dynamic_cast<JobTask*>(m_action)->getLabel() << std::endl;
        GuiFactory::Instance()->getLogWindow()->writeText(logmsg.str());
      }
    }
    if (!m_action) {
      m_action = ProcessGroup::find(fname);
    }
    if (!m_action) {
      if(fname == "AFTER_DB_LOGON" ||
         fname == "INIT" ||
         fname == "ON_CYCLE_EVENT" ||
         fname == "ON_CYCLE_SWITCH"
         ) {
        // automatically called functions after CYCLE actions (MessageQueueReply.inc)
        return op_Ok;
      }
      BUG_ERROR("function named '"<<fname<<"' not found" );
      return op_FatalError;
    }
  } else if(m_action) {
    fname = m_action->Name();
  }
  JobFunction* func = dynamic_cast<JobFunction*>(m_action);

  if(!m_action || (func && func->notImplemented())){
    std::stringstream logmsg;
    logmsg << DateAndTime() << " : ";
    logmsg << "ERROR: Function " << fname;
    if(m_xfer) {
      logmsg << " (Varname: " << m_xfer->getFullName(true) << ")";
    }
    if(func && func->notImplemented()) {
      logmsg << " NOT IMPLEMENTED";
    }
    logmsg << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    BUG_ERROR( logmsg.str() );
    return op_FatalError;
  }

  if (func) { // Func, Task
    BUG_DEBUG("Begin of FUNC/TASK " << m_action->Name());
    eng->callFunction( func );
    return op_Branch;
  } else {  // ProcessGroup, ...
    BUG_DEBUG("Begin of PROCESSGROUP " << m_action->Name());
    eng->getFunction()->nextJobAction( m_action );
    return op_Wait;
  }
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetHardcopy::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetHardcopy::execute");
  //  assert( m_reportId.size() );

  if( m_reportId.size() ) {
    GuiFactory::Instance()->createPrinterDialog()->setHardCopy( m_reportId );
    BUG_EXIT("Hardcopy '" << m_reportId << "' set" );
  }
  else{
    GuiFactory::Instance()->createPrinterDialog()->setHardCopy( "" );
    BUG_EXIT("no Hardcopy set");
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeStartHardcopy::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeStartHardcopy::execute");
  JobHardcopy *hardcopy = new JobHardcopy();
  hardcopy->setElement( eng->getFunction()->getGuiElement() );
  hardcopy->setHardCopyListener( m_hcl );
  hardcopy->setMode( m_mode );
  BUG_MSG("start Controller" );
  eng->getFunction()->nextJobAction( hardcopy );
  return op_Wait;
}
