
#include <QApplication>
#include <QProgressBar>
#include <QLabel>
#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QProcess>
#include <QScrollBar>

#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "gui/qt/GuiQtPixmap.h"
#include "gui/qt/GuiQtFieldgroup.h"
#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/QProgressIndicator.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtDialogWorkClock.h"
#include "datapool/DataVector.h"
#include "datapool/DataStringValue.h"
#include "utils/gettext.h"
#include "utils/Date.h"
#include "utils/Debugger.h"
#include "gui/qt/QtDialogProgressBar.h"
#include "operator/MessageQueue.h"
#include "operator/MessageQueuePublisher.h"
#include "job/JobManager.h"
#include "streamer/StreamManager.h"

INIT_LOGGER();

QtDialogProgressBar*  QtDialogProgressBar::s_dialogProgressBar = 0;
GuiQtFieldgroup*      QtDialogProgressBar::s_fieldgroupProgressBar = 0;
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

QtDialogProgressBar::QtDialogProgressBar( const std::string& name )
  : DialogProgressBar(name), GuiQtForm(0, false, name)
  , workClockDialog(0)
  , m_eventcontrol(0)
  , m_timerId(0)
  , m_raiseCounter(0)
  , dialog(0)
  , pixmap(0)
  , m_publisher(0)
  , mainFooterLabel(0)
{
  setMenuInstalled();
}


/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
/* --------------------------------------------------------------------------- */
/* newDetailFolderTab --                                                       */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::newDetailFolderTab(){
  QTextEdit* extensionData = new QTextEdit();
  QFont font = extensionData->font();
  extensionData->setFont( QtMultiFontString::getQFont( "@text@", font ) );
  extensionData->setReadOnly(true);
  extensionData->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  extensionFolder->addTab(extensionData, "Base");
}


/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::create(){
  BUG_DEBUG("QtDialogProgressBar::create");
  dialog = new QDialog(getParentWidget());
  QFont font = dialog->font();
  loopStatusLabel = new QLabel(QString::fromStdString(getLoopTitle()));
  mainStatusLabel = new QLabel(QString::fromStdString(getMainTitle()));
  mainFooterLabel = new QLabel(QString::fromStdString(getMainFooter()));
  mainProgressBar = new QProgressBar;
  std::string css= "QProgressBar::chunk {background-color: #5aafe8; width: 20px; margin: 0.5px;}";
  css +="QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;}";
  mainProgressBar->setStyleSheet( QString::fromStdString(css) );
  mainProgressBar->setValue(getMainPercentRate());
  subStatusLabel = new QLabel(QString::fromStdString(getSubTitle()));
  errorLabel = new QLabel(QString::fromStdString(getError()));
  subProgressBar = new QProgressBar;
  subProgressBar->setValue(getSubPercentRate());
  // pixmap
  XferDataItem *pmDataitem = new XferDataItem();
  pmDataitem->setDataReference(getPixmapDataReference());
  pixmap = dynamic_cast<GuiQtPixmap*>(GuiFactory::Instance()->createPixmap(this, pmDataitem));
  pixmap->setSize(300, 300);
  pixmap->create();
  // detail pixmap
  pmDataitem = new XferDataItem();
  pmDataitem->setDataReference(getDetailPixmapDataReference());
  detail_pixmap = dynamic_cast<GuiQtPixmap*>(GuiFactory::Instance()->createPixmap(this, pmDataitem));
  detail_pixmap->setSize(500, 350);
  //  detail_pixmap->setExpandable(status_ON);
  detail_pixmap->create();
  detail_pixmap->myWidget()->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  // pixmap layout
  pixWidget = new QWidget;
  QHBoxLayout *pixLayout = new QHBoxLayout;
  pixLayout->setContentsMargins(0,0,0,0);
  pixLayout->addWidget(pixmap->myWidget(), 10);
  pixLayout->addWidget(detail_pixmap->myWidget(), 10);
  pixWidget->setLayout(pixLayout);

  // extension
  extension = new QWidget;
  extensionDataTitle = new QLabel(QString::fromStdString(getDataTitle()));
  extensionDataFooter = new QLabel(QString::fromStdString(getDataFooter()));
  extensionFolder = new QTabWidget();
  extensionFolder->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  dialog->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

  QVBoxLayout *extensionLayout = new QVBoxLayout;
  extensionLayout->setContentsMargins(0,0,0,0);
  extensionLayout->addWidget(extensionDataTitle);
  extensionLayout->addWidget(extensionFolder);
  extensionLayout->addWidget(extensionDataFooter);
  extension->setLayout(extensionLayout);

  detailbutton = new QPushButton(_("Details"));
  detailbutton->setCheckable(true);

  quitButton = new QPushButton(_("Abort"));

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(detailbutton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

  connect(detailbutton, SIGNAL(toggled(bool)), extension, SLOT(setVisible(bool)));
  connect(quitButton, SIGNAL(clicked()), this, SLOT(slot_abort()));
  extension->setVisible(false);
  extension->setMinimumSize(600, 400);
  detailbutton->setVisible(false);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(loopStatusLabel);
  mainLayout->addWidget(mainStatusLabel);
  mainLayout->addWidget(mainProgressBar);
  mainLayout->addWidget(subStatusLabel);
  mainLayout->addWidget(errorLabel);
  mainLayout->addWidget(subProgressBar);
  mainLayout->addWidget(pixWidget, 1000);
  mainLayout->addWidget(buttonBox, 0, Qt::AlignHCenter);
  mainLayout->addWidget(extension, 1000);
  mainLayout->addWidget(mainFooterLabel);

  // create a default progress indicator
  progressIndicator = new QProgressIndicator();
  progressIndicator->setMinimumSize(30,30);
  progressIndicator->setAnimationDelay(1000);
  progressIndicator->startAnimation();
  mainLayout->addWidget(progressIndicator);

  mainLayout->addStretch(1);
  mainLayout->addSpacing(10);
  ///  mainLayout->addWidget(buttonBox, 0, Qt::AlignHCenter);
  dialog->setLayout(mainLayout);

  dialog->setWindowTitle(QString::fromStdString(getWindowTitle()));
  dialog->setModal(true);
}

void QtDialogProgressBar::slot_abort() {
  BUG( BugGui, "slot_abort" );

  if (m_timerId)
    killTimer( m_timerId );
  if (m_eventcontrol)
    m_eventcontrol->goAway();

  m_timerId = 0;
  m_eventcontrol = 0;

  execute_abort();
}

void QtDialogProgressBar::execute_abort(bool callAbortedFunc) {
  std::string s = getAbortCommand();
  BUG_INFO("QtDialogProgressBar::execute_abort, cmd: " << s);

  // does abort commant contain #
  bool abortJob = s.find("#") != 0;
  if (!abortJob)
    s = s.substr(1);
  BUG_INFO("QtDialogProgressBar::execute abort: " << abortJob << ", command: " << s);

  // process abort command
  if (s.size() > 1) {
    QString process_cmd;
    QStringList argList;
    QProcess proc;
#if defined HAVE_QT && defined _WIN32
    process_cmd =  QString::fromStdString("sh.exe");
#else
    process_cmd =  QString::fromStdString("/bin/sh");
#endif
    argList << "-c";
    argList <<  QString::fromStdString(s) ;
    proc.start( process_cmd, argList );
    proc.closeWriteChannel();
    if (proc.waitForFinished())  {
      BUG_DEBUG("Really done");
    }
    BUG_INFO("QtDialogProgressBar::execute_abort wait DONE");
  }
  if (abortJob) {
    // stop JobController
    abortJobController();

    // terminate zmq RequestThreads
    MessageQueue::terminateAllRequestThreads();
  }
#if 0
  // call Func
  if (callAbortedFunc) {
    JobFunction* jobFunction = JobManager::Instance().getFunction("AFTER_PROGRESS_DIALOG_ABORTED");
    if (jobFunction) {
      JobStarter *trigger = new Trigger( this, jobFunction );
      trigger->setName( "DialogProgressBar-Trigger-" + jobFunction->Name() );
      trigger->setReason( JobElement::cll_Function );
      trigger->startJob( true );
      BUG_DEBUG("started");
    }
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::manage() {
  QtDialogWorkClock::forceUnmap();
  if (!dialog)
    create();
  if (dialog->isVisible()) // alread shown
    return;
  m_eventcontrol = new GuiEventLoopListener( false );
  GuiQtManager::Instance().attachEventLoopListener( m_eventcontrol );
  m_timerId = startTimer(AppData::Instance().HeadlessWebMode() ? 1000 : 2000);
  m_raiseCounter = 4; //only  4 times raised this dialog on top
  detailbutton->setChecked(false);
  if (detailbutton->isChecked()) {
    extension->setVisible(detailbutton->isChecked());
  }
  m_startTime = QTime::currentTime();
  dialog->show();
  update(GuiElement::reason_Always);
}

/* --------------------------------------------------------------------------- */
/* map --                                                                      */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::map() {
  manage();
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::unmanage() {
  if (dialog) {
    dialog->hide();
    if (workClockDialog) {  // show workClockDialog
      workClockDialog->show();
      workClockDialog = 0;
    }
    resetData();
    if (m_timerId)
      killTimer( m_timerId );
    if (m_eventcontrol)
      m_eventcontrol->goAway();
    m_timerId = 0;
    m_eventcontrol = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* unmap --                                                                    */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::unmap() {
  unmanage();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::update(GuiElement::UpdateReason reason) {
  if (!dialog ||
      (dialog && reason != GuiElement::reason_Always && !dialog->isVisible())) return;
  setElapsedTime();
  publishData();

  std::string statusLabel = getLoopTitle();
  std::string errStr = getError();
  loopStatusLabel->setVisible(statusLabel.size() > 0);
  loopStatusLabel->setText(QString::fromStdString(statusLabel));
  mainStatusLabel->setText(QString::fromStdString(getMainTitle()));
  mainFooterLabel->setText(QString::fromStdString(getMainFooter()));
  mainProgressBar->setValue(getMainPercentRate());
  subStatusLabel->setText(QString::fromStdString(getSubTitle()));
  errorLabel->setText(QString::fromStdString(errStr));
  subProgressBar->setValue(getSubPercentRate());
  mainProgressBar->setVisible(getMainPercentRate() > 0);
  subProgressBar->setVisible(getSubPercentRate() > 0);
  pixmap->update(reason);
  pixmap->myWidget()->setVisible(pixmap->DataItem()->isValid());
  // minor data check
  std::string s;
  if (getPixmapDataReference())
    getPixmapDataReference()->GetValue(s);
  if (!pixmap->DataItem()->isValid() && s.size() < 5e2 &&
      !detail_pixmap->DataItem()->isValid())
    pixWidget->setVisible(false);
  else
    pixWidget->setVisible(true);
  detail_pixmap->update(reason);
  detail_pixmap->myWidget()->setVisible(detail_pixmap->DataItem()->isValid());
  errorLabel->setVisible(errStr.size() > 0);
  quitButton->setVisible(getAbortCommand().size() > 0);
  if (getMainPercentRate() <= 0 && getSubPercentRate() <= 0) {
    progressIndicator->show();
  } else
    progressIndicator->hide();

  // extension
  std::vector<std::string>  dataVec, dataLabelVec;
  getDataVector(dataVec);
  bool visible = dataVec.size() > 0 ||
    getDataTitle().size() > 0 ||
    getDataFooter().size() > 0;
  if(detailbutton->isVisible() != visible) {
    detailbutton->setVisible(visible);
  }
  detailbutton->setText(extension->isVisible() ? _("Collapse Details") : _("Details"));

  // extension visible?
  // do extension update
  if (extension->isVisible()) {
    extensionDataTitle->setText(QString::fromStdString(getDataTitle()));
    extensionDataFooter->setText(QString::fromStdString(getDataFooter()));

    std::vector<std::string>  vec;
    getDataLabelVector(dataLabelVec);
    std::string sL, sD;
    std::vector<std::string>::iterator iL=dataLabelVec.begin();
    std::vector<std::string>::iterator iD=dataVec.begin();
    int i=0;
    for(; iL != dataLabelVec.end() && iD != dataVec.end(); iL++, iD++, i++) {
      if (i == extensionFolder->count()) {
        newDetailFolderTab();
      }
      extensionFolder->setTabText(i, QString::fromStdString(*iL));
      QTextEdit* w = dynamic_cast<QTextEdit*>(extensionFolder->widget(i));
      if (w)
        w->setText(QString::fromStdString(*iD));
      QScrollBar* vsb = w->verticalScrollBar();
      if (vsb) {
        vsb->setValue(vsb->maximum());
      }
    }

    // remove trailing tabs
    while(i < extensionFolder->count()) {
      extensionFolder->removeTab(i);
    }
  }

  dialog->update();
  if (m_raiseCounter && dialog->isHidden()) {
    --m_raiseCounter;
    dialog->raise();
    dialog->show();
  }
  if (extension->isVisible()) {
    dialog->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  } else {
    dialog->setMaximumSize( dialog->sizeHint() );
  }
}

/* --------------------------------------------------------------------------- */
/* timerEvent --                                                               */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::timerEvent ( QTimerEvent * event ) {
  BUG_DEBUG("QtDialogProgressBar::timerEvent");
  update(GuiElement::reason_FieldInput);
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */
QtDialogProgressBar& QtDialogProgressBar::Instance() {
  if (s_dialogProgressBar == 0) {
    s_dialogProgressBar = new QtDialogProgressBar(DIALOG_NAME);
    s_fieldgroupProgressBar = new GuiQtFieldgroup(s_dialogProgressBar, FIELDGROUP_NAME);
  }
  return *s_dialogProgressBar;
}

/* --------------------------------------------------------------------------- */
/* publishData --                                                              */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::publishData() {
  if (!AppData::Instance().HeadlessWebMode()) return;
  BUG_DEBUG("QtDialogProgressBar::publishData");

  if (!m_publisher) {
    std::string pubname("mqReply_publisher_mq");
    std::string streamname("mqReply_progress_stream");
    m_publisher = MessageQueue::getPublisher(pubname);
    Stream* stream = StreamManager::Instance().getStream(streamname);
    if (m_publisher && stream) {
      std::vector<Stream*> streamVec;
      streamVec.push_back(stream);
      m_publisher->setPublishOutStreams(streamVec);
      m_publisher->setPublishHeader("progress_data");
    } else {
      if (!m_publisher) {
        BUG_WARN(compose("Undefined '%1' MESSAGE_QUEUE Publisher in MessageQueueReply.inc", pubname));
      }
      if (!stream) {
        BUG_WARN(compose("Undefined '%1' Stream in MessageQueueReply.inc", streamname));
      }
      return;
    }
  }
  m_publisher->startPublish(true);
}

/* --------------------------------------------------------------------------- */
/* getParentWidget --                                                          */
/* --------------------------------------------------------------------------- */
QWidget* QtDialogProgressBar::getParentWidget() {
  QWidget* pw = GuiQtManager::Instance().QApp()->activeWindow();

  // no DialogWorkClock as parent
  if (pw && pw->objectName() == "DialogWorkClock") {
    workClockDialog = pw;
    ///    pw->hide();
    pw = GuiQtManager::Instance().Toplevel();
  }
  return pw;
}

/* --------------------------------------------------------------------------- */
/* setElapsedTime --                                                           */
/* --------------------------------------------------------------------------- */
void QtDialogProgressBar::setElapsedTime() {
  int el =  m_startTime.secsTo(QTime::currentTime());
  int sec = el % 60;
  el -= sec;
  int hour = el / 3600;
  el -= hour*3600;
  int min = el / 60;
  QTime time(hour, min, sec);
  QString format = hour ? "hh:mm:ss": min ? "mm:ss" : "m:ss";
  std::string s(getMainFooter());
  const std::string ELAPSED_TIME = _("Total Elapsed Time: ");
  if (s.find(ELAPSED_TIME) == std::string::npos) {
    s += (s.size() ? " " : "") + std::string(ELAPSED_TIME) + time.toString(format).toStdString();
  } else {
    s = ELAPSED_TIME + time.toString(format).toStdString();
  }
  if (mainFooterLabel)
    mainFooterLabel->setText(QString::fromStdString(s));
  setMainFooter(s);
}
