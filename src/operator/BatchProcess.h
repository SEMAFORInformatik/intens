
#ifndef BATCH_PROCESS_H
#define BATCH_PROCESS_H

#include "operator/OpProcess.h"
class ProcessImplementation;
class QProcess;

/** BatchProcess
    The class BatchProcess manages Unix batch processes. A batch process is executed
    from a shell (/bin/sh) and is expected to read and write on file descriptors.
    @version $Id: BatchProcess.h,v 1.11 2006/03/08 15:00:58 amg Exp $
*/
class BatchProcess: public Process
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  BatchProcess( const std::string& name="", bool daemon=false );
  virtual ~BatchProcess();

protected:
  BatchProcess( const BatchProcess &process );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual Process *clone();

  virtual void setOutputBasicStreams();

  /** adds an new input stream to the batch process
     @param formatname identifies a previously declared stream
     @param fifo name of fifo file if not empty
  */
  virtual error_type addInputStream( const std::string &formatname, const std::string &fifo );
  /** adds an new input stream to the batch process
     @param stream pointer to a stream
     @param fifo name of fifo file if not empty
  */
  virtual error_type addInputStream( Stream *s, const std::string &fifo );

  /** adds an new output stream to the process
     @param ostr vector of format and fifo name pairs
  */
  virtual error_type setOutputStreams( const std::vector< std::pair<std::string,std::string> >&ostr );

  /** set the shell that runs the command
     @param shcmd full pathname of a shell that runs the command through -c or empty string of none
  */
  void setShell( const std::string &shcmd );
  /**
     Starts a batch process with its IO redirected in (at least) two pipes.
     One pipe (channel) must be filled by the InputChannel-Listeners the
     other ones have to be read by the OutputChannel-Listeners
     @returns true if the process has been successfully started, false otherwise.
  */
  virtual bool start();
  /** sends a SIGTERM signal to every process in the currently running process group
   */ 
  virtual bool stop();
  /** sends a SIGHUP signal to every process in the currently running process group
   */
  virtual bool hangup();
  /** returns true if the process has terminated
   */
  virtual bool hasTerminated();
  virtual bool waitForFinished();
  /** adds an InputChannel to the process
   @param inch new InputChannel object
  */
  void addChannel( InputChannel *inch );
  /** adds an OutputChannel to the process
   @param inch new OutputChannel object
  */
  void addChannel( OutputChannel *outch );
  /** reads all pending input
   */
  void getPendingInput();
  /** sets the targets
   */
  void setTargets();
  /** put matrix double values to external process
      @param dpar StreamParameter
      @param dims vector of dimension sizes
      @param vals vector representing the matrix
      @param isCell true if value is cell (incomplete array)
      @param inds  vector containing the indicator values (1 valid, -1 invalid)
  */
  virtual void putValues( StreamParameter &dpar, const std::vector<int> &dims
			  , const std::vector<double> &vals, bool isCell, const std::vector<int> &inds );
  /** put matrix complex values to external process
      @param dpar StreamParameter
      @param dims vector of dimension sizes
      @param vals vector representing the matrix
      @param inds  vector containing the indicator values (1 valid, -1 invalid)
  */
  virtual void putValues( StreamParameter &dpar, const std::vector<int> &dims
			  , const std::vector<dComplex> &vals, const std::vector<int> &inds );
  /** put matrix string values to external process
      @param dpar StreamParameter
      @param dims vector of dimension sizes
      @param vals vector representing the matrix
  */
  virtual void putValues( StreamParameter &dpar, const std::vector<int> &dims
			  , const std::vector<std::string> &vals );
  /** put matrix structure values to external process
      @param dpar StreamParameter
      @param fieldList vector representing the structure fields
      @param dims vector of dimension sizes
  */
  virtual void putValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldList
			  , const std::vector<int> &dims );
  /** get values from external process
      @param dpar DataStreamParameter
  */
  virtual void getValues( DataStreamParameter &dpar );
  /** get structure values from external process
      @param dpar DataStreamParameter
      @param fieldList vector specifiying the structure field names
  */
  virtual void getValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldList );
  virtual bool isDaemon() { return m_bDaemon; }

QProcess* getQProcess();
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  ProcessImplementation   *m_process;
  bool                     m_bDaemon;

  // unschoen, aber wollte Processimplementation aus dieser Klasse nehmen
  friend class ProcessPosix;  
  friend class ProcessQt;
};

#endif
