
/*******************************************************************************/
/* utils.h                                                                 */
/* Dieses Includefile zieht all die Utility-Includes hinein. Es ist fuer den   */
/* Benutzer des Utilities-Modules gedacht um ihm zu ersparen jedesmal exakt    */
/* herauszufinden, was nun eigentlich included werden soll.                    */
/*******************************************************************************/

#if !defined(UTILITIES_H__INCLUDED_)
#define UTILITIES_H__INCLUDED_

#include "utils/Root.h"
#include "utils/FileUtilities.h"
#include "utils/OsFileName.h"
#include "utils/Debugger.h"
#include "utils/Date.h"
#include "utils/gettext.h"

template<class T>
void exchange( T &a, T &b ){
  const T x = a;
  a = b;
  b = x;
}

template <class T>
inline const T &maximum( const T &a, const T &b ){
  return ( b > a ? b : a );
}

template <class T>
inline const T &maximum( const T &a, const T &b, const T &c ){
  return ( b > a ? maximum( b, c ) : maximum( a, c ) );
}

template <class T>
inline const T &minimum( const T &a, const T &b ){
  return ( b < a ? b : a );
}

template <class T>
inline const T &minimum( const T &a, const T &b, const T &c ){
  return ( b < a ? minimum( b, c ) : minimum( a, c ) );
}


double runden( double d, int i );
double runden5( double d );
double aufrunden( double d );
double abrunden( double d );
bool isInteger( double d );
std::ostream& indent( int i, std::ostream &ostr );
void mysetenv(const char *name, const char *value);

#endif // UTILITIES_H__INCLUDED_
