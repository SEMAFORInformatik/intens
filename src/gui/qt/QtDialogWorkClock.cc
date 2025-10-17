
#include <qstringlist.h>
#include <qsettings.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qobject.h>
#include <qfont.h>
#include <QGridLayout>
#include <QCheckBox>
#include <QTime>
#include <QMap>
#include <QCloseEvent>
#include <QMessageBox>
#include <qwt_compass.h>
#include <qwt_compass_rose.h>
#include <qwt_dial_needle.h>
#include <qwt_analog_clock.h>
#include "utils/utils.h"
#include "gui/Timer.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtDialogWorkClock.h"

class QtDialogWorkClock;

QtDialogWorkClock_QObject *QtDialogWorkClock::s_object = 0;
QtDialogWorkClock *QtDialogWorkClock::s_this = 0;
std::string QtDialogWorkClock::ObjectName = "DialogWorkClock";

class QtDialogWorkClock_myDialog : public QDialog {
public:
  QtDialogWorkClock_myDialog(QWidget *parent=0, bool cancelButton=true)
    : QDialog(parent, Qt::Dialog)
    , m_button( 0 )
    , m_closeCB( 0 )
  {
    setWindowTitle( _("Progress Dialog") );
    setObjectName(QString::fromStdString(QtDialogWorkClock::ObjectName));
    m_text= new QLabel( this );
    m_elTime= new QLabel( this );
    if ( cancelButton ) {
      m_button= new QPushButton( _("&Cancel"), this );
      m_button->setAutoDefault( false );
      m_closeCB = new QCheckBox( _("Close at the end") );
    }

    // set font
    QFont font = QDialog::font();
    font = QtMultiFontString::getQFont( "@bold@", font );
    QDialog::setFont( font );
    font = QtMultiFontString::getQFont( "@text@", font );
    m_text->setFont( font );
    m_elTime->setFont( font );
    if (m_closeCB) m_closeCB->setFont( font );
    font = QtMultiFontString::getQFont( "@button@", font );
    if ( m_button ) m_button->setFont( font );

    m_comp= new QwtCompass();
    m_comp->setRose( new QwtSimpleCompassRose() );
    m_comp->setNeedle( new QwtCompassWindArrow(QwtCompassWindArrow::Style2, Qt::yellow, Qt::blue) );
    QMap<double, QString> map;
    map[0] = "0";
#if QWT_VERSION < 0x060100
    map[m_comp->maxValue()/4.] = "3";
    map[m_comp->maxValue()/2.] = "6";
    map[m_comp->maxValue()*3./4.] = "9";
    m_comp->setLabelMap(map);
#else
    if (dynamic_cast<QwtCompassScaleDraw*>(m_comp->scaleDraw())) {
    map[m_comp->upperBound()/4.] = "3";
    map[m_comp->upperBound()/2.] = "6";
    map[m_comp->upperBound()*3./4.] = "9";
    dynamic_cast<QwtCompassScaleDraw*>(m_comp->scaleDraw())->setLabelMap(map);
    }
#endif
    m_comp->setMinimumSize(120,120);

    // Layout ui components
    QGridLayout* layout = new QGridLayout();
    layout->setSpacing( 10 );
    layout->setContentsMargins(10,10,10,10);
    layout->addWidget( m_comp, 0, 0, Qt::AlignCenter);
    layout->addWidget( m_elTime, 1, 0, Qt::AlignCenter);
    layout->addWidget(m_text, 2, 0, Qt::AlignCenter);
    if ( m_button )
      layout->addWidget( m_button, 3, 0, Qt::AlignCenter);
    if (m_closeCB)
      layout->addWidget( m_closeCB, 5, 0, Qt::AlignCenter);

    setLayout( layout );
    setMaximumSize( sizeHint() );

    setModal( false );
  }
  virtual ~QtDialogWorkClock_myDialog(){
  }

  // enterEvent -> set modal TRUE
  void enterEvent ( QEnterEvent * event ) {
    GuiQtManager::Instance().setWaitingModalDialog( true );
    setModal(true);
    QDialog::enterEvent(event);
  }

  // leaveEvent -> set modal FALSE
  void leaveEvent ( QEvent * event ) {
    GuiQtManager::Instance().setWaitingModalDialog( false );
    setModal(false);
    QDialog::leaveEvent(event);
  }
  QPushButton *CancelButton(){
    return m_button;
  }
  QCheckBox *CloseCheckBox(){
    return m_closeCB;
  }

  virtual void show() {
    m_startTime = QTime::currentTime();
    m_text->setText( _("Working, please wait...") );
    if ( m_button )
      m_button->setText( _("&Cancel") );
    if (m_closeCB)
      m_closeCB->setCheckState(Qt::Checked);
    QDialog::show();
  }

  virtual bool close() {

    char *s= _("Work finished.");
    if (m_closeCB && m_closeCB->checkState() != Qt::Checked &&
	m_text->text() != s) {
      m_text->setText( s );
      if ( m_button )
	m_button->setText( _("&Close") );
      return false;
    } else
      QDialog::hide();
    return true;
  }

  void setElapsedTime(QTime& time, QString format) {
    m_elTime->setText( "Elapsed Time: " + time.toString( format ) );
  }

  void update() {
    int el =  m_startTime.secsTo(QTime::currentTime());
    int sec = el % 60;
    el -= sec;
    int hour = el / 3600;
    el -= hour*3600;
    int min = el / 60;
    QTime time(hour, min, sec);
    if (hour)
      setElapsedTime( time, "hh:mm:ss" );
    else
      if (min)
	setElapsedTime( time, "mm:ss" );
      else
	setElapsedTime( time, "m:ss" );

    // set compass value
    int step = 60;
#if QWT_VERSION < 0x060100
    double fac = m_comp->maxValue()/step;
#else
    double fac = m_comp->upperBound()/step;
#endif
    m_comp->setValue( (sec%step)*fac  );
  }
protected:
  virtual void closeEvent( QCloseEvent* e){
    e->ignore();
    QMessageBox::information( this, "Close dialog",
			      _("Unable to close dialog from title bar.") );
  }

 private:
  QLabel       *m_text;
  QPushButton  *m_button;

  QLabel         *m_elTime;
  QwtCompass     *m_comp;
  QTime           m_startTime;
  QCheckBox      *m_closeCB;
};

/*=============================================================================*/
/* Constructor / Destructor of QtDialogWorkClock                               */
/*=============================================================================*/

QtDialogWorkClock::QtDialogWorkClock( DialogWorkClockListener *listener )
  : DialogWorkClock( listener ){
  if( s_object == 0 ){
    s_object = new QtDialogWorkClock_QObject( this );
  }
  else{
    s_object->setWorkClock( this );
  }
  s_this = this;
}

QtDialogWorkClock::~QtDialogWorkClock(){
  s_this = 0;
}

/* --------------------------------------------------------------------------- */
/* setMessage --                                                               */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock:: setMessage( const std::string &msg ){
  if( s_object != 0 )
    s_object->setMessage( msg );
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock::manage(){
  if( s_object == 0 ) {
    s_object = new QtDialogWorkClock_QObject( this );
  }
  s_object->manage();
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock::unmanage(){
  if( s_object != 0 ) {
    s_object->unmanage();
  }
  s_object = 0;
}

/* --------------------------------------------------------------------------- */
/* forceUnmap --                                                               */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock::forceUnmap(){
  if( s_this != 0 ) {
    s_this->unmanage();
  }
  s_this = 0; // to be sure
}

/* --------------------------------------------------------------------------- */
/* slotCancelButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock::slotCancelButtonPressed(){
  cancelButtonPressed();
}

/*=============================================================================*/
/* Constructor / Destructor of QtDialogWorkClock_QObject                       */
/*=============================================================================*/

QtDialogWorkClock_QObject::QtDialogWorkClock_QObject( QtDialogWorkClock *workclock )
  : m_timerID( 0 )
  , m_workclock( workclock )
  , m_myDialog( 0 ){
}

QtDialogWorkClock_QObject::~QtDialogWorkClock_QObject(){
}

// --------------------------------------------------------------------------- */
// setWorkClock --                                                             */
// --------------------------------------------------------------------------- */

void QtDialogWorkClock_QObject::setWorkClock( QtDialogWorkClock *workclock ){
  m_workclock = workclock;
}

// --------------------------------------------------------------------------- */
// timerEvent --                                                               */
// --------------------------------------------------------------------------- */

void QtDialogWorkClock_QObject::timerEvent( QTimerEvent *e ){
  // refresh elapsed time
  m_myDialog->update();
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock_QObject::manage(){
  if (m_myDialog && m_myDialog->isVisible()) return;
  m_timerID = QObject::startTimer( 1000 );
  if( m_myDialog == 0 ){

    m_myDialog = new QtDialogWorkClock_myDialog( qApp->activeWindow(), m_workclock->getListener() != NULL );
    if ( m_myDialog->CancelButton() )
      connect( m_myDialog->CancelButton(), SIGNAL(clicked(bool))
	       , this, SLOT(slotCancelButtonPressed(bool))
	       );
  }
  if(m_myDialog->CancelButton())
    m_myDialog->CancelButton()->setDisabled(false);
  m_myDialog->show();
  GuiQtManager::Instance().setWaitingModalDialog( false );
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock_QObject::unmanage(){
  if (!m_myDialog || m_myDialog->isHidden()) return;
  killTimer( m_timerID );
  m_timerID = 0;
  if (m_myDialog->close()) {
    // sicherer, wir löschen das (kann verschiedene Parents haben)
    m_myDialog->deleteLater();
    m_myDialog = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* slotCancelButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void QtDialogWorkClock_QObject::slotCancelButtonPressed(bool s){
  // set button disabled => prevent mutiple call
  m_myDialog->CancelButton()->setDisabled(true);
  // set checkbox checked => really hide dialog
  m_myDialog->CloseCheckBox()->setCheckState(Qt::Checked);
  if (m_timerID)
    m_workclock->slotCancelButtonPressed();
  else {
    m_myDialog->hide();
    // sicherer, wir löschen das (kann verschiedene Parents haben)
    m_myDialog->deleteLater();
    m_myDialog = 0;
  }
}
