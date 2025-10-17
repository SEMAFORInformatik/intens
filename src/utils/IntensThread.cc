
#include <assert.h>
#include "utils/IntensThread.h"
#include "utils/Debugger.h"

#if defined HAVE_QT
#include <qthread.h>

/*==================================================================*/
/* Global defines                                                   */
/*==================================================================*/

#define SAY(X) // std::cout << "SAY: " << X << std::endl

/*==================================================================*/
/* lokale Klasse die wir in Qt benutzt als Thread                   */
/*==================================================================*/

class MyQThread : public QThread {
public:
  MyQThread( ThreadListener *l ): m_listener( l ){}
private:
  virtual void run(){
    m_listener->startThreadListener();
  };
  ThreadListener *m_listener;
};
#else
/*==================================================================*/
/* Initialisation                                                   */
/*==================================================================*/

IntensMutex ThreadListener::s_thread_listener_mutex;

/*==================================================================*/
/* doExec                                                           */
/*==================================================================*/

void* IntensThread::doExec( void *args ){
  IntensThread* thread = static_cast<IntensThread*>(args);
  thread->m_listener->startThreadListener();
}
#endif

/*==================================================================*/
/* Constructor / Destructor                                         */
/*==================================================================*/

IntensThread::IntensThread( ThreadListener *listener )
  : m_listener( listener )
#if defined(HAVE_QT)
  , m_thread( 0 ){
  m_thread = new MyQThread( listener );
#else
  , m_thread( (pthread_t)0 ){
#endif
  listener->setThread( this );
}

IntensThread::~IntensThread(){
#ifdef HAVE_QT
  delete m_thread;
#endif
}

/*==================================================================*/
/* member functions of IntensThread                                 */
/*==================================================================*/

// ---------------------------------------------------------------- //
// start --                                                         //
// ---------------------------------------------------------------- //

void IntensThread::start() {
  BUG( BugUtilities, "IntensThread::start" );

#ifdef HAVE_QT
  m_thread->start();
#else
  m_listener->setRunning( true );

  pthread_attr_t pattr;
  pthread_attr_init( &pattr );
  pthread_attr_setdetachstate( &pattr, PTHREAD_CREATE_DETACHED );

  if( pthread_create( &m_thread, &pattr, &IntensThread::doExec, (void *)this ) ){
    perror( "IntensThread::start => cannot create pthread" );
  }
#endif
}

// ---------------------------------------------------------------- //
// terminate --                                                     //
// ---------------------------------------------------------------- //

void IntensThread::terminate( bool doLock ) {
  BUG( BugUtilities, "IntensThread::terminate" );

  if ( doLock ){
    lock();
  }

#if defined(HAVE_QT)
  if( m_thread->isRunning() ){
    m_thread->terminate();
    m_thread->wait();
  }
#else
  if( m_thread != (pthread_t)0 ){
    pthread_cancel( m_thread );
  }
  m_thread = (pthread_t)0;
#endif

  if ( doLock ){
    unlock();
  }
}

// ---------------------------------------------------------------- //
// isRunning --                                                     //
// ---------------------------------------------------------------- //

 bool IntensThread::isRunning() const{
#if defined(HAVE_QT)
   return m_thread->isRunning();
#else
   if( m_listener != 0 ){
     return m_listener->isRunning();
   }
   return false;
#endif
 }

 #if !defined(HAVE_QT)
/*==================================================================*/
/* class ThreadListenerMutex                                        */
/*==================================================================*/

class ThreadListenerMutex{
public:
  ThreadListenerMutex(){
    ThreadListener::lockThread();
  }
  virtual ~ThreadListenerMutex(){
    ThreadListener::unlockThread();
  }
};

#define THREAD_LISTENER_MUTEX_BLOCK ThreadListenerMutex  __thread_listener_mutex__
#endif

/*==================================================================*/
/* Constructor / Destructor of ThreadListener                       */
/*==================================================================*/

ThreadListener::ThreadListener()
  : m_my_thread( 0 )
#if !defined(HAVE_QT)
  , m_thread_is_running( false )
#endif
{
}

ThreadListener::~ThreadListener(){
}

/*==================================================================*/
/* member functions of ThreadListener                               */
/*==================================================================*/

// ---------------------------------------------------------------- //
// setThread --                                                     //
// ---------------------------------------------------------------- //

void ThreadListener::setThread( IntensThread *thread ){
  assert( m_my_thread == 0 );
  m_my_thread = thread;
}

// ---------------------------------------------------------------- //
// startThreadListener --                                           //
// ---------------------------------------------------------------- //

void ThreadListener::startThreadListener(){
  SAY ("ThreadListener::startThreadListener START");

  startThread();

  SAY ("ThreadListener::startThreadListener STOP");
#if !defined(HAVE_QT)
  setRunning( false );
#endif
}

// ---------------------------------------------------------------- //
// isRunning --                                                     //
// ---------------------------------------------------------------- //

bool ThreadListener::isRunning() const{
#if defined(HAVE_QT)
  return m_my_thread->isRunning();
#else
  THREAD_LISTENER_MUTEX_BLOCK;
  return m_thread_is_running;
#endif
}

#if !defined(HAVE_QT)
// ---------------------------------------------------------------- //
// setRunning --                                                    //
// ---------------------------------------------------------------- //

void ThreadListener::setRunning( bool yes ){
  THREAD_LISTENER_MUTEX_BLOCK;
  SAY ("ThreadListener::setRunning( " << yes << " )");
  assert( m_thread_is_running != yes );
  m_thread_is_running = yes;
}

// ---------------------------------------------------------------- //
// lockThread --                                                    //
// ---------------------------------------------------------------- //

void ThreadListener::lockThread(){
  s_thread_listener_mutex.lock();
}

// ---------------------------------------------------------------- //
// unlockThread --                                                  //
// ---------------------------------------------------------------- //

void ThreadListener::unlockThread(){
  s_thread_listener_mutex.unlock();
}
#endif
