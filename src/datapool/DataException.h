// ****************************************************************
// DataException
//
// $Id: DataException.h 1.14 2012/09/28 12:53:24Z bho Exp $
// ****************************************************************

#if !defined(__DP_EXCEPTION__)
#define __DP_EXCEPTION__

#include <exception>
#include <sstream>

#define DpException(c,f,m) DataException(f,c,m,__FILE__,__LINE__)
#define ThrowDpException(f,m) { \
  std::ostringstream msg; \
  msg << m; \
  throw DataException(f,__CLASSNAME__,msg.str(),__FILE__,__LINE__); \
  }

/** Beschreibung der Klasse DataException
    \todo Die Beschreibung der Klasse DataException fehlt
 */
class DataException : public std::exception
{
// ================================================================ //
// Constructor / Destructor                                         //
// ================================================================ //
public:
  /** Konstruktor
      \param func
      \param obj
      \param msg
      \param file
      \param line
      \todo Die Beschreibung der Funktion DataException fehlt
   */
  DataException( const std::string &func
               , const std::string &obj
               , const std::string &msg
               , const std::string &file
               , int line );

  /** Copy-Konstruktor
      \param ex
      \todo Die Beschreibung der Funktion DataException fehlt
   */
  DataException( const DataException &ex );

  /** Destruktor
   */
  virtual ~DataException()

/** Beschreibung der Funktion throw
    \todo Die Beschreibung der Funktion throw fehlt
 */
throw();

// ================================================================ //
// public Functions                                                 //
// ================================================================ //
public:
  /** Beschreibung der Funktion what
      \return
      \todo Die Beschreibung der Funktion what fehlt
   */
  virtual const char* what() const

/** Beschreibung der Funktion throw
    \todo Die Beschreibung der Funktion throw fehlt
 */
throw();

  /** Beschreibung der Funktion get_what
      \return
      \todo Die Beschreibung der Funktion get_what fehlt
   */
  const std::string get_what() const;

// ================================================================ //
// private Functions                                                //
// ================================================================ //
private:

// ================================================================ //
// private Data                                                     //
// ================================================================ //
private:
  std::string  m_what;
};

/** Beschreibung der Funktion operator<<
    \param ostream
    \param DataException
    \return
    \todo Die Beschreibung der Funktion operator<< fehlt
 */
std::ostream &operator<<( std::ostream &, const DataException & );

#endif
/** \file */
