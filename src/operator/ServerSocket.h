
#ifndef SERVER_SOCKET
#define SERVER_SOCKET

#include <vector>
#include <string>

class ServerSocket{
  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
 public:
  ServerSocket (){}
  virtual ~ServerSocket(){}

  //-------------------------------------------------
  // public functions
  //-------------------------------------------------
  virtual void endOfRead()=0;
  virtual void stop(){}
  virtual void start(){}
};

class ServerSocketListener{
public :
  ServerSocketListener(){}
  ~ServerSocketListener(){}
public :
  virtual bool newLine( std::string &line )=0;
  virtual bool response( std::ostream &os )=0;
  virtual const std::string &getName() = 0;

};

#endif
