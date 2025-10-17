
#if !defined(OPERATOR_PROCESS_H)
#define OPERATOR_PROCESS_H

#include <string>
#include <vector>
#include <map>

#include "streamer/Stream.h"
#include "job/JobAction.h"

class GuiButton;
class GuiPulldownMenu;

/** Process
    The base class Process is an abstract class for managing external processes.
    A process has a number of
    input and output streams, each containing a number of parameters that
    are beeing transfered to and from the external process.
    @version $Id: Process.h,v 1.28 2005/06/22 14:33:10 ked Exp $
*/
class Process : public StreamDestination
              , public StreamSource
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  Process( const std::string& name );
  virtual ~Process();

protected:
  Process( const Process &process );

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum error_type {O_NO_ERROR, O_STREAM_UNDECLARED, O_FIFO_IS_LOCKED};

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool isAborted() { return false; }
  bool isInstalled() { return m_installed; }
  void setInstalled() { m_installed = true; }

  virtual Process *clone() = 0;

  static void registerBasicStream( BasicStream *, const std::string &fifo );
  virtual void setOutputBasicStreams();

  /** adds an new input stream to the process
     @param formatname identifies a previously declared stream
     @param fifo name of fifo file if not empty (ignored by non batch type processes)
  */
  virtual error_type addInputStream( const std::string &formatname
				     , const std::string &fifo="" );
  /** adds an new input stream to the process
     @param stream pointer to a stream object
     @param fifo name of fifo file if not empty (ignored by non batch type processes)
  */
  virtual error_type addInputStream( Stream *s , const std::string &fifo="" );

  /** adds a set of output streams to the process
     @param ostr vector of stream and fifo name pairs
  */
  virtual error_type setOutputStreams( const std::vector< std::pair<std::string
				       ,std::string> >&ostr );
  /** adds a single output streams to the process
     @param ostr stream
  */
  virtual error_type addOutputStream( Stream *s );
  /** sets the execution command
     @param exec_cmd name of external commend
  */
  bool setExecCmd( const std::string &exec_cmd ){ m_exec_cmd=exec_cmd; return true; }
  /** returns the execution command string
   */
  const std::string&getExecCmd( )const{ return m_exec_cmd; }
  /** outputs the names of all input parameters separated commas
    @param os destination ostream object
  */
  int getInputParameters( std::ostream &os )const;
  /** outputs the names of all output parameters separated by commas
    @param os destination ostream object
  */
  int getOutputParameters( std::ostream &os )const;
  /** initiates a parameter output transfer action
      @param transposed matrix values are transposed if true
  */
  void putStreamValues( bool transposed );
  /** initiates a parameter input transfer action
  */
  void getStreamValues();
  /** sets the uiUpdateInterval
   */
  virtual void setUiUpdateInterval( int interval ){ m_uiUpdateInterval = interval; }
  /** gets the uiUpdateInterval
   */
  int getUiUpdateInterval(){ return m_uiUpdateInterval; }
  /** starts the execution (pure cirtual function)
   * @returns true if process has been successfully started
   */
  virtual bool start()=0;
  /** stops the execution (pure cirtual function)
   */
  virtual bool stop()=0;
  /** sends a hangup signal to the external process
   */
  virtual bool hangup() { return false; }
  /** returns true if the external process is still running
   */
  virtual bool isRunning()const{ return m_is_running; }
  /** returns true if the external process has terminated
   */
  virtual bool hasTerminated()=0;
  /** returns the exit status of the external process
   */
  virtual void setExitStatus(int state) {
    m_ExitStatus = state;
  }

  virtual int  getExitStatus()const{
    return m_ExitStatus; }
  /** clears all text windows
   */
  virtual void clearTextWindows();
  /** write all input streams
      (only used by pseudo process)
   */
  virtual void writeInputStreams();
  /** clears all output streams
      (only used by pseudo process)
   */
  virtual void clearOutputStreams();
  /** checks the input parameters on validity if requested
   */
  virtual bool checkInput();
  /** returns the process by name or 0 if not found
  @param name of process
  */
  static Process *find( const std::string &name );
  /** Diese Funktion liefert den Pointer auf den gewünschten Process, oder,
      falls dieser bereits in einer Processgroup installiert ist, einen
      Clone des Process. Falls kein Process mit dem Namen name existiert
      wird 0 zurückgegeben.
      @param pointer of process
  */
  static Process* getUninstalled( const std::string &name );
  /** sets the targets
      @param autoClear !!! if TRUE !!! clear target stream already if BasicStream::checkTargets are called
   */
  void setTargets(bool autoClear=false);
  /** returns the total number of input parameters
   */
  size_t getNumInputParameters() const;
  /** returns error messages if there are any
   */
  virtual std::string getErrorMessage() const { return m_messages; }
  virtual void setErrorMessage(const std::string& msg) { m_messages = msg; }
   //@}
  virtual void workFailed();
  virtual bool isDaemon() { return false; }
 private:
  static void add( const std::string &name, Process *p );

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  typedef std::vector< std::pair<BasicStream *, std::string> > BasicStreamList;

  static BasicStreamList  s_basicstreams;

  std::string  m_name;
  bool         m_is_running;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int          m_ExitStatus;
  std::string  m_messages;

  int                 m_uiUpdateInterval;
  typedef std::map<std::string, Process *> ProcessMap;

  static ProcessMap       s_procmap;

  typedef std::vector< Stream * >      InputStreamList;
  typedef std::vector< BasicStream * > OutputStreamList;

  InputStreamList     m_instreams;
  OutputStreamList    m_outstreams;

  std::string         m_exec_cmd;
  bool                m_installed;
};

#endif
