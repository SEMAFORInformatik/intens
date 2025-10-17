
#include "operator/SubscribeInterface.h"
#include "operator/MessageQueueSubscriber.h"
#include "app/Plugin.h"

#include "utils/Debugger.h"
#include "job/JobSubscribe.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
JobSubscribe::JobSubscribe( SubscribeInterface *reqObj
                            , const std::vector<Stream*>& outStreamVector
                            , const std::string& header
                            , JobFunction *subsFunc )
  : m_subscribeInterface( reqObj )
  , m_outStreamVector(outStreamVector)
  , m_header(header), m_function(subsFunc) {
  setLogOff();
}

JobSubscribe::~JobSubscribe(){
  delete m_subscribeInterface;
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobSubscribe::startJobAction(){
  BUG(BugJob,"JobSubscribe::startJobAction" );
  if( m_subscribeInterface ){
    m_subscribeInterface->setSubscribeOutStreams( m_outStreamVector );
    m_subscribeInterface->setSubscribeHeader( m_header );
    m_subscribeInterface->setSubscribeFunction( m_function );

    m_subscribeInterface->setSubscribeListener( this );
    m_subscribeInterface->doSubscribe();
  }
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobSubscribe::stopJobAction(){
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* connectionClosed --                                                         */
/* --------------------------------------------------------------------------- */

void JobSubscribe::connectionClosed(bool abort){
 BUG(BugJob,"JobRequest::connectionClosed" );
 if (abort)
   endJobAction( JobAction::job_Aborted );
 else
   endJobAction( JobAction::job_Ok );
}

/* --------------------------------------------------------------------------- */
/* backFromJobController --                                                    */
/* --------------------------------------------------------------------------- */

void JobSubscribe::backFromJobController( JobResult rslt ){
}

/* --------------------------------------------------------------------------- */
/* checkSubscribe --                                                           */
/* --------------------------------------------------------------------------- */

bool JobSubscribe::checkSubscribe() {
  return m_subscribeInterface->checkSubscribe();
}

/* --------------------------------------------------------------------------- */
/* getErrorMessage --                                                          */
/* --------------------------------------------------------------------------- */

std::string JobSubscribe::getErrorMessage() const {
  return m_subscribeInterface->getErrorMessage();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void JobSubscribe::serializeXML(std::ostream &os, bool recursive){
  os << "<jobSubscribe  name=\"" << Name() << "\">" << std::endl;
  os << "</jobSubscribe>" <<  std::endl;
}
