
#ifndef MFM_SERVER_SOCKET
#define MFM_SERVER_SOCKET

#include <QObject>
#include <QTimerEvent>

#include "operator/IntensServerSocket.h"

class IntensSocketListener;
class JobFunction;

class MFMServerSocket
  : public IntensServerSocket
#ifdef HAVE_QT
  , public QObject
#endif
{
// -----------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------
public:
  MFMServerSocket( const std::string &s
                   , int i
                   , IntensSocketListener *l
                   , JobFunction *f
                   , JobFunction *on_eos
                   , XferDataItem *xfer
                   , XferDataItem *thumb_xfer
                   , int thumb_width
                   , int thumb_height )
    : IntensServerSocket( s,i,l,f )
    , m_xfer(xfer)
    , m_on_eos(on_eos)
    , m_thumb_xfer(thumb_xfer)
    , m_thumb_width(thumb_width)
    , m_thumb_height(thumb_height)
    , m_remaining_EOS(0)
    , m_timeout_EOS(0.0)
    , m_timerId_EOS( -1 ){
  }
  virtual ~MFMServerSocket();

  virtual bool newLine( std::string & );
  virtual XferDataItem* getXfer(){ return m_xfer; }
  virtual XferDataItem* getThumbXfer(){ return m_thumb_xfer; }
  virtual int getThumbWidth(){ return m_thumb_width; }
  virtual int getThumbHeight(){ return m_thumb_height; }
private:
  #ifdef HAVE_QT
  void timerEvent( QTimerEvent * ); // TODO
  #endif

  XferDataItem *m_xfer;
  JobFunction  *m_on_eos;
  XferDataItem *m_thumb_xfer;
  int m_thumb_width;
  int m_thumb_height;
  int m_remaining_EOS;
  double m_timeout_EOS;
  int m_timerId_EOS;
};

#endif
