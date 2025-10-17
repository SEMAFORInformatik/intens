
#include <vector>

#include "job/JobStarter.h"

#if !defined(INITIAL_WORKER_H)
#define INITIAL_WORKER_H

class InitialWorker{
private:
  InitialWorker();
  InitialWorker( const InitialWorker & );
  InitialWorker &operator=( const InitialWorker & );

  /*=============================================================================*/
  /* private Definitions                                                         */
  /*=============================================================================*/
private:
  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter
  {
  public:
    Trigger( JobAction *f, InitialWorker* initialWorker)
      : JobStarter( f )
      , m_initialWorker(initialWorker) {
    }
    virtual ~Trigger() {}

    void setOldData(JobStackData *data) { JobStarter::setOldData(data); }
    virtual void backFromJobStarter( JobAction::JobResult rslt ){
      m_initialWorker->backFromJobStarter(rslt);
    }
  private:
    InitialWorker* m_initialWorker;
  };


public:
  virtual ~InitialWorker();
  static InitialWorker &Instance();

  void startJobAction(JobElement::CallReason reason=JobElement::cll_CallReasonEntries,
                      JobStackData *old_data=0);

  void appendAction( JobAction *action );
  void backFromJobStarter( JobAction::JobResult rslt );
private:
  static InitialWorker *s_instance;

  typedef std::vector<JobAction*>::iterator ActionIterator;
  std::vector<JobAction*> m_actions;
};

#endif
