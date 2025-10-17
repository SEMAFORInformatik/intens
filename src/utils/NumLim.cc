#include "NumLim.h"

#include <cmath>
#include <limits>

double NumLim::infinity(){
  return std::numeric_limits<double>::infinity();
}

// limits stuff
const double  NumLim::MAX_DOUBLE     = std::numeric_limits<double>::max();
const double  NumLim::MIN_DOUBLE     = std::numeric_limits<double>::lowest();
const double  NumLim::EPSILON_DOUBLE = std::numeric_limits<double>::epsilon();

const int  NumLim::MAX_INT        = std::numeric_limits<int>::max();
const int  NumLim::MIN_INT        = std::numeric_limits<int>::min();
