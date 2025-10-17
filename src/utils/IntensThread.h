
#ifndef UTILS_INTENS_THREAD_H
#define UTILS_INTENS_THREAD_H

#ifdef HAVE_QT
class QMutex;
class MyQThread;
#else
#include <pthread.h>
#endif
#include "utils/IntensMutex.h"

class IntensThread;
class ThreadListenerMutex;

class ThreadListener
{
  friend class IntensThread;
  friend class ThreadListenerMutex;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ThreadListener();
  virtual ~ThreadListener();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  ///
  void setThread( IntensThread *thread );
  ///
  void startThreadListener();
  ///
  bool isRunning() const;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  ///
  virtual void startThread() = 0;
#if !defined(HAVE_QT)
  ///
  static void lockThread();
  ///
  static void unlockThread();
  ///
  void setRunning( bool yes );
#endif

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  IntensThread       *m_my_thread;
#if !defined(HAVE_QT)
  static IntensMutex  s_thread_listener_mutex;
  bool                m_thread_is_running;
#endif
};


/** Die Klasse IntensThread
*/
class IntensThread : public IntensMutex
{
  friend class ThreadListener;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor.
      \param listener Pointer auf den Listener mit der aufzurufenden Funktion
      startThread().
   */
  IntensThread( ThreadListener* listener );

  /** Destruktor
   */
  virtual ~IntensThread();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion startet den Thread und ruft die Funktion startThread() des
      Listeners auf.
   */
  void start();

  /** Die Funktion stoppt den Thread.
      \attention Der Zustand von Daten nach einem Abbruch ist nicht bestimmt. Um
      Überraschungen zu vermeiden, sollte auf beiden Seiten mit dem Mutex
      gearbeitet werden.
      \param doLock true => Der Abbruch wird erst nach dem lock des Mutex durchgeführt.
   */
  void terminate( bool doLock=false );

  /** Die Funktion teilt mit, ob der Thread noch aktiv ist.
      \retval true Der Thread läuft noch
  */
  bool isRunning() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
#if defined HAVE_QT
  MyQThread         *m_thread;
#else
  static void* doExec( void *args ); // thread routine
  pthread_t          m_thread;
#endif
  ThreadListener    *m_listener;
};

#endif
