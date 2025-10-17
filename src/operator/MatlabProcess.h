
#ifndef MATLAB_PROCESS
#define MATLAB_PROCESS

#include "gui/InformationListener.h"
#include "operator/OpProcess.h"
#include "utils/IntensThread.h"

#ifdef HAVE_MATLAB
typedef struct mxArray_tag mxArray; /* Incomplete definition for matlab Array */
#else
typedef int mxArray;
#endif

class MatlabProcess: public Process, private ThreadListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  MatlabProcess( const std::string &name );
  virtual ~MatlabProcess();

protected:
  MatlabProcess( const MatlabProcess &process );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual Process *clone();
  virtual bool start();
  virtual bool stop();
  virtual bool hasTerminated();

  /// opens an connection to Matlab
  static bool open(const std::string &cmd );
  /// closes an MatlabConnection;
  static void close();
  /// returns true if there is at least one MatlabProcess:
  static bool needMatlab(){ return s_configured > 0; }

  /// StreamDestination interface
  virtual void putValue( StreamParameter &dpar, const std::string &val );

  virtual void putValues( StreamParameter &dpar, const std::vector<int> &dims
			  , const std::vector<double> &vals, bool isCell, const std::vector<int> &inds );
  virtual void putValues( StreamParameter &dpar, const std::vector<int> &dims
			  , const std::vector<dComplex> &vals, const std::vector<int> &inds );
  virtual void putValues( StreamParameter &dpar, const std::vector<int> &dims
			  , const std::vector<std::string> &vals );
  virtual void putValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldList
			  , const std::vector<int> &dims );

  /// StreamSource Interface
  virtual void getValues( DataStreamParameter &dpar );
  virtual void getValues( DataStreamParameter &dpar
			  , const std::vector<DataStreamParameter *>&fieldList );

  /// write messages to stream
  void printMessages( std::ostream &istr ) const;
  std::string getMessages( ) const;
  /// return number of errors
  int  countErrors() const;
  /** returns error messages if there are any
   */
  virtual std::string getErrorMessage() const;

/*=============================================================================*/
/* private ThreadListener function                                             */
/*=============================================================================*/
 private:
  virtual void startThread();

 private:
  void execute();
  typedef std::vector< mxArray *> MxArrayVec;
  MxArrayVec          m_mxArrayVec;
  static char        *s_msgBuffer;
  static int          s_configured;
  bool                m_terminated;
  std::string         m_evalString;
  std::ostream       *m_logstr;
  IntensThread        m_thread;
};

#endif
