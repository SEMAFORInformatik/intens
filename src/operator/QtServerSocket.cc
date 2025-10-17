
#include <QTcpSocket>
#include <sstream>
#include <iostream>

#include "utils/Date.h"
#include "utils/gettext.h"

#include "gui/Timer.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "operator/IntensServerSocket.h"
#include "operator/QtServerSocket.h"

#define FILE_LOG 0
#if FILE_LOG
#include <QTime>
#include <map>
#include <fstream>
static std::map<int, std::ofstream*> s_files;
#endif

INIT_LOGGER();

//-------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------
QtServerSocket::QtServerSocket( ServerSocketListener *listener, int port )
  : m_listener( listener )
  , m_currentSocket( 0 )
  , m_port( port )
  , m_tcpserver( 0 ){
  m_timer=GuiFactory::Instance()->createTimer( 0 );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
#if FILE_LOG
  if (s_files.find(port) == s_files.end()) {
    std::ofstream *of = new std::ofstream( std::string("/tmp/ssocket_"+QString::number(m_port).toStdString()+".log").c_str() );
    s_files.insert(  std::map<int, std::ofstream*>::value_type(m_port, of) );
  }
#endif
}

QtServerSocket::~QtServerSocket(){
  delete m_timer;
  delete m_task;
}


//-------------------------------------------------
// newLine
//-------------------------------------------------
bool QtServerSocket::newLine( std::string &line ){
  return m_listener->newLine( line );
}

//-------------------------------------------------
// stop
//-------------------------------------------------
void QtServerSocket::stop(){
  if( m_tcpserver ){
    delete m_tcpserver;
    m_tcpserver = 0;
  }
}

//-------------------------------------------------
// endOfRead
//-------------------------------------------------
void QtServerSocket::endOfRead(){
  if (!m_currentSocket) return;
  if (!m_currentSocket->getTcpSocket()) return;
  if( m_currentSocket->getTcpSocket()->state()!=QTcpSocket::ConnectedState &&
      m_currentSocket->getTcpSocket()->state()!=QTcpSocket::UnconnectedState){
    m_timer->start();
    return;
  }

  m_currentSocket->setWait( false );
  std::ostringstream os;
  if (m_listener->response(os)) {
    os << std::endl << "EOF" << std::endl;
    m_currentSocket->getTcpSocket()->write( os.str().c_str(),  os.str().size() );
#if FILE_LOG
  *s_files[m_port] << QTime::currentTime().toString(Qt::LocalDate).toStdString() <<":sw: " <<  os.str() << std::endl << std::flush;
#endif
  }
  else {
    m_currentSocket->getTcpSocket()->write( "EOF\n", 4 );
#if FILE_LOG
    *s_files[m_port] << QTime::currentTime().toString(Qt::LocalDate).toStdString() <<":sw: " <<  "EOF" << std::endl << std::flush;
#endif
  }
  m_currentSocket->getTcpSocket()->close();
  m_currentSocket = 0;
}

//-----------------------------------------------------
// start
//-----------------------------------------------------
void QtServerSocket::start(){
  if( m_tcpserver == 0 ){
    m_tcpserver = new QTcpServer(); // m_port, this );
    bool ret =
      m_tcpserver->listen( QHostAddress::Any, m_port);
    if (!ret) { // if fails
      std::ostringstream os;
      os << compose(_("Address already in use. (Port %1)"), m_port);
      os << std::endl << std::endl;
      os << _("Please fix the problem and restart the application.");
      GuiFactory::Instance()->showDialogWarning(NULL,"Socket Error", os.str(), NULL);
    }

    connect( m_tcpserver, SIGNAL(newConnection()), this, SLOT(slot_newConnection()) );
  }
}

//-----------------------------------------------------
// slot_newConnection
//-----------------------------------------------------
void QtServerSocket::slot_newConnection(){
  BUG_DEBUG("slot_newConnection: this: " << this << ": START");
  m_currentSocket = new MySocket( this, m_tcpserver->nextPendingConnection() );
#if FILE_LOG
  std::map<int, std::ofstream*>::iterator it=s_files.find(m_port);
  (*it).second->close();
  delete (*it).second;
  s_files[m_port] = new std::ofstream( std::string("/tmp/ssocket_"+QString::number(m_port).toStdString()+".log").c_str() );
#endif
  BUG_DEBUG("slot_newConnection: this: " << this
            << ": m_currentSocket = " << m_currentSocket
            << ": m_port = " << m_port
            << ", DONE");
}


//-----------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------
MySocket::MySocket( QtServerSocket *server, QTcpSocket *socket )
  : QObject()
    , m_socket( socket )
    , m_wait( false )
    , m_server( server ){
  m_timer=GuiFactory::Instance()->createTimer( 0 );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );

  //  s_os << "MySocket::MySocket " << m_server->getName() << " " << this << std::endl;

  connect( m_socket, SIGNAL(readyRead()),
	   SLOT(slot_readyRead()) );
//   connect( this, SIGNAL(delayedCloseFinished()),
// 	   SLOT(slot_delayedCloseFinished()) );
  connect( m_socket, SIGNAL(disconnected()),
	   SLOT(slot_deleteLater()) );
  connect( m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	   SLOT(slot_error(QAbstractSocket::SocketError)) );
}

MySocket::~MySocket(){
  delete m_timer;
  delete m_task;
  disconnect( this, SLOT(slot_readyRead()) );
//   connect( this, SIGNAL(delayedCloseFinished()),
// 	   SLOT(slot_delayedCloseFinished()) );
  disconnect( this, SLOT(slot_deleteLater()) );
  disconnect( this, SLOT(slot_error(QAbstractSocket::SocketError)) );
  if (m_socket)
    m_socket->deleteLater();
  if (m_server->m_currentSocket == this)
    m_server->m_currentSocket = 0;
#if FILE_LOG
  else
    *s_files[m_server->m_port] << QTime::currentTime().toString(Qt::LocalDate).toStdString() <<":~MySocket currentSocket NOT this" << std::endl << std::flush;
#endif

  m_socket = 0;
}

//-----------------------------------------------------
// slot_delayedCloseFinished
//-----------------------------------------------------
// void MySocket::slot_delayedCloseFinished(){
//   QTcpSocket* socket = (QTcpSocket*)sender();
//   delete socket;
// }

//-----------------------------------------------------
// slot_deleteLater
//-----------------------------------------------------
void MySocket::slot_deleteLater(){
  BUG_DEBUG("slot_deleteLater: this: " << this << ": START");
  disconnect( this, SLOT(slot_error(QAbstractSocket::SocketError)) );
  deleteLater();
  BUG_DEBUG("slot_deleteLater: this: " << this << ": DONE");
}

//-----------------------------------------------------
// slot_readyRead
//-----------------------------------------------------
void MySocket::slot_readyRead(){
  BUG_DEBUG("slot_readyRead: this: " << this << ": START");
  read();
  BUG_DEBUG("slot_readyRead: this: " << this << ": DONE");
}

//-----------------------------------------------------
// slot_error
//-----------------------------------------------------
void MySocket::slot_error(QAbstractSocket::SocketError socketError){
  BUG_DEBUG("slot_error: this: " << this << ": START");
  std::ostringstream err;
  err << _("ERROR : ServerSocket - ") <<  m_server->m_port << " - ";
  switch( socketError ){
  case QTcpSocket::ConnectionRefusedError :
    err << "connection refused";
    break;
  case QTcpSocket::HostNotFoundError :
    err << "host not found";
    break;
  case QTcpSocket::SocketAccessError :
    err << "access to socket failed";
    break;
  case QTcpSocket::RemoteHostClosedError :
    err << "remote host closed error";
    break;
  default :
    err << "undefined...["<<socketError<<"]";
    break;
  }
  BUG_WARN(err.str());
  err << std::endl;
  GuiFactory::Instance()->getLogWindow()->writeText( err.str() );
//   m_socket->deleteLater();
//   m_socket = 0;
//   deleteLater();
  BUG_DEBUG("slot_error: this: " << this << ": DONE");
}

//-----------------------------------------------------
// read
//-----------------------------------------------------
void MySocket::read(){
  BUG_DEBUG("read: this: " << this << ": START");
  if( m_wait ){
    m_timer->start();
    BUG_DEBUG("read: this: " << this << ": m_wait -> DONE");
    return;
  }
  bool endOfData(false);
  while ( m_socket->canReadLine() && !endOfData ) {
    m_wait = true;
    QByteArray ba = m_socket->readLine();
    std::string s(  ba );
#if FILE_LOG
    *s_files[m_server->m_port] << QTime::currentTime().toString(Qt::LocalDate).toStdString() <<":sr: " << s << std::flush;
#endif
//    std::string s(  m_socket->readLine() );
    while (ba.endsWith('\n') == 0) {
      ba = m_socket->readLine();
      s += std::string(ba);
    }

    endOfData=m_server->newLine( s );
  }
  if( !endOfData ){ // Protokoll spezifisch
    m_wait=false;
  }
  else{
  }
  BUG_DEBUG("read: this: " << this << ": DONE");
}
