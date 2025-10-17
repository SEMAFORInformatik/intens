#include "qostream.h"

//--------------------------------------
// class qoutbuf
//--------------------------------------

qoutbuf::qoutbuf( QProcess *process )
  : m_process( process )
  , m_pending_data(false)
  , m_close_later(false){
  assert( process != 0 ); 
  connect( process, SIGNAL( wroteToStdin () )
	   , this, SLOT( slot_wroteToStdin () ) );
}
  
qoutbuf::~qoutbuf(){
  if( m_pending_data ){
    new qprocess_close_listener( m_process );
  }
  else{
    close();
  }
}

void qoutbuf::close(){
  if( m_pending_data ){
    m_close_later=true;
  }
  else{
    m_process->closeWriteChannel();
  }
}

void qoutbuf::slot_wroteToStdin(){
  m_pending_data = false;
  if( m_close_later ){
    m_process->closeWriteChannel();
  }
}
