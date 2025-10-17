#ifndef PARSEREXCEPTION_H_
#define PARSEREXCEPTION_H_

#include "IntensException.h"

namespace ch_semafor_intens {

  class ParserException : public IntensException
  {
  public:
    ParserException ( std::string origin, std::string message )
      : IntensException(origin, message) {}
    virtual ~ParserException()  throw () {}
  };

} // end namespace ch_semafor_intens

#endif // PARSEREXCEPTION_H_
