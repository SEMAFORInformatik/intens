/* $Id: TimerTask.h,v 1.2 2005/03/17 10:10:38 ked Exp $
 * 
 *                             TimerTask class
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
