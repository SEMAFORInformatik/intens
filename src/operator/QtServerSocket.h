
#ifndef QT_SERVER_SOCKET
#define QT_SERVER_SOCKET

#include <string>

#include <qobject.h>
#include <QTcpServer>
#include <QTcpSocket>


#include "gui/TimerTask.h"
#include "operator/ServerSocket.h"

class Timer;
class MySocket;

class QtServerSocket : public QObject, public ServerSocket{
  Q_OBJECT
public:
  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
  QtServerSocket ( ServerSocketListener *listener, int port );
  QtServerSocket (){}
  virtual ~QtServerSocket();

private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(QtServerSocket* server)
      : m_server(server){}
    void tick() { m_server->endOfRead();  }
  private:
    QtServerSocket* m_server;
  };

public:
  void start();
  const std::string &getName(){ return m_listener->getName(); }
  //-------------------------------------------------
  // public functions
  //-------------------------------------------------
  bool newLine( std::string &line );
  virtual void endOfRead();
  virtual void stop();

  //-------------------------------------------------
  // private members
  //-------------------------------------------------

  // -----------------------------------------------------
  // Signals and Slots
  // -----------------------------------------------------
private slots:
  void slot_newConnection();
private:
  ServerSocketListener  *m_listener;
  MySocket              *m_currentSocket;
  QTcpServer           *m_tcpserver;
  Timer                 *m_timer;
  TimerTask             *m_task;
  int                    m_port;

  friend class MySocket;
};

class MySocket : public QObject {
  Q_OBJECT
  // -----------------------------------------------------
  // Constructor / Destructor
  // -----------------------------------------------------
public:
  MySocket( QtServerSocket *parent, QTcpSocket *socket);
  virtual ~MySocket();

  //-------------------------------------------------
  // private TimerTask class with tick method
  //-------------------------------------------------
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(MySocket* socket)
      : m_socket(socket){}
    void tick() { m_socket->read();  }
  private:
    MySocket* m_socket;
  };

public:
  QTcpSocket* getTcpSocket() { return m_socket; }
  void setWait( bool state ) { m_wait = state; }
  bool isWaiting()           { return m_wait; }
  void read();

  // -----------------------------------------------------
  // Signals and Slots
  // -----------------------------------------------------
private slots:
  void slot_readyRead();
  void slot_error( QAbstractSocket::SocketError socketError );
  void slot_deleteLater();


  // -----------------------------------------------------
  // private members
  // -----------------------------------------------------
private:
  Timer          *m_timer;
  TimerTask      *m_task;

  QtServerSocket *m_server;
  QTcpSocket     *m_socket;
  std::string     m_requestHeader;
  long            m_sizeToRead;
  bool            m_wait;
};

#endif
