
#include "utils/Debugger.h"

#include "operator/ProcessWrapper.h"
#include "operator/BatchProcess.h"
#include "streamer/StreamManager.h"
#include "streamer/Stream.h"
#include "streamer/DataStreamParameter.h"

#include "gui/GuiFactory.h"

INIT_LOGGER();

BatchProcess::BatchProcess( const std::string &name, bool daemon )
  : Process(name)/*, m_shcmd("/bin/sh")*/, m_bDaemon(daemon) {
#if defined HAVE_QT
  m_process = new ProcessQt(this);
#else
  m_process = new ProcessPosix(this);
#endif
}

BatchProcess::BatchProcess( const BatchProcess &process )
  : Process( process ), m_bDaemon(process.m_bDaemon) /*, m_shcmd(process.m_shcmd)*/ {
#if defined HAVE_QT
  m_process = new ProcessQt(this);
#else
  m_process = new ProcessPosix(this);
#endif
}

QProcess* BatchProcess::getQProcess() {
  QProcess *proc = (m_process)->getQProcess();
  assert( proc != 0 );
  return proc;
}

BatchProcess::~BatchProcess(){
  if (m_process)
    delete m_process;
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

Process *BatchProcess::clone(){
  return new BatchProcess( *this );
}

/* --------------------------------------------------------------------------- */
/* addInputStream --                                                           */
/* --------------------------------------------------------------------------- */

Process::error_type
BatchProcess::addInputStream( const std::string &streamid, const std::string &fifo ){
  Stream *s = StreamManager::Instance().getStream( streamid );
  return addInputStream( s, fifo );
}

/* --------------------------------------------------------------------------- */
/* addInputStream --                                                           */
/* --------------------------------------------------------------------------- */

Process::error_type
BatchProcess::addInputStream( Stream *s, const std::string &fifo ){
  if( s == 0 ){
    return O_STREAM_UNDECLARED;
  }
  Process::addInputStream( s, fifo );
  InputChannel *in = new InputChannel( s, fifo, m_process->getQProcess() );
  if( in == 0 )
    return O_FIFO_IS_LOCKED;

  m_process->addChannel( in );
  return O_NO_ERROR;
}

/*------------------------------------------------------------
  AddChannel --
    adds an input or output channel to the channel list.
  ------------------------------------------------------------
*/
void BatchProcess::addChannel( OutputChannel *oc ){
  m_process->addChannel( oc );
}
void BatchProcess::addChannel( InputChannel *ic ){
  m_process->addChannel( ic );
}

/* --------------------------------------------------------------------------- */
/* setOutputBasicStreams --                                                    */
/* --------------------------------------------------------------------------- */

void BatchProcess::setOutputBasicStreams(){
  BUG( BugOperator, "BatchProcess::setOutputBasicStreams" );
  OutputChannel *out;
  BasicStreamList::iterator it;
  for( it = s_basicstreams.begin(); it != s_basicstreams.end(); ++it ){
    out = new OutputChannel( (*it).first, (*it).second );
    assert( out != 0 );

    out->setWindow( (*it).first->getTextWindow() );
    m_process->addChannel( out );
  }
  Process::setOutputBasicStreams();
}

/*------------------------------------------------------------
 */
Process::error_type
BatchProcess::setOutputStreams( const std::vector< std::pair<std::string,std::string> > &outstr )
{
  Stream *s;
  OutputChannel *out;
  BUG( BugOperator, "BatchProcess::setOutputStreams" );
  std::vector< std::pair<std::string,std::string> >::const_iterator oi;

  for( oi=outstr.begin(); oi!=outstr.end(); ++oi ){
    if ( (s = StreamManager::Instance().getStream( (*oi).first )) == 0 ){
      return O_STREAM_UNDECLARED;
    }
    if( ( out = new OutputChannel( s, (*oi).second ) ) == 0 )
      return O_FIFO_IS_LOCKED;

    if( (*oi).second.empty() ){
      BUG_MSG( "setStandardWindow" );
      out->setWindow( GuiFactory::Instance()->getStandardWindow() );
    }
    else if( s != 0 ){
      BUG_MSG( "setStreamDefinedWindow" );
      out->setWindow( s->getTextWindow() );
    }
    m_process->addChannel( out );
  }
  return O_NO_ERROR;
}

/*------------------------------------------------------------
 * stop:
 *   send all processes the kill signal
 */
bool BatchProcess::stop(){
  return m_process ? m_process->stop() : false;
}

/*------------------------------------------------------------
 */
bool BatchProcess::hangup(){
  return m_process ? m_process->hangup() : false;
}

/*------------------------------------------------------------
 */
void BatchProcess::putValues( StreamParameter &dpar, const std::vector<int> &dims
			      , const std::vector<double> &vals, bool isCell, const std::vector<int> &inds ){
  std::cerr << "BatchProcess::putValues( double ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void BatchProcess::putValues( StreamParameter &dpar, const std::vector<int> &dims
			      , const std::vector<dComplex> &vals, const std::vector<int> &inds ){
  std::cerr << "BatchProcess::putValues( complex ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void BatchProcess::putValues( StreamParameter &dpar, const std::vector<int> &dims
			      , const std::vector<std::string> &vals ){
  std::cerr << "BatchProcess::putValues( std::string ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void BatchProcess::putValues( DataStreamParameter &dpar
			      , const std::vector<DataStreamParameter *>&fieldList
			      , const std::vector<int> &dims ){
  std::cerr << "BatchProcess::putValues( fieldlist ) " << dpar.getName() << std::endl;
}

/*------------------------------------------------------------
 */
void BatchProcess::getValues( DataStreamParameter &dpar
			      , const std::vector<DataStreamParameter *> &fieldList ){
  std::cerr << "BatchProcess::getValues( ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void BatchProcess::getValues( DataStreamParameter &dpar ){
  std::cerr << "BatchProcess::getValues( ) " << dpar.getName() << std::endl;
}

/*------------------------------------------------------------
 */
void BatchProcess::setShell( const std::string &shcmd ){
  assert( m_process != 0 );
  m_process->setShell(shcmd);
  //  m_shcmd = shcmd;
}

/*------------------------------------------------------------
  start -
    Starts a batch job with its IO redirected in (at least) two pipes.
    One pipe (channel) must be filled by the function "ChSendFunc" the
    other ones have to be read by the caller (out_channels).
    Returns the childs pid on success, -1 otherwise.

     INTENS   1 >-----> 0     BATCH     1 >------> 0  INTENS
                  pin                       pout
                  fin                       fout
               in_channel                out_channel
  ------------------------------------------------------------
*/
bool BatchProcess::start() {
  return m_process ? m_process->start() : false;
}

bool BatchProcess::hasTerminated(){
  //  BUG( BugOperator, "BatchProcess::hasTerminated" );
  if( m_process->ChildTerminated() ){
    setExitStatus( m_process->getExitStatus() );
    if( getExitStatus() != 0 ){
      ProcessImplementation::InputChannelList& inchannels = m_process->getInputChannels();
      std::vector<InputChannel *>::iterator iiter;
      for( iiter=inchannels.begin(); iiter!=inchannels.end(); ++iiter ){
	(*iiter)->stop();
      }
    }
    getPendingInput();
    m_is_running = false;
    BUG_DEBUG( getExecCmd() << "\nPID= " << m_process->getPID() <<" ExitStatus = " << m_process->getExitStatus() );
    // remove signal handler
    m_process->removeSignal();
    return true;
  }
  return false;
}

bool BatchProcess::waitForFinished() {
  return m_process ? m_process->waitForFinished() : false;
}
void BatchProcess::getPendingInput(){
  bool hasOpenChannels = true;
  std::vector<OutputChannel *>::iterator oiter;
  BUG( BugOperator, "BatchProcess::getInput" );
  while( hasOpenChannels ){
    hasOpenChannels = false;
    ProcessImplementation::OutputChannelList& outchannels = m_process->getOutputChannels();
    for( oiter=outchannels.begin(); oiter!=outchannels.end(); ++oiter ){
#if defined HAVE_QT
      if ( m_process ) {
	// zuerst noch den Buffer lesen
	QByteArray& str =  static_cast<ProcessQt*>(m_process)->readBuffer();
	(*oiter)->getInput(str.data(), str.size(), NULL);
	str.clear();

	// nun den Rest noch lesen
	str =  static_cast<ProcessQt*>(m_process)->readStdout();
	if (str.size())
	  (*oiter)->getInput(str.data(), str.size(), NULL);
      }
#else
      if( (*oiter)->isOpen() ){
       	(*oiter)->getInput();
	hasOpenChannels = true;
      }
#endif
    }
  }
}
