
#if !defined(DIALOG_WORKCLOCK_H)
#define DIALOG_WORKCLOCK_H

#include <assert.h>

#include "gui/TimerTask.h"

class Timer;
class DialogWorkClockListener;

#define CLOCK_TIMER_INTERVAL 1000L

class DialogWorkClock{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  DialogWorkClock(){ assert( false ); }
public:
  DialogWorkClock( DialogWorkClockListener *listener );
  virtual ~DialogWorkClock();

/*=============================================================================*/
/* private definitions                                                         */
/*=============================================================================*/
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(DialogWorkClock* wc)
      : m_dialogWorkClock(wc){}
    void tick() { m_dialogWorkClock->ClockTimeOver();  }
  private:
    DialogWorkClock* m_dialogWorkClock;
  };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void ClockTimeOver() = 0;
  virtual void manage() = 0;
  virtual void unmanage() = 0;
  void unsetListener() { m_listener=0; }
  DialogWorkClockListener* getListener() { return m_listener; }

  void removeClockTimer();
  Timer *timer(){ return m_timer; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  void cancelButtonPressed();
  void setStartTime();
  time_t elapsedTime();
  void setElapsedTime();

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  Timer                   *m_timer;
  TimerTask               *m_task;
  DialogWorkClockListener *m_listener;
  time_t                   m_start_time;
  time_t                   m_elapsed_time;
};



class DialogWorkClockListener{
public:
  DialogWorkClockListener(){}
  virtual ~DialogWorkClockListener(){}

  virtual void cancelButtonPressed() = 0;
};
#endif
