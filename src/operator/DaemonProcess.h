
#ifndef DAEMON_PROCESS_H
#define DAEMON_PROCESS_H

#include "operator/OpProcess.h"
#include "operator/Channel.h"

/** DaemonProcess
    The class DaemonProcess manages Unix daemon processes. A daemon process is executed
    from a shell (/bin/sh) and is expected to read and write on file descriptors.
    @version $Id: DaemonProcess.h,v 1.5 2003/12/31 12:38:20 amg Exp $
*/
class DaemonProcess: public Process
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  DaemonProcess( const std::string& name="" );
  virtual ~DaemonProcess();

protected:
  DaemonProcess( const DaemonProcess &process );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual Process *clone();

  /** set the shell that runs the command
     @param shcmd full pathname of a shell that runs the command through -c or empty string of none
  */
  void setShell( const std::string &shcmd );
  /**
     Starts a daemon process with its IO redirected in (at least) two pipes.
     One pipe (channel) must be filled by the InputChannel-Listeners the
     other ones have to be read by the OutputChannel-Listeners
     @eturns true on success, false otherwise
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

 protected:
  // wird nicht in die map aufgenommen, da es keine op_process_statements
  // geben darf !
  virtual void add( const std::string &name, Process *p ){ std::cout << "Daemon" << std::endl;}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
#if !defined HAVE_QT
  Signal               m_signal;
  ErrChannel           m_err_channel;
  pid_t                m_pid;
#endif
  std::string          m_shcmd;
};

#endif
