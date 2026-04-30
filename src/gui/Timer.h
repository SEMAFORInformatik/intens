
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#ifndef GUI_TIMER_H
#define GUI_TIMER_H

#include <vector>

class TimerTask;

/** Class Timer
    Timer-Objekte rufen in vorgebbaren Intervallen die tick()-Methode
    ihrer registrierten TimerTasks auf.
*/
class Timer {
public:
  /** Constructor.
   * @param interval Intervall in Millisekunden
   */
  Timer( unsigned long interval=1000, bool restartTimer = false );
  virtual ~Timer();

  /**
   * fügt ein TimerTask-Objekt hinzu
   * @param l TimerTask-Objekt, das hinzugefügt werden soll
   */
  void addTask( TimerTask *l );

  /** entfernt ein TimerTask-Objekt
   * @param l TimerTask-Objekt, das entfernt werden soll
   */
  void removeTask( TimerTask *l );
  /**
   * setzt die Intervalldauer
   * @param interval Dauer in Millisekunden
   */
  void setInterval( unsigned long interval );
  /**
   * gibt die Intervalldauer zurück
   * @return  Dauer in Millisekunden
   */
  unsigned long getInterval( ) const;
  /**
   * setzt die Delay
   * @param delay in Millisekunden
   */
  void setDelay( unsigned long delay );
  /**
   * gibt die Delay zurück
   * @return  Delay in Millisekunden
   */
  unsigned long getDelay( ) const;

  /**
   * startet den Timer
   */
  virtual void start()=0;
  /**
   * stoppt den Timer
   */
  virtual void stop()=0;

protected:
  // ruft die tick-Methode der TimerTask-Objekte auf
  void execute();
  bool m_restartTimer;
  bool m_stopped;

private:

  typedef std::vector< TimerTask *> TaskCollection;
  TaskCollection m_tasks;
  unsigned long  m_interval;
  unsigned long  m_delay;
};

#endif
