
#include <qtimer.h>

#include <assert.h>
#include "gui/qt/QtTimer.h"
#include "gui/qt/GuiQtManager.h"

QtTimer::QtTimer( unsigned long interval, bool restartTimer )
  : Timer(interval, restartTimer ) {
  connect( this, SIGNAL(timeout()), this, SLOT(doExecute()) );
}
QtTimer::~QtTimer() {
  stop();
}

void QtTimer::start(){
  //  std::cout<<"["<<this<<"]"<< "QtTimer::s t a r t () ******interval["<<getInterval()<<"]" <<std::endl<<std::flush;
  setSingleShot((getInterval()>0 ? false : true) );
  if (isSingleShot() || getDelay() == 0) {
    QTimer::start( getInterval() );
  } else {
    QTimer::start( getDelay() );
  }
  m_stopped = false;
}

void QtTimer::stop(){
  //   std::cout<<"["<<this<<"]"<< "QtTimer::=====stop () ******interval["<<getInterval()<<"]" <<std::endl<<std::flush;
  m_stopped = true;
  QTimer::stop();
}

void QtTimer::doExecute() {
  //  std::cout<<"["<<this<<"]"<< "QtTimer::doExecute() ******interval["<<getInterval()<<"]" <<std::endl<<std::flush;
  if( m_stopped )
    return;
  if( !m_restartTimer )
    stop();
  execute();
  if (QTimer::interval() != getInterval()) {
    QTimer::setInterval( getInterval() );
  }
}
