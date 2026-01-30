/* qt headers */
#include <iostream>
#include <QApplication>
#include <QStyle>
#include <QPalette>
#include <QFile>
#include <qcursor.h>
#include <QPixmap>
#include <QCloseEvent>
#include <QTimerEvent>
#if QT_VERSION >= 0x060000
#include <QRegularExpression>
#else
#include <QTextCodec>
#include <QRegExp>
#include <QDesktopWidget>
#endif
#include <QMessageBox>
#include <QSettings>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSplashScreen>
#include <QBitmap>
#include <QScrollArea>
#include <QScrollBar>
#include <QPluginLoader>
#include <QLibrary>
#include <QLayout>

#include <list>
#include <typeinfo>
#include <limits>
#include <algorithm>

#include "job/InitialWorker.h"
#include "job/JobManager.h"

#include "utils/Debugger.h"
#include "utils/StringUtils.h"
#include "utils/Date.h"

#include "streamer/SerializeFormStreamParameter.h"

#include "app/Plugin.h"
#include "gui/GuiFolder.h"
#include "gui/GuiFolderGroup.h"
#include "gui/qt/GuiQtImage.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "gui/qt/GuiQtTextfield.h"
#include "gui/qt/GuiQtPulldownMenu.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtButtonbar.h"
#include "gui/qt/GuiQtIndex.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtDialogFileSelection.h"
#include "gui/qt/QtDialogWorkClock.h"
#include "gui/qt/QtDialogProgressBar.h"
#include "gui/qt/QtIconManager.h"

#include "gui/qt/GuiQtText.h"
#include "gui/qt/GuiQtForm.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/ArrowKeyLineEdit.h"
#include "app/AppData.h"
#include "app/QuitApplication.h"
#include "app/SmartPluginInterface.h"
#include "utils/gettext.h"
#include "datapool/DataTTrailUndoDisable.h"
#include "operator/IntensServerSocket.h"
#include "job/JobWebApiResponse.h"

#ifdef __MINGW32__
#include <windows.h>
#define sleep(time) Sleep(1000*time)
#endif

std::set<QObject*> GuiQtManager::s_waitCursorList;
const int GuiQtManager::s_defaultStretchFactor = 10;
GuiQtManager *GuiQtManager::s_instance = 0;

enum ISTATUS {
  S_MOTIF,
  S_QUES,
  S_IMPL,
  S_EQUAL,
  S_NOTIMPL

};

INIT_LOGGER();

#include <exception>
class MyQApplication : public QApplication{
public :
  MyQApplication( int *argc, char **argv, GuiQtManager* manager )
    : QApplication( *argc, argv )
    , m_timerId(-1)
    , m_receiver(0)
    , m_guiManager( manager ){
#if QT_VERSION < 0x060000
    QTextCodec::setCodecForLocale( QTextCodec::codecForName ( "UTF-8" ) );
#endif

    QString app_home(getenv("APPHOME"));
    if (app_home.size())
      QApplication::addLibraryPath(app_home+"/intens/lib");
  }

  virtual bool notify ( QObject * receiver, QEvent * event ){
    m_guiManager->checkEventLoopListener();
    bool gb = m_guiManager->grabButton();
    try {
      if( receiver->isWidgetType() ){
        QWidget *qw = (QWidget*)receiver;
        const QEvent::Type eventType = event->type();

        // alle Events von eienm wartenden modalen Dialogen bleiben erhalten
        // und alle Events einer Scrollbar
        if (qw->metaObject()->className() == std::string("QScrollBar") ||
            m_guiManager->isWaitingModalDialog() && qw->window()->isModal()) {
        } else
          if( gb ){

            if( !m_guiManager->setWaitingWidget( false, qw ) ){
              // set start grab time
              if (m_receiver == 0) {
                m_eventGrabTime = QDateTime::currentDateTime();
              }
              switch( eventType ){
              case QEvent::KeyPress:
              case QEvent::KeyRelease:
              case QEvent::MouseButtonPress:
              case QEvent::MouseButtonRelease:
                appendEvent(receiver, event);
                event->ignore();
                return false;
                break;
              default:
                break;
              }
            }
          }
          else if( eventType == QEvent::MouseButtonRelease
                   || eventType == QEvent::Leave
                   || eventType == QEvent::KeyRelease ){
            m_guiManager->setWaitingWidget( false, qw );
          }
      }
      return QApplication::notify( receiver, event );
    } catch( const std::exception &e ) {
      BUG_ERROR(e.what());
    }
    catch ( ...) {
      BUG_ERROR("Manager::notify Error unknown");
    }
    event->ignore();
    return false;

  }

bool appendEvent(QObject * receiver, QEvent * event ){
  if (m_receiver && m_receiver != receiver) {
    return false;
  }
  qint64 timeDiff = m_eventGrabTime.msecsTo ( QDateTime::currentDateTime() );
  if (timeDiff > 100) { // more than 0.1 seconds since last grabEvent => ignore
    // std::cout << "  This Event is too late!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    return false;
  }
  m_receiver = receiver;

  // only interested in QMouseEvents
  QMouseEvent*  mevent = dynamic_cast<QMouseEvent*>(event);
  if (!mevent)
    return false;
#if QT_VERSION >= 0x060000
  QPoint pos(mevent->globalPosition().toPoint());
#else
  QPoint pos(mevent->pos());
#endif
  if (mevent) {
      QEvent* e = new QMouseEvent(event->type(), mevent->pos(), pos,
                                  mevent->button(), mevent->buttons(), Qt::NoModifier);
      m_eventVector.push_back(e);
      m_timerId=startTimer(110);
  }
  return true;
}

void timerEvent( QTimerEvent *event ) {

  if( m_timerId == event->timerId() ){
    killTimer( m_timerId );

    // process ignored event list
    std::vector<QEvent*>::iterator it = m_eventVector.begin();
    for ( ; it != m_eventVector.end(); ++it) {
      QEvent* e = (*it);
      if (!this->notify(m_receiver, e)) {
	// already ignored, restart timer
	m_timerId=startTimer(110); //
	return;
      }
      delete e;
    }

    // working done, now we clear all ignored event list
    m_eventGrabTime = QDateTime(); // clear eventGrabTime
    m_receiver = 0;
    m_eventVector.clear();
  }
}

private :
  GuiQtManager *m_guiManager;
  int m_timerId;
  QObject * m_receiver;
  std::vector<QEvent*>  m_eventVector;
  QDateTime m_eventGrabTime;
};

/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/

class MainWidget : public QDialog {
public:
  MainWidget() : QDialog(NULL, Qt::Window), m_screenNumber() {}
  virtual ~MainWidget() {}

#if defined Q_OS_WIN || defined Q_OS_CYGWIN
  void setHintSize( int w, int h ) {
    m_hintSize.setWidth ( w );
    m_hintSize.setHeight( h );
  }

  void resizeEvent ( QResizeEvent * event ) {
    // trick for Windows
    if ( m_hintSize.isValid() )
      QDialog::setMaximumSize(m_hintSize);
    QWidget::resizeEvent(event);
    if ( m_hintSize.isValid() )
      QDialog::setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  }
#endif

  void moveEvent ( QMoveEvent * event ) {
    setMaximumSize();
    QDialog::moveEvent(event);
  }

  virtual void showEvent ( QShowEvent * event ) {
    BUG(BugGui,"MainWidget::showEvent");
    setMaximumSize();
    event->accept();
  }

  void setMaximumSize(bool force=false) {
    BUG(BugGui,"MainWidget::setMaximumSize");
#if QT_VERSION < 0x060000
    int screenNumber = QApplication::desktop()->screenNumber(this);
#else
    QString screenNumber= QGuiApplication::primaryScreen()->serialNumber();
#endif
    if (!force && m_screenNumber == screenNumber)
      return;
    m_screenNumber = screenNumber;
    QSize hs = QWidget::sizeHint();
#if QT_VERSION < 0x060000
    QRect maxDesktop= QApplication::desktop()->availableGeometry(this);
#else
    QRect maxDesktop = QGuiApplication::primaryScreen()->availableGeometry();
#endif

    // evtl. noch Platz reservieren für die Scrollbars
    QScrollArea *scrollView = findChild<QScrollArea *>("FormScrollView");
    int add=0;
    if (scrollView) {
      add = scrollView->verticalScrollBar()->sizeHint().width();
      hs.setHeight(hs.height() + add);
      hs.setWidth(hs.width() + add);
    }

    // verkleinern der Höhe
    if (hs.height() > maxDesktop.height()) {
      hs.setHeight( maxDesktop.height() );
      move(x(),maxDesktop.y());
      BUG_MSG("Desktop height to small => set maximum dialog height to desktop height");
      if (scrollView == NULL) {
        setMinimumHeight(maxDesktop.height());
      }
      setMaximumHeight(maxDesktop.height());
    }
    // verkleinern der Breite
    if (hs.width() > maxDesktop.width()) {
      hs.setWidth( maxDesktop.width() );
      move(maxDesktop.x(),y());
      BUG_MSG("Desktop width to small => set maximum dialog width to desktop width");
      if (scrollView == NULL) {
        setMinimumWidth(maxDesktop.width());
      }
      setMaximumWidth(maxDesktop.width());
    }

    // je nach dialog expandPolicy wird evtl. die maximale Höhe oder Breite gesetzt
    GuiElement::Orientation dialog_exand_policy = GuiQtManager::Instance().getDialogExpandPolicy();
    BUG_MSG("DialogExpandPolicy: "<< dialog_exand_policy);
    if (!(dialog_exand_policy & GuiElement::orient_Horizontal)) {
      BUG_MSG("Set Maximum Dialog Width: " << hs.width());
      setMaximumWidth(hs.width());
    }
    else setMaximumWidth(QWIDGETSIZE_MAX);

    if (!(dialog_exand_policy & GuiElement::orient_Vertical)) {
      BUG_MSG("Set Maximum Dialog Height: " << hs.height());
      setMaximumHeight(hs.height());
    }
    else setMaximumHeight(QWIDGETSIZE_MAX);
  }

  virtual QSize sizeHint () const {
    if ( m_hintSize.isValid() ) return m_hintSize;
    QSize hs = QWidget::sizeHint();
#if QT_VERSION < 0x060000
    QRect drect = QApplication::desktop()->availableGeometry(this);
#else
    QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#endif
    if ( hs.width() > drect.width() ) {
      hs.setWidth( drect.width() );
    }
    if ( hs.height() > drect.height() ) {
      hs.setHeight( drect.height());
    }
    return hs;
  }
  // to catch esc key event
  virtual void reject(){
    QuitApplication *quit = QuitApplication::Instance();
    quit->ButtonPressed();
  }


private:
  virtual void closeEvent( QCloseEvent* e){
    QuitApplication *quit = QuitApplication::Instance();
    quit->ButtonPressed();
    e->ignore();
  }

  // private data
  QSize m_hintSize;
#if QT_VERSION < 0x060000
  int m_screenNumber;
#else
  QString m_screenNumber;
#endif
};
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/



/*=============================================================================*/
/* static variables                                                            */
/*=============================================================================*/
QSettings* GuiQtManager::m_settings(0);
std::string  GuiQtManager::m_fieldgroupShadow("raise");
QColor GuiQtManager::m_foregroundColor;
QColor GuiQtManager::m_backgroundColor;
QColor GuiQtManager::m_buttonColor;
QColor GuiQtManager::m_buttonTextColor;
QColor GuiQtManager::m_baseColor;
QColor GuiQtManager::m_textColor;
QColor GuiQtManager::m_brightTextColor;
QColor GuiQtManager::m_highlightColor;
QColor GuiQtManager::m_highlightedTextColor;
QColor GuiQtManager::m_linkColor;
QColor GuiQtManager::m_linkVisitedColor;

QColor GuiQtManager::m_editableForegroundColor;
QColor GuiQtManager::m_editableBackgroundColor;
QColor GuiQtManager::m_editableLForegroundColor;
QColor GuiQtManager::m_editableLBackgroundColor;
QColor GuiQtManager::m_optionalForegroundColor;
QColor GuiQtManager::m_optionalBackgroundColor;
QColor GuiQtManager::m_optionalLForegroundColor;
QColor GuiQtManager::m_optionalLBackgroundColor;
QColor GuiQtManager::m_readonlyForegroundColor;
QColor GuiQtManager::m_readonlyBackgroundColor;
QColor GuiQtManager::m_readonlyLForegroundColor;
QColor GuiQtManager::m_readonlyLBackgroundColor;
QColor GuiQtManager::m_lockedForegroundColor;
QColor GuiQtManager::m_lockedBackgroundColor;
QColor GuiQtManager::m_alarmForegroundColor;
QColor GuiQtManager::m_alarmBackgroundColor;
QColor GuiQtManager::m_warnForegroundColor;
QColor GuiQtManager::m_warnBackgroundColor;
QColor GuiQtManager::m_infoForegroundColor;
QColor GuiQtManager::m_infoBackgroundColor;
QColor GuiQtManager::m_helpForegroundColor;
QColor GuiQtManager::m_helpBackgroundColor;
QColor GuiQtManager::m_indexForegroundColor;
QColor GuiQtManager::m_imageLowerAlarmColor;
QColor GuiQtManager::m_imageUpperAlarmColor;
QColor GuiQtManager::m_imageHighlightColor;
QColor GuiQtManager::m_indexBackgroundColor;
QColor GuiQtManager::m_color1ForegroundColor;
QColor GuiQtManager::m_color1BackgroundColor;
QColor GuiQtManager::m_color2ForegroundColor;
QColor GuiQtManager::m_color2BackgroundColor;
QColor GuiQtManager::m_color3ForegroundColor;
QColor GuiQtManager::m_color3BackgroundColor;
QColor GuiQtManager::m_color4ForegroundColor;
QColor GuiQtManager::m_color4BackgroundColor;
QColor GuiQtManager::m_color5ForegroundColor;
QColor GuiQtManager::m_color5BackgroundColor;
QColor GuiQtManager::m_color6ForegroundColor;
QColor GuiQtManager::m_color6BackgroundColor;
QColor GuiQtManager::m_color7ForegroundColor;
QColor GuiQtManager::m_color7BackgroundColor;
QColor GuiQtManager::m_color8ForegroundColor;
QColor GuiQtManager::m_color8BackgroundColor;
QColor GuiQtManager::m_fieldgroupForegroundColor;
QColor GuiQtManager::m_fieldgroupBackgroundColor;
QColor GuiQtManager::m_navItemBorderColor;
int GuiQtManager::m_navItemBorderWidth;
int GuiQtManager::m_listItemHeight;
QStringList GuiQtManager::m_defaultColorPalette;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtManager::GuiQtManager()
  : GuiQtElement( 0 )
  , m_gui_transaction( 1 )
  , m_web_transaction( 1 )
  , m_gui_cycle( 0 )
  , m_gui_full_update( false )
  , m_with_gui( true )
  , m_waitingWidget( 0 )
  , m_waitingModalDialog( false )
  , m_startup( 0 )
  , m_updateLater(-1)
  , m_updateStylesheet(-1)
  , m_jobAfterUpdateFormsFunction(0)
{
  setFormZOrder(0);
}

GuiQtManager::~GuiQtManager() {
}

QApplication* GuiQtManager::QApp() {
  return dynamic_cast<QApplication*>(m_qapp);
}

/* --------------------------------------------------------------------------- */
/* resources ( colors ... ) --                                                 */
/* --------------------------------------------------------------------------- */

const QColor&  GuiQtManager::foregroundColor() { return m_foregroundColor; }
const QColor&  GuiQtManager::backgroundColor() { return m_backgroundColor; }
const QColor&  GuiQtManager::buttonColor()     { return m_buttonColor; }
const QColor&  GuiQtManager::buttonTextColor() { return m_buttonTextColor; }
const QColor&  GuiQtManager::highlightColor() { return m_highlightColor; }

const QColor&  GuiQtManager::editableForegroundColor() { return m_editableForegroundColor; }
const QColor&  GuiQtManager::editableBackgroundColor() { return m_editableBackgroundColor; }
const QColor&  GuiQtManager::editableLForegroundColor() { return m_editableLForegroundColor; }
const QColor&  GuiQtManager::editableLBackgroundColor() { return m_editableLBackgroundColor; }
const QColor&  GuiQtManager::optionalForegroundColor() { return m_optionalForegroundColor; }
const QColor&  GuiQtManager::optionalBackgroundColor() { return m_optionalBackgroundColor; }
const QColor&  GuiQtManager::optionalLForegroundColor() { return m_optionalLForegroundColor;}
const QColor&  GuiQtManager::optionalLBackgroundColor() { return m_optionalLBackgroundColor; }
const QColor&  GuiQtManager::readonlyForegroundColor() { return m_readonlyForegroundColor; }
const QColor&  GuiQtManager::readonlyBackgroundColor() { return m_readonlyBackgroundColor; }
const QColor&  GuiQtManager::readonlyLForegroundColor() { return m_readonlyLForegroundColor; }
const QColor&  GuiQtManager::readonlyLBackgroundColor() { return m_readonlyLBackgroundColor; }
const QColor&  GuiQtManager::lockedForegroundColor() { return m_lockedForegroundColor; }
const QColor&  GuiQtManager::lockedBackgroundColor() { return m_lockedBackgroundColor; }
const QColor&  GuiQtManager::alarmForegroundColor() { return m_alarmForegroundColor; }
const QColor&  GuiQtManager::alarmBackgroundColor() { return m_alarmBackgroundColor; }
const QColor&  GuiQtManager::warnForegroundColor() { return m_warnForegroundColor; }
const QColor&  GuiQtManager::warnBackgroundColor() { return m_warnBackgroundColor;  }
const QColor&  GuiQtManager::infoForegroundColor() { return m_infoForegroundColor; }
const QColor&  GuiQtManager::infoBackgroundColor() { return m_infoBackgroundColor; }
const QColor&  GuiQtManager::helpForegroundColor() { return m_helpForegroundColor; }
const QColor&  GuiQtManager::helpBackgroundColor() { return m_helpBackgroundColor; }
const QColor&  GuiQtManager::indexForegroundColor() { return m_indexForegroundColor; }
const QColor&  GuiQtManager::indexBackgroundColor() { return m_indexBackgroundColor; }
const QColor&  GuiQtManager::imageLowerAlarmColor() { return m_imageLowerAlarmColor; }
const QColor&  GuiQtManager::imageUpperAlarmColor() { return m_imageUpperAlarmColor; }
const QColor&  GuiQtManager::imageHighlightColor() { return m_imageHighlightColor; }

const QColor  GuiQtManager::colorForegroundColor(int numColor) {
  if (!m_settings) return m_color1ForegroundColor;
  return m_settings->value( compose("Colors/color%1Foreground", numColor).c_str(), "blue" ).value<QColor>();
}
const QColor  GuiQtManager::colorBackgroundColor(int numColor) {
  if (!m_settings) return m_color1BackgroundColor;
  return m_settings->value( compose("Colors/color%1Background", numColor).c_str(), "yellow" ).value<QColor>();
}
const QColor&  GuiQtManager::color1ForegroundColor() { return m_color1ForegroundColor; }
const QColor&  GuiQtManager::color1BackgroundColor() { return m_color1BackgroundColor; }
const QColor&  GuiQtManager::color2ForegroundColor() { return m_color2ForegroundColor; }
const QColor&  GuiQtManager::color2BackgroundColor() { return m_color2BackgroundColor; }
const QColor&  GuiQtManager::color3ForegroundColor() { return m_color3ForegroundColor; }
const QColor&  GuiQtManager::color3BackgroundColor() { return m_color3BackgroundColor; }
const QColor&  GuiQtManager::color4ForegroundColor() { return m_color4ForegroundColor; }
const QColor&  GuiQtManager::color4BackgroundColor() { return m_color4BackgroundColor; }
const QColor&  GuiQtManager::color5ForegroundColor() { return m_color5ForegroundColor; }
const QColor&  GuiQtManager::color5BackgroundColor() { return m_color5BackgroundColor; }
const QColor&  GuiQtManager::color6ForegroundColor() { return m_color6ForegroundColor; }
const QColor&  GuiQtManager::color6BackgroundColor() { return m_color6BackgroundColor; }
const QColor&  GuiQtManager::color7ForegroundColor() { return m_color7ForegroundColor; }
const QColor&  GuiQtManager::color7BackgroundColor() { return m_color7BackgroundColor; }
const QColor&  GuiQtManager::color8ForegroundColor() { return m_color8ForegroundColor; }
const QColor&  GuiQtManager::color8BackgroundColor() { return m_color8BackgroundColor; }
const QColor&  GuiQtManager::navItemBorderColor() { return m_navItemBorderColor; }

const QColor&  GuiQtManager::fieldgroupForegroundColor() { return m_fieldgroupForegroundColor; }
const QColor&  GuiQtManager::fieldgroupBackgroundColor() { return m_fieldgroupBackgroundColor; }
const QStringList&  GuiQtManager::defaultColorPalette() { return m_defaultColorPalette; }
const std::string&  GuiQtManager::fieldgroupShadow() { return m_fieldgroupShadow; }
const std::string& GuiQtManager::stylesheetName() { return m_stylesheetName; }
const int& GuiQtManager::navItemBorderWidth() { return m_navItemBorderWidth; }
const int& GuiQtManager::listItemHeight() { return m_listItemHeight; }

const std::string GuiQtManager::STYLESHEET_APPLY_INIT = "init";
const std::string GuiQtManager::STYLESHEET_APPLY_SHOWN = "shown";
const std::string GuiQtManager::STYLESHEET_APPLY_BOTH = "both";

std::string GuiQtManager::m_stylesheetName;
std::string GuiQtManager::m_stylesheetApply(GuiQtManager::STYLESHEET_APPLY_INIT);  // default
std::string GuiQtManager::m_prevSettingFileVersion;

/* --------------------------------------------------------------------------- */
/* createManager --                                                            */
/* --------------------------------------------------------------------------- */

GuiQtManager *GuiQtManager::createManager(){
  assert( s_instance == 0 );
  s_instance = new GuiQtManager();
  return s_instance;
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

GuiQtManager &GuiQtManager::Instance(){
  assert( s_instance != 0 );
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* Toplevel --                                                                 */
/* --------------------------------------------------------------------------- */

QDialog* GuiQtManager::Toplevel() {
  return m_toplevel;
}

/*=============================================================================*/
/* pure virtual form GuiManager                                                */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setMainTitle --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtManager::setMainTitle( const std::string &title ){
  m_toplevel->setWindowTitle( QString::fromStdString( title ) );
}

/* --------------------------------------------------------------------------- */
/* getDisplayName --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtManager::getDisplayName( std::string &title ){
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtManager::update( GuiElement::UpdateReason reason ){
  BUG_PARA( BugGuiMgr, "update", "Reason is " << reason );
  BUG_DEBUG("update Reason is " << reason );
  QTime startTime = QTime::currentTime();

  DataTTrailUndoDisable undo_disabler( true );

  if( reason != reason_Cycle && reason != reason_Always ){
    if( m_gui_cycle != DataPoolIntens::Instance().currentCycle() ){
      reason = reason_Cycle;
    }
    else
    if( m_gui_full_update ){
      m_gui_full_update = false;
      reason = reason_Always;
    }
  }

  if( reason == reason_Cycle ){
    GuiElementList::iterator it;
    GuiElementList elist;
    GuiElement::findElementType(elist, type_Index);
    GuiElement::findElementType(elist, type_CycleButton);
    for( it = elist.begin(); it != elist.end(); ++it )
      (*it)->update( reason );
  }

  GuiElementList::iterator it;
  GuiElementList flist, mlist;
  GuiElement::findElementType(flist, type_Form);
  GuiElement::findElementType(mlist, type_Main);
  flist.insert(flist.end(), mlist.begin(), mlist.end());
  for( it = flist.begin(); it != flist.end(); ++it )
    (*it)->update( reason );
  QtDialogProgressBar::Instance().update(reason);

  // handle after_update_forms function call
  if( !m_omitNextAfterUpdateForms &&
    m_toplevel && !m_toplevel->signalsBlocked() && m_jobAfterUpdateFormsFunction != 0 ){
    m_toplevel->blockSignals( true );
    runJobAfterUpdateFormsFunction( JobElement::cll_GuiUpdate );
  }
  m_omitNextAfterUpdateForms = false;

  // Immer nach einem totalen Update der Applikation beginnt eine
  // neue Transaktion. Zu diesem Zeitpunkt sind alle DataField's ok.
  m_gui_transaction = DataPoolIntens::NewTransaction();
  m_gui_cycle       = DataPoolIntens::Instance().currentCycle();
  int msec = startTime.msecsTo(QTime::currentTime());
  if (msec < 1000){
    BUG_DEBUG("GuiUpdate: " << Date::durationAsString(msec, false));
  }else{
    BUG_INFO("GuiUpdate: " << Date::durationAsString(msec, false));
  }
  BUG_EXIT("New Transaction " << m_gui_transaction);
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */
bool GuiQtManager::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  GuiElementList::iterator it;
  GuiElementList flist;
  GuiElement::findElementType(flist, type_Form);
  for( it = flist.begin(); it != flist.end(); ++it ) {
    if ((*it)->getQtElement()->myWidget())  // is created?
      if ((*it)->hasChanged( trans, xfer, show ))
        return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* initApplication --                                                          */
/* --------------------------------------------------------------------------- */
#include <QLibraryInfo>

void GuiQtManager::initApplication( int *argc, char **argv, const std::string & ){
  AppData &appdata = AppData::Instance();

  appdata.getApplicationResources();

  // create main widget and first set of style and font
  for (int c=0; c < *argc; ++c) {
    if (argv[c] == std::string("-persistfile") && AppData::Instance().ParserStartToken()) m_with_gui = false;
    if (argv[c] == std::string("-help")) m_with_gui = false;
    if (argv[c] == std::string("-version")) m_with_gui = false;
  }
  m_qapp = m_with_gui ? new MyQApplication(argc, argv, this) : new QCoreApplication(*argc, argv);
  if (m_with_gui)  {
    popupStartupBanner();

    m_toplevel = new  MainWidget();
#if QT_VERSION < 0x060000
    connect( QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(slot_desktop_workAreaResized(int)) );
#endif
  }
  dispatchPendingEvents();
}

static int ignore = 0;

/* --------------------------------------------------------------------------- */
/* updateLater --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtManager::updateLater( GuiElement::UpdateReason reason ){
//   if( m_updateLater == -1 ){
//     m_updateLater=startTimer(0);
//   }
  if( m_updateLater == -1 ){
    m_updateLater = 0;
    QTimer::singleShot(0, this, SLOT(slot_GuiUpdateEvent()));
  }
  else{
    ++ignore;
  }
}

/* --------------------------------------------------------------------------- */
/* slot_desktop_workAreaResized --                                             */
/* --------------------------------------------------------------------------- */

void GuiQtManager::slot_desktop_workAreaResized(int screen) {
  dynamic_cast<MainWidget*>(m_toplevel)->setMaximumSize(true);

  GuiElementList::iterator it;
  GuiElementList flist;
  GuiElement::findElementType(flist, type_Form);
  for( it = flist.begin(); it != flist.end(); ++it ) {
    if ((*it)->getQtElement()->myWidget())  // is created?
      (*it)->getQtElement()->getQtForm()->setMaximumSize(true);
  }
}

/* --------------------------------------------------------------------------- */
/* slot_GuiUpdateEvent --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtManager::slot_GuiUpdateEvent() {
  update( GuiElement::reason_Process );
  m_updateLater = -1;
}

/* --------------------------------------------------------------------------- */
/* slot_writeToDatapool --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtManager::slot_writeToDatapool(const std::string varnameData, const std::string& data, int maxlen) {
  // write Data
  BUG_DEBUG("stdout: " << data);
  QtDialogProgressBar& progressBar = QtDialogProgressBar::Instance();
  if (!progressBar.myWidget() || progressBar.myWidget()->isHidden()) return;
  DataReference* ref = DataPoolIntens::Instance().getDataReference(varnameData);
  if (ref != 0) {
    if (maxlen > 0) {
      std::string str;
      ref->GetValue(str);
      str += std::string("\n") + data;
      std::vector<std::string> res = split(str, "\n");
      join(res, '\n', str, std::max((int) res.size()-maxlen, 0));
      ref->SetValue(str);
    } else {
      ref->SetValue(data);
    }
    delete ref;
  }
}

/* --------------------------------------------------------------------------- */
/* timerEvent --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtManager::timerEvent( QTimerEvent *event ) {
  if( m_updateLater == event->timerId() ){
    killTimer( m_updateLater );
    m_updateLater = -1;
    update( reason_Process );
  }
  else if(m_updateStylesheet == event->timerId() &&
          !AppData::Instance().HeadlessWebMode()) {
    killTimer( m_updateStylesheet );
    m_updateStylesheet = -1;
    BUG_DEBUG("timerEvent,  m_stylesheetApply: " << m_stylesheetApply);
    if (m_stylesheetApply == STYLESHEET_APPLY_BOTH){
      GuiQtManager::Instance().myWidget()->setStyleSheet(qApp->styleSheet());  // only mainwindow
    } else if (m_stylesheetApply == STYLESHEET_APPLY_SHOWN){
      BUG_DEBUG("timerEvent,  m_stylesheetName: " << m_stylesheetName);
      GuiQtManager::Instance().setStylesheetName( m_stylesheetName );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* createApplication --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtManager::createApplication() {
  BUG(BugGui,"GuiQtManager::createApplication");
  loadResourceFile( AppData::Instance().ResourceFile() );
  m_jobAfterUpdateFormsFunction = JobManager::Instance().getFunction("AFTER_UPDATE_FORMS");
  QApplication* qapp = dynamic_cast<QApplication*>(m_qapp);
  // init gui
  if (qapp) {
    /**/
    if (AppData::Instance().QtGuiStyle().size()) {
      qapp->setStyle( QString::fromStdString(AppData::Instance().QtGuiStyle()) );
    }
    else
      if ( !QApplication::style()->objectName().isEmpty() )
        AppData::Instance().setQtGuiStyle( QApplication::style()->objectName().toStdString() );
    //  popupStartupBanner();
    QFont font =  qapp->font();
    font = QtMultiFontString::getQFont( "@default@", font );
    qapp->setFont( QtMultiFontString::getQFont( "@default@", font ) );
    BUG_DEBUG("setColors::set Font: family["<<font.family().toStdString()<<"] pointSize["<<font.pointSize()<<"]\n");
    //  std::cout << "  FONT fam["<<font.family().toStdString()<<"] ptsize["<<font.pointSize()<<"]\n"<<std::flush;
    // font =  QApplication::font();
    // QApplication::setFont( QtMultiFontString::getQFont( "@default@", font ) );
    /**/
  }
  GuiElementList elist;
  GuiElement::findElementType(elist, type_Main);
  assert( elist.size() == 1);
  elist[0]->create();
}

/* --------------------------------------------------------------------------- */
/* mapApplication --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtManager::mapApplication() {
  return true;
}

/* --------------------------------------------------------------------------- */
/* runApplication --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtManager::runApplication(){
  BUG_DEBUG("GuiQtManager::runApplication");
  // Unter Umständen ist der Datapool bereits mit gültigen Werten gefüllt.
  // Darum muss bereits zu Beginn für die Main-Form ein update durchgeführt werden.
  update( reason_Always );

  destroyStartupBanner();
  InitialWorker::Instance().startJobAction();

  // ganz wichtig wegen dem QScrollView
  // (wir wollen die hintSize als Appl-Groesse)
  //  m_toplevel->adjustSize();
  GuiElementList elist;
  GuiElement::findElementType(elist, type_Main);
  assert( elist.size() == 1);
  dynamic_cast<GuiQtForm*>(elist[0])->setSizeProperty();
  dynamic_cast<GuiQtForm*>(elist[0])->manage();

  if (elist[0]->getQtElement()->getQtDialog()->getQtWindowStates())
    m_toplevel->showMaximized();
  else
    m_toplevel->show();

  IntensServerSocket::startAll();

  // unPolish all Widgerts (since new QStyle)
  QStyle* style =  QApplication::style();
  const QWidgetList  &list = QApplication::allWidgets();
  for (int i = 0; i < list.size(); ++i)
    style->unpolish( list.at(i) );

  if( AppData::Instance().OutputFormat() == "xml" ){
    GuiElementList::iterator it;
    GuiElementList flist, mlist;
    GuiElement::findElementType(flist, type_Form);
    GuiElement::findElementType(mlist, type_Main);
    flist.insert(flist.end(), mlist.begin(), mlist.end());
    for( it = flist.begin(); it != flist.end(); ++it ) {
      GuiForm* gf = (*it)->getForm();
      assert( gf );
      SerializeFormStreamParameter *s = new SerializeFormStreamParameter( gf, "" );
      std::ostringstream os;

      std::string::size_type pos = AppData::Instance().OutputFile(). find_last_of('.');
      std::string filename;
      if ( pos != std::string::npos ) {
	filename = AppData::Instance().OutputFile().substr(0, pos) + "_" + gf->getElement()->getName() + AppData::Instance().OutputFile().substr(pos);
      } else
	filename = gf->getElement()->getName() +"_"+ AppData::Instance().OutputFile();
      if( filename.empty() )
	filename = "out.xml";
      std::ofstream _file( filename.c_str() );
      s->write( os );
      _file << os.str();
      delete s;
      _file.close();
    }

    return true;
  }

  // create other visible Dialogs (see ini file)
  std::vector<std::string> vis_diag;
  QStringList keys = m_settings->childGroups();
  int dlevel = m_settings->value("Intens/SettingDialogLevel", std::numeric_limits<int>::max()).toInt();
  if (!AppData::Instance().HeadlessWebMode() &&
      dlevel >= 2) {  // nur groesser zwei werden alte Dialoge wieder geoeffnet
    m_settings->beginGroup("Dialog");
    keys = m_settings->childKeys();
    for ( QStringList::Iterator it = keys.begin(); it != keys.end(); ++it ) {
      // remove size settings if intens version changed
      if (!GuiQtManager::Instance().hasSettingFileActualVersion())
        if ((*it).endsWith(".size"))
          m_settings->remove((*it));
      // interpret visible flag
      if (!(*it).endsWith(".visible")) continue;
      QString value = m_settings->value((*it), false).toString();
      if (value =="ignore") {
        BUG_DEBUG("ignore visible flag from Dialog named '"<<(*it).toStdString());
        continue;
      }
      bool b= m_settings->value((*it), false).toBool();
      if (b)
        vis_diag.push_back((*it).left( (*it).size()-8 ).toStdString());
    }
    m_settings->endGroup();
    GuiElementList::iterator it;
    GuiElementList flist;
    GuiElement::findElementType(flist, type_Form);
    for( it = flist.begin(); it != flist.end(); ++it ) {
      if (std::find( vis_diag.begin(), vis_diag.end()
                     , dynamic_cast<GuiQtDialog*>((*it)->getDialog())->getDialogName() ) != vis_diag.end()) {
        if ( !dynamic_cast<GuiQtDialog*>((*it)->getDialog())->isApplicationModal() )
          (*it)->manage();
      }
    }
  }

  // read main Settings
  // if (elist.size() == 1) {
  //   dynamic_cast<GuiQtDialog*>(elist[0])->readSettings();
  //   m_toplevel->setVisible(true);
  // }
  if (__debugLogger__) {
    std::ostringstream os;
    printSizeInfo(os, 0, true);
    BUG_DEBUG(">>>>>= MAXIMUM WINDOW SIZES  =<<<<\n" << os.str());
  }

  // Eventloop starten
  m_updateStylesheet=startTimer(500);
  m_qapp->exec();
  return true;
}

/* --------------------------------------------------------------------------- */
/* closeApplication --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiQtManager::closeApplication() {
  writeSettings();
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeSettings --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtManager::writeSettings() {
  if (AppData::Instance().HeadlessWebMode())
    return;

  // write Settings
  m_settings->setValue("Intens.VersionString", QString::fromStdString(AppData::Instance().VersionString()));
  m_settings->setValue("Intens/Stylesheet", QString::fromStdString(m_stylesheetName));

  // write main Settings
  GuiElementList elist;
  GuiElement::findElementType(elist, type_Main);
  if (elist.size() == 1)
    elist[0]->writeSettings();

  //nop??  writeSettings();

  // write dialog settings
  GuiElementList::iterator it;
  GuiElementList flist;
  GuiElement::findElementType(flist, type_Form);
  for( it = flist.begin(); it != flist.end(); ++it )
    (*it)->writeSettings();

  // write folder settings
  flist.clear();
  GuiElement::findElementType(flist, type_Folder);
  for( it = flist.begin(); it != flist.end(); ++it ) {
		if ( ! (*it)->getFolder()->IsHideButton() )
			(*it)->writeSettings();
  }

  // write list settings
  flist.clear();
  GuiElement::findElementType(flist, type_List);
  for( it = flist.begin(); it != flist.end(); ++it ) {
    (*it)->writeSettings();
  }

  // write qwt plot  settings
  flist.clear();
  GuiElement::findElementType(flist, type_QWTPlot);
  for( it = flist.begin(); it != flist.end(); ++it ) {
    (*it)->writeSettings();
  }

  // write qwt3d plot  settings
  flist.clear();
  GuiElement::findElementType(flist, type_3dPlot);
  for( it = flist.begin(); it != flist.end(); ++it ) {
    (*it)->writeSettings();
  }

  // write appdate settings
  AppData::Instance().writeSettings();

  m_settings->sync();
}

bool GuiQtManager::replace( GuiElement *old_el, GuiElement *new_el ) {
  BUG_DEBUG("GuiQtManager::replace: " << old_el->getName() << " => " << new_el->getName());
  if (AppData::Instance().HeadlessWebMode()) {
    JobWebApiResponse::addGuiElementReplaceData(old_el->getMyForm(), old_el, new_el);
  }

  // get QWidgets
  QWidget* old_widget = old_el->getQtElement()->myWidget();
  QWidget* new_widget = new_el->getQtElement()->myWidget();

  // old widget should always be created
  if (!old_widget) {
    BUG_DEBUG("old widget should always be created");
    return false;
  }

  // create new widget if not already created
  if (!new_widget) {
    BUG_DEBUG("create new widget");
    new_el->setParent( old_el->getParent() );
    new_el->getQtElement()->create();
    new_widget = new_el->getQtElement()->myWidget();
  }

  // now, we replace qwidget
  QGridLayout* gridLayout = dynamic_cast<QGridLayout*>(old_widget->parentWidget()->layout());
  QBoxLayout*  boxLayout = dynamic_cast<QBoxLayout*>(old_widget->parentWidget()->layout());

  // replace within gridLayout
  if (gridLayout) {
    int row=0, column=0, rowSpan=0, columnSpan=0;
    int idx = gridLayout->indexOf( old_widget );
    if (idx < 0) {
      BUG_DEBUG("Invalid GridLayout Index");
      return false;
    }
    gridLayout->getItemPosition (idx, &row, &column, &rowSpan, &columnSpan );
    gridLayout->removeWidget(old_widget);
    gridLayout->addWidget(new_widget, row, column, rowSpan, columnSpan);
  } else if (boxLayout) {
    // replace within boxLayout
    int idx = boxLayout->indexOf( old_widget );
    boxLayout->removeWidget(old_widget);
    boxLayout->insertWidget(idx, new_widget);
  } else {
    // replace within simple layout
    QLayout* layout = old_widget->parentWidget()->layout();
    int idx = layout->indexOf( old_widget );
    layout->addWidget(new_widget);
    layout->removeWidget(old_widget);
  }
  old_widget->hide();
  new_widget->show();
  new_el->update( reason_Always );

  // replacement for parent widget
  if (old_el->getParent()->Type() == type_Main ||
      old_el->getParent()->Type() == type_Form ||
      old_el->getParent()->Type() == type_FieldgroupLine ||
      old_el->getParent()->Type() == type_Container) {
    old_el->getParent()->replace( old_el, new_el );
  }

  // set size policy parent
  QWidget* dialogWidget =  new_el->myParent( type_Form )->getQtElement()->myWidget();
  GuiElement::Orientation ed =  new_el->myParent( type_Form )->getQtElement()->getDialogExpandPolicy();
  dialogWidget->setSizePolicy(QSizePolicy(ed & orient_Horizontal ? QSizePolicy::MinimumExpanding :
					  QSizePolicy::Fixed ,
					  ed & orient_Vertical ? QSizePolicy::MinimumExpanding :
					  QSizePolicy::Fixed ) );
#if QT_VERSION < 0x060000
  QRect drect = QApplication::desktop()->availableGeometry(dialogWidget);
#else
  QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#endif
#if defined Q_OS_WIN || defined Q_OS_CYGWIN
  drect.setHeight( drect.height() - 38 );
#endif

  // update geometry
  dialogWidget->update();
  dialogWidget->updateGeometry();

  // hide/show - trick (in of qt 4.8 bug)
  // becourse of later GuiElement justify
  new_widget->hide();
  new_widget->show();
  new_el->manage();

  // get sizeHint from FormScrollView
  QScrollArea *scrollView = dialogWidget->findChild<QScrollArea *>("FormScrollView");
  QList<QScrollArea *> allSB = dialogWidget->findChildren<QScrollArea *>();
  if (!scrollView) {
    return true; // should not happen
  }
  dynamic_cast<GuiQtForm*>(new_el->myParent( type_Form ))->setMaximumSize();
  return true;
}

/* --------------------------------------------------------------------------- */
/* LastGuiUpdate --                                                            */
/* --------------------------------------------------------------------------- */

TransactionNumber GuiQtManager::LastGuiUpdate() {
  if (!m_override_gui_transactions.empty()){
    return m_override_gui_transactions.back();
  }
  return m_gui_transaction;
}

/* --------------------------------------------------------------------------- */
/* LastWebUpdate --                                                            */
/* --------------------------------------------------------------------------- */

TransactionNumber GuiQtManager::LastWebUpdate() {
  return m_web_transaction;
}

/* --------------------------------------------------------------------------- */
/* setWebUpdateTimestamp --                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtManager::setWebUpdateTimestamp() {
  m_web_transaction = DataPoolIntens::NewTransaction();
}

/* --------------------------------------------------------------------------- */
/* overrideLastUpdate --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtManager::overrideLastUpdate(TransactionNumber trans) {
  m_override_gui_transactions.push_back(trans);
}

/* --------------------------------------------------------------------------- */
/* resetOverrideLastUpdate --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtManager::resetOverrideLastUpdate() {
  if (!m_override_gui_transactions.empty())
    m_override_gui_transactions.pop_back();
}

void GuiQtManager::setUpdateAlways() {
}

void GuiQtManager::initErrorBox() {
}

std::ostream *GuiQtManager::getErrorBoxStream() {
  return 0;
}

void GuiQtManager::showErrorBox( GuiElement *el ) {
}

/* --------------------------------------------------------------------------- */
/* popupStartupBanner --                                                       */
/* --------------------------------------------------------------------------- */
void GuiQtManager::popupStartupBanner(){
  assert( m_startup == 0 );
  QtIconManager &iconMgr = QtIconManager::Instance();

  std::string startup_image = AppData::Instance().StartupImage();
  if( !startup_image.empty() ){
    QPixmap pixmap;
    if( iconMgr.getPixmap( startup_image, pixmap) ){
      QSplashScreen* splash = new QSplashScreen(pixmap);
      // 2008-11-21 hpabst wollte das nicht
      //      splash->showMessage( QString::fromStdString(copyright_text), Qt::AlignHCenter); //, Qt::AlignLeft,  m_highlightedTextColor );
      splash->setMask(pixmap.mask());
      m_startup = splash;
      QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
      m_startup->show();
      if (drect.width() > m_startup->width() && drect.height() > m_startup->height())
        m_startup->move((drect.width()-m_startup->width())/2, (drect.height()-m_startup->height())/2 );
      m_startup->show();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* destroyStartupBanner --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtManager::destroyStartupBanner(){
  if( m_startup != 0 ){
    static_cast<QSplashScreen*>(m_startup)->finish( m_toplevel );
    m_startup->deleteLater();
    m_startup = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* showStartupBannerMessage --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtManager::showStartupBannerMessage(const std::string& message){
  if( m_startup != 0 ){
    dynamic_cast<QSplashScreen*>(m_startup)->showMessage(QString::fromStdString(message),
                                                         Qt::AlignHCenter, Qt::red);
    sleep(5);  // 10 sec
  }
}

void GuiQtManager::dispatchPendingEvents() {
  m_qapp -> processEvents();
}

GuiElement* GuiQtManager::getElement()  {
  return this;
}
GuiDialog* GuiQtManager::getDialog()  {
  return this;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtManager::create(){
  std::cerr << "GuiQtManager::create: FATAL ERROR: use createApplication" << std::endl;
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtManager::manage(){
  std::cerr << "GuiQtManager::manage: FATAL ERROR: use runApplication" << std::endl;
  assert( false );
}

void GuiQtManager::getSize( int &w, int &h ) {
  w = h = 0;
}
QWidget*  GuiQtManager::myWidget() {
  return  m_toplevel;
}

/* --------------------------------------------------------------------------- */
/* getPlugin --                                                                */
/* --------------------------------------------------------------------------- */

Plugin *GuiQtManager::getPlugin( const std::string &name ){
  GuiElement *element = GuiElement::findElement( name );
  if( element != 0 ){
    if( element->Type() == type_Plugin ){
      return dynamic_cast<Plugin *>(element);
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createFormMenu --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtManager::createFormMenu( GuiPulldownMenu *menu ){
  GuiElementList::iterator it;
  GuiElementList flist;
  GuiElement::findElementType(flist, type_Form);
  for( it = flist.begin(); it != flist.end(); ++it ) {
    GuiQtForm *form = (*it)->getQtElement()->getQtForm();
    if( !form->MenuInstalled() ){
      GuiButtonListener *listener = form->getButtonListener();
      GuiQtMenuButton *pushbutton = new GuiQtMenuButton( menu->getElement(), listener );
      menu->attach( pushbutton );
      pushbutton->setDialogLabel( form->getTitle() );
      if( form->hasHelptext() ){
	std::string text;
	form->getHelptext( text );
	pushbutton->setHelptext( text );
      }
      form->setMenuInstalled();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getAvailableStylesheets --                                                  */
/* --------------------------------------------------------------------------- */
std::set<std::string> GuiQtManager::getAvailableStylesheets(bool bAbsolutPath) {
  std::set<std::string> list;
  QStringList baseDirs;
  BUG_PARA( BugGuiMgr, "getAvailableStylesheets", "bAbsolutPath: " << bAbsolutPath );

  // get baseDirs where to search for *.qss files
  // search for qss subdirs in $INTENS_HOME, $APPHOME, $HOME
  baseDirs << QString::fromStdString(AppData::Instance().IntensHome()  + QDir::separator().toLatin1() + "qss");
  QString app_home(getenv("APPHOME"));
  if (app_home.size()) {
    QDir dir( app_home + QDir::separator()+ "qss" );
    if (dir.exists())
      baseDirs << dir.absolutePath();
  }
  QDir home_qss(QDir::home().absolutePath()  +QDir::separator()+ "qss");
  if (home_qss.exists())
    baseDirs << home_qss.absolutePath();

  // look in basedir AND also look inside subdirs
  QStringList filters;
  filters << "*.qss";
  QFileInfoList flist;
  for ( QStringList::Iterator it = baseDirs.begin(); it != baseDirs.end(); ++it ) {
    QDir dir( (*it) );
    dir.setNameFilters(filters);
    flist << dir.entryInfoList (filters, QDir::Files);
    BUG_MSG("baseDirs: " <<  (*it).toStdString());

    QStringList dirs = dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);
    for ( QStringList::Iterator sit = dirs.begin(); sit != dirs.end(); ++sit ) {
      QDir sdir( (*it)+QDir::separator()+(*sit) );
      sdir.setNameFilters(filters);
      flist << sdir.entryInfoList (filters, QDir::Files);
    }
  }

  // copy result into result list
  for ( QFileInfoList::Iterator it = flist.begin(); it != flist.end(); ++it ) {
    list.insert( bAbsolutPath ? (*it).absoluteFilePath().toStdString() : (*it).completeBaseName().toStdString() );
  }

  return list;
}

/* --------------------------------------------------------------------------- */
/* setStylesheetName --                                                        */
/* --------------------------------------------------------------------------- */
static const char* resImgName = "images";
static QStringList _spath = QDir::searchPaths ( resImgName ) ;
std::string GuiQtManager::setStylesheetName(const std::string& stylesheet) {
  BUG_DEBUG( "stylesheet: " << stylesheet );
  if (AppData::Instance().HeadlessWebMode()) return "";

  std::set<std::string> list = getAvailableStylesheets(true);
  std::string fname;
  std::set<std::string>::const_iterator it;

  // should ends with ".qss" => maybe append
  QString qStylesheet = QString::fromStdString(stylesheet);
  if (qStylesheet.endsWith(".qss") == false) {
    qStylesheet += ".qss";
  }
  QFile filetest(qStylesheet);
  if (filetest.exists()) {
    fname =  qStylesheet.toStdString();
  } else {
    for (it = list.begin(); it != list.end(); ++it) {
      // absolute patch
      if ((*it)==qStylesheet.toStdString()) {
	fname = (*it);
	break;
      } else
	// ends width
	if (QString::fromStdString(*it).endsWith(qStylesheet)) {
	  fname = (*it);
	  break;
	}
    }
  }
  BUG_DEBUG("Stylesheet: '" << fname << "'");
  if (fname.size()==0) {
    return "";
  }

  QFile file( QString::fromStdString(fname) );
  if (file.exists() && file.open(QFile::ReadOnly)) {
    m_stylesheetName = stylesheet;
    QString styleSheet = file.readAll();
    //    styleSheet.replace(QRegularExpression("url[\(]([^:,^\\.]+\\.)"), "url("+QString(resImgName)+":\\1");
    BUG_DEBUG("QSS String: " << styleSheet.toStdString());

    // set QDir::setSearchPaths for images resources
    QFileInfo fi( QString::fromStdString(fname) );
    QStringList spath(_spath);
    spath << QStringList(fi.absolutePath());
    spath << QString::fromStdString(AppData::Instance().IntensHome());
    spath << QString::fromStdString(AppData::Instance().IntensHome())+QDir::separator()+"qss";
    spath << QString::fromStdString(AppData::Instance().AppHome());
    spath << QString::fromStdString(AppData::Instance().AppHome())+QDir::separator()+"qss";
    QDir::setSearchPaths(resImgName, spath);
    spath = QDir::searchPaths ( resImgName ) ;

    if (stylesheet.size()) {
      qApp->setStyleSheet(styleSheet);
      update( reason_Always );
    }
    file.close();
    return fname;
  }
  BUG_WARN("stylesheet '" << fname << "' not found");
  return "";
}

/* --------------------------------------------------------------------------- */
/* loadResourceFile --                                                         */
/* --------------------------------------------------------------------------- */
bool GuiQtManager::loadResourceFile( const std::string &resfilename ){
  if (!GuiQtManager::Instance().m_with_gui || m_settings) return true;
  BUG_PARA( BugGuiMgr, "loadResourceFile","resfilename:" << resfilename);
  // set QSettings defaults
  AppData &appdata = AppData::Instance();
  QFileInfo rf( QString::fromStdString(appdata.ResourceFile()) );
  QFileInfo df( QString::fromStdString(appdata.DesFile()) );
  QString prog_name(df.baseName());
  QString res_name( rf.baseName().size() ? rf.baseName() : prog_name );
  appdata.setResfile(res_name.toStdString());
  QString app_home(rf.size() ? rf.absolutePath() : "");
  QCoreApplication::setOrganizationName(res_name);
  QCoreApplication::setOrganizationDomain("semafor.ch");
  QCoreApplication::setApplicationName( prog_name );

  // set config directory for User
  if (app_home.size()) {
    // set system scope to whished directory
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, app_home);
    // reset user scope to default directory because it was previously overriden
#if defined Q_OS_WIN || defined Q_OS_CYGWIN
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QString::fromStdString(getenv("APPDATA")));
#else
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QString::fromStdString(getenv("HOME"))+"/.config");
#endif
  }

  if (m_settings) delete m_settings;
  m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                             QCoreApplication::organizationName(),
                             QCoreApplication::applicationName());
  if( m_settings->allKeys().empty() &&
      !AppData::Instance().DisplayVersion() &&
      !AppData::Instance().DisplayHelp() ) {
    BUG_WARN(compose(_("WARNING: Resource file '%1' is empty."), AppData::Instance().ResourceFile() ))
  }
  // get Qt default colors
  QPalette pal=  QApplication::palette();
  m_foregroundColor = getSettingColor(*m_settings, "DefaultColors/foreground", pal.color(QPalette::WindowText).name().toStdString().c_str());
  m_backgroundColor = getSettingColor(*m_settings, "DefaultColors/background", pal.color(QPalette::Window).name().toStdString().c_str());
  m_buttonColor = getSettingColor(*m_settings, "DefaultColors/button", pal.color(QPalette::Button).name().toStdString().c_str());
  m_buttonTextColor = getSettingColor(*m_settings, "DefaultColors/buttonText", pal.color(QPalette::ButtonText).name().toStdString().c_str());
  m_baseColor = getSettingColor(*m_settings, "DefaultColors/base", pal.color(QPalette::Base).name().toStdString().c_str());
  m_textColor = getSettingColor(*m_settings, "DefaultColors/text", pal.color(QPalette::Text).name().toStdString().c_str());
  m_brightTextColor = getSettingColor(*m_settings, "DefaultColors/brightText", pal.color(QPalette::BrightText).name().toStdString().c_str());
  m_highlightColor = getSettingColor(*m_settings, "DefaultColors/highlight", pal.color(QPalette::Highlight).name().toStdString().c_str());
  m_highlightedTextColor = getSettingColor(*m_settings, "DefaultColors/highlightedText", pal.color(QPalette::HighlightedText).name().toStdString().c_str());
  m_linkColor = getSettingColor(*m_settings, "DefaultColors/link", pal.color(QPalette::Link).name().toStdString().c_str());
  m_linkVisitedColor = getSettingColor(*m_settings, "DefaultColors/linkVisited", pal.color(QPalette::LinkVisited).name().toStdString().c_str());
  m_defaultColorPalette = m_settings->value( "DefaultColors/colorPalette", "" ).toStringList();

  // set colorgroups
  pal.setColor(QPalette::Window,          m_backgroundColor );
  pal.setColor(QPalette::WindowText,      m_foregroundColor );

  pal.setColor(QPalette::Button,          m_buttonColor );
  pal.setColor(QPalette::ButtonText,      m_buttonTextColor );

  pal.setColor(QPalette::Base,            m_baseColor );
  pal.setColor(QPalette::Text,            m_textColor );
  pal.setColor(QPalette::BrightText,      m_brightTextColor );

  pal.setColor(QPalette::Highlight,       m_highlightColor );
  pal.setColor(QPalette::HighlightedText, m_highlightedTextColor );

  pal.setColor(QPalette::Link,            m_linkColor );
  pal.setColor(QPalette::LinkVisited,     m_linkVisitedColor );

  // set default values for inactive colorgroups
  pal.setColor(QPalette::Inactive, QPalette::WindowText,      m_foregroundColor.lighter(110) );
  pal.setColor(QPalette::Inactive, QPalette::ButtonText,      m_buttonTextColor.lighter(110) );
  pal.setColor(QPalette::Inactive, QPalette::Text,            m_textColor.lighter(110) );
  pal.setColor(QPalette::Inactive, QPalette::BrightText,      m_brightTextColor.lighter(110) );

  // set default values for disabled colorgroups
  QColor disabledTextColor( m_textColor.lighter(150) );
  if (disabledTextColor == m_textColor) // black is black
    disabledTextColor.setRgb(160,160,160);
  pal.setColor(QPalette::Disabled, QPalette::WindowText,      disabledTextColor);
  pal.setColor(QPalette::Disabled, QPalette::ButtonText,      disabledTextColor);
  pal.setColor(QPalette::Disabled, QPalette::Text,            disabledTextColor );

  QApplication::setPalette(pal);

  // get intens internal colors
  // pal.color(QPalette::).name().toStdString().c_str()
  m_editableForegroundColor = getSettingColor(*m_settings, "Colors/editableForeground", pal.color(QPalette::Text).name().toStdString().c_str());
  m_editableBackgroundColor = getSettingColor(*m_settings, "Colors/editableBackground", pal.color(QPalette::Base).name().toStdString().c_str());
  m_editableLForegroundColor = getSettingColor(*m_settings, "Colors/editableLForeground", pal.color(QPalette::Text).name().toStdString().c_str());
  m_editableLBackgroundColor = getSettingColor(*m_settings, "Colors/editableLBackground", pal.color(QPalette::Base).name().toStdString().c_str());
  m_optionalForegroundColor = getSettingColor(*m_settings, "Colors/optionalForeground", "darkBlue");
  m_optionalBackgroundColor = getSettingColor(*m_settings, "Colors/optionalBackground", "cyan");
  m_optionalLForegroundColor = getSettingColor(*m_settings, "Colors/optionalLForeground", "darkBlue");
  m_optionalLBackgroundColor = getSettingColor(*m_settings, "Colors/optionalLBackground", "magenta");
  m_readonlyForegroundColor = getSettingColor(*m_settings, "Colors/readonlyForeground", pal.color(QPalette::Text).name().toStdString().c_str());
  m_readonlyBackgroundColor = getSettingColor(*m_settings, "Colors/readonlyBackground", pal.color(QPalette::Base).darker(110).name().toStdString().c_str());
  m_readonlyLForegroundColor = getSettingColor(*m_settings, "Colors/readonlyLForeground", pal.color(QPalette::Text).name().toStdString().c_str());
  m_readonlyLBackgroundColor = getSettingColor(*m_settings, "Colors/readonlyLBackground", pal.color(QPalette::Base).darker(110).name().toStdString().c_str());
  m_lockedForegroundColor = getSettingColor(*m_settings, "Colors/lockedForeground", pal.color(QPalette::Text).name().toStdString().c_str());
  m_lockedBackgroundColor = getSettingColor(*m_settings, "Colors/lockedBackground", pal.color(QPalette::Base).darker(110).name().toStdString().c_str());
  m_alarmForegroundColor = getSettingColor(*m_settings, "Colors/alarmForeground", "darkRed");
  m_alarmBackgroundColor = getSettingColor(*m_settings, "Colors/alarmBackground", pal.color(QPalette::Base).darker(50).name().toStdString().c_str());
  m_warnForegroundColor = getSettingColor(*m_settings, "Colors/warnForeground", "darkBlue");
  m_warnBackgroundColor = getSettingColor(*m_settings, "Colors/warnBackground", pal.color(QPalette::Base).darker(50).name().toStdString().c_str());
  m_infoForegroundColor = getSettingColor(*m_settings, "Colors/infoForeground", "darkBlue");
  m_infoBackgroundColor = getSettingColor(*m_settings, "Colors/infoBackground", pal.color(QPalette::Base).darker(80).name().toStdString().c_str());
  m_helpForegroundColor = getSettingColor(*m_settings, "Colors/helpForeground", "darkBlue");
  m_helpBackgroundColor = getSettingColor(*m_settings, "Colors/helpBackground", pal.color(QPalette::Base).darker(80).name().toStdString().c_str());
  m_indexForegroundColor = getSettingColor(*m_settings, "Colors/indexForeground", "black");
  m_indexBackgroundColor = getSettingColor(*m_settings, "Colors/indexBackground", pal.color(QPalette::Window).darker(80).name().toStdString().c_str());
  m_imageLowerAlarmColor = getSettingColor(*m_settings, "Colors/imageLowerAlarm", "blue");
  m_imageUpperAlarmColor = getSettingColor(*m_settings, "Colors/imageUpperAlarm", "blue");
  m_imageHighlightColor = getSettingColor(*m_settings, "Colors/imageHighlight", "blue");
  m_color1ForegroundColor = getSettingColor(*m_settings, "Colors/color1Foreground", "yellow");
  m_color1BackgroundColor = getSettingColor(*m_settings, "Colors/color1Background", "black");
  m_color2ForegroundColor = getSettingColor(*m_settings, "Colors/color2Foreground", "red");
  m_color2BackgroundColor = getSettingColor(*m_settings, "Colors/color2Background", "darkRed");
  m_color3ForegroundColor = getSettingColor(*m_settings, "Colors/color3Foreground", "black");
  m_color3BackgroundColor = getSettingColor(*m_settings, "Colors/color3Background", "darkRed");
  m_color4ForegroundColor = getSettingColor(*m_settings, "Colors/color4Foreground", "blue");
  m_color4BackgroundColor = getSettingColor(*m_settings, "Colors/color4Background", "darkBlue");
  m_color5ForegroundColor = getSettingColor(*m_settings, "Colors/color5Foreground", "cyan");
  m_color5BackgroundColor = getSettingColor(*m_settings, "Colors/color5Background", "darkCyan");
  m_color6ForegroundColor = getSettingColor(*m_settings, "Colors/color6Foreground", "magenta");
  m_color6BackgroundColor = getSettingColor(*m_settings, "Colors/color6Background", "darkMagenta");
  m_color7ForegroundColor = getSettingColor(*m_settings, "Colors/color7Foreground", "yellow");
  m_color7BackgroundColor = getSettingColor(*m_settings, "Colors/color7Background", "white");
  m_color8ForegroundColor = getSettingColor(*m_settings, "Colors/color8Foreground", pal.color(QPalette::Text).name().toStdString().c_str());
  m_color8BackgroundColor = getSettingColor(*m_settings, "Colors/color8Background", pal.color(QPalette::Base).darker(110).name().toStdString().c_str());
  m_fieldgroupForegroundColor = getSettingColor(*m_settings, "Colors/fieldgroupForeground", pal.color(QPalette::WindowText).name().toStdString().c_str());
  m_fieldgroupBackgroundColor = getSettingColor(*m_settings, "Colors/fieldgroupBackground", pal.color(QPalette::Window).name().toStdString().c_str());
  m_fieldgroupShadow = getSettingColor(*m_settings, "Colors/fieldgroupShadow", QString::fromStdString(m_fieldgroupShadow)).name().toStdString();
  m_navItemBorderColor = getSettingColor(*m_settings, "Colors/navItemBorderColor", "black");
  m_navItemBorderWidth = m_settings->value("Navigator/navItemBorderWidth", 0).toInt();
  m_listItemHeight = m_settings->value("List/listItemHeight", 0).toInt();

  // set Stylesheet if set in resource in resfile
  m_stylesheetName = m_settings->value( "Intens/Stylesheet", "default").toString().remove(".qss").toStdString();
  // StylesheetApply
  m_stylesheetApply = m_settings->value( "Intens/StylesheetApply", QString::fromStdString(m_stylesheetApply)).toString().toStdString();
  //   validate the value from the resources
  if (!(m_stylesheetApply == GuiQtManager::STYLESHEET_APPLY_INIT ||
        m_stylesheetApply == GuiQtManager::STYLESHEET_APPLY_SHOWN ||
        m_stylesheetApply == GuiQtManager::STYLESHEET_APPLY_BOTH)
      ){
    // unexpected value, use default instead
    m_stylesheetApply = GuiQtManager::STYLESHEET_APPLY_INIT;
  }
  if (m_stylesheetApply == STYLESHEET_APPLY_INIT || m_stylesheetApply == STYLESHEET_APPLY_BOTH)
    GuiQtManager::Instance().setStylesheetName( m_stylesheetName );
  BUG_DEBUG("read qss styleseheet : " << m_stylesheetName << ", styleseheetApply: " << m_stylesheetApply);

  // read Intens Version (of Intens, who previously wrote settings file)
  m_prevSettingFileVersion = m_settings->value("Intens.VersionString", "").toString().toStdString();

  return true;
}

/* --------------------------------------------------------------------------- */
/* readResourceFile --                                                         */
/* --------------------------------------------------------------------------- */
QColor GuiQtManager::readResourceColor( const std::string &name ){
  QColor color;
  std::string resname( "Colors/" + name );
  QString colname( m_settings->value( resname.c_str(), "" ).toString() );
#if QT_VERSION >= 0x060000
  color.fromString( colname );
#else
  color.setNamedColor( colname );
#endif
  return color;
}

/* --------------------------------------------------------------------------- */
/* writeResourceFile --                                                        */
/* --------------------------------------------------------------------------- */
bool GuiQtManager::writeResourceFile(){
  if (!AppData::Instance().createResFile())
    return false;

#ifdef Q_OS_UNIX
  BUG_DEBUG(compose(_("Writing resource file....[%1]"), AppData::Instance().newResFileName()))

  QFileInfo fi( QString::fromStdString(AppData::Instance().newResFileName()) );
  QString existFileName("");
  if( resFileExists( existFileName ) &&
      QMessageBox::question
      ( Instance().Toplevel(),
        _("Overwrite File? --  Create new Resource File"),
        QString::fromStdString(compose(_("Creating new resource file.\n\nFile '%1' already exists.\nDo you want to overwrite it?"),
                                       existFileName.toStdString())),
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::Yes)
      )
    {
      QString newFile = QtDialogFileSelection::getSaveFileName
        ( fi.absolutePath(),
          "Res files (*rc)",
          qApp->activeWindow(),
          _("save file dialog\nChoose a filename to save resources")
          );
      if(newFile.isEmpty()){   // Cancel in FileSelectDialog
        BUG_WARN("No new resource file written!");
        return false;
      }
      else{
        AppData::Instance().setNewResFileName(newFile.toStdString());
        writeResourceFile();
        return true;
      }
    }

  QString settingsRoot = fi.fileName();
  settingsRoot.replace( QRegularExpression("rc$"), "" );
  //  QSettings::setPath( QSettings::IniFormat,  QSettings::UserScope, settingsRoot);
  QSettings settings( settingsRoot, QSettings::IniFormat);
#elif defined Q_OS_WIN || defined Q_OS_CYGWIN
  QFileInfo fi( QString::fromStdString(AppData::Instance().newResFileName()) );
  if (AppData::Instance().newResFileName().empty())
    return false;

  // set setting root
  QString settingsRoot = QString::fromStdString(AppData::Instance().newResFileName());
  QSettings settings( settingsRoot, QSettings::IniFormat);

  // test if resource entry exists
  std::string testString("shouldnotexist");
  std::string resString( settings.value( settingsRoot + "Colors/foreground", QString::fromStdString(testString) ).toString().toStdString() );
  if (resString != testString) {
    BUG_WARN("WARNING  Registry Entry Colors/foreground already exists");
    return false;
  }

#endif
  BUG_DEBUG("Create File ["<<fi.fileName().toStdString()<<"]["<<settings.fileName().toStdString()<<"]");
  BUG_DEBUG("Create File ["<<AppData::Instance().newResFileName()<<"]["
           <<  fi.absoluteFilePath().toStdString() <<"]");

  settings.setValue( "Intens/apptitle.text", "I N T E N S" );
  settings.setValue( "Intens/appsubtitle.text", "" );
  settings.setValue( "Intens/ApplicationIcon", "" );
  settings.setValue( "Intens/Copyright.text", "" );
  settings.setValue( "Intens/Stylesheet", "");
  settings.setValue( "Intens/SettingDialogLevel", "" );
  settings.setValue( "Intens/SettingPlot2DLevel", 2 );

  const QPalette  pal = QApplication::palette();
  // qt deafaults
  settings.setValue( "DefaultColors/foreground",      pal.color(QPalette::WindowText).name() );
  settings.setValue( "DefaultColors/background",      pal.color(QPalette::Window).name() );
  settings.setValue( "DefaultColors/buttonText",      pal.color(QPalette::ButtonText).name() );
  settings.setValue( "DefaultColors/button",          pal.color(QPalette::Button).name() );

  settings.setValue( "DefaultColors/base",            pal.color(QPalette::Base).name() );
  settings.setValue( "DefaultColors/text",            pal.color(QPalette::Text).name() );
  settings.setValue( "DefaultColors/brightText",      pal.color(QPalette::BrightText).name() );

  settings.setValue( "DefaultColors/highlight",       pal.color(QPalette::Highlight).name() );
  settings.setValue( "DefaultColors/highlightedText", pal.color(QPalette::HighlightedText).name() );

  settings.setValue( "DefaultColors/link",            pal.color(QPalette::Link).name() );
  settings.setValue( "DefaultColors/linkVisited",     pal.color(QPalette::LinkVisited).name() );

  // others
  settings.setValue( "Colors/editableForeground", pal.color(QPalette::Text).name() );
  settings.setValue( "Colors/editableBackground", pal.color(QPalette::Base).name() );
  settings.setValue( "Colors/editableLForeground", pal.color(QPalette::Text).name() );
  settings.setValue( "Colors/editableLBackground", pal.color(QPalette::Base).name() );
  settings.setValue( "Colors/optionalForeground", "darkBlue" );
  settings.setValue( "Colors/optionalBackground", "cyan" );
  settings.setValue( "Colors/optionalLForeground", "darkBlue" );
  settings.setValue( "Colors/optionalLBackground", "magenta" );
  settings.setValue( "Colors/readonlyForeground", pal.color(QPalette::Text).name() );
  settings.setValue( "Colors/readonlyBackground", pal.color(QPalette::Base).darker(110).name() );
  settings.setValue( "Colors/readonlyLForeground", pal.color(QPalette::Text).name() );
  settings.setValue( "Colors/readonlyLBackground", pal.color(QPalette::Base).darker(110).name() ); //"lightGray" );
  settings.setValue( "Colors/lockedForeground", pal.color(QPalette::Text).name() );
  settings.setValue( "Colors/lockedBackground", pal.color(QPalette::Base).darker(110).name() );
  settings.setValue( "Colors/alarmForeground", "darkRed" );
  settings.setValue( "Colors/alarmBackground", pal.color(QPalette::Base).darker(50).name() );
  settings.setValue( "Colors/warnForeground", "darkBlue" );
  settings.setValue( "Colors/warnBackground", pal.color(QPalette::Base).darker(50).name() );
  settings.setValue( "Colors/infoForeground", "darkBlue" );
  settings.setValue( "Colors/infoBackground", pal.color(QPalette::Base).darker(80).name() );
  settings.setValue( "Colors/helpForeground", "darkBlue" );
  settings.setValue( "Colors/helpBackground", pal.color(QPalette::Base).darker(80).name() );
  settings.setValue( "Colors/indexForeground", "black" );
  settings.setValue( "Colors/indexBackground", pal.color(QPalette::Window).darker(80).name() );
  settings.setValue( "Colors/color1Foreground", "yellow" );
  settings.setValue( "Colors/color1Background", "black" );
  settings.setValue( "Colors/color2Foreground", "red" );
  settings.setValue( "Colors/color2Background", "darkRed" );
  settings.setValue( "Colors/color3Foreground", "black" );
  settings.setValue( "Colors/color3Background", "darkRed" );
  settings.setValue( "Colors/color4Foreground", "blue" );
  settings.setValue( "Colors/color4Background", "darkBlue" );
  settings.setValue( "Colors/color5Foreground", "cyan" );
  settings.setValue( "Colors/color5Background", "darkCyan" );
  settings.setValue( "Colors/color6Foreground", "magenta" );
  settings.setValue( "Colors/color6Background", "darkMagenta" );
  settings.setValue( "Colors/color7Foreground", "yellow" );
  settings.setValue( "Colors/color7Background", "white" );
  settings.setValue( "Colors/color8Foreground", pal.color(QPalette::Text).name() );
  settings.setValue( "Colors/color8Background", pal.color(QPalette::Base).darker(110).name() );

  settings.setValue( "Colors/fieldgroupForeground", pal.color(QPalette::WindowText).name() );
  settings.setValue( "Colors/fieldgroupBackground", pal.color(QPalette::Window).name() );
  settings.setValue( "Colors/fieldgroupShadow", QString::fromStdString(m_fieldgroupShadow) );

  QtMultiFontString::writeResourceFile( settings );

  return true;
}

/* --------------------------------------------------------------------------- */
/* getColorName --                                                             */
/* --------------------------------------------------------------------------- */

QString GuiQtManager::getColorName( const QString &col ){

  QStringList list = QColor::colorNames();
  int index = list.indexOf( col );
  if( index == -1 ){
    BUG_WARN(compose(_("Named color '%1' not found. Setting it to '#000000' (black)."),col.toStdString() ));
    return "#000000";
  }

  return col;
}

/* --------------------------------------------------------------------------- */
/* getSettingColor --                                                          */
/* --------------------------------------------------------------------------- */

QColor GuiQtManager::getSettingColor(QSettings& settings, const char* setting_name, const QString& default_color_name) {
  QString color_name (settings.value(setting_name, default_color_name).toString());
#if QT_VERSION >= 0x060000
  return QColor::fromString(color_name);
#else
  return QColor(color_name);
#endif
}

/* --------------------------------------------------------------------------- */
/* stylesheetContains --                                                       */
/* --------------------------------------------------------------------------- */
bool GuiQtManager::stylesheetContains(const std::string qclass){
  static std::map<std::string, bool> map;
  std::map<std::string, bool> ::iterator it = map.find(qclass);
  if (it != map.end()) {
    return (*it).second;
  }
  QString qss = qApp->styleSheet();
  map[qclass] = qss.contains(QString::fromStdString(qclass));
  return map[qclass];
}

/* --------------------------------------------------------------------------- */
/* resFileExists --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtManager::resFileExists( QString &foundFile ){

  // newResFileName()   used by option --createRes '...'

  QString file( QString::fromStdString(AppData::Instance().newResFileName()) );
  QString fileRc = file + "rc";
  QFileInfo fi( file );
  QFileInfo fi2( fileRc );
  QFileInfo fi3( file.toLower() );
  QFileInfo fi4( fileRc.toLower() );

  foundFile = "";
  if( fi.exists() ){
    foundFile = fi.fileName();
    return true;
  }
  if( fi2.exists() ){
    foundFile = fi2.fileName();
    return true;
  }
  if( fi3.exists() ){
    foundFile = fi3.fileName();
    return true;
  }
  if( fi4.exists() ){
    foundFile = fi4.fileName();
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* insertToWaitCursorList --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtManager::insertToWaitCursorList( QObject* o ){
  s_waitCursorList.insert( o );
}

/* --------------------------------------------------------------------------- */
/* removeWaitCursorsFromList --                                                */
/* --------------------------------------------------------------------------- */

void GuiQtManager::removeWaitCursorsFromList(){
  std::set<QObject*>::iterator iter;
  for( iter = s_waitCursorList.begin(); iter != s_waitCursorList.end(); ++iter ){
    assert( false );
  }
  s_waitCursorList.clear();
}

/* --------------------------------------------------------------------------- */
/* setWaitingModalDialog --                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtManager::setWaitingModalDialog(bool activate){
  m_waitingModalDialog = activate;
}

/* --------------------------------------------------------------------------- */
/* isWaitingModalDialog --                                                     */
/* --------------------------------------------------------------------------- */

bool GuiQtManager::isWaitingModalDialog(){
  return m_waitingModalDialog;
}

/* --------------------------------------------------------------------------- */
/* getWaitingWidget --                                                         */
/* --------------------------------------------------------------------------- */

QWidget *GuiQtManager::getWaitingWidget(){
  return m_waitingWidget;
}

/* --------------------------------------------------------------------------- */
/* setWaitingWidget --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiQtManager::setWaitingWidget( bool set, QWidget *w ){
  if( set ){
    // In seltenen Fällen führt das zu einem Absturz. Die Kontrolle scheint
    // unnötig zu sein. Nur der Result scheint wichtig zu sein. Für die Funktion
    // getWaitungWidget() interessiert sich zur Zeit niemand. (20.11.2010/bh)
    // assert( m_waitingWidget == 0 || m_waitingWidget == w );
    m_waitingWidget = w;
    return true;
  }
  else
  if( m_waitingWidget == w || w == 0 ){
    m_waitingWidget = 0;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getActiveWindow --                                                          */
/* --------------------------------------------------------------------------- */

QWidget *GuiQtManager::getActiveWindow(){
  QWidget *activeWindow = qApp->activeWindow();
  // if QtDialogWorkClock => clear activeWindow
  if (activeWindow &&
      activeWindow->objectName().toStdString() == QtDialogWorkClock::ObjectName)
    activeWindow = 0;
  return activeWindow;
}

/* --------------------------------------------------------------------------- */
/* printSizeInfo --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtManager::printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds){
  QSize s = m_toplevel->sizeHint();
  int i(0);
  GuiElementList mlist;
  GuiElement::findElementType(mlist, type_Main);
  (*mlist.begin())->printSizeInfo(os, ++intent, onlyMaxChilds);
}

/* --------------------------------------------------------------------------- */
/* runJobAfterUpdateFormsFunction --                                           */
/* --------------------------------------------------------------------------- */

void GuiQtManager::runJobAfterUpdateFormsFunction( JobElement::CallReason reason){
  BUG_DEBUG("runJobAfterUpdateFormsFunction Started");

  if( m_jobAfterUpdateFormsFunction == 0 ){
    BUG_DEBUG("no Job available");
    return;
  }
  //  setRunningMode();
  JobStarter *trigger = new Trigger( this, m_jobAfterUpdateFormsFunction );
  trigger->setName( "GuiManager-Trigger-" + m_jobAfterUpdateFormsFunction->Name() );
  trigger->setReason( reason );
  trigger->startJob( true );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtManager::serializeXML(std::ostream &os, bool recursive){
  GuiManager::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtManager::Trigger::backFromJobStarter( JobAction::JobResult result  ){
  //m_manager->doEndOfWork( result != JobAction::job_Ok, getReason() );
  if (getReason() == JobElement::cll_GuiUpdate) {
    m_manager->m_toplevel->blockSignals(false);
  }
  BUG_DEBUG("runJobAfterUpdateFormsFunction Ended");
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtManager::getDialogExpandPolicy() {
  GuiElementList elist;
  GuiElement::findElementType(elist, type_Main);
  assert( elist.size() == 1);
  return elist[0]->getQtElement()->getDialogExpandPolicy();
}

/* --------------------------------------------------------------------------- */
/* loadPlugin --                                                               */
/* --------------------------------------------------------------------------- */

SmartPluginInterface* GuiQtManager::loadPlugin(const std::string& plugin_name) {

  // search inside directories (${INTENS_HOME}/plugins[/.libs], ${APPHOME}/plugins[/.libs]) for a plugin named 'plugin_name'
  std::vector<std::string> dirs;
  if (AppData::Instance().IntensHome().size())
    dirs.push_back(AppData::Instance().IntensHome());
  if (AppData::Instance().AppHome().size())
    dirs.push_back(AppData::Instance().AppHome());
  for (std::vector<std::string>::const_iterator it = dirs.begin();
       it != dirs.end(); ++it) {
    std::vector<QString> dirsVec;
    dirsVec.push_back(QString::fromStdString(*it)+QDir::separator()+"plugins");
    dirsVec.push_back(QString::fromStdString(*it)+QDir::separator()+"plugins/.libs");
    for (std::vector<QString>::const_iterator dit = dirsVec.begin();
	 dit != dirsVec.end(); ++dit) {
      QDir pluginsDir = QDir(*dit);
      foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
	if (QFileInfo(fileName).baseName().toStdString() != plugin_name ||
	    QLibrary::isLibrary(fileName) == false)
	  continue;
	QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
	QObject *plugin = loader.instance();
	if (!plugin || !loader.isLoaded())
	  BUG_ERROR("Cannot load plugin '"<< plugin_name <<"' ErrorMessage["<<loader.errorString().toStdString()<<"]");
	if (plugin) {
	  SmartPluginInterface *iFilter = qobject_cast<SmartPluginInterface *>(plugin);
	  if (iFilter) {
	    return iFilter;
	  }
	}
      }
    }
  }
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* hasSettingFileActualVersion --                                              */
/* --------------------------------------------------------------------------- */

bool  GuiQtManager::hasSettingFileActualVersion() {
  return m_prevSettingFileVersion == AppData::Instance().VersionString() ? true : false;
}


/* --------------------------------------------------------------------------- */
/* isWindowsInDarkMode --                                                      */
/* --------------------------------------------------------------------------- */

bool GuiQtManager::isWindowsInDarkMode() {
#if defined Q_OS_WIN
    // Access registry: HKEY_CURRENT_USER
    QSettings settings(R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
                       QSettings::NativeFormat);

    // Check if key exists
    if (!settings.contains("AppsUseLightTheme")) {
        // Fallback: assume light mode
        return false;
    }

    // Read value: 0 = dark, 1 = light
    int lightTheme = settings.value("AppsUseLightTheme").toInt();

    return (lightTheme == 0); // True if dark mode
#endif
    return false;
}
