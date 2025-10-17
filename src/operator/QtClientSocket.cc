
#include <QTimerEvent>
#include <QTime>

#include <iostream>

#include "utils/gettext.h"
#include "utils/Date.h"

#include "gui/GuiScrolledText.h"
#include "gui/Timer.h"
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/GuiElement.h"

#include "streamer/Stream.h"

#include "gui/Timer.h"
#include "operator/SocketException.h"
#include "operator/ClientSocket.h"
#include "operator/ConnectionListener.h"
#include "operator/QtClientSocket.h"

#define FILE_LOG 0
#if FILE_LOG
#include <map>
#include <fstream>
static std::map<int, std::ofstream*> s_files;
#endif

INIT_LOGGER();

// -----------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------
QtClientSocket::QtClientSocket( const std::string &header
				, Stream *out_stream
				, Stream *in_stream )
  : ClientSocket( out_stream, in_stream )
  , m_header( header )
  , m_socket( 0 )
  , m_timeout( -1 )
  , m_timeout2( -1 ){
  m_timer=GuiFactory::Instance()->createTimer( 0 );
  m_task = new  MyTimerTask(this);
  m_timer->addTask( m_task );
}

QtClientSocket::~QtClientSocket(){
  if( m_socket ){
    m_socket->deleteLater();
  }
  delete m_task;
  delete m_timer;
  if (m_timeout!= -1)
    killTimer(m_timeout);
  if (m_timeout2!= -1)
    killTimer(m_timeout2);
}

// -----------------------------------------------------
// start
// -----------------------------------------------------

void QtClientSocket::start(){
  if( !m_socket ){
#if FILE_LOG  // als Debug-Möglichkeit: auch alles in LogFiles geschrieben.
    if (s_files.find(getPort()) == s_files.end()) {
      std::ofstream *of = new std::ofstream( std::string("/tmp/csocket_"+QString::number(getPort()).toStdString()+".log").c_str());
    s_files.insert(  std::map<int, std::ofstream*>::value_type(getPort(), of) );
    }
#endif
    m_socket = new QTcpSocket();

    connect( m_socket, SIGNAL(connected()),
	     this, SLOT(slot_connected()) );
    connect( m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	     this, SLOT(slot_error(QAbstractSocket::SocketError)) );
    connect( m_socket, SIGNAL(disconnected() ),
 	     this, SLOT(slot_connectionClosed() ) );
    connect( m_socket, SIGNAL(readyRead() ),
	     this, SLOT(slot_readyRead() ) );
  }
  if(m_timeout != -1) {
    BUG_WARN("start: this: " << this
             << " called when m_timeout was " << m_timeout << " (not -1)");
    // killTimer(m_timeout);
    // m_timeout = -1;
  }
  if(m_timeout2 != -1) {
    BUG_WARN("start: this: " << this
             << " called when m_timeout2 was " << m_timeout2 << " (not -1)");
    // killTimer(m_timeout2);
    // m_timeout2 = -1;
  }
  m_timeout = startTimer( 1000 );
  BUG_DEBUG("start: this: " << this << ": m_timeout = " << m_timeout);
  m_socket->connectToHost( getHost().c_str(), getPort() );
}

// -----------------------------------------------------
// slot_connected
// -----------------------------------------------------

void QtClientSocket::slot_connected(){
  if (m_timeout!= -1)
    killTimer(m_timeout);
  m_timeout = -1;

  std::ostringstream ostr;
  if( getOutStream() ){
    getOutStream()->write( ostr );
  }
  // Clear is now done before getInStream()->read( );
  //   if( getInStream() ){
  //     getInStream()->clearRange( );
  //   }
  std::ostringstream fullStream;
  if ( m_header.size() ) {
    fullStream.width( 2 );
    fullStream.fill( '0' );
    fullStream << m_header.size();
    fullStream << m_header;
    fullStream.width( 8 );
    fullStream.fill( '0' );
    fullStream << ostr.str().size();
  }
  fullStream << ostr.str() << std::endl;
  if ( !m_header.size() ) {
    fullStream << "EOF" << std::endl;
  }

#if FILE_LOG   // als Debug-Möglichkeit: auch alles in LogFiles geschrieben.
  *s_files[getPort()] << QTime::currentTime().toString(Qt::LocalDate).toStdString() <<":cw: " << fullStream.str() << std::flush;
#endif
  m_socket->write( fullStream.str().c_str(), fullStream.str().size() );
  if( getInStream() == 0 ){
    m_timer->start();
  }
  else {
    m_timeout2 = startTimer(60000);
    BUG_DEBUG("slot_connected: this: " << this << ": m_timeout2 = " << m_timeout2);
  }
}

// -----------------------------------------------------
// slot_error
// -----------------------------------------------------

void QtClientSocket::slot_error(QAbstractSocket::SocketError socketError){
  if (m_timeout != -1)
    killTimer(m_timeout);
  if (m_timeout2 != -1)
    killTimer(m_timeout2);
  m_timeout = -1;
  m_timeout2 = -1;

  std::string err = DateAndTime() + ": " + _("ERROR : ClientSocket - ");
  switch( socketError ){
  case QTcpSocket::RemoteHostClosedError :
    err += _("RemoteHostClosedError : ");
    break;
  case QTcpSocket::ConnectionRefusedError :
    err += _("Connection Refused : ");
    break;
  case QTcpSocket::HostNotFoundError :
    err += _("Host not found : ");
    break;
  case QTcpSocket::SocketAccessError :
    err += _("Socket Access failed : ");
    break;
  default :
    err +=  " undefined...["+QString::number(socketError).toStdString()+"] ";
    break;
  }

  std::ostringstream ostr;
  if( getOutStream() ){
    getOutStream()->write( ostr );
  }
  err += getHost() + ":" + QString::number(getPort()).toStdString()
    + "(" + m_header + ":" + ostr.str() + ")";
  BUG_WARN("slot_error: this: " << this << ": " << err);
    err += "\n";
  GuiFactory::Instance()->getLogWindow()->writeText( err );
  m_timer->start();
}

// -----------------------------------------------------
// slot_delayedCloseFinished
// -----------------------------------------------------

void QtClientSocket::slot_delayedCloseFinished(){
  connectionClosed();
}

// -----------------------------------------------------
// slot_connectionClosed
// -----------------------------------------------------

void QtClientSocket::slot_connectionClosed(){
  if (!m_socket) return; // socket already closed
  m_timer->start();
}

// -----------------------------------------------------
// connectionClosed
// -----------------------------------------------------

void QtClientSocket::connectionClosed(){
  if (!m_socket) return; // socket already closed
  if( getInStream() ){
    if( !m_input.str().empty() ){
      std::istringstream is( m_input.str() );
      m_input.str("");
      getInStream()->clearRange( ); // no longer done in slot_connected()
      getInStream()->read( is );
    }
  }
  m_socket->deleteLater();
  m_socket=0;
  if( getListener() )
    getListener()->connectionClosed();
}

// -----------------------------------------------------
// slot_readyRead
// -----------------------------------------------------

void QtClientSocket::slot_readyRead(){
  if (m_timeout2 != -1)
    killTimer(m_timeout2);
  m_timeout2 = -1;
//   std::cerr << "QtClientSocket::slot_readyRead : ";
//   if( getOutStream() )
//     std::cerr << getOutStream()->Name();
//   std::cerr << " this=" << this << std::endl;

  if( !m_socket->canReadLine() ) {
    std::cerr << " QtClientSocket::slot_readyRead called but no data can be read.["<<m_socket->readAll().data()<<"] Port["<<getPort()<<"]\n" <<std::flush;
  }
  while ( m_socket->canReadLine() ) {
    std::string s( m_socket->readLine() );
#if FILE_LOG   // als Debug-Möglichkeit: auch alles in LogFiles geschrieben.
  *s_files[getPort()] << QTime::currentTime().toString(Qt::LocalDate).toStdString() <<":cr: " << s << std::flush;
#endif
    if( s.substr(0,3) == "EOF" ) {
      //std::cerr << "QtClientSocket::slot_readyRead : EOF" << std::endl;
      if( !m_input.str().empty() ){
	std::istringstream iis( m_input.str() );
	m_input.str("");
	if( getInStream() ) {
	  getInStream()->clearRange( ); // no longer done in slot_connected()
	  getInStream()->read( iis );
	}
      }
      clear();
      //      m_timer->start();
      return;
    }
    else{
      m_input << s;
    }
  }
}

// -----------------------------------------------------
// clear
// -----------------------------------------------------

void QtClientSocket::clear(){
//   std::cerr << "QtClientSocket::clear : ";
//   if( getOutStream() )
//     std::cerr << getOutStream()->Name();
//   std::cerr << " this=" << this << std::endl;
  if( m_socket ){
    m_socket->close();
    if (m_socket->state() == QTcpSocket::ClosingState ) {
      disconnect(m_socket, SIGNAL(disconnected()), 0, 0);
      connect( m_socket, SIGNAL(disconnected()),
	       this, SLOT(slot_delayedCloseFinished()) );
      return;
    }
  }
  connectionClosed();
}

void QtClientSocket::timerEvent( QTimerEvent *e ){
  int id = e->timerId();
  BUG_WARN("timerEvent: this: " << this
           << ": id = " << id
           << ", m_timeout = " << m_timeout
           << ", m_timeout2 = " << m_timeout2);
  // assert( id == m_timeout || id == m_timeout2);
  slot_error( QTcpSocket::ConnectionRefusedError );
}
