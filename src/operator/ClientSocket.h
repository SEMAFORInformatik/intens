
#ifndef CLIENT_SOCKET
#define CLIENT_SOCKET

#include <string>
#include <iostream>
#include <assert.h>

class Stream;
class ConnectionListener;
class XferDataItem;

class ClientSocket{
  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
public:
  ClientSocket(	Stream *
		, Stream * );

  virtual ~ClientSocket();

  //-------------------------------------------------
  // public functions
  //-------------------------------------------------
  bool checkInput();
  const std::string &getErrorMessage() const{ return m_message; }

  virtual void start(){
    std::cout << "start()" << std::endl;
  }

  Stream *getOutStream(){ return m_out_stream; }
  Stream *getInStream(){ return m_in_stream; }
  int getPort();
  const std::string &getHost();
  ConnectionListener *getListener(){ return m_listener; }
  void setListener( ConnectionListener *listener ){
    m_listener = listener;
  }

  void setPort( int port ){ m_port = port; }
  void setPort( XferDataItem *xfer ){ m_portxfer = xfer; }
  void setHost( const std::string & host ){ m_host = host; }
  void setHost( XferDataItem *xfer ){ m_hostxfer = xfer; }

private:
  Stream                                 *m_out_stream; // dieser Stream wird auf TCP geschrieben
  Stream                                 *m_in_stream;  // in diesen Stream die Antwort geschrieben
  int                                     m_port;
  std::string                             m_host;
  std::string                             m_ip;
  XferDataItem *                          m_portxfer;
  XferDataItem *                          m_hostxfer;
  ConnectionListener                     *m_listener;

  std::string m_message;
};


#endif
