
#include  <iomanip>

#include "job/JobIncludes.h"

#include "job/JobManager.h"
#include "job/JobAction.h"
#include "job/JobController.h"
#include "job/JobWebApiResponse.h"
#include "operator/MessageQueueThreads.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "gui/Timer.h"

INIT_LOGGER();
JobAction::WebReplyResultData* JobAction::s_replyResultData = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobAction::JobAction( std::string name )
  : m_controller( 0 )
  , m_next_controller( 0 )
  , m_silent( false )
  , m_silent_function( false )
  , m_update_forms( true )
  , m_high_priority( false )
  , m_log_on( true )
  , m_block_undo( false )
  , m_timer( 0 )
  , m_timerTask( 0 )
  , m_mqReplyThread( 0 )
  , m_name( name ){
  BUG_DEBUG("Constructor JobAction [" << m_name << "]");
}

JobAction::~JobAction() {
  if( m_timer ){
    delete m_timer;
    m_timer = 0;
  }
  if( m_timerTask ){
    delete m_timerTask;
    m_timerTask = 0;
  }
  BUG_DEBUG("Destructor JobAction [" << m_name << "]");
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setName --                                                                  */
/* --------------------------------------------------------------------------- */

void JobAction::setName( const std::string &name ){
  m_name = name;
}

/* --------------------------------------------------------------------------- */
/* Name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string &JobAction::Name() {
  return m_name;
}

/* --------------------------------------------------------------------------- */
/* CallingActionName --                                                        */
/* --------------------------------------------------------------------------- */

std::string JobAction::CallingActionName() {
  return  m_controller ? m_controller->CallingActionName() : "";
}

/* --------------------------------------------------------------------------- */
/* getBaseCallingAction --                                                     */
/* --------------------------------------------------------------------------- */

JobAction* JobAction::getBaseCallingAction() {
  return  m_controller ? m_controller->getBaseCallingAction() : this;
}
/* --------------------------------------------------------------------------- */
/* getBaseCallingAction --                                                     */
/* --------------------------------------------------------------------------- */

MessageQueueReplyThread* JobAction::getMessageQueueReplyThread() {
  return m_mqReplyThread ? m_mqReplyThread :
    getBaseCallingAction() != this ? getBaseCallingAction()->getMessageQueueReplyThread() : 0;
}

/* --------------------------------------------------------------------------- */
/* setJobController --                                                         */
/* --------------------------------------------------------------------------- */

void JobAction::setJobController( JobController *c ){
  assert( c != 0 );
  m_controller  = c;
}

/* --------------------------------------------------------------------------- */
/* endJobAction --                                                             */
/* --------------------------------------------------------------------------- */

void JobAction::endJobAction( JobResult rslt ){
  BUG_DEBUG("endJobAction rslt: " << rslt << " name=" << m_name << ", this=" << this);
  assert( m_controller != 0 );

  // Ein JobController hat uns gestartet. Also erhält er die Kontrolle
  // über das Geschehen zurück.
  JobController *c = m_controller;
  m_controller = 0;
  c->end( rslt );
  JobFunction* jobAfterUpdateFormsFunction = JobManager::Instance().getFunction("AFTER_UPDATE_FORMS");
  BUG_DEBUG("endJobAction nPendingFunctions: " << JobStarter::nPendingFunctions()
            << ", s_replyResultData: " << s_replyResultData
            << ", isWaiting: " << JobStarter::isWaiting()
            << ", Name: " << m_name);
  // send webReplyResult
  if (JobStarter::nPendingFunctions() == 0 ||
      ((JobStarter::nPendingFunctions() == 1 && JobStarter::isWaiting() &&
        (!jobAfterUpdateFormsFunction || Name() == "AFTER_UPDATE_FORMS")))
      ) {
    if (s_replyResultData) {
      s_replyResultData->sendWebReplyResult();
      BUG_DEBUG("endJobAction SEND Result");
    }
  }
  else {
    BUG_DEBUG("endJobAction no SEND (later) Result");
  }

}

/* --------------------------------------------------------------------------- */
/* startNextJobAction --                                                       */
/* --------------------------------------------------------------------------- */

void JobAction::startNextJobAction( JobAction *action, bool silent ){
  BUG_DEBUG("startNextJobAction name=" << m_name << ", undo blocked=" << m_block_undo << ", this=" << this );

  if( !m_timer ){
    m_timer = GuiFactory::Instance()->createTimer( 0 );
  }
  if( !m_timerTask ){
    m_timerTask = new  MyTimerTask();
    m_timer->addTask( m_timerTask );
  }

  if( m_next_controller == 0 ){
    BUG_DEBUG( "create a new JobController for action " << action->Name() );
    m_next_controller = new JobController( action, this, silent );
    m_next_controller->setBlockUndo( m_block_undo );
  }

  BUG_DEBUG( "Next Controller @" << m_next_controller );
  m_timerTask->setJobController( m_next_controller );
  if( action->highPriority() ){
    m_next_controller->start();
  }
  else{
    m_timer->start();
  }
  BUG_DEBUG( "Timer started ");
}

/* --------------------------------------------------------------------------- */
/* startTransaction --                                                         */
/* --------------------------------------------------------------------------- */

void JobAction::startTransaction( JobAction *action ){
  BUG_DEBUG("startTransaction name=" << m_name << ", undo blocked="
            << m_block_undo << ", this=" << this);

  assert( m_next_controller == 0 );
  m_next_controller = new JobController( action, this );
  m_next_controller->setBlockUndo( m_block_undo );
  m_next_controller->startTransaction();
}

/* --------------------------------------------------------------------------- */
/* setMessageQueueReplyThread --                                               */
/* --------------------------------------------------------------------------- */

void JobAction:: setMessageQueueReplyThread(MessageQueueReplyThread* mqReplyThread) {
   m_mqReplyThread = mqReplyThread;
}

/* --------------------------------------------------------------------------- */
/* haveNextJobAction --                                                        */
/* --------------------------------------------------------------------------- */

bool JobAction::haveNextJobAction() const {
  BUG_DEBUG("JobAction::haveNextJobAction [" << m_name << "]  Next Controller @"
            << m_next_controller );

  return m_next_controller != 0;
}

/* --------------------------------------------------------------------------- */
/* stopNextJobAction --                                                        */
/* --------------------------------------------------------------------------- */

void JobAction::stopNextJobAction(){
  BUG_DEBUG("JobAction::stopNextJobAction [" << m_name << "]  Next Controller @"
            << m_next_controller );

  assert( m_next_controller != 0 );
  m_next_controller->stop();
}

/* --------------------------------------------------------------------------- */
/* endJobController --                                                         */
/* --------------------------------------------------------------------------- */

void JobAction::endJobController( JobResult rslt ){
  BUG_DEBUG("JobAction::endJobController [" << m_name << "]  this=" << this );

  assert( m_next_controller != 0 );
  BUG_DEBUG( "Next Controller @" << m_next_controller << " is NULL now" );
  m_next_controller = 0;

  // backFromNextJobController !!
  backFromJobController( rslt );
}

/* --------------------------------------------------------------------------- */
/* setSilent --                                                                */
/* --------------------------------------------------------------------------- */

void JobAction::setSilent(){
  m_silent = true;
}

/* --------------------------------------------------------------------------- */
/* setSilentFunction --                                                          */
/* --------------------------------------------------------------------------- */

void JobAction::setSilentFunction(){
  m_silent = true;
  m_silent_function = true;
}

/* --------------------------------------------------------------------------- */
/* setBlockUndo --                                                             */
/* --------------------------------------------------------------------------- */

void JobAction::setBlockUndo(){
  BUG_DEBUG("setBlockUndo name=" << m_name << ", @" << this);
  m_block_undo = true;
}

/* --------------------------------------------------------------------------- */
/* setUpdateForms --                                                           */
/* --------------------------------------------------------------------------- */

void JobAction::setUpdateForms( bool update ){
  m_update_forms = update;
}

/* --------------------------------------------------------------------------- */
/* getUpdateForms --                                                           */
/* --------------------------------------------------------------------------- */

bool JobAction::getUpdateForms(){
 return m_update_forms;
}

/* --------------------------------------------------------------------------- */
/* isSilent --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobAction::isSilent(){
  return m_silent;
}

/* --------------------------------------------------------------------------- */
/* isSilentFunction --                                                           */
/* --------------------------------------------------------------------------- */

bool JobAction::isSilentFunction(){
  return m_silent_function;
}

/* --------------------------------------------------------------------------- */
/* isBlockUndo --                                                              */
/* --------------------------------------------------------------------------- */

bool JobAction::isBlockUndo() const{
  return m_block_undo;
}

/* --------------------------------------------------------------------------- */
/* setLogOff --                                                                */
/* --------------------------------------------------------------------------- */

void JobAction::setLogOff(){
  m_log_on = false;
}

/* --------------------------------------------------------------------------- */
/* isLogOn --                                                                  */
/* --------------------------------------------------------------------------- */

bool JobAction::isLogOn(){
  return m_log_on && !m_silent_function;
}

/* --------------------------------------------------------------------------- */
/* printMessage --                                                             */
/* --------------------------------------------------------------------------- */

void JobAction::printMessage( const std::string &message, GuiElement::MessageType type ){
  if( !m_silent_function ){
    JobManager::Instance().printMessage( message, type );
  }
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

void JobAction::destroy(){
  // !! VORSICHT !! //
  // !! Darf nur aufgerufen werden bevor der Job gestartet wurde !!
  if( m_next_controller ){
    delete m_next_controller;
  }
  delete this;
}

/* --------------------------------------------------------------------------- */
/* setStartTime --                                                             */
/* --------------------------------------------------------------------------- */

void JobAction::setStartTime( std::string startTime ) {
  m_startTime = startTime;
}

/* --------------------------------------------------------------------------- */
/* tick --                                                                     */
/* --------------------------------------------------------------------------- */

void JobAction::MyTimerTask::tick() {
  BUG_DEBUG("JobAction::MyTimerTask::tick");
  if( m_jobController ){
    ((JobController*)m_jobController)->start();
    BUG_DEBUG("Controller started : " << this );
  }
}

/* --------------------------------------------------------------------------- */
/* setJobController --                                                         */
/* --------------------------------------------------------------------------- */

void JobAction::MyTimerTask::setJobController( JobController *c ){
  m_jobController = c;
}

/* --------------------------------------------------------------------------- */
/* registerWebReplyResult --                                                   */
/* --------------------------------------------------------------------------- */

void JobAction::registerWebReplyResult(WebReplyResultData* webReply) {
  s_replyResultData = webReply;
}

/* --------------------------------------------------------------------------- */
/* sendWebReplyResult --                                                       */
/* --------------------------------------------------------------------------- */

void JobAction::WebReplyResultData::sendWebReplyResult() {
  // webapi result erwünscht
  if (s_replyResultData &&
      s_replyResultData->m_xfer) {
    // wurde der Cycle geändert?
    if (!m_updateAlways) {
      m_updateAlways = m_previousCycleNum != DataPoolIntens::Instance().currentCycle();
      if (!m_updateAlways) {
        // cycle name geaendert
        std::string cycleName;
        DataPoolIntens::Instance().getCycleName(m_previousCycleNum, cycleName);
        m_updateAlways = cycleName != m_previousCycleName;
      }
    }
    std::ostringstream os;
    webApiSetResponse(os);
    s_replyResultData->m_xfer->setValue(os.str());
    BUG_INFO("Response: "<<os.str().substr(0, 50)<<"... , Len: "<<os.str().size());
    // send result
    m_mqReply->setStatus(MessageQueueReplyThread::RECEIVE_DONE);
    delete s_replyResultData;
    s_replyResultData = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* webApiSetResponse --                                                        */
/* --------------------------------------------------------------------------- */

void JobAction::WebReplyResultData::webApiSetResponse(std::ostream &os) {
  std::vector<std::string> forms, tabs;
  BUG_DEBUG("webApiSetResponse LastWebxUpdate["<<GuiManager::Instance().LastWebUpdate()
            <<"] CurrentTransaction["<<DataPoolIntens::Instance().CurrentTransaction()
            <<"] DataPoolCurrentTransaction["<<DataPool::getDatapool().GetCurrentTransaction());

  JobWebApiResponse api;
  if (m_startTransactionId > 0)
    GuiManager::Instance().overrideLastUpdate(m_startTransactionId);
  if (m_protobuf) {
#if HAVE_PROTOBUF
    auto reply = in_proto::WebAPIResponse();
    api.serializeForms(&reply);
    api.serializeCycles(&reply);
    api.serializeElements(&reply, m_updateAlways);
    api.serializeFolderTab(&reply);
    api.serializeFunctionStatus(&reply, m_rslt, *m_action);
    api.serializeClipboard(&reply);
    reply.SerializePartialToOstream(&os);
#endif
  } else {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    api.serializeForms(jsonElem);
    api.serializeCycles(jsonElem);
    api.serializeElements(jsonElem, m_updateAlways);
    api.serializeFolderTab(jsonElem);
    api.serializeFunctionStatus(jsonElem, m_rslt, *m_action);
    api.serializeClipboard(jsonElem);
    os << ch_semafor_intens::JsonUtils::value2string(jsonElem);
  }

  if (m_startTransactionId > 0) {
    GuiManager::Instance().resetOverrideLastUpdate();
  }
  GuiManager::Instance().setWebUpdateTimestamp();
}
