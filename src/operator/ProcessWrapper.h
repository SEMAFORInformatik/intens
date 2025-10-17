
#ifndef PROCESS_WRAPPER_H
#define PROCESS_WRAPPER_H

#include <QProcess>
#include "operator/Channel.h"

class BatchProcess;

//--------------------------------------------------------------
// ProcessImplementation
//--------------------------------------------------------------

class ProcessImplementation {
  // ------------------------
  // Constructor / Destructor
  // ------------------------
public:
  ProcessImplementation();
  ProcessImplementation( const ProcessImplementation &process );
  virtual ~ProcessImplementation();
  // ------------------------
  // Public member functions
  // ------------------------
public:
  /** set the shell that runs the command
      @param shcmd full pathname of a shell that runs the command through -c or empty string of none
  */
  void setShell( const std::string &shcmd );
  /** get the shell that runs the command
   */
  const std::string& getShell() { return m_shcmd; }

  virtual bool start() = 0;
  virtual bool hangup() = 0;

  virtual bool stop() = 0;
  virtual bool ChildTerminated() = 0;
  virtual bool waitForFinished() = 0;
  virtual int getExitStatus() = 0;
  virtual int getPID() = 0;

  void addChannel( OutputChannel *oc );
  void addChannel( InputChannel *oc );

  virtual QProcess* getQProcess() = 0;

  typedef std::vector<OutputChannel *> OutputChannelList;
  typedef std::vector<InputChannel *>  InputChannelList;
  OutputChannelList& getOutputChannels() { return m_outchannels; }
  InputChannelList&  getInputChannels() { return m_inchannels; }

  // signal staff
  /*   virtual void setSignal(int sig) {} */
  virtual void removeSignal() {}
  // ------------------
  // private members
  // ------------------
private:
  std::string          m_shcmd;
  OutputChannelList    m_outchannels;
  InputChannelList     m_inchannels;
};

//--------------------------------------------------------------
// ProcessQt
//--------------------------------------------------------------

#ifdef HAVE_QT
class ProcessQt : public QObject, public ProcessImplementation {
  Q_OBJECT
  // ------------------------
  // Constructor / Destructor
  // ------------------------
 public:
  ProcessQt(BatchProcess *proc);
  ProcessQt( const ProcessQt &process );
  virtual ~ProcessQt();
  // ------------------------
  // public member functions
  // ------------------------
public:
  virtual bool start();
  virtual bool stop();
  virtual bool hangup();
  virtual bool ChildTerminated();
  virtual bool waitForFinished();
  virtual int getExitStatus();
  virtual int getPID();
  QByteArray readStdout() { assert(m_qtProcess); return m_qtProcess->readAllStandardOutput(); }
  virtual QProcess* getQProcess() { return m_qtProcess; }
  QByteArray&  readBuffer() { return m_fromStdout; }
  /* void readBufferClear() { return m_fromStdout.clear(); } */

  // ------------------------
  // signals
  // ------------------------
signals:
  void writeToDatapool(const std::string varnameData, const std::string& data, int maxlen);

  // ------------------------
  // public slots
  // ------------------------
 public slots:
   void  readFromStdout();
   void  readFromStderr();

   void  started();
   void  error(QProcess::ProcessError error);
   void  finished(int exitCode, QProcess::ExitStatus exitStatus);
   void  stateChanged(QProcess::ProcessState newState);
  // ------------------------
  // private members
  // ------------------------
 private:
  BatchProcess *m_batchProcess;
  QProcess     *m_qtProcess;
  int           m_exitCode;
  bool          m_started;
  QByteArray    m_fromStdout;
};
#endif

//--------------------------------------------------------------
// ProcessPosix
//--------------------------------------------------------------

#ifndef HAVE_QT
class ProcessPosix : public ProcessImplementation {
  // ------------------------
  // Constructor / Destructor
  // ------------------------
public:
  ProcessPosix(BatchProcess *proc);
  ProcessPosix( const ProcessPosix &process );
  // ------------------------
  // public member functions
  // ------------------------
public:
  bool start();
  bool stop();
  virtual bool hangup();
  bool ChildTerminated();
  virtual bool waitForFinished() { assert(false);};
  int getExitStatus();
  int getPID()  { return m_pid; }
  virtual QProcess* getQProcess() { return NULL; }

/*   virtual void setSignal(int sig); */
  virtual void removeSignal();
  // ------------------------
  // private members
  // ------------------------
private:
  BatchProcess      *m_batchProcess;
  pid_t              m_pid;
  Signal             m_signal;
  ErrChannel         m_err_channel;
};
#endif

#ifdef HAVE_QT
class OutputChannel;
class QSocketNotifier;

//--------------------------------------------------------------
// ChannelNotifier
//--------------------------------------------------------------

class ChannelNotifier : public QObject {
  Q_OBJECT
  // -------------------------
  // Constructor / Destructor
  // -------------------------
 public:
  ChannelNotifier(OutputChannel* o) : m_oc(o), m_notifier(0) {}
  ~ChannelNotifier(){}
  // -------------------------
  // public member functions
  // -------------------------
public:
  void deleteNotifier();
  void setNotifier(QSocketNotifier* n) { m_notifier = n; }
  // -------------------------
  // public slots
  // -------------------------
public slots:
  void dataOutputReceived(int fd);
  void dataErrorReceived(int fd);
  // -------------------------
  // private members
  // -------------------------
private:
  OutputChannel   *m_oc;
  QSocketNotifier *m_notifier;
};
#endif

#endif
