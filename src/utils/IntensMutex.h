
#ifndef UTILS_INTENS_MUTEX_H
#define UTILS_INTENS_MUTEX_H

#ifdef HAVE_QT
class QMutex;
#else
#include <pthread.h>
#endif

/** \file
    Die Datei enthält die Klasse IntensMutex
*/
/** Die Klasse wird verwendet, um gleichzeitige Zugriffe auf heikle
    Datenbereiche durch Threads zu verhindern (thread-save).
*/
class IntensMutex
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  IntensMutex();

  /** Destruktor
   */
  virtual ~IntensMutex();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion lockt den Mutex für alleinigen Zugriff auf Daten.
      Sollte ein anderer Process den Mutex bereits geockt haben, wird gewartet
      bis zur Freigabe.
   */
  void lock();

  /** Der Mutex wird wieder freigegeben.
   */
  void unlock();

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
#if defined HAVE_QT
  QMutex          *m_mutex; // Qt
#else
  pthread_mutex_t  m_mutex; // posix
#endif
};

#endif
