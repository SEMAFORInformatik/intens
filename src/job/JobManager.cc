#include <string>

#include "job/JobIncludes.h"

#include "job/JobManager.h"
#include "job/JobFunction.h"
#include "job/JobCodeClearTextWindow.h"
#include "job/JobTask.h"
#include "job/JobStackItem.h"
#include "job/JobCodeExecute.h"
#include "job/JobController.h"
#include "job/JobClientSocket.h"
#include "job/JobRequest.h"
#include "job/JobSubscribe.h"
#include "job/JobMsgQueuePublish.h"
#include "job/JobCodeRestService.h"
#include "job/JobCodeTimer.h"
#include "job/JobCodeFile.h"

#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiPulldownMenu.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiIndex.h"
#include "operator/ProcessGroup.h"
#include "operator/ClientSocket.h"
#include "operator/MessageQueueRequest.h"
#include "operator/MessageQueuePublisher.h"
#include "app/Plugin.h"
#include "parser/IdManager.h"

JobManager *JobManager::s_instance = 0;
int JobStackItem::s_item_count = 0;


/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

JobManager &JobManager::Instance(){
  if( s_instance == 0 ){
    s_instance = new JobManager();
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* printMessage --                                                             */
/* --------------------------------------------------------------------------- */

void JobManager::printMessage( const std::string &txt
                             , GuiElement::MessageType type
                             , time_t delay ){

  // flick it !!! m_job_gui_element zeigt manchmal ins Nirvana....
  GuiElement::printMessage( 0/*m_job_gui_element*/, txt, type, delay );
}

/* --------------------------------------------------------------------------- */
/* getInitialAction --                                                         */
/* --------------------------------------------------------------------------- */

JobAction *JobManager::getInitialAction( const std::string &name ){
  JobFunction *f = getFunction( name );
  if( f != 0 ){
    return f;
  }
  f = getTask( name );
  if( f != 0 ){
    return f;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setRunningMode --                                                           */
/* --------------------------------------------------------------------------- */

int JobManager::setRunningMode( int key ){
#if defined(RUNNING_MODE_CONTROL)
  if( key != 0 ){ // already locked ??
    return m_job_running_key == key ? key : 0;
  }
  if( m_job_running_key != 0 ){
    return 0;
  }
  m_job_running_key = ++m_job_current_key;
  return m_job_running_key;
#else
  if( key == 0 ){
    return ++m_job_current_key;
  }
  return key;
#endif
}

/* --------------------------------------------------------------------------- */
/* unsetRunningMode --                                                         */
/* --------------------------------------------------------------------------- */

void JobManager::unsetRunningMode( int key ){
#if defined(RUNNING_MODE_CONTROL)
  assert( m_job_running_key == key );
  m_job_running_key = 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* isRunning --                                                                */
/* --------------------------------------------------------------------------- */

bool JobManager::isRunning(){
  return m_job_running_key != 0;
}

/* --------------------------------------------------------------------------- */
/* newFunction --                                                              */
/* --------------------------------------------------------------------------- */

JobFunction *JobManager::newFunction( const std::string &name ){
  JobFunctionList::iterator f = m_function_list.find( name );
  if( f != m_function_list.end() ){
    if( (*f).second->notImplemented() && !(*f).second->isTask() ){
      return (*f).second;
    }
    return 0;
  }
  JobFunction *func = new JobFunction( name );
  func->setSilent();
  m_function_list.insert( JobFunctionList::value_type( name, func ) );
  return func;
}

/* --------------------------------------------------------------------------- */
/* getFunction --                                                              */
/* --------------------------------------------------------------------------- */

JobFunction *JobManager::getFunction( const std::string &name, bool create ){
  JobFunctionList::iterator f = m_function_list.find( name );
  if( f != m_function_list.end() ){
    if( !(*f).second->isTask() ){
      return (*f).second;
    }
    return 0;
  }

  if ( create ) {
    if( !IdManager::Instance().registerId( name, IdManager::id_Function ) ){
      return 0; // should never happen
    }
    return JobManager::Instance().newFunction( name );
  }

  return 0;
}

/* --------------------------------------------------------------------------- */
/* printUnimplementedFunctions --                                              */
/* --------------------------------------------------------------------------- */

bool JobManager::printUnimplementedFunctions( std::ostream &ostr ){
  bool unimplementedFunctions = false;

  for ( JobFunctionList::iterator f = m_function_list.begin( );
        f != m_function_list.end();
        ++f ) {
    if ( (*f).second->notImplemented() ) { // && !(*f).second->isTask()
      unimplementedFunctions = true;
      ostr << "Warning: " << (*f).first << " not implemented!" << std::endl;
    }
  }

  return unimplementedFunctions;
}

/* --------------------------------------------------------------------------- */
/* newTask --                                                                  */
/* --------------------------------------------------------------------------- */

JobTask *JobManager::newTask( const std::string &name ){
  JobFunctionList::iterator f = m_function_list.find( name );
  if( f != m_function_list.end() ){
    if( (*f).second->notImplemented() && (*f).second->isTask() ){
      return (JobTask *)((*f).second);
    }
    return 0;
  }
  JobTask *func = new JobTask( name );
  func->setLabel( name );
  m_function_list.insert( JobFunctionList::value_type( name, func ) );
  return func;
}

/* --------------------------------------------------------------------------- */
/* getTask --                                                                  */
/* --------------------------------------------------------------------------- */

JobTask *JobManager::getTask( const std::string &name ){
  JobFunctionList::iterator f = m_function_list.find( name );
  if( f != m_function_list.end() ){
    if( (*f).second->isTask() ){
      return (JobTask *)((*f).second);
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createTaskMenu --                                                           */
/* --------------------------------------------------------------------------- */

void JobManager::createTaskMenu( GuiPulldownMenu *menu ){
  JobTask *task = 0;
  JobFunctionList::iterator f;
  for( f=m_function_list.begin(); f != m_function_list.end(); ++f ){
    if( (*f).second->isTask() ){
      task = static_cast<JobTask *>((*f).second);
      if( !task->MenuInstalled() ){
        GuiButtonListener *listener = task->getButtonListener();
        GuiMenuButton *pushbutton = GuiFactory::Instance()->createMenuButton( menu->getElement(), listener );
        menu->attach( pushbutton->getElement() );
        if( task->getLabel().empty() )
          pushbutton->setLabel( (*f).first );
        else
          pushbutton->setLabel( task->getLabel() );
        task->setMenuInstalled();
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* pushDataRef --                                                              */
/* --------------------------------------------------------------------------- */

void JobManager::pushDataRef( JobDataReference *data, int level ){
  m_dataref_stack.push( JobDataRefStack::value_type( data, level ) );
}

/* --------------------------------------------------------------------------- */
/* popDataRef --                                                               */
/* --------------------------------------------------------------------------- */

JobDataReference *JobManager::popDataRef( int &level ){
  if( m_dataref_stack.size() == 0 ){
    return 0;
  }
  level = m_dataref_stack.top().second;
  JobDataReference *data = m_dataref_stack.top().first;
  m_dataref_stack.pop();
  return data;
}

/* --------------------------------------------------------------------------- */
/* opPushVariable --                                                           */
/* --------------------------------------------------------------------------- */

int JobManager::opPushVariable( JobFunction *func, JobDataReference *data ){
  JobCodePushVariable *code = new JobCodePushVariable( data );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSetThis --                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opSetThis( JobFunction *func, JobDataReference *data ){
  JobCodeSetThis *code = new JobCodeSetThis( data );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSetBase --                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opSetBase( JobFunction *func, JobDataReference *data ){
  JobCodeSetBase *code = new JobCodeSetBase( data );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opAssignThis --                                                             */
/* --------------------------------------------------------------------------- */

int JobManager::opAssignThis( JobFunction *func/*, JobDataReference *data*/ ){
  JobCodeAssignThis *code = new JobCodeAssignThis( /*data*/ );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opAssignFuncIndex --                                                        */
/* --------------------------------------------------------------------------- */

int JobManager::opAssignFuncIndex( JobFunction *func ){
  JobCodeAssignFuncIndex *code = new JobCodeAssignFuncIndex();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opAssignFuncReason --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opAssignFuncReason( JobFunction *func ){
  JobCodeAssignFuncReason *code = new JobCodeAssignFuncReason();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opAssignFuncArgs --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opAssignFuncArgs( JobFunction *func ){
  JobCodeAssignFuncArguments *code = new JobCodeAssignFuncArguments();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSetSource --                                                              */
/* --------------------------------------------------------------------------- */

int JobManager::opSetSource( JobFunction *func, JobDataReference *data, JobDataReference *data_idx ){
  JobCodeSetSource *code = new JobCodeSetSource( data, data_idx );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSetSource2 --                                                             */
/* --------------------------------------------------------------------------- */

int JobManager::opSetSource2( JobFunction *func, JobDataReference *data ){
  JobCodeSetSource2 *code = new JobCodeSetSource2( data );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPushReal --                                                               */
/* --------------------------------------------------------------------------- */

int JobManager::opPushReal( JobFunction *func, double val ){
  JobCodePushReal *code = new JobCodePushReal( val );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPushInteger --                                                            */
/* --------------------------------------------------------------------------- */

int JobManager::opPushInteger( JobFunction *func, int val ){
  JobCodePushInteger *code = new JobCodePushInteger( val );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPushSortOrder --                                                            */
/* --------------------------------------------------------------------------- */

int JobManager::opPushSortOrder( JobFunction *func, const std::string &listId ){
  JobCodePushSortOrder *code = new JobCodePushSortOrder( listId );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPushString --                                                             */
/* --------------------------------------------------------------------------- */

int JobManager::opPushString( JobFunction *func, const std::string &val ){
  JobCodePushString *code = new JobCodePushString( val );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPushEndOfLine --                                                             */
/* --------------------------------------------------------------------------- */

int JobManager::opPushEndOfLine( JobFunction *func ){
  JobCodePushEndOfLine *code = new JobCodePushEndOfLine();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPushReason --                                                             */
/* --------------------------------------------------------------------------- */

int JobManager::opPushReason( JobFunction *func, CallReason reason ){
  switch( reason ){
  case cll_Input:
    return func->attach( &m_op_reason_input );
  case cll_Insert:
    return func->attach( &m_op_reason_insert );
  case cll_Dupl:
    return func->attach( &m_op_reason_duplicate );
  case cll_Clear:
    return func->attach( &m_op_reason_clear );
  case cll_Delete:
    return func->attach( &m_op_reason_delete );
  case cll_Pack:
    return func->attach( &m_op_reason_pack );
  case cll_Select:
    return func->attach( &m_op_reason_select );
  case cll_Unselect:
    return func->attach( &m_op_reason_unselect );
  case cll_SelectPoint:
    return func->attach( &m_op_reason_select_point );
  case cll_SelectRectangle:
    return func->attach( &m_op_reason_select_rectangle );
  case cll_Sort:
    return func->attach( &m_op_reason_sort );
  case cll_Activate:
    return func->attach( &m_op_reason_activate );
  case cll_Open:
    return func->attach( &m_op_reason_open );
  case cll_Close:
    return func->attach( &m_op_reason_close );
  case cll_Drop:
    return func->attach( &m_op_reason_drop );
  case cll_Move:
    return func->attach( &m_op_reason_move );
  case cll_NewConnection:
    return func->attach( &m_op_reason_newConnection );
  case cll_RemoveConnection:
    return func->attach( &m_op_reason_removeConnection );
  case cll_RemoveElement:
    return func->attach( &m_op_reason_removeElement );
  case cll_CycleClear:
    return func->attach( &m_op_reason_cycleClear );
  case cll_CycleDelete:
    return func->attach( &m_op_reason_cycleDelete );
  case cll_CycleNew:
    return func->attach( &m_op_reason_cycleNew );
  case cll_CycleRename:
    return func->attach( &m_op_reason_cycleRename );
  case cll_CycleSwitch:
    return func->attach( &m_op_reason_cycleSwitch );
  case cll_FocusIn:
    return func->attach( &m_op_reason_focusIn);
  case cll_FocusOut:
    return func->attach( &m_op_reason_focusOut );
  case cll_Function:
    return func->attach( &m_op_reason_function );
  case cll_Task:
    return func->attach( &m_op_reason_task );
  case cll_GuiUpdate:
    return func->attach( &m_op_reason_guiUpdate );
  default:
    assert( func == 0 ); // desaster
  }

  return opPushInteger( func, 0 );
}

/* --------------------------------------------------------------------------- */
/* opBranch --                                                                 */
/* --------------------------------------------------------------------------- */

int JobManager::opBranch( JobFunction *func, int addr ){
  JobCodeBranchWithAddr *code = new JobCodeBranchWithAddr( addr );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opIfOperator --                                                             */
/* --------------------------------------------------------------------------- */

JobCodeOpIf *JobManager::opIfOperator( JobFunction *func ){
  JobCodeOpIf *code = new JobCodeOpIf();
  func->attach( code );
  return code;
}

/* --------------------------------------------------------------------------- */
/* opWhileOperator --                                                          */
/* --------------------------------------------------------------------------- */

JobCodeOpWhile *JobManager::opWhileOperator( JobFunction *func ){
  JobCodeOpWhile *code = new JobCodeOpWhile();
  func->attach( code );
  return code;
}

/* --------------------------------------------------------------------------- */
/* opConfirm --                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opConfirm( JobFunction *func, bool cancelBtn,
						   const std::map<GuiElement::ButtonType, std::string>& buttonText ) {
  JobCodeConfirm *code = new JobCodeConfirm( cancelBtn, buttonText );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opGetText --                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opGetText( JobFunction *func ) {
  JobCodeGetText *code = new JobCodeGetText();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opOpenFile -                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opOpenFile( JobFunction *func, FileStream *filesteam,
							bool readStack, const std::string &filename) {

  JobCodeOpenFile *code = new JobCodeOpenFile( filesteam, readStack, filename );

  return func->attach( code );

}

/* --------------------------------------------------------------------------- */
/* opSaveFile -                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opSaveFile( JobFunction *func, FileStream *filesteam, GuiElement* guiElem,
                            bool readStack, const std::string &filename,
                            bool readBaseStack, const std::string &base_filename) {

  JobCodeSaveFile *code = new JobCodeSaveFile( filesteam, guiElem, readStack, filename,
                                               readBaseStack, base_filename );

  return func->attach( code );

}

/* --------------------------------------------------------------------------- */
/* opFileDialog --                                                                   */
/* --------------------------------------------------------------------------- */

int JobManager::opFileDialog( JobFunction *func
                              , const std::string &dirname
                              , const std::string &filtername
                              , bool dironly
                              , bool open
                              , XferDataItem *dirnamexfer ) {

  JobCodeFileDialog *code = new JobCodeFileDialog( dirname, filtername, dironly, open, dirnamexfer );

  return func->attach( code );

}

/* --------------------------------------------------------------------------- */
/* opSend --                                                                   */
/* --------------------------------------------------------------------------- */

int JobManager::opSend( JobFunction *func
                        , const std::string &host
                        , XferDataItem *hostxfer
                        , int port
                        , XferDataItem *portxfer
                        , const std::string & header
                        , Stream *out_stream
                        , Stream *in_stream ){

  ClientSocket *socket = GuiFactory::Instance()->createClientSocket( header
                                                                     , out_stream
                                                                     , in_stream );
  if( hostxfer ){
    socket->setHost( hostxfer );
  }
  else{
    socket->setHost( host );
  }
  if( portxfer ){
    socket->setPort( portxfer );
  }
  else{
    socket->setPort( port );
  }

  JobCodeSend *code = new JobCodeSend( new JobClientSocket( socket ) );

  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opRequest --                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opRequest( JobFunction *func
                           , Plugin *requestObj
                           , const std::vector<Stream*>& out_streams
                           , const std::vector<Stream*>& in_streams
                           , const std::string& header
                           , const int timeout ){

  JobCodeRequest *code = new JobCodeRequest( new JobRequest( requestObj,
                                                             out_streams,
                                                             in_streams,
                                                             header,
                                                             timeout) );
  return func->attach( code );
}

int JobManager::opRequest( JobFunction *func
                           , MessageQueueRequest *requestObj
                           , const std::vector<Stream*>& out_streams
                           , const std::vector<Stream*>& in_streams
                           , const std::string& header
                           , const int timeout ){

  JobCodeRequest *code = new JobCodeRequest( new JobRequest( requestObj,
                                                             out_streams,
                                                             in_streams,
                                                             header,
                                                             timeout) );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPublish --                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opPublish( JobFunction *func
                          , MessageQueuePublisher *msgPublisher
                          , const std::vector<Stream*>& out_streams
                          , const std::string& header){

  JobCodePublish *code = new JobCodePublish( new JobMsgQueuePublish( msgPublisher,
                                                                    out_streams,
                                                                    header) );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSubscribe --                                                              */
/* --------------------------------------------------------------------------- */

int JobManager::opSubscribe( JobFunction *func
                             , SubscribeInterface *subscribeObj
                             , const std::vector<Stream*>& out_streams
                             , const std::string& header
                             , JobFunction *subsFunc ){

  JobCodeSubscribe *code = new JobCodeSubscribe( new JobSubscribe(subscribeObj,
                                                                  out_streams,
                                                                  header,
                                                                  subsFunc) );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSetMessageQueueHost --                                                    */
/* --------------------------------------------------------------------------- */

int JobManager::opSetMessageQueueHost( JobFunction *func ){
  JobCodeSetMessageQueueHost *code = new JobCodeSetMessageQueueHost();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opRestServiceGet --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opRestServiceGet( JobFunction *func,
                                  const std::string &path,
                                  XferDataItem *pathXfer,
                                  Stream *pathStream,
                                  Stream *filterStream,
                                  Stream *responseStream ){

  JobCodeRestServiceGet *code
    = new JobCodeRestServiceGet( path, pathXfer, pathStream,
                                 filterStream, responseStream );
  return func->attach( code );
}
/* --------------------------------------------------------------------------- */
/* opRestServiceDelete --                                                      */
/* --------------------------------------------------------------------------- */

int JobManager::opRestServiceDelete( JobFunction *func,
                                  const std::string &path,
                                  XferDataItem *pathXfer,
                                  Stream *pathStream,
                                  Stream *filterStream,
                                  Stream *responseStream ){

  JobCodeRestServiceDelete *code
    = new JobCodeRestServiceDelete( path, pathXfer, pathStream,
                                 filterStream, responseStream );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opRestServicePut --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opRestServicePut( JobFunction *func,
                                  const std::string &path,
                                  XferDataItem *pathXfer,
                                  Stream *pathStream,
                                  Stream *dataStream,
                                  Stream *responseStream,
                                  bool setDbTimestamp ){

  JobCodeRestServicePut *code
    = new JobCodeRestServicePut( path, pathXfer, pathStream,
                                 dataStream, responseStream,
                                 setDbTimestamp );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opRestServicePost --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opRestServicePost( JobFunction *func,
                                   const std::string &path,
                                   XferDataItem *pathXfer,
                                   Stream *pathStream,
                                   Stream *filterStream,
                                   Stream *dataStream,
                                   Stream *responseStream,
                                   bool setDbTimestamp ){

  JobCodeRestServicePost *code
    = new JobCodeRestServicePost( path, pathXfer, pathStream,
                                  filterStream, dataStream,
                                  responseStream, setDbTimestamp );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opRestServiceLogon --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opRestServiceLogon( JobFunction *func,
                                    XferDataItem *baseUrl,
                                    XferDataItem *username,
                                    XferDataItem *password,
                                    XferDataItem *message,
                                    Stream *responseStream ){

  JobCodeRestServiceLogon *code = new JobCodeRestServiceLogon(baseUrl, username, password, message, responseStream);
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opRestServiceJwtLogon --                                                    */
/* --------------------------------------------------------------------------- */

int JobManager::opRestServiceJwtLogon( JobFunction *func,
				       XferDataItem *baseUrl,
				       XferDataItem *jwt,
				       XferDataItem *message,
				       Stream *responseStream ){

  JobCodeRestServiceJwtLogon *code = new JobCodeRestServiceJwtLogon(baseUrl, jwt, message, responseStream);
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opRestServiceLogoff --                                                      */
/* --------------------------------------------------------------------------- */

int JobManager::opRestServiceLogoff( JobFunction *func ){

  JobCodeRestServiceLogoff *code = new JobCodeRestServiceLogoff();
  return func->attach( code );
}
/* --------------------------------------------------------------------------- */
/* opTimerStart --                                                             */
/* --------------------------------------------------------------------------- */

int JobManager::opTimerStart( JobFunction *func
              , TimerFunction * timer
                              , double period
                              , XferDataItem *periodXfer
                              , double delay
                              , XferDataItem *delayXfer){
  JobCodeTimer* timer_func = new JobCodeTimer( timer, true,
                                               period, periodXfer,
                                               delay, delayXfer);
  return func->attach( timer_func );
}

/* --------------------------------------------------------------------------- */
/* opTimerStop --                                                              */
/* --------------------------------------------------------------------------- */

int JobManager::opTimerStop( JobFunction *func, TimerFunction *timer){
  JobCodeTimer* timer_func = new JobCodeTimer( timer, false, 0, NULL, 0, NULL );
  return func->attach( timer_func );
}

/* --------------------------------------------------------------------------- */
/* opExecuteJobAction --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opExecuteJobAction( JobFunction *func, JobAction *action ){
  JobCodeExecuteAction *code = new JobCodeExecuteAction( action );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opExecuteProcess --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opExecuteProcess( JobFunction *func, ProcessGroup *proc ){
  return opExecuteJobAction( func, proc );
}


/* --------------------------------------------------------------------------- */
/* opExecutePlugin --                                                          */
/* --------------------------------------------------------------------------- */

int JobManager::opExecutePlugin( JobFunction *func, Plugin *plugin ){
  return opExecuteJobAction( func, plugin );
}

/* --------------------------------------------------------------------------- */
/* opPluginInitialise --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opPluginInitialise( JobFunction *func, Plugin *plugin ){
  JobCodePluginInitialise *code = new JobCodePluginInitialise( plugin );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPluginParameter --                                                        */
/* --------------------------------------------------------------------------- */

int JobManager::opPluginParameter( JobFunction *func, Plugin *plugin ){
  JobCodePluginParameter *code = new JobCodePluginParameter( plugin );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opExecuteTask --                                                            */
/* --------------------------------------------------------------------------- */

int JobManager::opExecuteTask( JobFunction *func, const std::string &name ){
  JobFunction *f = getTask( name );
  assert( f != 0 );
  JobCodeExecuteFunction *code = new JobCodeExecuteFunction( f );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opExecuteFunction --                                                        */
/* --------------------------------------------------------------------------- */

int JobManager::opExecuteFunction( JobFunction *func, const std::string &name ){
  JobFunction *f = getFunction( name, true );
  assert( f != 0 );
  JobCodeExecuteFunction *code = new JobCodeExecuteFunction( f );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opExecuteFunction --                                                        */
/* --------------------------------------------------------------------------- */

int JobManager::opExecuteFunction( JobFunction *func, XferDataItem* xferDataItem ){
  JobCodeExecuteFunction *code = new JobCodeExecuteFunction( xferDataItem );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSerializeForm --                                                          */
/* --------------------------------------------------------------------------- */
int JobManager::opSerializeForm( JobFunction *func, GuiForm *form ){
  JobCodeSerializeForm *code = new JobCodeSerializeForm( form );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSerializeGuiElement --                                                    */
/* --------------------------------------------------------------------------- */
int JobManager::opSerializeGuiElement(JobFunction *func, GuiElement *elem, AppData::SerializeType type,
									  bool readStack, const std::string &filename){
  JobCodeSerializeGuiElement *code = new JobCodeSerializeGuiElement( elem, type, readStack, filename );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSerializeElement --                                                       */
/* --------------------------------------------------------------------------- */
int JobManager::opSerializeElement(JobFunction *func, GuiElement *elem, AppData::SerializeType type,
								   const std::string &filename){
  JobCodeSerializeElement *code = new JobCodeSerializeElement( elem, type, filename );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opWriteSettings --                                                          */
/* --------------------------------------------------------------------------- */
int JobManager::opWriteSettings( JobFunction *func ){
  JobCodeWriteSettings *code = new JobCodeWriteSettings();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opMap --                                                                    */
/* --------------------------------------------------------------------------- */

int JobManager::opMap(JobFunction *func, XferDataItem* xferDataItem, int nargs){
  JobCodeMap *code = new JobCodeMap(xferDataItem, nargs);
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opMap --                                                                    */
/* --------------------------------------------------------------------------- */

int JobManager::opMap(JobFunction *func, GuiElement *elem, int nargs){
  JobCodeMap *code = new JobCodeMap(elem, nargs);
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opUnmap --                                                                  */
/* --------------------------------------------------------------------------- */

int JobManager::opUnmap( JobFunction *func ){
  JobCodeUnmap *code = new JobCodeUnmap();
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opVisible --                                                                  */
/* --------------------------------------------------------------------------- */

int JobManager::opVisible( JobFunction *func, XferDataItem *xferdataitem ){
  JobCodeVisible *code = new JobCodeVisible( xferdataitem );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opEditable --                                                               */
/* --------------------------------------------------------------------------- */

int JobManager::opEditable( JobFunction *func, GuiElement* elem){
  JobCodeEditable *code = new JobCodeEditable( elem );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSetCurrentForm --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opSetCurrentForm( JobFunction *func, XferDataItem *xferdataitem ){
  JobCodeSetCurrentForm *code = new JobCodeSetCurrentForm( xferdataitem );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opMapForm --                                                                */
/* --------------------------------------------------------------------------- */

int JobManager::opMapForm( JobFunction *func, GuiForm *form ){
  JobCodeMapForm *code = new JobCodeMapForm( form );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opUnmapForm --                                                              */
/* --------------------------------------------------------------------------- */

int JobManager::opUnmapForm( JobFunction *func, GuiForm *form ){
  JobCodeUnmapForm *code = new JobCodeUnmapForm( form );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opVisibleForm --                                                            */
/* --------------------------------------------------------------------------- */

int JobManager::opVisibleForm( JobFunction *func, GuiForm *form ){
  JobCodeVisibleForm *code = new JobCodeVisibleForm( form );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opMapFolderGroup --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opMapFolderGroup( JobFunction *func,
				  const std::string &name,
				  GuiFolderGroup::OmitMap omit_map ){
  JobCodeMapFolderGroup *code = new JobCodeMapFolderGroup( name, omit_map );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opUnmapFolderGroup --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opUnmapFolderGroup( JobFunction *func, const std::string &name ){
  JobCodeUnmapFolderGroup *code = new JobCodeUnmapFolderGroup( name );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opVisibleFolderGroup --                                                     */
/* --------------------------------------------------------------------------- */

int JobManager::opVisibleFolderGroup( JobFunction *func, const std::string &name ){
  JobCodeVisibleFolderGroup *code = new JobCodeVisibleFolderGroup( name );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opMapGuiElement --                                                          */
/* --------------------------------------------------------------------------- */

int JobManager::opMapGuiElement( JobFunction *func, const std::string &name ){
  JobCodeMapGuiElement *code = new JobCodeMapGuiElement( name );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opUnmapGuiElement --                                                        */
/* --------------------------------------------------------------------------- */

int JobManager::opUnmapGuiElement( JobFunction *func, const std::string &name ){
  JobCodeUnmapGuiElement *code = new JobCodeUnmapGuiElement( name );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opMapTableLine --                                                           */
/* --------------------------------------------------------------------------- */

int JobManager::opMapTableLine( JobFunction *func, const std::string &name ){
  JobCodeMapTableLine *code = new JobCodeMapTableLine( name );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opUnmapTableLine --                                                         */
/* --------------------------------------------------------------------------- */

int JobManager::opUnmapTableLine( JobFunction *func, const std::string &name ){
  JobCodeUnmapTableLine *code = new JobCodeUnmapTableLine( name );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opUpdateForms --                                                            */
/* --------------------------------------------------------------------------- */

int JobManager::opUpdateForms( JobFunction *func ){
  return opUpdateGuiElement( func, GuiManager::Instance().getElement() );
}

/* --------------------------------------------------------------------------- */
/* opUpdateGuiElement --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opUpdateGuiElement( JobFunction *func, GuiElement *el ){
  JobCodeUpdateGuiElement *code = new JobCodeUpdateGuiElement( el );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opEnableGuiElement --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opEnableGuiElement( JobFunction *func, GuiElement *el ){
  assert( el != 0 );
  JobCodeEnableGuiElement *code = new JobCodeEnableGuiElement( el );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opDisableGuiElement --                                                      */
/* --------------------------------------------------------------------------- */

int JobManager::opDisableGuiElement( JobFunction *func, GuiElement *el ){
  assert( el != 0 );
  JobCodeDisableGuiElement *code = new JobCodeDisableGuiElement( el );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opDisableDragGuiElement --                                                      */
/* --------------------------------------------------------------------------- */

int JobManager::opDisableDragGuiElement( JobFunction *func, GuiElement *el ){
  assert( el != 0 );
  JobCodeDisableDragGuiElement *code = new JobCodeDisableDragGuiElement( el );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opReplaceGuiElement --                                                      */
/* --------------------------------------------------------------------------- */

int JobManager::opReplaceGuiElement( JobFunction *func
                                     , GuiElement *old_el, GuiElement *new_el ){
  JobCodeReplaceGuiElement *code = new JobCodeReplaceGuiElement( old_el, new_el );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opGuiElementMethod --                                                       */
/* --------------------------------------------------------------------------- */

int JobManager::opGuiElementMethod(JobFunction *func
                                   , GuiElement *gui_el){
  JobCodeGuiElementMethod *code = new JobCodeGuiElementMethod( gui_el );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opSetGuiFieldgroupRange --                                                  */
/* --------------------------------------------------------------------------- */

int JobManager::opSetGuiFieldgroupRange( JobFunction *func, GuiFieldgroup *fg ){
  JobCodeSetGuiFieldgroupRange *code = new JobCodeSetGuiFieldgroupRange( fg );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opAllow --                                                                  */
/* --------------------------------------------------------------------------- */

int JobManager::opAllow( JobFunction *func, GuiListenerController *ctrl ){
  assert( ctrl != 0 );
  JobCodeAllow *code = new JobCodeAllow( ctrl );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opDisallow --                                                               */
/* --------------------------------------------------------------------------- */

int JobManager::opDisallow( JobFunction *func, GuiListenerController *ctrl ){
  assert( ctrl != 0 );
  JobCodeDisallow *code = new JobCodeDisallow( ctrl );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opAssignIndex --                                                            */
/* --------------------------------------------------------------------------- */

int JobManager::opAssignIndex( JobFunction *func, const std::string &name ){
  GuiElement *el = GuiElement::findElement( name );
  assert( el != 0 );
  assert( el->Type() == GuiElement::type_Index );
  JobCodeAssignIndex *code = new JobCodeAssignIndex( (el)->getGuiIndex() );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPushIndex --                                                              */
/* --------------------------------------------------------------------------- */

int JobManager::opPushIndex( JobFunction *func, const std::string &name ){
  GuiElement *el = GuiElement::findElement( name );
  assert( el != 0 );
  assert( el->Type() == GuiElement::type_Index );
  JobCodePushGuiIndex *code = new JobCodePushGuiIndex( (el)->getGuiIndex() );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opClearTextWindow --                                                        */
/* --------------------------------------------------------------------------- */

int JobManager::opClearTextWindow( JobFunction *func, GuiScrolledText *tw ){
  JobCodeClearTextWindow *code = new JobCodeClearTextWindow( tw );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opCopy --                                                                   */
/* --------------------------------------------------------------------------- */

int JobManager::opCopy( JobFunction *func, Stream* stream, GuiElement* elem ){
  JobCodeCopy *code = new JobCodeCopy(stream, elem);
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opPaste --                                                                  */
/* --------------------------------------------------------------------------- */

int JobManager::opPaste( JobFunction *func, Stream* stream ){
  JobCodePaste *code = new JobCodePaste( stream );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opParse --                                                                  */
/* --------------------------------------------------------------------------- */

int JobManager::opParse( JobFunction *func, XferDataItem* xfer ){
  JobCodeParse *code = new JobCodeParse( xfer );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* opParse --                                                                  */
/* --------------------------------------------------------------------------- */

int JobManager::opParse( JobFunction *func, const std::string& filename ){
  JobCodeParse *code = new JobCodeParse( filename );
  return func->attach( code );
}

/* --------------------------------------------------------------------------- */
/* serializeFunctions --                                                       */
/* --------------------------------------------------------------------------- */
void  JobManager::serializeFunctions( std::ostream &os, AppData::SerializeType type, bool recursive ){
  JobFunctionList::iterator f;
  for( f = m_function_list.begin(); f != m_function_list.end(); ++f ){
    if (type == AppData::serialize_XML)
      (*f).second->serializeXML(os, recursive);
  }
}

/* --------------------------------------------------------------------------- */
/* LopLog --                                                                   */
/* --------------------------------------------------------------------------- */
int JobManager::opLog( JobFunction *func, const std::string& level ) {
  JobCodeLog *code = new JobCodeLog( level );
  return func->attach( code );
}
