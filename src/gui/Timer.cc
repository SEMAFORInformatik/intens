#ifndef TIMER_H
#define TIMER_H
#include <algorithm>
#include "Timer.h"
#include "TimerTask.h"
#include <assert.h>
//
// Constructor.
//
Timer::Timer(unsigned long interval, bool restartTimer)
  : m_interval(interval)
  , m_delay(0)
  , m_restartTimer( restartTimer )
  , m_stopped( true ){ /* empty */ }

//
// Destructor
//
Timer::~Timer(){ /*empty */}

//-------------------------------------------------------------
//
// add listener
//
void Timer::addTask( TimerTask *l ){
  m_tasks.push_back( l );
}
//-------------------------------------------------------------
//
// remove listener
//
void Timer::removeTask( TimerTask *l ){
  TaskCollection::iterator li=std::find( m_tasks.begin()
					 ,m_tasks.end()
					 ,l );
  if( li == m_tasks.end() )
    return;
  m_tasks.erase( li );
}

void Timer::setInterval( unsigned long interval ){
  m_interval = interval;
}

unsigned long Timer::getInterval( ) const {
  return m_interval;
}

void Timer::setDelay( unsigned long delay ){
  m_delay = delay;
}

unsigned long Timer::getDelay( ) const {
  return m_delay;
}

void Timer::execute( ){
  int size = m_tasks.size();
  for( TaskCollection::iterator l=m_tasks.begin();
       l!=m_tasks.end();
       ++l ){
    if (--size < 0) break;
    (*l)->tick();
  }
}

#endif
