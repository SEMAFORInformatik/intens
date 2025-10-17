
#ifndef NUMLIM_H
#define NUMLIM_H

#include <math.h>
#ifdef _MSC_VER
#include <float.h>
#endif
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

class NumLim {
 public:
  static double infinity ();

  static bool isFinite( double x ){
#ifdef HAVE_FINITE
    return finite(x);
#elif _MSC_VER
    return _finite(x);
#else
    return isfinite(x);
#endif
  }

  static const double MAX_DOUBLE;
  static const double MIN_DOUBLE;
  static const double EPSILON_DOUBLE;

  static const int MAX_INT;
  static const int MIN_INT;
 private:
  static double _infinity;
};

#endif
