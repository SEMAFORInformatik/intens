
#if !defined(QT_DIALOGPROGRESSBAR_H)
#define QT_DIALOGPROGRESSBAR_H

#include <QTime>
#include "gui/DialogProgressBar.h"
#include "gui/InformationListener.h"
#include "gui/qt/GuiQtForm.h"

class QDialog;
class QProgressBar;
class GuiQtPixmap;
class QLabel;
class QPushButton;
class QTabWidget;
class QProgressIndicator;
class QDialogButtonBox;
class GuiQtFieldgroup;
class GuiEventLoopListener;
class MessageQueuePublisher;

class QtDialogProgressBar : public GuiQtForm, public DialogProgressBar
{
    Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
    QtDialogProgressBar( const std::string& name );
  virtual ~QtDialogProgressBar() {}
  void newDetailFolderTab();

/*=============================================================================*/
/* public GuiElement functions                                                 */
/*=============================================================================*/
public:
  void create();
  void manage();
  void map();
  void unmanage();
  void unmap();
  void serializeXML(std::ostream&, bool recursive = false) {}
  QWidget* myWidget() { return dialog; }

/*=============================================================================*/
/* public DialogProgressBar functions                                          */
/*=============================================================================*/
  virtual GuiElement* getGuiElement() { return this; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static QtDialogProgressBar& Instance();

  void update(GuiElement::UpdateReason);
  void timerEvent (QTimerEvent * event);
  virtual void execute_abort(bool callAbortedFunc=true);

public slots:
    void slot_abort();


/*=============================================================================*/
/* private class definition                                                    */
/*=============================================================================*/
private:
  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter
  {
  public:
    Trigger( QtDialogProgressBar *progressBar, JobFunction *f )
      : JobStarter( f )
      , m_progressBar( progressBar ){
    }
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult jobResult ) {}
  private:
    QtDialogProgressBar *m_progressBar;
  };

/*=============================================================================*/
/* private functions                                                           */
/*=============================================================================*/
private:
  void publishData();
  QWidget* getParentWidget();
  void setElapsedTime();

/*=============================================================================*/
/* private member variables                                                    */
/*=============================================================================*/
private:
  static QtDialogProgressBar* s_dialogProgressBar;
  static GuiQtFieldgroup* s_fieldgroupProgressBar;

  QDialog *dialog;
  GuiQtPixmap *pixmap;
  GuiQtPixmap *detail_pixmap;
  QProgressBar *mainProgressBar;
  QProgressBar *subProgressBar;
  QLabel *loopStatusLabel;
  QLabel *mainStatusLabel;
  QLabel *mainFooterLabel;
  QLabel *subStatusLabel;
  QLabel *errorLabel;
  QProgressIndicator *progressIndicator;
  QTime  m_startTime;

  QWidget *pixWidget;
  QWidget *extension;
  QLabel  *extensionDataTitle;
  QLabel  *extensionDataFooter;
  QTabWidget *extensionFolder;

  QPushButton *detailbutton;
  QPushButton *quitButton;
  QDialogButtonBox *buttonBox;
  QWidget *workClockDialog;

  GuiEventLoopListener *m_eventcontrol;
  int                   m_timerId;
  int                   m_raiseCounter;
  MessageQueuePublisher* m_publisher;
};

#endif
