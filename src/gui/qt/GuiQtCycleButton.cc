
/* QT headers */
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QImage>
#include <QString>

#include "utils/Debugger.h"

#include "gui/qt/GuiQtCycleButton.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/QtMultiFontString.h"

#include "utils/gettext.h"
#include "job/InitialWorker.h"
#include "gui/GuiButtonListener.h"

#include "job/JobManager.h"
#include "job/JobStackDataInteger.h"
#include "app/DataPoolIntens.h"
#include "app/AppData.h"

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

QWidget* GuiQtCycleButton::myWidget(){
  return m_cyclewidget;
}

/* --------------------------------------------------------------------------- */
/* addTabGroup --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::addTabGroup(){
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::create(){
  BUG_DEBUG("GuiQtCycleButton::create");

  m_cyclewidget = new QWidget( getParent()->getQtElement()->myWidget() );

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addSpacing(20);
  m_labelwidget = new QLabel( "Case Label" );
  layout->addWidget( m_labelwidget, 1 );
  layout->addSpacing(5);

  QPixmap arrowLeft, arrowRight;
  QtIconManager::Instance().getPixmap( "left", arrowLeft );
  QtIconManager::Instance().getPixmap( "right", arrowRight );

  // Arrow Down
  QPushButton *arrow_down = new QPushButton();
  arrow_down -> setIcon( QIcon(arrowLeft) );
  arrow_down -> setAutoDefault( false );
  QObject::connect( arrow_down, SIGNAL(clicked()), this, SLOT( cycleDown() ) );
  layout->addWidget( arrow_down );

  // Arrow Up
  QPushButton *arrow_up = new QPushButton();
  arrow_up -> setIcon( QIcon(arrowRight) );
  arrow_up -> setAutoDefault( false );
  QObject::connect( arrow_up, SIGNAL(clicked()), this, SLOT( cycleUp() ) );
  layout->addWidget( arrow_up );

  // Button New Cycle
  QPushButton *newCycle = new QPushButton( _("New") );
  newCycle -> setAutoDefault( false );
  QObject::connect( newCycle, SIGNAL(clicked()), this, SLOT( cycleNew() ) );
  layout->addWidget( newCycle , 5 );
  layout->addSpacing(10);

  // set button font
  QFont font =  m_cyclewidget->font();
  m_cyclewidget->setFont( QtMultiFontString::getQFont( "@button@", font ) );
  m_labelwidget->setFont( QtMultiFontString::getQFont( "@button@", font ) );
  arrow_down->setFont( QtMultiFontString::getQFont( "@button@", font ) );
  arrow_up->setFont( QtMultiFontString::getQFont( "@button@", font ) );
  newCycle->setFont( QtMultiFontString::getQFont( "@button@", font ) );

  // Hoehe setzen
  int h =  (int)floor(0.5 + (1.8*QFontInfo(QtMultiFontString::getQFont( "@button@", font )).pixelSize()));
  arrow_down->setMinimumHeight( h );
  arrow_down->setMaximumHeight( h );
  arrow_up->setMinimumHeight( h );
  arrow_up->setMaximumHeight( h );
  newCycle->setMinimumHeight( h );
  newCycle->setMaximumHeight( h );

  // set layout
  layout->setContentsMargins(0,0,0,0);
  m_cyclewidget->setLayout(layout);
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::manage(){
  if( m_cyclewidget != 0 ){
    m_cyclewidget -> show();
  }
}

/* --------------------------------------------------------------------------- */
/* updateSize --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::updateSize( QWidget *widget, int spacing ){

}

/* --------------------------------------------------------------------------- */
/* getLabel --                                                                 */
/* --------------------------------------------------------------------------- */

std::string GuiQtCycleButton::getLabel( int num ){
  BUG_DEBUG("getLabel()");
  std::string value;
  if( num < DataPoolIntens::getDataPool().NumCycles() ){
    DataPoolIntens::Instance().getCycleName( num, value );
  }
  return compose("%1 : %2",num+1, value );
}
/* --------------------------------------------------------------------------- */
/* updateLabel --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::updateLabel( int num ){
  BUG_DEBUG("updateLabel()");
  const int CYCLE_STR_LENGTH = 300;
  const int MAX_CYCLE_STR_LENGTH = 600;
  BUG_DEBUG(" ==> Label #" << num << " is '" << getLabel(num) << "'");
  QString qstr(QString::fromStdString(getLabel(num)));
  m_labelwidget->setText(qstr);
  if ( m_labelwidget->sizeHint().width() > MAX_CYCLE_STR_LENGTH) {
#if QT_VERSION > 0x050200
    m_labelwidget->setToolTipDuration(AppData::Instance().ToolTipDuration());
#endif
    m_labelwidget->setToolTip(qstr);
    int pos = floor(qstr.length() * MAX_CYCLE_STR_LENGTH)/m_labelwidget->sizeHint().width();
    qstr.resize(pos-3);
    qstr += QString(3, '.');
    m_labelwidget->setText(qstr);
  }
  else{
    m_labelwidget->setToolTip("");
  }
  m_labelwidget->setMinimumWidth(CYCLE_STR_LENGTH);
  int charlen = m_labelwidget->fontMetrics().horizontalAdvance( qstr );
  if (charlen > CYCLE_STR_LENGTH)
    m_labelwidget->setMinimumWidth(std::min(charlen, MAX_CYCLE_STR_LENGTH));
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::getSize( int &w, int &h ){
  w = m_width;
  h = m_height;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::update( UpdateReason reason ){
  BUG_DEBUG("update()");
  int currCycle = DataPoolIntens::Instance().currentCycle();

  if( reason == reason_Cycle || reason == reason_Always ){
    updateLabel( currCycle );
    ResetLastWebUpdated();  // reason_Always for webtens
    return;
  }

  if( DataPoolIntens::Instance().isCycleNameUpdated( currCycle ) ){
    BUG_DEBUG("cycle label updated");
    updateLabel( currCycle );
  }
}

/* --------------------------------------------------------------------------- */
/* cycleDown --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::cycleDown(){
  DataPool &datapool = DataPoolIntens::getDataPool();
  int current = datapool.GetCurrentCycle();
  if( current == 0 ){
    printMessage( _("No previous case available !"), msg_Warning );
    return;
  }
  datapool.SetCycle( current -1 );
  GuiQtManager::Instance().update( reason_Cycle );
  BUG_DEBUG("cycleDown, current: " << current);
  DES_INFO(compose("  GOCYCLE(%1);", current));
  PYLOG_INFO(compose(PYLOG_GOCYCLE, current));

  JobAction *action = JobManager::Instance().getInitialAction( "ON_CYCLE_SWITCH" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction();
  }

  // new ON_CYCLE_EVENT
  action = JobManager::Instance().getInitialAction( "ON_CYCLE_EVENT" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction( JobElement::cll_CycleSwitch,
                                              new JobStackDataInteger(current+1) );
  }
}

/* --------------------------------------------------------------------------- */
/* cycleUp --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::cycleUp(){
  DataPool &datapool = DataPoolIntens::getDataPool();
  int current = datapool.GetCurrentCycle();
  int num     = datapool.NumCycles();
  if( !(++current < num) ){
    printMessage( _("No next case available !"), msg_Warning );
    return;
  }
  datapool.SetCycle( current );
  GuiQtManager::Instance().update( reason_Cycle );
  BUG_DEBUG("cycleUp, current: " << current);
  DES_INFO(compose("  GOCYCLE(%1);", current+1));
  PYLOG_INFO(compose(PYLOG_GOCYCLE, current+1));

  JobAction *action = JobManager::Instance().getInitialAction( "ON_CYCLE_SWITCH" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction();
  }

  // new ON_CYCLE_EVENT
  action = JobManager::Instance().getInitialAction( "ON_CYCLE_EVENT" );
  if( action != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
    InitialWorker::Instance().appendAction( action );
    InitialWorker::Instance().startJobAction( JobElement::cll_CycleSwitch,
                                              new JobStackDataInteger(current));
  }
}

/* --------------------------------------------------------------------------- */
/* cycleNew --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::cycleNew(){
  GuiQtCycleDialog* cycleDlg =
    dynamic_cast<GuiQtCycleDialog*>(GuiQtFactory::Instance()->createCycleDialog());
  if (cycleDlg) {
    cycleDlg->ButtonPressed();
    cycleDlg->newEvent();
  }
}

/* --------------------------------------------------------------------------- */
/* getHelptext --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::getHelptext( std::string &text ){
  DataPoolIntens &dp = DataPoolIntens::Instance();
  if( !dp.getCycleName( dp.currentCycle(), text ) ){
    text = "no name of the case found";
  }
}

/* --------------------------------------------------------------------------- */
/* hasHelptext --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtCycleButton::hasHelptext(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* showHelptext --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtCycleButton::showHelptext(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtCycleButton::serializeXML(std::ostream &os, bool recursive){
  GuiCycleButton::serializeXML(os, recursive);
//   os << "<GuiQtCycleButton";
//   os << ">" << endl;
//   os<<"</GuiQtCycleButton>"<<endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtCycleButton::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiCycleButton::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtCycleButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiCycleButton::serializeProtobuf(eles, onlyUpdated);
}
#endif
