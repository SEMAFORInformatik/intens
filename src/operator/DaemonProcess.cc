
#include "utils/Debugger.h"

#include "operator/DaemonProcess.h"
#include "streamer/StreamManager.h"
#include "streamer/Stream.h"
#include "streamer/DataStreamParameter.h"

// only needed to prevent undeclared compile errors with QT
#ifdef HAVE_QT
typedef short Position;
#endif
// FIX IT!

#include "gui/GuiManager.h"

DaemonProcess::DaemonProcess( const std::string &name )
  : Process(name), m_shcmd("/bin/sh") {
#if !defined  HAVE_QT
  m_signal.set( SIGCHLD );
  m_signal.set( SIGPIPE );
#endif
}

DaemonProcess::DaemonProcess( const DaemonProcess &process )
  : Process( process ), m_shcmd(process.m_shcmd) {

#if !defined  HAVE_QT
  m_signal.set( SIGHUP );
  //  m_signal.set( SIG_IGN );
#endif
}

DaemonProcess::~DaemonProcess(){
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

Process *DaemonProcess::clone(){
  return new DaemonProcess( *this );
}

/*------------------------------------------------------------
 * stop:
 *   send all processes the kill signal
 */
bool DaemonProcess::stop(){
#if defined HAVE_QT
  assert(false);
  return false;
#else
  return kill( -m_pid, SIGTERM ) == 0;
#endif
}

/*------------------------------------------------------------
 */
bool DaemonProcess::hangup(){
#if defined HAVE_QT
  assert(false);
  return false;
#else
  if( m_pid == 0 ){
    return false;
  }
  return kill( m_pid, SIGHUP ) == 0;
#endif
}

/*------------------------------------------------------------
 */
void DaemonProcess::setShell( const std::string &shcmd ){
  BUG(BugOperator,"DaemonProcess::setShell()" );
  m_shcmd = shcmd;
}

/*------------------------------------------------------------
  start -
    Starts a daemon job.
    Returns the childs pid on success, -1 otherwise.
*/
bool DaemonProcess::start()
{
  return true;
}

bool DaemonProcess::hasTerminated( ){
  BUG( BugOperator, "DaemonProcess::hasTerminated" );
  assert(false);
  return false;

}

/*------------------------------------------------------------
 */
void DaemonProcess::putValues( StreamParameter &dpar, const std::vector<int> &dims
			      , const std::vector<double> &vals, bool isCell, const std::vector<int> &inds ){
  std::cerr << "DaemonProcess::putValues( double ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void DaemonProcess::putValues( StreamParameter &dpar, const std::vector<int> &dims
			      , const std::vector<dComplex> &vals, const std::vector<int> &inds ){
  std::cerr << "DaemonProcess::putValues( complex ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void DaemonProcess::putValues( StreamParameter &dpar, const std::vector<int> &dims
			      , const std::vector<std::string> &vals ){
  std::cerr << "DaemonProcess::putValues( std::string ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void DaemonProcess::putValues( DataStreamParameter &dpar
			      , const std::vector<DataStreamParameter *>&fieldList
			      , const std::vector<int> &dims ){
  std::cerr << "DaemonProcess::putValues( fieldlist ) " << dpar.getName() << std::endl;
}

/*------------------------------------------------------------
 */
void DaemonProcess::getValues( DataStreamParameter &dpar
			      , const std::vector<DataStreamParameter *> &fieldList ){
  std::cerr << "DaemonProcess::getValues( ) " << dpar.getName() << std::endl;
}
/*------------------------------------------------------------
 */
void DaemonProcess::getValues( DataStreamParameter &dpar ){
  std::cerr << "DaemonProcess::getValues( ) " << dpar.getName() << std::endl;
}
