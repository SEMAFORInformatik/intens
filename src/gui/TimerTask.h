
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#ifndef TIMER_TASK_H
#define TIMER_TASK_H

class TimerTask;

/** \class TimerTask
 * Objekte dieser Klasse können von einem Timer-Objekt
 * über die Tick()-Methode aufgerufen werden.
 */
class TimerTask{
public:
  TimerTask(){}
  virtual ~TimerTask(){}
  virtual void tick()=0;
};

#endif
