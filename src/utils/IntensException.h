#ifndef INTENSEXCEPTION_H_
#define INTENSEXCEPTION_H_

#include <string>
#include <exception>

namespace ch_semafor_intens {

  class IntensException : public std::exception
  {
  public:
    IntensException ( std::string origin, std::string message )
      : origin ( origin )
      , message ( message ) {
      setCompleteMessage();
      }
    virtual ~IntensException() throw () {}

    virtual const char* what() const throw() {
      return completeMessage.c_str();
    }

    void setMessage(std::string message) {
      this->message = message;
      setCompleteMessage();
    }
    const std::string& getMessage(void) const {
      return message;
    }

  private:
    void setCompleteMessage() {
      if ( ! origin.empty() ) {
	completeMessage = origin + " : " + message;
      } else {
	completeMessage = message;
      }
    }

    std::string origin;
    std::string message;
    std::string completeMessage;


  };

} // end namespace ch_semafor_intens

#endif // INTENSEXCEPTION_H_
