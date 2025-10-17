#ifndef QTTIMER_H
#define QTTIMER_H

#include <qtimer.h>

#include "gui/Timer.h"

class QtTimer : public QTimer, public  Timer {
  Q_OBJECT
public:
  QtTimer(unsigned long interval, bool restartTimer = false );
  ~QtTimer();
  void start();
  void stop();
private slots:
  void doExecute();
};

#endif
