
#if !defined(SMART_INTENS_INTERFACE_IMPL_H)
#define SMART_INTENS_INTERFACE_IMPL_H

#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "SmartIntensInterface.h"

class Plugin;

/**
Smart Intens Interface definition
*/
class SmartIntensInterface_Impl : public SmartIntensInterface {

 public:
  SmartIntensInterface_Impl(Plugin*);
  virtual ~SmartIntensInterface_Impl();

  virtual void subscribedData(const TransferData& pData);

// -----------------------------------------------------
// private definitions
// -----------------------------------------------------
private:
  class Trigger : public JobStarter
  {
  public:
    Trigger( SmartIntensInterface_Impl *ii, JobFunction *f )
      : JobStarter( f )
      , m_intensInterface( ii ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    SmartIntensInterface_Impl *m_intensInterface;
  };

 private:
  Plugin  *m_plugin;

};

#endif
