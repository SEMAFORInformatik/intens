
#if !defined(JOB_CLIENT_SOCKET)
#define JOB_CLIENT_SOCKET

#include "job/JobController.h"
#include "operator/ConnectionListener.h"
#include "operator/ClientSocket.h"


class JobEngine;
class GuiElement;

class JobClientSocket : public JobAction
		      , public ConnectionListener
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  JobClientSocket( ClientSocket *socket )
    : m_socket( socket ){
    setLogOff();
  }
  virtual ~JobClientSocket(){
    delete m_socket;
  }

  /*=============================================================================*/
  /* public Functions                                                            */
  /*=============================================================================*/
public:
  bool checkInput();
  const std::string &getErrorMessage() const;
  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction();
  virtual void connectionClosed(bool abort=false);
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  /*=============================================================================*/
  /* protected Functions                                                         */
  /*=============================================================================*/
protected:
  virtual void backFromJobController( JobResult rslt ){}

  /*=============================================================================*/
  /* private Data                                                                */
  /*=============================================================================*/
private:
  ClientSocket *m_socket;
};

#endif
