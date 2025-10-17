#include <errno.h>

#include "operator/Channel.h"
#if defined HAVE_QT
#include <qprocess.h>
#include <qstring.h>
#include <qsocketnotifier.h>
#ifdef __MINGW32__
#include <qfile.h>
#include <qtextstream.h>
#endif
#endif
#include <sys/stat.h>
#include <limits.h>
#include <string>
#include <zlib.h>

#include <sstream>

#include <assert.h>
#include <complex>
#include <fcntl.h>

#include "utils/Debugger.h"
#include "utils/Date.h"
#include "utils/FileUtilities.h"

#include "operator/Channel.h"
#include "operator/ProcessWrapper.h" /*TEST*/
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/GuiScrolledText.h"
#include "parser/errorhandler.h"
#include "app/AppData.h"
#if 1
#include "utils/gzstream.h"
#endif
#include "operator/OpProcess.h"
// #include "utils/MultiLanguage.h"
#include "utils/gettext.h"
#include <fstream>
#if ( __GNUC__ > 2 )
  #include "utils/fdiostream.h"
#endif
#include "streamer/Stream.h"

#define CLOSED -1
#define CHANNEL_BUFSIZ 4096
#ifdef __MINGW32__
#define PIPE_BUF 4096
#endif
#define MAX_BUFSIZ   PIPE_BUF


#if defined HAVE_QT
#else
//#if defined( __osf__ )
#define BLOCK_SIGNALS
//#endif
#endif

INIT_LOGGER();

#if !defined HAVE_QT
ErrChannel::ErrChannel()
#if defined HAVE_QT
  : m_notifier(0)
#endif
{
  m_fd[0] = CLOSED;
  m_fd[1] = CLOSED;
}

ErrChannel::~ErrChannel(){
#if defined HAVE_QT
  if (m_notifier) delete m_notifier;
#endif
}
/*------------------------------------------------------------
  ErrChannel::Close --
  ------------------------------------------------------------
*/
void ErrChannel::close(){
//   cerr << "ErrorChannel::close " << m_fd[0] << "/" << m_fd[1] << endl;
  if( m_fd[0] != CLOSED ){
    ::close( m_fd[0] );
    m_fd[0] = CLOSED;
  }
}

/*------------------------------------------------------------
 */
void ErrChannel::getInput( XtPointer po, int *fid, XtInputId *id ){
  ErrChannel *obj = (ErrChannel *)po;
  char         buf[CHANNEL_BUFSIZ+1];
  int          nbytes;


  BUG( BugOperator, "ErrChannel::getInput" );
#ifdef BLOCK_SIGNALS
  Signal::block();
#endif
  if ( (nbytes = read( *fid, buf, CHANNEL_BUFSIZ)) == -1 ) {
#ifdef BLOCK_SIGNALS
    Signal::unblock();
#endif
    perror( "ErrChannel::getInput" );
//     IPrintMessage( __FILE__, __LINE__, "ML_CH_FILE_ERROR" );
    IPrintMessage( _("Read error.") );
#ifdef HAVE_QT
    XtRemoveInput( *id );
#endif
    return;
  }
#ifdef BLOCK_SIGNALS
  Signal::unblock();
#endif
  if ( nbytes == 0 ){ /* pipe is empty */

    if( obj!=0 )
      obj->close( );
#ifdef HAVE_QT
    XtRemoveInput( *id );
#endif
    return;
  }

  buf[nbytes]='\0';

  if( nbytes > 0 ){
    GuiFactory::Instance()->getLogWindow()->writeText( buf );
  }
  BUG_MSG( buf );
  return;
}

/*------------------------------------------------------------
 * register ErrChannel::getInput as Input Event Handler
 */
void ErrChannel::addInput( )
{
  BUG(BugOperator, "ErrChannel::addInput()" ) ;
  BUG_MSG( "m_fd[0] " << m_fd[0] );
  ::close( m_fd[1] );
#if defined HAVE_QT
  m_notifier = new ChannelNotifier(NULL);
  QSocketNotifier *sn;
  sn = new QSocketNotifier( m_fd[0], QSocketNotifier::Read, m_notifier);
  m_notifier->setNotifier(sn);
  QObject::connect( sn, SIGNAL(activated(int)),
		    m_notifier, SLOT(dataErrorReceived(int)) );
#else
  // TODO?
#endif
}
/*------------------------------------------------------------
  ErrChannelCreatePipe --
  ------------------------------------------------------------
*/
bool ErrChannel::createPipe( )
{
  if( pipe(m_fd) != 0 )
    return false;
//   cerr << "ErrChannel::createPipe " << m_fd[0] << "/" << m_fd[1] << endl;
  return true;
}
#endif

#ifndef __MINGW32__
/*------------------------------------------------------------
 */
Fifo::Fifo( const std::string &fifo ): m_fifo(fifo){

  if( !fifo.empty() ) {
    if ( (mkfifo( fifo.c_str(), 0666 ) < 0 ) ){
      /* fifo already exists !? */
      struct stat statbuf;
      if( stat( fifo.c_str(), &statbuf )!= 0 ){
	perror( fifo.c_str() );
	abort();
      }
      if( !S_ISFIFO( statbuf.st_mode ) ){
	std::cerr << fifo << " is not a named pipe." << std::endl;
	abort();
      }
    }
  }
}

/*------------------------------------------------------------
  Fifo::createName --

  ------------------------------------------------------------
*/
std::string *Fifo::createName( const std::string &name )
{
  std::ostringstream buf;
  buf << "/tmp/" << (int)getpid() << name;
  std::string *title = new std::string;
  *title = buf.str();
  return title;
}

int Fifo::open(int oflag) {
  return ::open( m_fifo.c_str(), oflag );
}
#endif

/*------------------------------------------------------------
 */
OutputChannel::OutputChannel(ChannelListener *s, const std::string &fifo, QProcess* qproc )
  : m_listener( s )
  , m_window( 0 )
  , m_ostr( 0 )
  , m_ostrNotClose(false)
  , m_xtid( 0 )
  , m_event( this )
  , m_uiUpdateInterval( 0 )
  , m_getInputCounter( 0 )
#if defined HAVE_QT
  , m_notifier(0)
  , m_qtProcess(qproc) {
#else
  , m_fifo( fifo ) {
  m_fd[0] = CLOSED;
  m_fd[1] = CLOSED;
#endif
}

/*------------------------------------------------------------
 */
OutputChannel::~OutputChannel(){
#if defined HAVE_QT
  if (m_notifier) delete m_notifier;
#endif
  if (m_ostr && !m_ostrNotClose) delete m_ostr;
  if (m_listener)
    m_listener->resetContinuousUpdate();
}

/*------------------------------------------------------------
 */
 InputChannel::InputChannel( ChannelListener *s, const std::string &fifo, QProcess* qproc ):
  m_listener(s), m_istr( 0 )
  , m_event(this)
  //  , m_send_thread( this )
#if defined HAVE_QT
  , m_qtProcess(qproc) {
#else
  , m_fifo(fifo)
  , m_ostr( 0 )
  {
    m_fd[0] = CLOSED;
    m_fd[1] = CLOSED;
#endif
}

InputChannel::~InputChannel() {
#if !defined HAVE_QT
  delete m_ostr;
#endif
}

/*------------------------------------------------------------
  write --
  starts a new thread
  ------------------------------------------------------------
*/
bool InputChannel::write( ){
 BUG_DEBUG("InputChannel::write" );
#if !defined HAVE_QT
  if( m_ostr == 0 ){
    BUG_MSG( "opening ofstream " << m_fd[1] );
#if ( __GNUC__ > 2 )
    m_ostr = new fdostream( m_fd[1] );
#else
    m_ostr = new std::ofstream( m_fd[1] );
#endif
  }
#endif
  //  m_send_thread.start();
  startThread();
  return true;
}

/*------------------------------------------------------------
 */
void OutputChannel::close(){
  BUG( BugOperator, "OututChannel::close" );
  if( m_ostr && !m_ostrNotClose){
    delete m_ostr;
  }
  m_ostr = 0;
#if !defined HAVE_QT
  if( m_fd[0] != CLOSED ){
    ::close( m_fd[0] );
  }
  m_fd[0] = CLOSED;
#endif
}

#if !defined HAVE_QT
bool OutputChannel::isOpen(){
  return m_fd[0] != CLOSED;
}
#endif

// void InputChannel::startThread() {
//   InputChannel::doWrite( this );
// }

/*------------------------------------------------------------
  doWrite --
    THREAD routine
*/
void InputChannel::startThread() {
  BUG_DEBUG("InputChannel::startThread()" );

  if( m_istr == 0 ){
    if( m_listener!=0 ){
      BUG_DEBUG( "stream write" );
      if( !m_listener->write( m_event ) ) {
	GuiFactory::Instance() -> showDialogWarning
	  ( 0
	    , _("Warning")
	    , _("Write Error in input channel.\nRuntime error in Stream Process.\n\n(For details see log window)")
	    , 0 );
      return;
      }
    }
  }
  else {
    std::streamsize bufsiz = MAX_BUFSIZ;
    char *buf = new char [ bufsiz+1 ];
    while(true){
      std::streamsize nbytes;
      m_istr->read( buf, bufsiz );
      nbytes = m_istr->gcount();
      if( nbytes > 0 ){
        BUG_DEBUG( "read " << nbytes );
        buf[nbytes] = '\0';
#if defined HAVE_QT
        assert(m_qtProcess != 0 );
        std::ostringstream os(buf);
        m_qtProcess->write( QByteArray(os.str().c_str())  );
        if (0) {
#else
        if( !m_ostr->write( buf, nbytes ) ){
#endif
          perror( "InputStream::doWrite" );
          break;
        }
        else {
          BUG_DEBUG( "wrote " << nbytes);
        }
      }
      else {
        break;
        // can we ignore it? on winnt a closed stream is not good!
        //          if( !pin->m_istr->good() )
        //            perror( "InputStream::doWrite" );
      }
    }
    delete [] buf;
    }
#if defined HAVE_QT
        m_qtProcess->closeWriteChannel();
#endif
  close();
}
/*------------------------------------------------------------
  StopInput --
*/
bool InputChannel::stop( ){
 BUG_DEBUG("InputChannel::stop");
  //  m_send_thread.terminate( true );
  return true;
}
#if !defined HAVE_QT
/*------------------------------------------------------------
  ChannelAddInput --
   registers OutputChannel::getInput as input event handler
*/
int OutputChannel::addInput( )
{
  BUG(BugOperator,"OutputChannel::addInput()" );
  if( m_fifo() ){
    BUG_MSG("Opening fifo " << m_fifo.getName() );
    if( (m_fd[0] = m_fifo.open( O_RDONLY )) < 0 ){
      perror( m_fifo.getName().c_str() );
      if( errno != EAGAIN )
	return -1;
    }
  }
  else {
    BUG_MSG("PIPES " << m_fd[0] << "/" << m_fd[1] );
    ::close( m_fd[1] );
  }
#if defined HAVE_QT
  m_notifier = new ChannelNotifier(this);
  QSocketNotifier *sn;
  sn = new QSocketNotifier( m_fd[0], QSocketNotifier::Read, m_notifier);
  m_notifier->setNotifier(sn);
  QObject::connect( sn, SIGNAL(activated(int)),
		    m_notifier, SLOT(dataOutputReceived(int)) );
#else
  // TODO?
#endif
  return 1 ;
}
#endif

/*------------------------------------------------------------
  getInput -
    This is the callback routine installed by XtAppAddInput.
    It reads the file pointed by *fid and calls stream -> read
    if all data has been read and if pp is not NULL otherwise
    the data will be printed in the error window.

  ------------------------------------------------------------
*/
void OutputChannel::getInput( XtPointer po, int *fid, XtInputId *id ){
  OutputChannel            *out = (OutputChannel *)po;
  out->getInput( fid, id );
}

void OutputChannel::getInput( int *fid, XtInputId *id ){
  char                      buf[CHANNEL_BUFSIZ+1];
  int                       nbytes;
  BUG(BugOperator, "OutputChannel::getInput(int *fid, XtInputId *id )" );
  BUG_MSG( "Channel = " << this << " fileId = " << fid  << " XtInputId = " << id );
#ifdef BLOCK_SIGNALS
  Signal::block();
#endif
  if ( (nbytes = ::read( *fid, buf, CHANNEL_BUFSIZ)) == -1 ) {
    perror( "OutputChannel::getInput" );
//     IPrintMessage( __FILE__, __LINE__, "ML_CH_FILE_ERROR" );
    IPrintMessage( _("Read error.") );
    std::cerr << "Channel: " << this << " File: " << *fid << " XtInput: " << *id << std::endl;
#ifdef BLOCK_SIGNALS
    Signal::unblock();
#endif
    return;
  }
#ifdef BLOCK_SIGNALS
  Signal::unblock();
#endif
  getInput(buf, nbytes, id);
}

/* --------------------------------------------------------------------------- */
/* getInput                                                                    */
/* --------------------------------------------------------------------------- */

void OutputChannel::getInput(const char* buf, int nbytes, void* id) {
  BUG(BugOperator,"OutputChannel::getInput(char* ...)" );
  //  // this is beacause of continuousUpdate
  bool uiUpdate( false );
  if( m_uiUpdateInterval != 0 )
    if( (uiUpdate = (nbytes == 0 ? false : (buf[nbytes -1] == '\n' ? true : false))) )
      ++m_getInputCounter;

  if ( nbytes == 0 || ( ( m_uiUpdateInterval == m_getInputCounter ) && uiUpdate ) ){
    m_getInputCounter = 0;
    if( nbytes != 0 ){
      // we are here because of continuousUpdate
      if( m_window )
        // send the output to the text window if there is one registered
        m_window->writeText( std::string(buf, nbytes) );
      m_buffer.append( buf, nbytes );
      // BUG_MSG( "read nbytes (not yet finished) " << nbytes << " Buffer length: " << out->m_buffer.length()  );
    }
    // Send the buffer to the output stream
    // or invoke the listener's read function. Everything else
    // is silently ignored
    if( m_ostr != 0 ){
      // BUG_MSG( "ostr " << m_buffer.length() );
      *m_ostr << m_buffer;
    }
    else if( m_listener != 0 ){
#ifdef BLOCK_SIGNALS
      Signal::block();
#endif
      BUG_MSG( "read nbytes " << m_buffer.length()  );
      if ( !m_listener->read( m_event ) ) {
        GuiFactory::Instance() -> showDialogWarning
          ( 0
            , _("Warning")
            , _("Read Error in output channel.\nRuntime error in Stream Process.\n\n(For details see log window)")
            , 0 );
        return;
      }
#ifdef BLOCK_SIGNALS
      Signal::unblock();
#endif
    }
    if( nbytes == 0 ){
      // we are here because we have read all Data. This is definitive the End of Input !
#if defined HAVE_QT
      if (m_notifier)
        m_notifier->deleteNotifier();
#endif
      close();
      if( m_listener != 0 )
        // We have to reset the Stream. Otherwise it will wait for more Input, and wait, and wait ....
        m_listener->resetContinuousUpdate();
    }
    else{ // we are here because of continuousUpdate, so we will do that !
      GuiManager::Instance().update( GuiElement::reason_Process );
    }
    m_buffer = "";
  }
  else{
    if( m_window ) // send the output to the text window if there is one registered
      m_window->writeText( std::string(buf, nbytes) );
    m_buffer.append( buf, nbytes );
    //  BUG_MSG( "read nbytes (not yet finished) " << nbytes << " Buffer length: " << out->m_buffer.length()  );
  }
}

#if defined HAVE_QT
/*------------------------------------------------------------
  write --
  write to Stdin of QProcess
  ------------------------------------------------------------
*/
bool InputChannel::write(std::ostringstream& os){
  if ( os.str().empty() ) {
    m_qtProcess->closeWriteChannel();
    return true;  // false ? Warum ein leerer Stream  return false?
  }
  assert(m_qtProcess != 0);
  qint64 len = m_qtProcess->write( QByteArray(os.str().c_str()) );
  if( len != os.str().size()){
    GuiFactory::Instance()->getLogWindow()->writeText(  DateAndTime() + ": process input channel write ERROR\n"  );
    if (m_listener) {
      BasicStream* bStream = dynamic_cast<BasicStream*>(m_listener);
      if (bStream) {
        GuiFactory::Instance()->getLogWindow()->writeText(  DateAndTime() + ": stream named: " + bStream->Name() + "\n"  );
      }
    }
  }
  //amg 2015-02-21 muss nicht sein ???
  m_qtProcess->closeWriteChannel();
  return true;
}
#else
/*------------------------------------------------------------
  InChannelDup --
  ------------------------------------------------------------
*/
bool InputChannel::dup(){
  if( m_fifo() ){
    return 0;
  }
  if( dup2( m_fd[0], STDIN_FILENO ) != STDIN_FILENO )
    return false;
  if( ::close( m_fd[0] ) != 0 || ::close( m_fd[1] ) != 0 )
    return false;

  return true;
}
/*------------------------------------------------------------
  ErrChannelDup --
  ------------------------------------------------------------
*/
bool ErrChannel::dup( )
{
//   cerr << "ErrorChannel::dup " << m_fd[0] << "/" << m_fd[1] << endl;
  if ( dup2( m_fd[1], STDERR_FILENO ) != STDERR_FILENO ) exit( -1 );
  if ( (::close( m_fd[0] ) != 0) || (::close( m_fd[1] ) != 0) )
      return false;
  return true;
}

/*------------------------------------------------------------
  InChannelCreatePipe --
  ------------------------------------------------------------
*/
bool InputChannel::createPipe( )
{
  if( m_fifo() ) {
    return true;
  }
  /* no named pipes */
  if( pipe(m_fd) != 0 ) {
      return false;
  }
  return true;
}
#endif

#if !defined HAVE_QT
/*------------------------------------------------------------
  OutChannelCreatePipe --
  ------------------------------------------------------------
*/
bool OutputChannel::createPipe()
{
  BUG(BugOperator, "OutputChannel::createPipe()" );
  if( m_fifo() )
    return true;
  /* no named pipes */
  if( pipe(m_fd) != 0 )
      return false;
  return true;
}

/*------------------------------------------------------------
  OutChannelDup --
  ------------------------------------------------------------
*/
bool OutputChannel::dup( ){
  if( m_fifo() ){
    return true;
  }
  if( dup2( m_fd[1], STDOUT_FILENO ) != STDOUT_FILENO )
    return false;
  if( (::close( m_fd[0] ) != 0) || (::close( m_fd[1] ) != 0) )
    return false;

  return true;
}
#endif

/*------------------------------------------------------------
  InChannelClose --
  ------------------------------------------------------------
*/
void InputChannel::close( )
{
  BUG_DEBUG("InputChannel::close()");
  if( m_istr != 0 ){
    BUG_DEBUG( "delete m_istr" );
    delete m_istr;
    m_istr=0;
  }
#if !defined HAVE_QT
  if( m_ostr != 0 ){
    BUG_DEBUG("delete m_ostr");
    delete m_ostr;
    m_ostr=0;
  }
//   else {
    ::close( m_fd[1] );
//   }
//   if( m_fd[0] != CLOSED )
    ::close( m_fd[0] );

  m_fd[0] = CLOSED;
  m_fd[1] = CLOSED;
#endif
}

/*------------------------------------------------------------
  OutChannelOpen --
  ------------------------------------------------------------
*/
bool OutputChannel::open( const std::string &filename ){
  BUG( BugOperator, "OutputChannel::open" );
  if( m_ostr != 0 )
    delete m_ostr;
  BUG_MSG( filename );
  m_ostr = new std::ofstream( filename.c_str() );
  if( !m_ostr ){
//     IPrintMessage( __FILE__, __LINE__, "ML_CH_CANT_OPEN_FILE", filename.c_str() );
    IPrintMessage( compose(_("Cannot open '%1.'."),filename) );
    return false;
  }
  return true;
}
/*------------------------------------------------------------
  InChannelOpen --
  ------------------------------------------------------------
*/
bool InputChannel::open( const std::string &filename )
{
  BUG_DEBUG("InputChannel::open, file: " << filename);
  if( m_istr != 0 )
    delete m_istr;
#if 1
  m_istr = new igzstream( filename.c_str() );
#else
  m_istr = new gzifstream( filename );
#endif
  if( !(*m_istr ) ){
    m_istr = new std::ifstream( filename.c_str() );
#ifdef __MINGW32__
    // !!! der ifstream kann nicht mit Zeilenenden "\n\r" umgehen
    // => daher wird der ifstream zu einem istringstream (2007-06-05; gcc 3.4.2)
    std::ostringstream ofs;
    char c;
    while( (c=m_istr->get()) != EOF ) ofs << c;
    delete m_istr;

    BUG_DEBUG("First attempt, size: " << ofs.str().size());
    if (ofs.str().size() == 0) {
      m_istr = FileUtilities::ReadFile(filename);
    } else {
      m_istr = new std::istringstream( ofs.str().c_str() );
    }
#endif
  }
  if( !m_istr ){
    IPrintMessage( compose(_("Cannot open '%1.'."),filename) );
    BUG_DEBUG(compose(_("Cannot open '%1.'."),filename));
    return false;
  }

  BUG_DEBUG(filename);
  return true;
}
/*------------------------------------------------------------
  clearTextWindow --

*/
bool OutputChannel::clearTextWindow( )
{
  if( m_window ){
    m_window->clearText();
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setWindow --                                                                */
/* --------------------------------------------------------------------------- */

bool OutputChannel::setWindow( GuiScrolledText *t ){
  m_window = t;
  return true;
 }

/* --------------------------------------------------------------------------- */
/* setUiUpdateInterval --                                                      */
/* --------------------------------------------------------------------------- */

void OutputChannel::setUiUpdateInterval( int interval ){
  m_uiUpdateInterval = interval;
  if( m_listener != 0 )
    m_listener->setContinuousUpdate( interval != 0 );
}
