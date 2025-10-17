#ifndef JSONEXCEPTION_H_
#define JSONEXCEPTION_H_

#include "IntensException.h"

namespace ch_semafor_intens {

  class JsonException : public IntensException
  {
  public:
    JsonException ( std::string origin, std::string message )
      : IntensException(origin, message) {}
    virtual ~JsonException()  throw () {}
  };

} // end namespace ch_semafor_intens

#endif // JSONEXCEPTION_H_
