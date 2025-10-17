
#if !defined(JOB_ACTION_INCLUDED_H)
#define JOB_ACTION_INCLUDED_H

#include "gui/TimerTask.h"
#include "gui/GuiElement.h"
#include "gui/GuiButtonListener.h"
#include "job/JobElement.h"

class JobController;
class Timer;
class XferDataItem;
class JobStackData;
class MessageQueueReplyThread;

class JobAction
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobAction( std::string name = "" );
  virtual ~JobAction();

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum JobResult{
    job_Undefined
    , job_Ok
    , job_Ok_ignore
    , job_Canceled
    , job_Aborted
    , job_FatalError
    , job_Wait
    , job_Nok
  };

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  virtual void serializeXML(std::ostream &os, bool recursive = false) = 0;
  void setName( const std::string &name );
  const std::string &Name();
  std::string CallingActionName();
  JobAction* getBaseCallingAction();
  const std::string &getStartTime() const { return m_startTime; }
  virtual void printLogTitle( std::ostream &ostr ) = 0;
  virtual GuiElement *getGuiElement() { return 0; }
  void setJobController( JobController *c );

  virtual void startJobAction() = 0;
  virtual void stopJobAction() = 0;

  void startNextJobAction( JobAction *action, bool silent=false );
  void stopNextJobAction();
  void endJobController( JobResult rslt );

  void setSilent();
  void setSilentFunction();
  void setBlockUndo();
  void setUpdateForms( bool update );
  void setHighPriority(){ m_high_priority = true; }
  bool highPriority(){ return m_high_priority; }
  bool getUpdateForms();
  bool isSilent();
  bool isSilentFunction();
  bool isBlockUndo() const;
  void setLogOff();
  bool isLogOn();
  virtual bool isTask() { return false; }
  void startTransaction( JobAction *action );
  void setMessageQueueReplyThread(MessageQueueReplyThread* mqReplyThread);
  MessageQueueReplyThread* getMessageQueueReplyThread();

  virtual void setReason( JobElement::CallReason reason ){}
  virtual void setData( XferDataItem *new_data, JobStackData *old_data ){}
  virtual void setBaseDataItem( XferDataItem * ){}
  virtual void setSourceDataItem( XferDataItem * ){}
  virtual void setSourceDataItem( const std::vector<XferDataItem*>& ){}
  virtual void setSource2DataItem( XferDataItem * ){}
  virtual void setWebReplyResultDataItem( XferDataItem * ){}
  virtual void setWebReplyResultDataItemProto(){}
  virtual void setIndex( int index ){}
  virtual void setDiagramXPos(double r) {}
  virtual void setDiagramYPos(double r) {}
  virtual void setSortCriteria(const std::string& critera) {}
  void destroy();
  virtual bool doDeleteAfterEndOfAction() const { return false; }

  /** set start timestamp
      @param start time as string
  */
  virtual void setStartTime( std::string startTime );

/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  void endJobAction( JobResult rslt );
  virtual void backFromJobController( JobResult rslt ) = 0;
  bool haveNextJobAction() const;
  void printMessage( const std::string &, GuiElement::MessageType );
  bool hasJobController() { return m_controller != 0; }

/*=============================================================================*/
/* WebReplyResultData Functions                                                */
/*=============================================================================*/
  public:
  class WebReplyResultData {
  public:
    WebReplyResultData(JobAction* act, XferDataItem* replyXfer, bool updateAlways,
                       int transactionId, JobResult rslt,
                       MessageQueueReplyThread* mqReply,
                       std::string& oldCycleName,
                       int oldCycleNum, bool protobuf ):
      m_action(act)
    , m_xfer(replyXfer)
    , m_rslt(rslt)
    , m_startTransactionId(transactionId)
    , m_mqReply(mqReply)
    , m_updateAlways(updateAlways)
    , m_previousCycleName(oldCycleName)
    , m_previousCycleNum(oldCycleNum)
    , m_protobuf(protobuf)
    {}
    void sendWebReplyResult();
    void webApiSetResponse(std::ostream &os);
  private:
    JobAction*    m_action;
    XferDataItem* m_xfer;
    bool          m_updateAlways;
    bool          m_protobuf;
    JobResult     m_rslt;
    int           m_startTransactionId;
    MessageQueueReplyThread* m_mqReply;
    std::string  m_previousCycleName;
    int          m_previousCycleNum;
  };
  static void registerWebReplyResult(WebReplyResultData* webReply);

  static WebReplyResultData* s_replyResultData;

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private TimerTask class with tick method                                    */
/*=============================================================================*/
private:
  class MyTimerTask : public TimerTask
  {
  public:
    MyTimerTask(){}
    void tick();
    void setJobController( JobController *c );
  private:
    JobController *m_jobController;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  JobController  *m_controller;
  JobController  *m_next_controller;
  bool            m_initialized;
  bool            m_silent;
  bool            m_silent_function;
  bool            m_update_forms;
  bool            m_high_priority;
  bool            m_log_on;
  bool            m_block_undo;
  Timer          *m_timer;
  MyTimerTask    *m_timerTask;
  std::string     m_name;
  std::string     m_startTime;
  MessageQueueReplyThread* m_mqReplyThread;
  friend class ReportGen;
};



class JobActionButtonListener : public GuiButtonListener
                              , public JobAction
{
public:
  JobActionButtonListener( JobAction *action )
    : m_action( action ){}
  virtual void ButtonPressed() {
    startNextJobAction( m_action );
    // was geschieht mit dem GuiElement getButton() ??
  }

  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction() {}
  virtual void stopJobAction() {}
  virtual void serializeXML(std::ostream &os, bool recursive = false){}
  virtual JobAction *getAction(){ return m_action; }

protected:
  virtual void backFromJobController( JobResult rslt ){}

private:
  JobAction    *m_action;
};

#endif
