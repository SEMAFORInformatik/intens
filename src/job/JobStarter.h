
#if !defined(JOB_STARTER_INCLUDED_H)
#define JOB_STARTER_INCLUDED_H

#include <queue>

#include "job/JobElement.h"
#include "gui/GuiManager.h"
#include "gui/GuiEventData.h"
#include "gui/TimerTask.h"
#include "job/JobAction.h"

class JobFunction;
class XferDataItem;
class JobStackData;
class Timer;

class JobStarter : public JobAction
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStarter( JobAction *fun );
  virtual ~JobStarter();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  void setGuiElement( GuiElement * ){
    std::cerr << "!! remove JobStarter::setGuiElement !!" << std::endl;
  } // remove It !!!
  void setReason( JobElement::CallReason reason ) { m_reason = reason; }
  JobElement::CallReason getReason() { return m_reason; };
  void setDataItem( XferDataItem *dataitem );
  void setDataItem( XferDataItem *dataitem, std::string &txt );
  void setBase( XferDataItem *dataitem );
  void setSource( XferDataItem *dataitem );
  void setSource( const std::vector<XferDataItem*>& sourceList );
  void setSource2( XferDataItem *dataitem );
  void setIndex( int i ) { m_index = i; }
  void setDiagramXPos(double r) { m_diagramXPos = r; }
  void setDiagramYPos(double r) { m_diagramYPos = r; }
  void setSortCriteria( const std::string& criteria ) { m_sortCriteria = criteria; }
  const std::string& getSortCriteria() { return m_sortCriteria; };
  void startJob( bool block_undo=false );
  static bool isWaiting() { return s_wait; }
  static unsigned int nPendingFunctions() {
    // return number of pending functions:
    // s_jobqueue.size() plus running function
    // s_wait true: a function is running
    // s_jobqueue.size(): number of functions in jobqueue
    return (s_wait ? 1 : 0) + s_jobqueue.size(); }
  virtual void printLogTitle( std::ostream &ostr ){}
  void startJobAction() { assert( false ); }
  void stopJobAction() { assert( false ); }
  virtual void serializeXML(std::ostream &os, bool recursive = false){}
  void startTransaction();

/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  virtual void backFromJobController( JobResult rslt );
  virtual void backFromJobStarter( JobResult rslt ) = 0;
  virtual void prepareToStartJob(){}
  void setOldData(JobStackData *data) { m_old_data = data; }
  const std::string& getFuncName() { return m_function->Name(); };

/*=============================================================================*/
/* private TimerTask class with tick method                                    */
/*=============================================================================*/
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(JobStarter* js)
      : m_jobstarter(js){}
    void tick() { JobStarter::tryAgain( m_jobstarter );  }
  private:
    JobStarter* m_jobstarter;
  };

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  static void tryAgain( JobStarter *caller );
  void startNow();
  static void push( JobStarter *starter );
  static JobStarter *pop();
  static void cleanup( JobAction::JobResult rslt );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::queue< JobStarter * > JobQueue;

  static bool               s_wait;
  static std::string        s_waitName;
  static JobQueue           s_jobqueue;

  JobAction                *m_function;
  JobElement::CallReason    m_reason;

  XferDataItem             *m_new_dataitem;
  XferDataItem             *m_baseDataitem;
  XferDataItem             *m_sourceDataitem;
  std::vector<XferDataItem*> m_sourceListDataitem;
  XferDataItem             *m_source2Dataitem;
  std::string               m_sortCriteria;
  JobStackData             *m_old_data;
  int                       m_index;
  double                    m_diagramXPos;
  double                    m_diagramYPos;
  Timer                    *m_timer;
  TimerTask                *m_task;

  static JobStarter        *s_to_delete;
};

#endif
