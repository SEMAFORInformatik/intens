
#if !defined(JOB_SUBSCRIBE_H)
#define JOB_SUBSCRIBE_H

#include "job/JobController.h"
#include "operator/ConnectionListener.h"

class Stream;
class JobFunction;
class SubscribeInterface;

class JobSubscribe : public JobAction, public ConnectionListener
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  JobSubscribe( SubscribeInterface *reqObj,
		const std::vector<Stream*>& out,
		const std::string& header,
		JobFunction *subsFunc );
  virtual ~JobSubscribe();

  /*=============================================================================*/
  /* public Functions                                                            */
  /*=============================================================================*/
public:
  bool checkSubscribe();
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
  SubscribeInterface   *m_subscribeInterface;
  std::vector<Stream*>  m_outStreamVector;
  std::vector<Stream*>  m_inStreamVector;
  std::string           m_header;
  JobFunction*          m_function;
};

#endif
