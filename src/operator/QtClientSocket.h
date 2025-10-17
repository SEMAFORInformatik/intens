
#ifndef QT_CLIENT_SOCKET
#define QT_CLIENT_SOCKET

#include <qobject.h>
#include <sstream>

#include "gui/TimerTask.h"
#include "operator/ClientSocket.h"

#include <QTcpSocket>

class Stream;
class Timer;

class QtClientSocket : public QObject, public ClientSocket{
  Q_OBJECT
// -----------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------
public:
  QtClientSocket( const std::string &
		  , Stream *
		  , Stream * );
  virtual ~QtClientSocket();

protected:
  void timerEvent( QTimerEvent * );

  //-------------------------------------------------
  // private TimerTask class with tick method
  //-------------------------------------------------
private:
  class MyTimerTask : public TimerTask {
  public:
    MyTimerTask(QtClientSocket* server)
      : m_server(server){}
    void tick() { m_server->clear();  }
  private:
    QtClientSocket* m_server;
  };

  // -----------------------------------------------------
  // public member functions of ClienSocket
  // -----------------------------------------------------
public:
  virtual void start();
  void clear();

  // -----------------------------------------------------
  // private member functions
  // -----------------------------------------------------
  void connectionClosed();
  // -----------------------------------------------------
  // Signals and Slots
  // -----------------------------------------------------
private slots:
  void slot_connected();
  void slot_error(QAbstractSocket::SocketError socketError);
  void slot_connectionClosed();
  void slot_delayedCloseFinished();
  void slot_readyRead();

// -----------------------------------------------------
// private data
// -----------------------------------------------------
private:
  std::string                             m_header;
  QTcpSocket                             *m_socket;
  std::ostringstream                      m_input;
  MyTimerTask                            *m_task;
  Timer                                  *m_timer;
  bool                                    m_allreadyClosed;
  int                                     m_timeout;
  int                                     m_timeout2;
};

#endif
