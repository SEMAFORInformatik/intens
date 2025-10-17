
#if !defined(NEW_JOB_CONTROLLER_INCLUDED_H)
#define NEW_JOB_CONTROLLER_INCLUDED_H

#include "gui/DialogWorkClock.h"
#include "gui/GuiElement.h"
#include "job/JobAction.h"
#include "datapool/TransactionOwner.h"

class GuiEventLoopListener;
class MessageQueueReplyThread;

class JobController : public DialogWorkClockListener
                    , public TransactionOwner
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobController( JobAction *action,JobAction *callingAction, bool silent=false  );
  virtual ~JobController();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  std::string CallingActionName();
  JobAction* getBaseCallingAction();
  MessageQueueReplyThread* getMessageQueueReplyThread();
  void start();
  void stop();
  void end( JobAction::JobResult rslt );
  virtual void cancelButtonPressed();
  void startTransaction();
  void setBlockUndo( bool block_undo );

/*=============================================================================*/
/* public Member Functions of TransactionOwner                                 */
/*=============================================================================*/
public:
  virtual std::string ownerId() const;

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  void writeLog( const std::string &what );
  void createDialog();
  void deleteDialog();
  void commitTransaction();
  void abortTransaction();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
private:
  static int                   s_objectCounter;

  JobAction                   *m_action;        // Diese Action wird hier aufgerufen
  JobAction                   *m_callingAction; // Von dieser Action wurden wir aufgerufen
  GuiEventLoopListener        *m_loopcontrol;
  DialogWorkClock             *m_dialog;
  bool                         m_silent;
  bool                         m_block_undo;
  bool                         m_stopped;
  bool                         m_transactionStarted;
};

#endif
