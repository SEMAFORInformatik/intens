/* $Id: Timer.h,v 1.2 2003/12/09 08:38:48 ked Exp $
 * 
 *                             Timer class
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Author: Ronald Tanner                   Created: 2001/11/10
 *
 */
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
