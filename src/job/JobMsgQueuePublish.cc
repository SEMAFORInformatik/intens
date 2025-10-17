
#include "operator/MessageQueuePublisher.h"

#include "utils/Debugger.h"
#include "job/JobMsgQueuePublish.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
JobMsgQueuePublish::JobMsgQueuePublish( MessageQueuePublisher *pub,
                                        const std::vector<Stream*>& outStreamVector,
                                        const std::string& header )
  : m_msgQueuePublisher( pub )
  , m_outStreamVector(outStreamVector), m_header(header) {
  setLogOff();
}

JobMsgQueuePublish::~JobMsgQueuePublish(){
  delete m_msgQueuePublisher;
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobMsgQueuePublish::startJobAction(){
  BUG(BugJob,"JobMsgQueuePublish::startJobAction" );
  if( m_msgQueuePublisher ){
    if (m_outStreamVector.size())
      m_msgQueuePublisher->setPublishOutStreams( m_outStreamVector );
    if (m_header.size())
      m_msgQueuePublisher->setPublishHeader( m_header );
    m_msgQueuePublisher->setListener( this );
    m_msgQueuePublisher->startPublish();
  }
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobMsgQueuePublish::stopJobAction(){
  assert( false );
}
void JobMsgQueuePublish::backFromJobController( JobResult rslt ){
}

/* --------------------------------------------------------------------------- */
/* connectionClosed --                                                         */
/* --------------------------------------------------------------------------- */

void JobMsgQueuePublish::connectionClosed(bool abort){
 BUG(BugJob,"JobMsgQueuePublish::connectionClosed" );
 if (abort)
   endJobAction( JobAction::job_Aborted );
 else
   endJobAction( JobAction::job_Ok );
}

bool JobMsgQueuePublish::checkMessageQueue() {
  return m_msgQueuePublisher->checkPublish();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void JobMsgQueuePublish::serializeXML(std::ostream &os, bool recursive){
  os << "<msgQueuePublish  name=\"" << Name() << "\">" << std::endl;
  os << "</msgQueuePublish>" <<  std::endl;
}
