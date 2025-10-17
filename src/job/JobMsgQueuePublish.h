
#if !defined(JOB_MSG_QUEUE_PUBLISH)
#define JOB_MSG_QUEUE_PUBLISH

#include "job/JobController.h"
#include "operator/ConnectionListener.h"


class JobEngine;
class GuiElement;
class MessageQueuePublisher;

class JobMsgQueuePublish : public JobAction, public ConnectionListener
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  JobMsgQueuePublish( MessageQueuePublisher *pub,
		      const std::vector<Stream*>& out, const std::string& header);
  virtual ~JobMsgQueuePublish();

  /*=============================================================================*/
  /* public Functions                                                            */
  /*=============================================================================*/
public:
  bool checkMessageQueue();
  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction();
  virtual void connectionClosed(bool abort=false);
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  /*=============================================================================*/
  /* protected Functions                                                         */
  /*=============================================================================*/
protected:
  virtual void backFromJobController( JobResult rslt );//{}

  /*=============================================================================*/
  /* private Data                                                                */
  /*=============================================================================*/
private:
  MessageQueuePublisher  *m_msgQueuePublisher;
  std::vector<Stream*>    m_outStreamVector;
  std::string             m_header;
};

#endif
