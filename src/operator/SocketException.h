#ifndef SocketException_class
#define SocketException_class

#include <string>
#include <exception>

class SocketException : public std::exception
{
 public:
  SocketException ( std::string s ) : m_s ( s ) {};
  virtual ~SocketException()  throw () {};

  virtual const char* what() const throw() {
    return  m_s.c_str();
  }

 private:
  std::string m_s;

};

#endif
