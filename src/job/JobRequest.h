
#if !defined(JOB_REQUEST_H)
#define JOB_REQUEST_H

#include "job/JobController.h"
#include "operator/ConnectionListener.h"


class JobEngine;
class GuiElement;
class Stream;
class RequestInterface;

class JobRequest : public JobAction, public ConnectionListener
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  JobRequest( RequestInterface *reqObj,
              const std::vector<Stream*>& outStreamVector, const std::vector<Stream*>& inStreamVector,
              const std::string& header,
              const int timeout);
  virtual ~JobRequest();

  /*=============================================================================*/
  /* public Functions                                                            */
  /*=============================================================================*/
public:
  bool checkMessageQueue();
  std::string getErrorMessage() const;
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
  RequestInterface     *m_requestInterface;
  std::vector<Stream*>  m_outStreamVector;
  std::vector<Stream*>  m_inStreamVector;
  std::string           m_header;
  int                   m_timeout;
};

#endif
