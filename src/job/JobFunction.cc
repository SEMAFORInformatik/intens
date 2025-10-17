
#include <iomanip>
#include <limits>
#include "job/JobIncludes.h"

#include "job/JobManager.h"
#include "job/JobFunction.h"
#include "job/JobEngine.h"
#include "job/JobController.h"
#include "job/JobStackItem.h"
#include "job/JobStackProcess.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"
#include "job/JobStackDataStructure.h"
#include "job/JobStackDataInput.h"
#include "job/JobWebApiResponse.h"
#ifdef HAVE_QT
#include "gui/qt/GuiQtManager.h"
#else
#include "gui/GuiManager.h"
#endif
#include "gui/GuiElement.h"
#include "gui/GuiNavigator.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/Timer.h"
#include "gui/GuiNavigatorMenu.h"
#include "datapool/DataReference.h"
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferGenericParameter.h"
#include "gui/GuiPopupMenu.h"
#include "operator/MessageQueueThreads.h"

#include "utils/gettext.h"
#include "utils/JsonUtils.h"

INIT_LOGGER();

/*=============================================================================*/
/* local Classes                                                               */
/*=============================================================================*/

class RunFuncListener : public GuiButtonListener
{
public:
  RunFuncListener( JobFunction *func ): m_function( func ){}
  virtual ~RunFuncListener() {}
  virtual void ButtonPressed();
    JobAction* getAction(){ return m_function; }
private:
  JobFunction  *m_function;
};

class NavigatorRunFuncListener : public GuiButtonListener
{
public:
  NavigatorRunFuncListener( GuiElement *menu, JobFunction *func )
    : m_function( func ){
    assert( menu != 0 );
    m_parent = menu->getPopupMenu();
  }
  virtual ~NavigatorRunFuncListener() {}
  virtual void ButtonPressed();
  virtual void setParent( GuiElement *par ) {
    m_parent = dynamic_cast<GuiPopupMenu*>(par);
  }
  JobAction* getAction(){ return m_function; }
private:
  JobFunction  *m_function;
  GuiPopupMenu *m_parent;
};

class Trigger : public JobStarter
{
public:
  Trigger( JobFunction *f )
    : JobStarter( f ){}
  virtual ~Trigger() {}
protected:
  virtual void backFromJobStarter( JobResult rslt );
};

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobFunction::JobFunction( const std::string &name )
  : JobAction( name )
  , m_engine( 0 )
  , m_reason( cll_Function )
  , m_canceled( false )
  , m_new_dataitem( 0 )
  , m_baseDataitem( 0 )
  , m_sourceDataitem( 0 )
  , m_source2Dataitem( 0 )
  , m_webReplyResultDataitem( 0 )
  , m_webReplyResultDataitemProto( false )
  , m_new_this_ref( 0 )
  , m_old_data( 0 )
  , m_index( -1 )
  , m_diagramXPos(std::numeric_limits<double>::max())
  , m_diagramYPos(std::numeric_limits<double>::max())
  , m_source2Index( -1 )
  , m_print_stream( 0 )
  , m_error_stream( 0 )
  , m_errorbox_stream( 0 )
  , m_local_variables( 0 )
  , m_used( false )
  , m_previousCycleNum(-1)
  , m_saveWepApiTransaction(-1)
  , m_isStarted( false)
  , m_gui_element(0) {
  m_timer=GuiFactory::Instance()->createTimer( 0 );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

JobFunction::~JobFunction(){
  if (m_timer) delete m_timer;
  if (m_task)  delete m_task;
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

void JobFunction::setGuiElement( GuiElement *el ) {
  m_gui_element = el;
  assert( dynamic_cast<GuiElement*>(el));
  assert(false);
}

void JobFunction::addParameter( const char *paramName ){
#ifdef HAVE_PYTHON
    // XferGenericParameter *x = XferGenericParameter::get(paramName);
    // parameterList.push_back( x );
    // m_module -> addArg( paramName );
#endif
}

/* --------------------------------------------------------------------------- */
/* printLogTitle --                                                            */
/* --------------------------------------------------------------------------- */

void JobFunction::printLogTitle( std::ostream &ostr ){
  ostr << "FUNCTION " << Name();
}

/* --------------------------------------------------------------------------- */
/* setLocalVariables --                                                        */
/* --------------------------------------------------------------------------- */

void JobFunction::setLocalVariables( DataReference *ref ){
  m_local_variables = ref;
}

/* --------------------------------------------------------------------------- */
/* attach --                                                                   */
/* --------------------------------------------------------------------------- */

int JobFunction::attach( JobCodeItem *code ){
  m_code.push_back( code );
  return (int)m_code.size() -1;
}

/* --------------------------------------------------------------------------- */
/* insert --                                                                   */
/* --------------------------------------------------------------------------- */

void JobFunction::insert( int pos, JobCodeItem *code ){
  assert( m_code[pos] == 0 );
  m_code[pos] = code;
}

/* --------------------------------------------------------------------------- */
/* nextAddress --                                                              */
/* --------------------------------------------------------------------------- */

int JobFunction::nextAddress(){
  return (int)m_code.size();
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobFunction::startJobAction(){
  BUG_DEBUG("JobFunction::startJobAction [" << Name() << "]");

  if (m_webReplyResultDataitem) {  // WebApi mode
    m_previousCycleNum = DataPoolIntens::Instance().currentCycle();
    DataPoolIntens::Instance().getCycleName(m_previousCycleNum, m_previousCycleName);
    m_saveWepApiTransaction = DataPoolIntens::Instance().CurrentTransaction();
  }
  // std::cout << "===START==== <JobFunction><startJobAction> LastUpdate["<<GuiManager::Instance().LastUpdate()
  //           <<"] CurrentTransaction["<<DataPoolIntens::Instance().CurrentTransaction()
  //           <<"] DataPoolCurrentTransaction["<<DataPool::getDatapool().GetCurrentTransaction()<<"]\n";

  // Jede Function oder Task wird über diese Funktion gestartet. Als erstes
  // werden alle notwendigen Initialisierungsarbeiten verrichtet. Das aufrufende
  // GuiElement muss sich vor dem Start anmelden, sonst läuft hier nichts.
  assert( m_engine == 0 );

  m_canceled = false;
  m_engine   = new JobEngine( this );
  BUG_DEBUG(  "Timer:  start: this = " << this);
  if( highPriority() ){
    start();
  }
  else{
    m_timer->start();
  }
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobFunction::stopJobAction(){
  BUG_DEBUG("JobFunction::stopJobAction [" << Name() << "]");

  m_timer->stop();

  if( haveNextJobAction() ){
    m_canceled = true;
    stopNextJobAction();
    BUG_DEBUG(  "stop next JobAction" );
    return;
  }

  epilog( op_Canceled );
  BUG_DEBUG(  "JobController stopped now" );
}

/* --------------------------------------------------------------------------- */
/* nextJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobFunction::nextJobAction( JobAction *action ){
  startNextJobAction( action );
}

/* --------------------------------------------------------------------------- */
/* start --                                                                    */
/* --------------------------------------------------------------------------- */

void JobFunction::start() {
  //BUG_DEBUG("JobFunction::start [" << Name() << "]" );

  if (!m_isStarted) {
    m_isStarted = true;
    execute();
  }
  else{
    executeNextOperator( op_Ok );
  }
}

/* --------------------------------------------------------------------------- */
/* backFromJobController --                                                    */
/* --------------------------------------------------------------------------- */

void JobFunction::backFromJobController( JobAction::JobResult rslt ){
  BUG_DEBUG(  "JobFunction::backFromJobController Result: " << rslt);
  OpStatus op_status = op_Ok;

  switch( rslt ){
  case job_Ok:
  case job_Ok_ignore:
    break;

  case job_Canceled:
    printMessage( "Job canceled", GuiElement::msg_Information );
    break;

  case job_Aborted:
    printMessage( "Job aborted", GuiElement::msg_Warning );
    break;

  case job_FatalError:
    m_engine->setError();
    printMessage( "Fatal error", GuiElement::msg_Error );
    op_status = op_FatalError;
    break;

  default:
    break;
  }

  // Nach jeder Rückkehr aus einer JobAction wird der Result auf
  // dem Stack abgelegt. Der darauf folgende Operator muss sich
  // gefälligst dafür interessieren.

  if( rslt != JobAction::job_Ok_ignore ){
    // siehe ticket 281
    // pushInvalid wurde am 2012-06-06 (amg) eingefuegt
    // da beim Confirmation Dialog ausser Yes(1), No(0) auch Cancel(Invalid)
    // zurueckgegeben werden soll
    if( rslt == job_Canceled )
      m_engine->pushInvalid();
    else
      m_engine->pushTrue( rslt == JobAction::job_Ok );
  }

  // Nun gehts weiter in der Verarbeitung
  executeNextOperator( op_status );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

void JobFunction::execute(){
  BUG_DEBUG(  "JobFunction::execute Stack Items: "
		 << JobStackItem::getItemCount() );
  assert( m_engine != 0 );
  assert( !m_canceled );

  OpStatus status=op_Ok;

  if( notImplemented() ){
    status = op_NotImplemented;
  }
  if( status != op_Ok ){
    epilog( status );
    return;
  }

  executeNextOperator( status );
}

/* --------------------------------------------------------------------------- */
/* executeNextOperator --                                                      */
/* --------------------------------------------------------------------------- */

void JobFunction::executeNextOperator( OpStatus op_status ){
  //BUG_DEBUG( "JobFunction::executeNextOperator Status: " << op_status);

  if( m_canceled ){
    BUG_DEBUG(  "canceled" );
    epilog( op_Canceled );
    return;
  }
  assert( m_engine != 0 );

  while( op_status != op_Aborted    &&
         op_status != op_FatalError &&
         op_status != op_EndOfJob    ){
    switch( op_status ){
    case op_Ok:
      break;

    case op_Branch:
      assert(false);
      break;

    case op_Wait:
      // Wir haben die Ausführung des Codes bereits verlassen und hoffen darauf,
      // das wir wieder aufgerufen werden. Auf ein freudiges Wiedersehen.
      return;

    case op_Interrupt:
      if( highPriority() ){
	start();
      }
      else{
	m_timer->start();
      }
      return;

    case op_Warning:
      break;

    default:
      assert(false);
    }
    op_status = m_engine->execute();
  }

  // Dies ist das Ende einer Function.
  epilog( op_status );
}

/* --------------------------------------------------------------------------- */
/* epilog --                                                                   */
/* --------------------------------------------------------------------------- */

void JobFunction::epilog( OpStatus op_status ){
  BUG_DEBUG(  "JobFunction::epilog Status=" << op_status );

  std::ostringstream message;
  JobAction::JobResult rslt = JobAction::job_Ok;
  bool webApiUpdate(false);

  switch( op_status ){
  case op_EndOfJob:
    if( m_engine->getError() ){
      op_status = op_Aborted;
      rslt = job_Aborted;
    }

    if( !m_engine->Message().empty() ){
      if( rslt == job_Aborted ){
	printMessage( m_engine->Message(), GuiElement::msg_Warning );
        BUG_DEBUG("End of FUNC " << Name() << " (ABORTED)");
      }
      else {
        printMessage( m_engine->Message(), GuiElement::msg_Information );
        BUG_DEBUG("End of FUNC " << Name());
      }
    }
    else
    if( m_engine->getError() ){
      message << "Error: Function '" << Name() << "' failed (see in Log-Window)";
      printMessage( message.str(), GuiElement::msg_Warning );
      BUG_DEBUG("End of FUNC " << Name() << " (ERROR)");
      webApiUpdate = true;
    }
    else
    if( AppData::Instance().Helpmessages() != AppData::NoneType ){
      message << "Function '" << Name() << "' completed ok";
      printMessage( message.str(), GuiElement::msg_Information );
      BUG_DEBUG("End of FUNC " << Name());
    }
    break;

  case op_Canceled:
    message << "Cancel: Function '" << Name() << "' interrupted";
    printMessage( message.str(), GuiElement::msg_Warning );
    BUG_DEBUG("End of FUNC " << Name() << " (CANCEL)");
    rslt = job_Canceled;
    m_engine->cancel();
    webApiUpdate = true;
    break;

  case op_Aborted:
    if( !m_engine->Message().empty() ){
      printMessage( m_engine->Message(), GuiElement::msg_Warning );
    }
    else{
      message << "Abort: Function '" << Name() << "' failed";
      printMessage( message.str(), GuiElement::msg_Warning );
    }
    BUG_DEBUG("End of FUNC " << Name() << " (ABORT)");
    rslt = job_Aborted;
    webApiUpdate = true;
    break;

  case op_FatalError:
    message << "FATAL ERROR: Function '" << Name() << "' failed";
    printMessage( message.str(), GuiElement::msg_Error );
    BUG_DEBUG("End of FUNC " << Name() << " (FATAL ERROR)");
    rslt = job_FatalError;
    webApiUpdate = true;
    break;

  case op_NotImplemented:
    message << "ERROR: Function '" << Name() << "' not implemented";
    printMessage( message.str(), GuiElement::msg_Error );
    BUG_DEBUG("End of FUNC " << Name() << " (NOT IMPLEMENTED)");
    rslt = job_Aborted;
    break;

  default:
    assert(false);
  }

  if (m_webReplyResultDataitem) {
    // webapi result erwuenscht
    // mqReply_datapool_func, mqReply_function_func, mqReply_control_func
    BUG_DEBUG("registerWebReplyResult, ReplyThread: " << getMessageQueueReplyThread() << ", Name: " << Name());
    JobAction::registerWebReplyResult(new WebReplyResultData(this,m_webReplyResultDataitem,
                                                             webApiUpdate,
                                                             m_saveWepApiTransaction,
                                                             rslt,
                                                             getMessageQueueReplyThread(),
                                                             m_previousCycleName,
                                                             m_previousCycleNum,
                                                             m_webReplyResultDataitemProto));
    m_previousCycleNum = -1; // reset
  } else if (getMessageQueueReplyThread()) {
    // mqReply_uimanager_func
    BUG_DEBUG("No registerWebReplyResult, ReplyThread: " << getMessageQueueReplyThread());
    getMessageQueueReplyThread()->setStatus(MessageQueueReplyThread::RECEIVE_DONE);
  }

  // if( JobStackItem::getItemCount() != 0 ){
  //   std::cerr << "WARNING: After epilog of Function " << Name() << ": "
  // 	      << JobStackItem::getItemCount() << " StackItems in memory, "
  // 	      << JobDataReference::getRefCount() << " Refs in memory and "
  // 	      << m_engine->sizeOfStack() << " StackItems in the stack" << std::endl;
  // }

  cleanup(); // Aufraeumen

  endJobAction( rslt );
  BUG_DEBUG(  "Stack Items: " << JobStackItem::getItemCount() );
}

/* --------------------------------------------------------------------------- */
/* cleanup --                                                                  */
/* --------------------------------------------------------------------------- */

void JobFunction::cleanup(){
  BUG_DEBUG( "JobFunction::cleanup" );

  m_isStarted = false;
  m_timer->stop();

  // Alle Outputstreaams werden ausgegeben
  flushPrintStream( true );
  flushErrorStream( true );

  // Eventuell vorhandene lokale Variablen werden geloescht.
  // ACHTUNG: Dies geschieht immer am Ende einer Function.
  if( m_local_variables != 0 ){
    m_local_variables->clearAllElements();
  }

  // Eventuell vorhandene Informationen über alte und neue Eingabewerte
  // werden hiermit gelöscht.
  if( m_new_dataitem != 0 ){
    delete m_new_dataitem;
    m_new_dataitem = 0;
  }
  if( m_new_this_ref != 0 ){
    delete m_new_this_ref;
    m_new_this_ref = 0;
  }
  if( m_old_data != 0 ){
    delete m_old_data;
    m_old_data = 0;
  }
  if( m_baseDataitem != 0 ){
    delete m_baseDataitem;
    m_baseDataitem = 0;
  }
  if( m_sourceDataitem != 0 ){
    delete m_sourceDataitem;
    m_sourceDataitem = 0;
  }
  if( m_sourceListDataitem.size() ){
    m_sourceListDataitem.clear();
  }
  if( m_source2Dataitem != 0 ){
    delete m_source2Dataitem;
    m_source2Dataitem = 0;
  }
  // als letztes wird die Engine eliminiert. Es wird bei jedem Start
  // eine neue JobEngine erstellt ohne Altlasten.
  if( m_engine != 0 ){
    delete m_engine;
  }
  m_engine          = 0;
  m_errorbox_stream = 0;
  m_index           = -1;
  m_diagramXPos     = std::numeric_limits<double>::max();
  m_diagramYPos     = std::numeric_limits<double>::max();
  m_sortCriteria.clear();
}

/* --------------------------------------------------------------------------- */
/* PrintStream --                                                              */
/* --------------------------------------------------------------------------- */

std::ostream &JobFunction::PrintStream(){
  if( m_print_stream == 0 ){
    m_print_stream = new std::ostringstream();
    *m_print_stream << std::setprecision(17);
  }
  return *m_print_stream;
}

/* --------------------------------------------------------------------------- */
/* flushPrintStream --                                                         */
/* --------------------------------------------------------------------------- */

void JobFunction::flushPrintStream( bool final ){
  BUG_DEBUG(  "JobFunction::flushPrintStream");
  if( final ){
    if( m_print_stream == 0 ) return;
    if( m_print_stream->str().empty() ) return;
  }
  if( m_print_stream == 0 ){
    PrintStream();
  }
  BUG_DEBUG(   m_print_stream->str() );
  *m_print_stream << std::endl;
  GuiFactory::Instance()->getLogWindow()->writeText( "Print> " + m_print_stream->str());
  //  GuiFactory::Instance()->getLogWindow()->writeText( m_print_stream->str() );
  delete m_print_stream;
  m_print_stream = 0;
}

/* --------------------------------------------------------------------------- */
/* ErrorStream --                                                              */
/* --------------------------------------------------------------------------- */

std::ostream &JobFunction::ErrorStream(){
  if( m_error_stream == 0 ){
    m_error_stream = new std::ostringstream();
  }
  return *m_error_stream;
}

/* --------------------------------------------------------------------------- */
/* flushErrorStream --                                                         */
/* --------------------------------------------------------------------------- */

void JobFunction::flushErrorStream( bool final ){
  BUG_DEBUG(  "JobFunction::flushErrorStream");
  if( final ){
    if( m_error_stream == 0 ) return;
    if( m_error_stream->str().empty() ) return;
  }
  if( m_error_stream ){
    ErrorStream();
  }
  BUG_DEBUG(  m_error_stream->str() );
  *m_error_stream << std::endl;
  if( m_errorbox_stream != 0 ){
    *m_errorbox_stream << m_error_stream->str();
  }
  GuiFactory::Instance()->getLogWindow()->writeText( "Error> " );
  GuiFactory::Instance()->getLogWindow()->writeText( m_error_stream->str() );
  delete m_error_stream;
  m_error_stream = 0;
}

/* --------------------------------------------------------------------------- */
/* LogStream --                                                                */
/* --------------------------------------------------------------------------- */

std::ostringstream& JobFunction::LogStream(){
  return m_log_stream;
}

/* --------------------------------------------------------------------------- */
/* clearLogStream --                                                           */
/* --------------------------------------------------------------------------- */

void JobFunction::clearLogStream(){
  m_log_stream.str("");
}

/* --------------------------------------------------------------------------- */
/* setData --                                                                  */
/* --------------------------------------------------------------------------- */

void JobFunction::setData( XferDataItem *new_data, JobStackData *old_data ){
  assert( m_new_dataitem == 0 );
  assert( m_old_data == 0 );
  m_new_dataitem = new_data;
  m_old_data     = old_data;

  m_index = -1;
  if( new_data != 0 ){
    getIndex( m_index, -1, 0 );
  }
}

/* --------------------------------------------------------------------------- */
/* replaceData --                                                              */
/* --------------------------------------------------------------------------- */

void JobFunction::replaceData( DataReference *new_data, JobStackData *old_data ){
  if ( m_new_dataitem )
    delete m_new_dataitem;
  if (m_new_this_ref) {
    delete m_new_this_ref;
    m_new_this_ref = 0;
  }
  m_new_dataitem = new XferDataItem( new_data );
  if (old_data)
    m_old_data = old_data;


  m_index = -1;
  if( new_data != 0 ){
    getIndex( m_index, -1, 0 );
  }
}

/* --------------------------------------------------------------------------- */
/* setBaseDataItem --                                                          */
/* --------------------------------------------------------------------------- */

void JobFunction::setBaseDataItem( XferDataItem *baseDataitem ){
  assert( m_baseDataitem == 0 );
  m_baseDataitem = baseDataitem;
}

/* --------------------------------------------------------------------------- */
/* setSourceDataItem --                                                        */
/* --------------------------------------------------------------------------- */

void JobFunction::setSourceDataItem( XferDataItem *sourceDataitem ){
  assert( m_sourceDataitem == 0 );
  m_sourceDataitem = sourceDataitem;
}

/* --------------------------------------------------------------------------- */
/* setSourceDataItem --                                                        */
/* --------------------------------------------------------------------------- */

void JobFunction::setSourceDataItem( const std::vector<XferDataItem*>& sourceListDataitem) {
  m_sourceListDataitem = sourceListDataitem;
}

/* --------------------------------------------------------------------------- */
/* setSource2Data --                                                            */
/* --------------------------------------------------------------------------- */

void JobFunction::setSource2DataItem( XferDataItem *source2Dataitem ){
  assert( m_source2Dataitem == 0 );
  m_source2Dataitem = source2Dataitem;

  m_source2Index = -1;
  if( source2Dataitem != 0 ){
    getSource2Index( m_source2Index, -1, 0 );
  }
}

/* --------------------------------------------------------------------------- */
/* setWebReplyResultDataItem --                                                     */
/* --------------------------------------------------------------------------- */

void JobFunction::setWebReplyResultDataItem( XferDataItem *webReplyResultDataitem ){
  assert( m_webReplyResultDataitem == 0 );
  m_webReplyResultDataitem = webReplyResultDataitem;
  JobAction::setWebReplyResultDataItem(webReplyResultDataitem);
}

/* --------------------------------------------------------------------------- */
/* setWebReplyResultDataItemProto --                                                     */
/* --------------------------------------------------------------------------- */

void JobFunction::setWebReplyResultDataItemProto(){
  m_webReplyResultDataitemProto = true;
  JobAction::setWebReplyResultDataItemProto();
}

/* --------------------------------------------------------------------------- */
/* getOldData --                                                               */
/* --------------------------------------------------------------------------- */

JobStackData *JobFunction::getOldData(){
  BUG_DEBUG(  "JobFunction::getOldData");
  if( m_old_data == 0 ){
      BUG_DEBUG(  "No Input available");
      return new JobStackDataInteger();
  }
  BUG_DEBUG(  "ok");
  return m_old_data->clone();
}

/* --------------------------------------------------------------------------- */
/* setOldData --                                                               */
/* --------------------------------------------------------------------------- */

void JobFunction:: setOldData(JobStackData* data){
  if (data != m_old_data) delete m_old_data;
  m_old_data = data;
}

/* --------------------------------------------------------------------------- */
/* getNewData --                                                               */
/* --------------------------------------------------------------------------- */

JobStackData *JobFunction::getNewData(){
  BUG_DEBUG(  "JobFunction::getNewData");
  if( m_new_dataitem == 0 ){
      BUG_DEBUG( "No Input available");
    return new JobStackDataInteger();
  }
  BUG_DEBUG(  "ok" );
  return new JobStackDataInput( *m_new_dataitem );
}

/* --------------------------------------------------------------------------- */
/* getNewDataReference --                                                      */
/* --------------------------------------------------------------------------- */

DataReference *JobFunction::getNewDataReference(){
  BUG_DEBUG( "JobFunction::getNewDataReference");
  if( m_new_dataitem == 0 ){
    BUG_DEBUG(  "No Input available");
    return DataPoolIntens::getDataPool().newDataReference();
  }
  BUG_DEBUG(  "ok");
  return DataPool::newDataReference( *m_new_dataitem->Data() );
}

/* --------------------------------------------------------------------------- */
/* getBaseDataReference --                                                     */
/* --------------------------------------------------------------------------- */

DataReference *JobFunction::getBaseDataReference(){
  BUG_DEBUG( "JobFunction::getBaseDataReference");
  if( m_baseDataitem == 0 ){
    BUG_DEBUG(  "No Base available");
    return DataPoolIntens::getDataPool().newDataReference();
  }
  BUG_DEBUG(  "ok");
  return DataPool::newDataReference( *m_baseDataitem->Data() );
}

/* --------------------------------------------------------------------------- */
/* getSourceDataReference --                                                   */
/* --------------------------------------------------------------------------- */

DataReference *JobFunction::getSourceDataReference(){
  BUG_DEBUG( "JobFunction::getSourceDataReference");
  if( m_sourceDataitem == 0 ){
    BUG_DEBUG(  "No Source available");
    return DataPoolIntens::getDataPool().newDataReference();
  }
  BUG_DEBUG(  "ok");
  return DataPool::newDataReference( *m_sourceDataitem->Data() );
}

/* --------------------------------------------------------------------------- */
/* getSourceListDataReference --                                               */
/* --------------------------------------------------------------------------- */

std::vector<DataReference*> JobFunction::getSourceListDataReference(){
  BUG_DEBUG( "JobFunction::getSourceListDataReference");
  std::vector<DataReference*> retList;
  if( m_sourceListDataitem.size() == 0 ){
    BUG_DEBUG(  "No Source List available");
    return retList;
  }
  BUG_DEBUG(  "ok");
  for (std::vector<XferDataItem*>::iterator it = m_sourceListDataitem.begin();
       it != m_sourceListDataitem.end(); ++it )
    retList.push_back( DataPool::newDataReference( *(*it)->Data() ) );
  return retList;
}

/* --------------------------------------------------------------------------- */
/* getSource2DataReference --                                                  */
/* --------------------------------------------------------------------------- */

DataReference *JobFunction::getSource2DataReference(){
  BUG_DEBUG( "JobFunction::getSource2DataReference");
  if( m_source2Dataitem == 0 ){
    BUG_DEBUG(  "No Source2 available");
    return DataPoolIntens::getDataPool().newDataReference();
  }
  BUG_DEBUG(  "ok");
  return DataPool::newDataReference( *m_source2Dataitem->Data() );
}

/* --------------------------------------------------------------------------- */
/* getNewThisReference --                                                      */
/* --------------------------------------------------------------------------- */

DataReference *JobFunction::getNewThisReference(){
  BUG_DEBUG( "JobFunction::getNewThisReference");

  if( m_new_this_ref != 0 ){
    return DataPool::newDataReference( *m_new_this_ref );
  }
  m_new_this_ref = getNewDataReference();
  if( m_new_this_ref->RefValid() ){
    BUG_DEBUG("DataType of " << m_new_this_ref->fullName(true) <<
              " is " << m_new_this_ref->getDataType());
    if( m_new_this_ref->getDataType() != DataDictionary::type_StructVariable ){
      if( !(m_new_this_ref->ConvertToParentReference() ) ){
        std::string msg("WARNING: Invalid THIS, cannot convert to parent reference : "
                        + m_new_this_ref->fullName( true ));
        GuiFactory::Instance()->getLogWindow()->writeText(msg);
        GuiFactory::Instance()->getLogWindow()->writeText(compose("WARNING: Occurred inside Function '%1'",
                                                                  Name()));
        BUG_DEBUG( msg );
        return 0;
      }
      else
        BUG_DEBUG( "DataType of " << m_new_this_ref->fullName(true) <<
                   " is " << m_new_this_ref->getDataType() );
    }
  }
  return DataPool::newDataReference( *m_new_this_ref );
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobFunction::getIndex( int &value, int level, int index ){
  if( m_new_dataitem == 0 ){
    return false;
  }
  if( m_new_dataitem->getNumberOfLevels() > level ){
    value = m_new_dataitem->getIndex( level, index );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getSourceIndex --                                                           */
/* --------------------------------------------------------------------------- */

bool JobFunction::getSourceIndex( int &value, int level, int index ){
  if( m_sourceDataitem == 0 ){
    return false;
  }
  if( m_sourceDataitem->getNumberOfLevels() > level ){
    value = m_sourceDataitem->getIndex( level, index );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getSource2Index --                                                           */
/* --------------------------------------------------------------------------- */

bool JobFunction::getSource2Index( int &value, int level, int index ){
  if( m_source2Dataitem == 0 ){
    return false;
  }
  if( m_source2Dataitem->getNumberOfLevels() > level ){
    value = m_source2Dataitem->getIndex( level, index );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobFunction::getIndex( int &value ){
  if( m_index < 0 ){
    return false;
  }
  value = m_index;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void JobFunction::setIndex( int index ){
  m_index = index;
}

/* --------------------------------------------------------------------------- */
/* getDiagramXPos--                                                            */
/* --------------------------------------------------------------------------- */

bool JobFunction::getDiagramXPos( double &value ){
  if( m_diagramXPos == std::numeric_limits<double>::max()){
    return false;
  }
  value = m_diagramXPos;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setDiagramXPos --                                                           */
/* --------------------------------------------------------------------------- */

void JobFunction::setDiagramXPos( double p ){
  m_diagramXPos = p;
}

/* --------------------------------------------------------------------------- */
/* getDiagramYPos--                                                            */
/* --------------------------------------------------------------------------- */

bool JobFunction::getDiagramYPos( double &value ){
  if( m_diagramYPos == std::numeric_limits<double>::max()){
    return false;
  }
  value = m_diagramYPos;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setDiagramYPos --                                                           */
/* --------------------------------------------------------------------------- */

void JobFunction::setDiagramYPos( double p ){
  m_diagramYPos = p;
}

/* --------------------------------------------------------------------------- */
/* getSortCriteria--                                                           */
/* --------------------------------------------------------------------------- */

bool JobFunction::getSortCriteria( std::string &sortCriteria ){
  if(m_sortCriteria.empty()){
    return false;
  }
  sortCriteria = m_sortCriteria;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setSortCriteria--                                                           */
/* --------------------------------------------------------------------------- */

void JobFunction::setSortCriteria(const std::string &sortCriteria){
  m_sortCriteria = sortCriteria;
}

/* --------------------------------------------------------------------------- */
/* reasonIsEqual --                                                            */
/* --------------------------------------------------------------------------- */

bool JobFunction::reasonIsEqual( CallReason reason ) {
  assert( reason > 0 && reason < cll_CallReasonEntries );
  return m_reason == reason;
}

/* --------------------------------------------------------------------------- */
/* createNavigatorButtonListener --                                            */
/* --------------------------------------------------------------------------- */

GuiButtonListener *JobFunction::createNavigatorButtonListener( GuiElement *menu ){
  return new NavigatorRunFuncListener( menu, this );
}

/* --------------------------------------------------------------------------- */
/* createButtonListener --                                                     */
/* --------------------------------------------------------------------------- */

GuiButtonListener *JobFunction::createButtonListener(){
  return new RunFuncListener( this );
}

/* --------------------------------------------------------------------------- */
/* serialize --                                                                */
/* --------------------------------------------------------------------------- */

void JobFunction::serializeXML(std::ostream &os, bool recursive){
  os << "<function name=\"" << Name() << "\" ";
  os << " line=\"" << Lineno() << "\"";
  auto filename = Filename();
  std::replace(filename.begin(), filename.end(), '"', '\'');
  os << " file=\"" << filename << "\"";
  os << ">" << std::endl;
  if( recursive ){
    JobCodeStream::iterator it;
    for( it = m_code.begin(); it != m_code.end(); ++it ){
      (*it)->serializeXML(os, recursive);
    }
  }
  os << "</function>" <<  std::endl;
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void RunFuncListener::ButtonPressed(){
  JobStarter *starter = new Trigger( m_function );

  starter->setReason( JobElement::cll_Function );
  //  starter->setGuiElement( m_button );
  starter->startJob();
  // Der JobStarter zerstört sich am Ende selbständig.
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void NavigatorRunFuncListener::ButtonPressed(){
  assert( m_parent != 0 );
  GuiElement *el = m_parent->getElement()->getParent();
  assert( el != 0 );
  GuiNavigator *nav = el->getNavigator();
  assert( nav != 0 );
  if( !nav->setRunningMode() ) assert( false );

  XferDataItem *input = dynamic_cast<GuiNavigatorMenu*>(m_parent)->GuiNavigatorMenu::getXfer();
  JobStarter *trigger = nav->getNewTrigger( m_function );
  trigger->setReason( JobElement::cll_Function );
  if( input != 0 ){
    trigger->setDataItem( input );
  }
  GuiNavigatorMenu* navMenu = dynamic_cast<GuiNavigatorMenu*>(m_parent);
  if (navMenu) {
    trigger->setDiagramXPos( navMenu->getXPosition() );
    trigger->setDiagramYPos( navMenu->getYPosition() );
    // used by an drop menu function
    if (navMenu->getXferSourceList().size() > 0) {
      trigger->setSource( navMenu->getXferSourceList() );
      trigger->setReason( JobElement::cll_Drop );
    }
    if (navMenu->getXferSource()) {
      trigger->setSource( navMenu->getXferSource() );
      trigger->setReason( JobElement::cll_Drop );
    }
  }
  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // doEndOfWork() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // löscht sich am Ende selbständig.
  // Da wir kein GuiElement sind, überlassen wir diese Arbeit dem Navigator
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */

void Trigger::backFromJobStarter( JobAction::JobResult rslt ){
  // Wird vielleicht später benötigt
}
