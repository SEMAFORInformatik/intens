
#include <QApplication>
#include <QSocketNotifier>

#include "operator/ProcessWrapper.h"
#include "operator/BatchProcess.h"
#include "utils/Debugger.h"
#include "utils/Date.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

#include "gui/GuiFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "gui/qt/QtDialogProgressBar.h"

INIT_LOGGER();

// --------------------------------------------------------------
//  ProcessImplementation -- Konstruktor Destruktor
// --------------------------------------------------------------
ProcessImplementation::ProcessImplementation() :  m_shcmd(getenv("SHELL") ? getenv("SHELL") : "") {
#ifndef _WIN32
  // makes intens for webtens happy
  // no SHEll envvar is set
  if (m_shcmd.empty()) {
    m_shcmd = "/bin/sh";
  }
#endif
}

ProcessImplementation::ProcessImplementation( const ProcessImplementation &process ) :  m_shcmd(process.m_shcmd) {
}

ProcessImplementation::~ProcessImplementation() {
  std::vector<OutputChannel *>::iterator outiter;
  for( outiter=m_outchannels.begin(); outiter!= m_outchannels.end(); ++outiter ){
    delete *outiter;
  }
  std::vector<InputChannel *>::iterator initer;
  for( initer=m_inchannels.begin(); initer!= m_inchannels.end(); ++initer ){
    delete *initer;
  }
}

//--------------------------------------------------------------
//  setShell
//--------------------------------------------------------------

void ProcessImplementation::setShell( const std::string &shcmd ){
  m_shcmd = shcmd;
}

//--------------------------------------------------------------
//  AddChannel
//--------------------------------------------------------------

void ProcessImplementation::addChannel( OutputChannel *oc ){
  m_outchannels.push_back( oc );
}
void ProcessImplementation::addChannel( InputChannel *ic ){
  m_inchannels.push_back( ic );
}


#if defined HAVE_QT
//--------------------------------------------------------------
// ProcessQt -- Konstruktor Destruktor
//--------------------------------------------------------------

ProcessQt::ProcessQt(BatchProcess *proc)
  : m_batchProcess(proc), m_exitCode(-1), m_started(false)  {
  m_qtProcess = new QProcess();

  // connect slots
  connect( m_qtProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()) );
  connect( m_qtProcess, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()) );
  connect( m_qtProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)) );
  connect( m_qtProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)) );
  connect( m_qtProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stateChanged(QProcess::ProcessState)) );
  if (AppData::Instance().HeadlessWebMode()) {
    connect( this, SIGNAL(writeToDatapool(const std::string, const std::string&, int)),
             &GuiQtManager::Instance(), SLOT(slot_writeToDatapool(const std::string, const std::string&, int)) );
  }

#if defined HAVE_QT && defined _WIN32
  // get env SHELL (if exists on windows)
  const char* cp;
  BUG_INFO("Shell (Windows) : " << getenv("SHELL"));
  if( (cp = getenv("SHELL"))!=NULL ){
    setShell( cp );
  } else {
    BUG_INFO("USE_SHELL (Windows) : " << getenv("USE_SHELL"));
    if (getenv("USE_SHELL")) {
      // makes mingw happy, because SHELL is not a environment
      setShell( "sh.exe" );
    } else {
      BUG_INFO("No Shell used (Windows)");
    }
  }
#endif
}

ProcessQt::ProcessQt(const ProcessQt& proc)
  : m_batchProcess(proc.m_batchProcess), m_exitCode(-1), m_started(false)  {
  if (m_qtProcess)
    m_qtProcess = new QProcess( m_qtProcess );

  // connect slots
  connect( m_qtProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()) );
  connect( m_qtProcess, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()) );
  connect( m_qtProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)) );
  connect( m_qtProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)) );
  connect( m_qtProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stateChanged(QProcess::ProcessState)) );
  if (AppData::Instance().HeadlessWebMode()) {
    connect( this, SIGNAL(writeToDatapool(const std::string, const std::string&, int)),
             &GuiQtManager::Instance(), SLOT(slot_writeToDatapool(const std::string, const std::string&, int)) );
  }
}

ProcessQt::~ProcessQt() {
  //  stop();
  if (m_qtProcess) {
    delete  m_qtProcess;
  }
}

//--------------------------------------------------------------
// start
//--------------------------------------------------------------

bool ProcessQt::start() {
  std::vector<OutputChannel *>::iterator oiter;
  std::vector<InputChannel *>::iterator iiter;
  QString process_cmd;
  QStringList argList;

  for( oiter=getOutputChannels().begin(); oiter!=getOutputChannels().end(); ++oiter ){
    (*oiter)->setUiUpdateInterval( m_batchProcess->getUiUpdateInterval() );
  }
  BUG(BugOperator,"BatchQt::start()" );
  if( getInputChannels().size()==0 ) { // .. and one input channel as well
    m_qtProcess->closeWriteChannel();
  }

  // set args to process and start child process
  if( getShell().size() ){ // use a shell
    process_cmd =  QString::fromStdString(getShell() /*+  " -c "*/);
    argList << "-c";
    argList << QString::fromStdString(m_batchProcess->getExecCmd());
  } else {
    std::vector<std::string> res = split(m_batchProcess->getExecCmd(), " ");
    process_cmd = QString::fromStdString(res[0]);
    for (int i=1; i< res.size(); ++i) {
      argList << QString::fromStdString(res[i]);
    }
  }

  // start
  m_exitCode = -1;
  m_started = false;
  bool bWin(false);
#ifdef _WIN32
  bWin = true;
#endif
  BUG_DEBUG("process_cmd:" << process_cmd.toStdString());
  BUG_DEBUG("    argList:" << argList.join("#").toStdString());
  if ( !bWin && m_batchProcess->isDaemon()) {
    m_qtProcess->startDetached(process_cmd, argList);
  } else
    m_qtProcess->start(process_cmd, argList);
  //  std::cout << "  Process started cmd["<<process_cmd.toStdString()<<"] args["<<argList.join("#").toStdString()<<"]\n"<<std::flush;
  // back to parent process
  if ( bWin || !m_batchProcess->isDaemon())
    m_batchProcess->m_is_running = true;
  m_batchProcess->setExitStatus(0);
  m_batchProcess->setErrorMessage("");

  for( iiter=getInputChannels().begin(); iiter!=getInputChannels().end(); ++iiter ){
    (*iiter)->write();
  }
  m_qtProcess->closeWriteChannel();
  return true;
}

//--------------------------------------------------------------
// stop
//--------------------------------------------------------------

bool ProcessQt::stop() {
  if (!m_qtProcess)
    return false;
  m_qtProcess->kill(); //terminate();
  m_started = false;
  return true;
}

//--------------------------------------------------------------
// hangup
//--------------------------------------------------------------

bool ProcessQt::hangup() {
  return false;
}

//--------------------------------------------------------------
// ChildTerminated
//--------------------------------------------------------------

bool  ProcessQt::ChildTerminated() {

  if (m_exitCode == -1)
    return  false;
  return m_qtProcess ? m_qtProcess->state()==QProcess::NotRunning: false;
}

//--------------------------------------------------------------
// waitForFinished
//--------------------------------------------------------------

bool  ProcessQt::waitForFinished() {
  return m_qtProcess ? m_qtProcess->waitForFinished() : false;
}

//--------------------------------------------------------------
// getExitStatus
//--------------------------------------------------------------

int  ProcessQt::getExitStatus()   {
  BUG_PARA( BugOperator, "ProcessQt::getExitStatus: ", m_batchProcess->getExitStatus());
  return m_batchProcess->getExitStatus();
}

//--------------------------------------------------------------
// getPID
//--------------------------------------------------------------

int   ProcessQt::getPID() {
  return m_qtProcess ? m_qtProcess->processId() : 0;
}

//--------------------------------------------------------------
// readFromStdout
//--------------------------------------------------------------
void ProcessQt::readFromStdout() {
  BUG(BugOperator,"ProcessQt::readFromStdout" );
  //  if (!m_qtProcess->canReadLine())
  //    GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + ": readFromStdout should NOT called [BytesAvailable["
  //						       + QString::number(m_qtProcess->bytesAvailable()).toStdString()+"]" );
  std::vector<OutputChannel *>::iterator oiter;
  for( oiter=getOutputChannels().begin(); oiter!=getOutputChannels().end(); ++oiter ){
    // es gibt nur einen OutputChannel.... ???
    //    QByteArray str; // = m_qtProcess->readAllStandardOutput();
    m_fromStdout += m_qtProcess->readAllStandardOutput();
    QByteArray str_tmp;
    while ( (str_tmp = m_qtProcess->readLine() ).size() ) {
      m_fromStdout += str_tmp;
      while ( !str_tmp.endsWith('\n')) { // noch keine ganze Linie
        str_tmp = m_qtProcess->readLine();
        if (str_tmp.size() == 0)
          break;
        m_fromStdout += str_tmp;
      }
      if ( m_fromStdout.endsWith('\n')) {   // rode a complete line
        // amg 2009-04-29 viel schneller am Ende alle Zeilen gleichzeitig an Intens(Streamer) zu uebergeben
        continue;
        (*oiter)->getInput( m_fromStdout.data(),  m_fromStdout.size(), NULL);
        m_fromStdout.clear();
      }
    }
    if (AppData::Instance().HeadlessWebMode()) {
      emit writeToDatapool("ProgressDialog.DataLabel[0]", "Std Window", -1);
      emit writeToDatapool("ProgressDialog.Data[0]", m_fromStdout.data(), 5);
    }

    // append uncomplete data to StreamBuffer
    // complete getInput call !!can!! have problems with uncomplete input lines (windows)
    int lpos = m_fromStdout.lastIndexOf('\n');
    if (lpos >= 0 && m_fromStdout.size()) {
      ++lpos;
      (*oiter)->getInput( m_fromStdout.data(),  lpos, NULL );
      m_fromStdout.remove(0, lpos);
      //std::cout << "   ??? Rest size["<< m_fromStdout.size()<<"] str["<<m_fromStdout.data()<<"]\n"<<std::flush;
      //      m_fromStdout.clear();
    }
  }
}

//--------------------------------------------------------------
// resdFromStderr
//--------------------------------------------------------------

void ProcessQt::readFromStderr() {
  BUG(BugOperator,"ProcessQt::readFromStderr" );
  QByteArray str;
  while ( (str = m_qtProcess->readAllStandardError()) != QString()) {
#ifdef _WIN32
    int p;
    while((p=str.indexOf('\r')) != -1) {
      str.remove(p,1);
    }
#endif
    BUG_MSG("read from StdErr["<<str.data()<<"]" );
    m_batchProcess->setErrorMessage( m_batchProcess->getErrorMessage() + str.data() );

    // if not main thread => only return (Qt don't like it)
    if (QApplication::activeWindow() && thread() != QApplication::activeWindow()->thread()) {
      return;
    }
    if ( strlen(str.data()) < 5  )
      GuiFactory::Instance()->getLogWindow()->writeText( str.data());
    else
      GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + ": " + str.data());
  }
  if (AppData::Instance().HeadlessWebMode()) {
    emit writeToDatapool("ProgressDialog.DataLabel[1]", "Log Window", -1);
    emit writeToDatapool("ProgressDialog.Data[1]", str.data(), 20);
  }
}


//--------------------------------------------------------------
// error
//--------------------------------------------------------------

void ProcessQt::error(QProcess::ProcessError error) {
  BUG(BugOperator,"ProcessQt::error");
  BUG_MSG("ProcessError: '"<<error<<"'" );
  m_batchProcess->setExitStatus(1);
  std::string msg;
  switch (error) {
    case QProcess::FailedToStart:
      msg = compose(_("Process failed to start: '%1'"),m_batchProcess->getExecCmd() );
      break;
    case QProcess::Crashed:
      if (m_started)
	msg = compose(_("Process exited unsuccessfully: '%1'"),m_batchProcess->getExecCmd() );
      break;
    case QProcess::Timedout:
      msg = compose(_("Process Timed out: '%1'"),m_batchProcess->getExecCmd() );

      break;
    case QProcess::WriteError:
      msg = compose(_("Process WriteError: '%1'"),m_batchProcess->getExecCmd() );

      break;
    case QProcess::ReadError:
      msg = compose(_("Process ReadError: '%1'"),m_batchProcess->getExecCmd() );
      break;
    default:
      msg = compose(_("Unknown Process Error: '%1'"),m_batchProcess->getExecCmd() );
  }
  if (msg.size()) {
    GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : " + msg);
    GuiFactory::Instance() -> showDialogWarning
      ( 0
	, _("Error")
	, msg
	, 0 );
  }
}
//--------------------------------------------------------------
// finished
//--------------------------------------------------------------

void ProcessQt::finished(int exitCode, QProcess::ExitStatus exitStatus) {
  BUG_PARA( BugOperator, "BatchProcess::finished exitCode: ", exitCode << " exitStatus: " << exitStatus << "  cmd: " << m_batchProcess->getExecCmd() );

  m_exitCode = exitCode != 0 ? 1 : exitStatus; // QProcess::CrashExit => 1, QProcess::NormalExit => 0
  m_batchProcess->setExitStatus( exitCode != 0 ? 1 : exitStatus/*Code*/ );
  if (m_fromStdout.size()) {
    std::vector<OutputChannel *>::iterator oiter;
    for( oiter=getOutputChannels().begin(); oiter!=getOutputChannels().end(); ++oiter )
    (*oiter)->getInput( m_fromStdout.data(),  m_fromStdout.size(), NULL );
    m_fromStdout.remove(0, m_fromStdout.size());
  }
}
//--------------------------------------------------------------
// stateChanged
//--------------------------------------------------------------

void ProcessQt::stateChanged( QProcess::ProcessState newState ) {
  BUG_PARA( BugOperator, "BatchProcess::stateChanged newState: ", newState << "  cmd: " << m_batchProcess->getExecCmd() );
  if (newState == QProcess::Running) {
    m_started = true;
  }
}

//--------------------------------------------------------------
// started
//--------------------------------------------------------------

void ProcessQt::started() {
  std::string msg = compose(_("Process '%1' started."),m_batchProcess->getExecCmd() );
  GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : " + msg);
}
#endif

#if !defined HAVE_QT
//--------------------------------------------------------------
// ProcessPosix - Constructor / Destructor
//--------------------------------------------------------------

ProcessPosix::ProcessPosix(BatchProcess *proc) : m_batchProcess(proc), m_pid(0) {
  m_signal.set( SIGCHLD );
  m_signal.set( SIGPIPE );
}

ProcessPosix::ProcessPosix(const ProcessPosix& proc)
  : ProcessImplementation(proc), m_batchProcess(proc.m_batchProcess), m_pid(proc.m_pid) {
  m_signal.set( SIGCHLD );
  m_signal.set( SIGPIPE );
}

//--------------------------------------------------------------
// start
//--------------------------------------------------------------

bool ProcessPosix::start() {
  pid_t            pid;
  std::vector<OutputChannel *>::iterator oiter;
  std::vector<InputChannel *>::iterator iiter;

  BUG(BugOperator,"BatchProcess::start()" );
  if( getOutputChannels().size()==0 ) // we need at least one output channel
    getOutputChannels().push_back( new OutputChannel(0) );
  for( oiter=getOutputChannels().begin(); oiter!=getOutputChannels().end(); ++oiter ){
    (*oiter)->setUiUpdateInterval( m_batchProcess->getUiUpdateInterval() );
    (*oiter)->createPipe();
  }

  if( getInputChannels().size()==0 ) // .. and one input channel as well
    getInputChannels().push_back( new InputChannel(0,"", NULL) );
  for( iiter=getInputChannels().begin(); iiter!=getInputChannels().end(); ++iiter ){
    (*iiter)->createPipe();
  }
  m_err_channel.createPipe();

  // install signal handlers
  m_signal.install();
  switch ( pid = fork() ){
    //#endif
  case -1:
    return false;
    //      break;

  case 0: /* this is the child */
    for( oiter=getOutputChannels().begin(); oiter!=getOutputChannels().end(); ++oiter ){
      (*oiter)->dup();
    }
    for( iiter=getInputChannels().begin(); iiter!=getInputChannels().end(); ++iiter ){
      (*iiter)->dup();
    }
    m_err_channel.dup();

    setpgid( 0, 0 );

    if( getShell().empty() ){ // we do not use a shell
      //maybe we should consider rsh also? || getExecCmd().find("rsh ")==0 )
      char **argv;
      std::istringstream is(m_batchProcess->getExecCmd());
      std::vector< std::string > sargs;
      std::string sbuf;
      while( is >> sbuf ){
	sargs.push_back( sbuf );
      }
      argv = new char * [ sargs.size() + 1 ];
      for( int i=0; i<sargs.size() ; ++i ){
	argv[i]=new char [ sargs[i].size() + 1 ];
	strcpy( argv[i], sargs[i].c_str() );
      }
      argv[sargs.size()] = 0;
      execvp( argv[0], argv );
    }
    else { // we use a shell
#if defined( __CYGWIN__)
      if (std::string(getenv("SHELL")).empty())
        execl( "/bin/sh", "sh", "-c", m_batchProcess->getExecCmd().c_str(), (char *)0 );
      else
        execl( getenv("SHELL"), "sh.exe", "-c", m_batchProcess->getExecCmd().c_str(), (char *)0 );
#else
      execl( "/bin/sh", "sh", "-c", m_batchProcess->getExecCmd().c_str(), (char *)0 );
#endif
    }
    perror( m_batchProcess->getExecCmd().c_str() );
    exit( -1 ); /* should never return */
  }
  m_batchProcess->m_is_running = true;
  m_batchProcess->setExitStatus(0);

  for( iiter=getInputChannels().begin(); iiter!=getInputChannels().end(); ++iiter ){
    (*iiter)->write();
  }

  for( oiter=getOutputChannels().begin(); oiter!=getOutputChannels().end(); ++oiter ){
    (*oiter)->addInput();
  }
  m_err_channel.addInput();
  BUG_MSG( " Started " <<  m_batchProcess->getExecCmd() << " PID=" << pid );
  m_pid=pid;
  return true;
}

//--------------------------------------------------------------
// stop
//--------------------------------------------------------------

bool ProcessPosix::stop() {
  return m_pid ? kill( -m_pid, SIGTERM ) == 0 : false;
}

//--------------------------------------------------------------
// hangup
//--------------------------------------------------------------

bool ProcessPosix::hangup() {
  return (m_pid == 0) ? false : (kill( m_pid, SIGHUP ) == 0);
}

//--------------------------------------------------------------
// ChildTerminated
//--------------------------------------------------------------

bool  ProcessPosix::ChildTerminated() {
  return m_signal.ChildTerminated( m_pid );
}

//--------------------------------------------------------------
// getExitStatus
//--------------------------------------------------------------

int ProcessPosix::getExitStatus()   {
  return m_signal.getExitStatus( m_pid );
}

//--------------------------------------------------------------
// removeSignal
//--------------------------------------------------------------

void ProcessPosix::removeSignal() {
  m_signal.remove();
}

#endif

#ifdef HAVE_QT

//--------------------------------------------------------------
// CkannelNotifier
//--------------------------------------------------------------

//--------------------------------------------------------------
// dataOutputReceived
//--------------------------------------------------------------

void ChannelNotifier::dataOutputReceived(int fd) {
  assert(m_oc);
  m_oc->getInput(&fd, NULL);
}

//--------------------------------------------------------------
// dataErrorReceived
//--------------------------------------------------------------

void ChannelNotifier::dataErrorReceived(int fd) {
  char                      buf[100+1];
  int                       nbytes;
  std::string               sbuf;
  while ( (nbytes = ::read( fd, buf, 100)) > 0 ) {
    sbuf += buf;
  }
  if ( sbuf.size() ) std::cout << "  ++>  ERROR [" << sbuf << "]\n";

  if( sbuf.size() ){
    GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + " : " + sbuf );
  }
  deleteNotifier();
}

//--------------------------------------------------------------
// deleteNotifier
//--------------------------------------------------------------

void ChannelNotifier::deleteNotifier() {
  delete m_notifier;
}

#endif
