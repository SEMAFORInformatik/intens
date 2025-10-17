
#if !defined(WORKER_H)
#define WORKER_H

#include <string>
#include <assert.h>

#include "job/JobAction.h"
#include "gui/TimerTask.h"
#include "gui/SimpleAction.h"

class Timer;

#define WORK_TIMER_INTERVAL  0L

class WorkerController;

class Worker{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  Worker( WorkerController *listener, int intv2=WORK_TIMER_INTERVAL);
  virtual ~Worker();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask( Worker* w )
      : m_worker( w ){}
    void tick() { m_worker->work();  }
  private:
    Worker* m_worker;
  };

  class MyDelete : public SimpleAction::Object {
  public:
    MyDelete( Worker *w ) : m_worker( w ){}
    virtual void start() { delete m_worker; }
  private:
    Worker* m_worker;
  };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void start();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void work();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  Timer             *m_timer;
  TimerTask         *m_task;

  WorkerController  *m_controller;
  bool               m_stopped;
  bool               m_normal_eot;
  static Worker     *s_to_delete;
  int                m_interval2;
};


/** Alle Prozesse, welche vom Worker
    kontrolliert werden sollen, müssen vom WorkerController abgeleitet sein.
*/
class WorkerController
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  WorkerController(){
  }
  virtual ~WorkerController(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Mit dieser Funktion soll der Prozess in Gang gebracht werden. Sie wird
      durch showDialog() aufgerufen. ACHTUNG: showDialog() gibt die Kontrolle
      nicht mehr ab, bis die Geschichte vorbei ist.
  */
  virtual void startWorker() = 0;

  /** Die Funktion work() wird immer wieder aufgerufen, bis als Resultat
      true zurueckgegeben wird. Dies bedeutet, dass die Arbeit beendet ist.
      Ob die Arbeit erfolgreich war, erfahren wir erst durch EndOfWork().
      @return True: => Ende
  */
  virtual JobAction::JobResult work() = 0;

  /** Wenn die Funktion work() als Resultat 'true' zurueckgegeben hat, wird
      als letzte Aktion EndOfWork() aufgerufen. Dort können noch Abschlussarbeiten
      folgen. Sie meldet uns auch, ob es sich um ein normales Ende des
      Prozesses handelt.
      @return True: => normales Ende
  */

  void endWorker( JobAction::JobResult rslt );
  virtual void backFromWorker( JobAction::JobResult rslt ) = 0;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
};
#endif
