
#include "job/JobIncludes.h"
#include "job/JobManager.h"
#include "job/JobTask.h"
#include "job/JobEngine.h"
#include "job/JobStackItem.h"
#include "job/JobStackProcess.h"
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "gui/GuiFactory.h"
#include "gui/GuiDialog.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/Timer.h"
#include "utils/Date.h"
#include "utils/Debugger.h"
#include "utils/gettext.h"

INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobTask::JobTask( const std::string &name )
  : JobFunction( name )
  , m_result( job_Ok ){
  setReason( cll_Task );
}

JobTask::~JobTask(){
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* printLogTitle --                                                            */
/* --------------------------------------------------------------------------- */

void JobTask::printLogTitle( std::ostream &ostr ){
  ostr << "TASK " << getLabel();
}

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */

void JobTask::setLabel( const std::string &label ) {
  if( !label.empty() ){
    m_label = label;
  }
}

/* --------------------------------------------------------------------------- */
/* getLabel --                                                                 */
/* --------------------------------------------------------------------------- */

const std::string &JobTask::getLabel() {
  if( !m_label.empty() ){
    return m_label;
  }
  return Name();
}

/* --------------------------------------------------------------------------- */
/* epilog --                                                                   */
/* --------------------------------------------------------------------------- */

void JobTask::epilog( OpStatus op_status ){
  BUG(BugJobStart,"JobTask::epilog");
  DataPoolIntens &datapool = DataPoolIntens::Instance();
  std::ostringstream message;

  m_result = job_Ok;

  switch( op_status ){
  case op_EndOfJob:
    if( m_engine->getError() ){
      op_status = op_Aborted;
      m_result = job_Aborted;
    }

    if( !m_engine->Message().empty() ){
      if( m_result == job_Aborted )
	printMessage( m_engine->Message(), GuiElement::msg_Warning );
      else
	printMessage( m_engine->Message(), GuiElement::msg_Information );
    }
    else
    if( m_engine->getError() ){
      message << "Error: Task '" << getLabel() << "' failed (see in Log-Window)";
      printMessage( message.str(), GuiElement::msg_Warning );
    }
    else
    if( AppData::Instance().Helpmessages() != AppData::NoneType ){
      message << "Task '" << getLabel() << "' completed ok";
      printMessage( message.str(), GuiElement::msg_Information );
    }
    break;

  case op_Canceled:
    message << "Cancel: Task '" << getLabel() << "' interrupted";
    printMessage( message.str(), GuiElement::msg_Warning );
    m_result = job_Canceled;
    m_engine->cancel();
    break;

  case op_Aborted:
    if( !m_engine->Message().empty() ){
      printMessage( m_engine->Message(), GuiElement::msg_Warning );
    }
    else{
      message << "Abort: Task '" << getLabel() << "' failed";
      printMessage( message.str(), GuiElement::msg_Warning );
    }
    m_result = job_Aborted;
    break;

  case op_FatalError:
    message << "FATAL ERROR: Task '" << getLabel() << "' failed";
    printMessage( message.str(), GuiElement::msg_Error );
    m_result = job_FatalError;
    break;

  case op_NotImplemented:
    message << "ERROR: Task '" << Name() << "' not implemented";
    printMessage( message.str(), GuiElement::msg_Error );
    m_result = job_Aborted;
    break;

  default:
    assert(false);
  }

  // if( JobStackItem::getItemCount() != 0 ){
  //   std::cerr << "WARNING: After epilog of Task " << Name() << ": "
  // 	      << JobStackItem::getItemCount() << " StackItems in memory, "
  // 	      << JobDataReference::getRefCount() << " Refs in memory and "
  // 	      << m_engine->sizeOfStack() << " StackItems in the stack" << std::endl;
  // }

  cleanup(); // Aufraeumen

  endJobAction( m_result );
  BUG_EXIT("Stack Items: " << JobStackItem::getItemCount() );
}

/* --------------------------------------------------------------------------- */
/* createButtonListener --                                                     */
/* --------------------------------------------------------------------------- */

GuiButtonListener *JobTask::createButtonListener(){
  return new JobTaskButtonListener( this );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void JobTask::serializeXML(std::ostream &os, bool recursive){
  os << "<task name=\"" << Name() << "\"";
  os << " line=\"" << Lineno() << "\"";
  auto filename = Filename();
  std::replace(filename.begin(), filename.end(), '"', '\'');
  os << " file=\"" << filename << "\"";
  os  << ">" << std::endl;
  if( recursive ){
    JobCodeStream::iterator it;
    for( it = getCodeStream()->begin(); it != getCodeStream()->end(); ++it ){
      (*it)->serializeXML(os, recursive);
    }
  }
  os << "</task>" <<  std::endl;
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void JobTask::JobTaskButtonListener::ButtonPressed(){
  BUG(BugJobStart,"JobTask::JobTaskButtonListener::ButtonPressed");
  m_function->setReason( cll_Function );
  if (CallingActionName().empty()) {
    PYLOG_INFO(compose(PYLOG_CALL_PROCESS, m_function->Name(), "Task"));
    DES_INFO(compose("  RUN(%1); // JobTask", m_function->Name()));
    if (AppData::Instance().PyLogMode()) {
      std::cout << "<Abort> JobTask::JobTaskButtonListener::ButtonPressed()\n";
      return;
    }
  }
  JobActionButtonListener::ButtonPressed();
}
