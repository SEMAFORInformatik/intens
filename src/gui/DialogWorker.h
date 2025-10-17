
#if !defined(DIALOG_WORKER_H)
#define DIALOG_WORKER_H

#include <string>
#include <assert.h>
#include <time.h>

#include "gui/TimerTask.h"
#include "gui/SimpleAction.h"

class Timer;

#define DIALOG_WORK_TIMER_INTERVAL  50L

class WorkerListener;


class DialogWorker
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DialogWorker( bool use_timer
		, WorkerListener *listener );
  virtual ~DialogWorker();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask( DialogWorker* dw )
      : m_dialogworker( dw ){}
    void tick() { m_dialogworker->work();  }
  private:
    DialogWorker* m_dialogworker;
  };

  class MyDelete : public SimpleAction::Object {
  public:
    MyDelete( DialogWorker *dw ) : m_dialogworker( dw ){}
    virtual void start() { delete m_dialogworker; }
  private:
    DialogWorker* m_dialogworker;
  };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void manage() = 0;
  virtual void unmanage() = 0;
  virtual bool work();
  virtual void setMessage( const std::string & ) = 0;
  virtual bool isDialogInvisible() = 0;
  virtual void popParentDialog() = 0;

  bool continueWork();
  void startProcedure();
  void setTimerInterval( long t );
  unsigned long interval();
  bool timer_used(){
    return m_timer_used;
  }
  void unsetTimerUsed(){
    m_timer_used = false;
  }
  void setTimerUsed(){
    m_timer_used = true;
  }
  WorkerListener *listener(){
    return m_listener;
  }
  time_t start(){
    return m_start_time;
  }
  void setStart( time_t t ){
    m_start_time = t;
  }
  time_t elapsed(){
    return m_elapsed_time;
  }
  void setElapsedTime(){
    m_elapsed_time = time(0) - m_start_time;
  }
  bool stopped(){
    return m_stopped;
  }
  void setStopped(){
    m_stopped = true;
  }
  bool normalEot(){
    return m_normal_eot;
  }
  void setNormalEot( bool n ){
    m_normal_eot = n;
  }
  void setTitle( const std::string &title ) {
    m_title = title;
  }
  const std::string &title(){
    return m_title;
  }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  Timer             *m_timer;
  TimerTask         *m_task;

  std::string        m_title;
  unsigned long      m_interval;
  bool               m_timer_used;
  WorkerListener    *m_listener;
  time_t             m_start_time;
  time_t             m_elapsed_time;
  bool               m_stopped;
  bool               m_normal_eot;
};


/** All DialogWorker controlled processes need to be derived from WorkerListener
*/
class WorkerListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  WorkerListener()
    : m_dialog( 0 ){
  }
  virtual ~WorkerListener(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Mit dieser Funktion soll der Prozess in Gang gebracht werden. Sie wird
      durch showDialog() aufgerufen. ACHTUNG: showDialog() gibt die Kontrolle
      nicht mehr ab, bis die Geschichte vorbei ist.
  */
  virtual void StartOfWork() = 0;

  /** Die Funktion work() wird immer wieder aufgerufen, bis als Resultat
      true zurueckgegeben wird. Dies bedeutet, dass die Arbeit beendet ist.
      Ob die Arbeit erfolgreich war, erfahren wir erst durch EndOfWork().
      @return True: => Ende
  */
  virtual bool work() = 0;

  /** Wenn die Funktion work() als Resultat 'true' zurueckgegeben hat, wird
      als letzte Aktion EndOfWork() aufgerufen. Dort können noch Abschlussarbeiten
      folgen. Sie meldet uns auch, ob es sich um ein normales Ende des
      Prozesses handelt.
      @return True: => normales Ende
  */
  virtual bool EndOfWork() = 0;

  /** Nach dem Stoppen einer Task durch den Operator muss zuerst der Listener
      informiert werden. Erst wenn er das Ok gibt, kann der Stop wirklich durchgeführt
      werden.
      @return True: => stop ok
  */
  virtual bool stopped() { return true; }

  /** Falls die laufende Arbeit mit dem Cancel-Button abgebrochen wird,
      wird als letzte aktion workCancelButtonPressed() aufgerufen. Die
      Funktionen work() und EndOfWork() kommen nicht mehr zum Einsatz.
  */
  virtual void workCancelButtonPressed() = 0;

  /** Diese Funktion gibt dem Listener die Möglichkeit, nach dem Ende
      des Work-Dialogs noch etwas zu tun (z.B. MessageBox).
  */
  virtual void workFailed() {}

  /** Diese Funktion wird immer am Schluss als Letztes aufgerufen, unabhängig vom
      Ausgang der Verarbeitung.
   */
  virtual void workTerminated() {}

  /** Diese Funktion wird intern benoetigt. Sie wird von showDialog() des
      DialogWorker-Objekts aufgerufen um dem Listener einen Pointer dieses
      Objekts zu uebergeben. Damit kann der Listener dem DialogWorker
      Mitteilungen machen.
  */
  void setDialog( DialogWorker *dialog ) { m_dialog = dialog; }

  DialogWorker *getDialog() { return m_dialog; }

  /** Der Listener kann mit setDialogMessage() die aktuell gezeigte
      Meldung aendern.
  */
  void setDialogMessage( const std::string &msg ){
    if( m_dialog != 0 ) m_dialog->setMessage( msg );
  }

  /** Der Listener kann das Zeitintervall zwischen zwei Aufrufen von work()
      jederzeit aendern. Der Parameter t entspricht der Zeit in 1/1000
      Sekunden. Er kann auch den Wert 0 haben.
  */
  void setTimerInterval( unsigned long t ){
    if( m_dialog != 0 ) m_dialog->setTimerInterval( t );
  }

  /** Mit Dieser Funktion kann der Worker einfach ins Log schreiben. Im
      Fehlerfall sollte er dies tun, da der DialogWorker in diesem Fall
      auf das LogWindow verweist.
      @param msg Message für das Log
  */
  void writeLog( const std::string &msg );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  DialogWorker *m_dialog;
};
#endif
