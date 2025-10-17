
#if !defined(JOB_ACTION_SIMPLE_INCLUDED_H)
#define JOB_ACTION_SIMPLE_INCLUDED_H

#include "job/JobAction.h"

class JobActionSimple : public JobAction
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobActionSimple( const std::string &name );
  virtual ~JobActionSimple();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  virtual void serializeXML(std::ostream &os, bool recursive = false){}
  virtual void printLogTitle( std::ostream &ostr ){}
  virtual GuiElement *getGuiElement() { return 0; }

  virtual void startJobAction();
  virtual void stopJobAction();
  virtual bool doDeleteAfterEndOfAction() const { return true; }

/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  virtual void backFromJobController( JobResult rslt );
  virtual void executeSimpleAction() = 0;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};

#endif
