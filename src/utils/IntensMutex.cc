
#include "utils/Debugger.h"
#include "utils/IntensMutex.h"
#if defined HAVE_QT
#include <qmutex.h>
#endif

INIT_LOGGER();

/*==================================================================*/
/* Constructor / Destructor                                         */
/*==================================================================*/

IntensMutex::IntensMutex()
#if defined(HAVE_QT)
  : m_mutex( 0 )
{
  m_mutex = new QMutex();
#else
{
  pthread_mutex_init( &m_mutex, 0 );
#endif
}

IntensMutex::~IntensMutex(){
#ifdef HAVE_QT
  delete m_mutex;
#endif
}

/*==================================================================*/
/* member functions of IntensMutex                                  */
/*==================================================================*/

// ---------------------------------------------------------------- //
// lock --                                                          //
// ---------------------------------------------------------------- //

void IntensMutex::lock(){
  BUG_DEBUG( "IntensMutex::lock" );
#if defined(HAVE_QT)
  m_mutex->lock();
#else
  pthread_mutex_lock( &m_mutex );
#endif
}

// ---------------------------------------------------------------- //
// unlock --                                                        //
// ---------------------------------------------------------------- //

void IntensMutex::unlock(){
  BUG_DEBUG( "IntensMutex::unlock" );
#if defined(HAVE_QT)
  m_mutex->unlock();
#else
  pthread_mutex_unlock( &m_mutex );
#endif
}
