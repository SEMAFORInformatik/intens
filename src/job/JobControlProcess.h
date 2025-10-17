
#if !defined(JOB_CONTROLPROCESS_INCLUDED_H)
#define JOB_CONTROLPROCESS_INCLUDED_H

#include "job/JobController.h"

class JobEngine;
class JobAction;
class GuiElement;

class JobControlProcess : public JobController
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobControlProcess( JobAction *action )
    : m_engine( 0 )
    , m_element( 0 )
    , m_next_action( action ){}
  virtual ~JobControlProcess(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  void setEngine( JobEngine *eng ) { m_engine = eng; }
  void setElement( GuiElement *el ) { m_element = el; }

  virtual bool executeJobController();
  virtual bool stopJobController();
  virtual void backFromJobAction( JobResult rslt );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  JobEngine    *m_engine;
  GuiElement   *m_element;
  JobAction    *m_next_action;
};

#endif
