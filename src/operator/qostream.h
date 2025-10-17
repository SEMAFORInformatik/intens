#include <QProcess>
#include <streambuf>
#include <iostream>
#include <assert.h>
#include <fstream>

#ifndef Q_OSTREAM
#define Q_OSTREAM

//------------------------------------------------------
// class qoutbuf
//------------------------------------------------------

class qoutbuf : public QObject, public std::streambuf {
Q_OBJECT  
public:
  qoutbuf( QProcess *process ); 
  virtual ~qoutbuf();
  
protected:
  virtual int_type overflow( int_type c = EOF) {
    if( c != EOF ) {
      if( c != EOF ) {
	char z[2] = {'\0','\0'};
	z[0] = c;
        m_pending_data=true;
	m_process->write( z );
      }
    }
    return c;
  }
  
  virtual std::streamsize xsputn( const char* s , std::streamsize anz ) {
    m_pending_data=true;
    m_process->write( s );
    return anz;
  }
  
  virtual int sync() {
    return 0;
  }

public:
  void close();

private slots:
void slot_wroteToStdin();

protected:
  QProcess *m_process;
private:
  bool m_pending_data;
  bool m_close_later;
};

//------------------------------------------------------
// class qostream
//------------------------------------------------------

class qostream : public std::ostream{
public:
  qostream( QProcess *process ) 
    : m_buf( process )
    , std::ostream(&m_buf ){
    rdbuf(&m_buf);
  }

  virtual ~qostream(){}

  void close(){
    m_buf.close();
  }
  
protected:
  qoutbuf m_buf;
};

//-------------------------------------
// class close_listener
//-------------------------------------

class qprocess_close_listener : public QObject{
Q_OBJECT
public:
qprocess_close_listener( QProcess *process ){
  connect( process, SIGNAL( wroteToStdin () )
	   , this, SLOT( slot_wroteToStdin () ) );
}
  virtual ~qprocess_close_listener(){}
private slots:
void slot_wroteToStdin(){
  m_process->closeWriteChannel();
  delete this;
}
private:
  QProcess *m_process;
};

#endif
