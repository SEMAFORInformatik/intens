
#if !defined(QUITAPPLICATION_INCLUDED_H)
#define QUITAPPLICATION_INCLUDED_H

#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiDialog.h"
#include "gui/ConfirmationListener.h"

class QuitApplication : public GuiButtonListener
                      , public ConfirmationListener
{
  /*=============================================================================*/
  /* private Definitions                                                         */
  /*=============================================================================*/
private:
  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter,  public ConfirmationListener
  {
  public:
    Trigger( QuitApplication *quit, JobFunction *f )
      : JobStarter( f )
      , m_quit( quit ){
      assert( quit != 0 );
    }
    virtual ~Trigger() {}

    virtual void backFromJobStarter( JobAction::JobResult rslt );

    // public member functions of ConfirmationListener
    virtual void confirmYesButtonPressed();
    virtual void confirmNoButtonPressed();

  private:
    QuitApplication *m_quit;
  };

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  QuitApplication( GuiDialog *dialog )
    : m_dialog( dialog ), m_exit(false) {
  }
  virtual ~QuitApplication(){
  }
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  JobAction* getAction(){ return 0; }
  static QuitApplication *Instance( GuiDialog * dialog = 0 );
  virtual void ButtonPressed();
#ifdef HAVE_QT
  void doAsk() {  ask(); }
#else
  static void deleteWindowCallback( Widget w, XtPointer, XtPointer );
#endif

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();
  void exitApplication();
  void setExitFlag(bool exit) { m_exit = exit; }
  bool ExitFlag() { return m_exit; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void ask();
  void lastActions();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  static QuitApplication *s_instance;
  GuiDialog  *m_dialog;
  bool        m_exit;
};

#endif
