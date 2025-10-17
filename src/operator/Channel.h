#ifndef CHANNEL_H
#define CHANNEL_H

#ifdef __CYGWIN__
#include <sys/signal.h>
#endif

#if defined HAVE_QT && defined WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <fstream>
#include <vector>
#include <sstream>

//#include <fcntl.h>
#include <sys/types.h>
#include <assert.h>

#include "operator/ChannelListener.h"
#include "operator/InputChannelEvent.h"
#include "operator/OutputChannelEvent.h"
//#include "utils/IntensThread.h"

// dummy typedef to prevent compile errors
typedef void*           XtPointer;
typedef unsigned long	XtInputId;

class GuiScrolledText;
class ChannelNotifier;
class QProcess;

#ifndef __MINGW32__
/** Fifo class for temporary named pipes
 */
class Fifo
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  Fifo( const std::string &fifo );
  virtual ~Fifo() { if( !m_fifo.empty() ) unlink( m_fifo.c_str() ); }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  bool operator()()const{ return !m_fifo.empty(); }
  int open(int oflag);
  const std::string &getName() const{ return m_fifo; }

  /** create a unique Fifo-Name
      @param name base name component will create something like /tmp/nnnn<name>
  */
  static std::string *createName( const std::string &name );
/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  const std::string m_fifo;
};
#endif



class OutputChannel  {
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  OutputChannel( ChannelListener *s, const std::string &fifo="", QProcess* qproc=NULL  );
  virtual ~OutputChannel();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setUiUpdateInterval( int interval );
  void setListener( ChannelListener *s ) { m_listener = s; }
  bool setWindow( GuiScrolledText *t );
#if defined HAVE_QT
  void getInput(const char* buf, int nbytes, void* id);
#else
  void getInput(){ if( m_fd[0]>0 ) getInput( &m_fd[0], &m_xtid ); }
  int addInput();
  bool isOpen();
  bool dup();
  bool createPipe();
  // private:
  void getInput(const char* buf, int nbytes, void* id);
 public:
#endif
  void close();
  bool open( const std::string &filename );
  bool clearTextWindow();
  void setFileFormat( int ){ std::cerr << "WARNING: FileFormat ignored!"<<std::endl; }
  std::string &getStreamBuffer(){ return m_buffer; }

  void setOstream(std::ostream& os) {
    if( m_ostr != 0  && !m_ostrNotClose)
      delete m_ostr;
    m_ostrNotClose = true;
    m_ostr = &os;
  }
  std::ostream& getOstream() { assert( m_ostr != 0 ); return *m_ostr; }
  void getInput( int *fid, XtInputId *id ); // since MSVC this must be public

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  static void getInput( XtPointer po, int *fid, XtInputId *id );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  int                 m_uiUpdateInterval;
  int                 m_getInputCounter;
  ChannelListener    *m_listener;
  GuiScrolledText    *m_window;
  std::string         m_buffer;
  std::ostream       *m_ostr;
  bool                m_ostrNotClose;
  XtInputId           m_xtid;
  OutputChannelEvent  m_event;
#if defined HAVE_QT
  QProcess           *m_qtProcess;
  ChannelNotifier *m_notifier;
#else
  Fifo                m_fifo;
  int                 m_fd[2];
#endif
};

class InputChannel //: private ThreadListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  InputChannel( ChannelListener *s, const std::string &fifo, QProcess* qpro );
  virtual ~InputChannel();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setListener( ChannelListener *s ) { m_listener = s; }
  bool write();
  void close();
  bool stop();

  bool open( const std::string &filename );

  std::istream& getIstream() { assert( m_istr != 0 ); return *m_istr; }
#if defined HAVE_QT
  void setQProcess(QProcess* proc) { m_qtProcess = proc; }
  bool write(std::ostringstream& os);
#else
 public:
  bool dup();
  bool createPipe();
  FILE* getFilePointer() { return fdopen(m_fd[1], "w"); }
  std::ostream& getOstream() { assert( m_ostr != 0 ); return *m_ostr; }
#endif

/*=============================================================================*/
/* private ThreadListener function                                             */
/*=============================================================================*/
 private:
  virtual void startThread();

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
 ChannelListener      *m_listener;
#if defined HAVE_QT
  QProcess            *m_qtProcess;
#else
  Fifo                 m_fifo;
  int                  m_fd[2];
  std::ostream        *m_ostr;
#endif
  //  IntensThread         m_send_thread;
  std::istream        *m_istr;
  InputChannelEvent    m_event;
};

#if !defined HAVE_QT
class ErrChannel
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ErrChannel();
  virtual ~ErrChannel();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  bool dup();
  bool createPipe( );
  void addInput();
  void close();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  static void getInput( XtPointer po, int *fid, XtInputId *id );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  int             m_fd[2];
  XtInputId       m_xtid;
#if defined HAVE_QT
  ChannelNotifier *m_notifier;
#endif
};
#endif

std::string       *CHgetFifoTitle( const std::string & );

#endif
