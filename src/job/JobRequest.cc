
#include "operator/RequestInterface.h"
#include "operator/MessageQueueRequest.h"
#include "app/Plugin.h"

#include "utils/Debugger.h"
#include "job/JobRequest.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
JobRequest::JobRequest( RequestInterface *reqObj
                        , const std::vector<Stream*>& outStreamVector
                        , const std::vector<Stream*>& inStreamVector
                        , const std::string& header
                        , const int timeout )
  : m_requestInterface( reqObj )
  , m_outStreamVector(outStreamVector), m_inStreamVector(inStreamVector)
  , m_header(header), m_timeout(timeout) {

  // default timeout des request jobs
  if (m_timeout == -1) {
    MessageQueueRequest *req = dynamic_cast<MessageQueueRequest*>(reqObj);
    if (reqObj)
      m_timeout  = req->getTimeout();
    BUG_DEBUG("Default Timeout: " << m_timeout );
  } else {
    BUG_DEBUG("JobRequest Timeout: " << m_timeout );
  }
  setLogOff();
}

JobRequest::~JobRequest(){
  delete m_requestInterface;
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobRequest::startJobAction(){
  BUG_DEBUG("JobRequest::startJobAction, header=" << m_header
            << ", timeout=" << m_timeout );
  if( m_requestInterface ){
    m_requestInterface->setRequestOutStreams( m_outStreamVector );
    m_requestInterface->setRequestInStreams( m_inStreamVector );
    m_requestInterface->setRequestHeader( m_header );
    m_requestInterface->setRequestTimeout( m_timeout );

    m_requestInterface->setRequestListener( this );
    m_requestInterface->startRequest();
  }
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobRequest::stopJobAction(){
  BUG_DEBUG("JobRequest::stopJobAction" );
  if (m_requestInterface)
    m_requestInterface->terminateRequest();
}

void JobRequest::backFromJobController( JobResult rslt ){
}

/* --------------------------------------------------------------------------- */
/* connectionClosed --                                                         */
/* --------------------------------------------------------------------------- */

void JobRequest::connectionClosed(bool abort){
 BUG_DEBUG("JobRequest::connectionClosed" );
 if (abort)
   endJobAction( JobAction::job_Aborted ); //Aborted ); FatalError is a really abort
 else
   endJobAction( JobAction::job_Ok );
}

bool JobRequest::checkMessageQueue() {
  return m_requestInterface->checkRequest();
}

std::string JobRequest::getErrorMessage() const{
  return m_requestInterface->getErrorMessage();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void JobRequest::serializeXML(std::ostream &os, bool recursive){
  os << "<jobRequest  name=\"" << Name() << "\">" << std::endl;
  os << "</jobRequest>" <<  std::endl;
}
