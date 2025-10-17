
#include "gui/GuiImage.h"
#include "operator/MFMServerSocket.h"
#include "xfer/XferDataItem.h"

#include "gui/GuiScrolledText.h"
#include "gui/GuiFactory.h"
#include "utils/Date.h"
#include "utils/Debugger.h"

INIT_LOGGER();

MFMServerSocket::~MFMServerSocket(){
  delete m_xfer;
  #ifdef HAVE_QT
  if (m_timerId_EOS!= -1)
    killTimer(m_timerId_EOS);
  #endif
}

bool MFMServerSocket::newLine( std::string &line ){
  BUG_DEBUG("newLine: " << line);
  MFMImageMapper *mapper=0;
  if( getRequestListener() ){
    mapper=static_cast<MFMImageMapper*>(getRequestListener());
  }
  if( ( !mapper &&
        line.substr(0,3) != "EOL" // EOL braucht keinen mapper
        ) ||
      line.size() < 2
      ) {
    getServer()->endOfRead();
    return false;
  }

  if( line.substr(0,3) == "EOL" ){ // END OF LINE
    if( line.size() >= 10 && line.substr(4,6) == "NOFUNC" ){
      // END OF LINE, but don't call function (i.E. for Zurich Instrument data)
      getServer()->endOfRead();
      return true;
    }

    Trigger *t = 0;
    if( ( t = getTrigger() ) ){
      t->startJob();
      // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
    }
    else{
      getServer()->endOfRead();
    }
  }
  else if( line.substr(0,3) == "NEW" ){ // NEW SCAN, do not clear images
    if( line.substr(4,3) == "IMG" ){
      GuiImage::setImageScan();
    }
    else{
      GuiImage::setGeneralScan();
    }
    GuiImage::setEos( false );
    std::string logmsg;
    logmsg = DateAndTime() + " : NEW SCAN";
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg );

    std::istringstream is( line.substr(7) );
    int w,h;
    std::string s, scanningScheme = "sawtooth";
    is >> w;
    is >> h;
    if ( is >> s ) scanningScheme = s;
    is >> m_remaining_EOS;
    is >> m_timeout_EOS;
    BUG_DEBUG("newLine: NEW -> m_remaining_EOS " << m_remaining_EOS);
    BUG_DEBUG("newLine: NEW -> m_timeout_EOS " << m_timeout_EOS);

    mapper->clearValues();
    mapper->newScan(false);
    mapper->setScanningScheme( scanningScheme );
    mapper->setImageScale( w, h );

    getServer()->endOfRead();
  }
  else if( line.substr(0,3) == "EOS" ){ // END OF SCAN
    m_remaining_EOS--; // received a EOS -> decrease remaining EOS
    BUG_DEBUG("newLine: EOS -> m_remaining_EOS " << m_remaining_EOS);

    if ( m_remaining_EOS != 0 ) {
      if ( m_timerId_EOS == -1 ) {
#ifdef HAVE_QT
        m_timerId_EOS = startTimer( m_timeout_EOS*1000 );
#endif
      }

      // did not receive all EOS -> return
      getServer()->endOfRead();
      return true;
    }

    // received last remaining EOS -> do EOS
    if (m_timerId_EOS!= -1) {
      BUG_DEBUG("newLine: EOS -> kill EOS timer");
#ifdef HAVE_QT
      killTimer(m_timerId_EOS);
#endif
      m_timerId_EOS = -1;
    }

    GuiImage::setEos( true );
    mapper->endOfScan();
    if( m_on_eos ){
      getTrigger( m_on_eos )->startJob();
      // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
    }
    else{
      getServer()->endOfRead();
    }
  }
  else if( line.substr(0,4) == "NEXT" ){ // NEXT SCAN
    mapper->nextScan();
    getServer()->endOfRead();
  }
  else if( line.substr(2,4) == "READ" ){ // READ FROM DATAPOOL
    BUG_DEBUG("READ from Datapool: " << line);

    if( line.substr(18,3) == "IMG" ){
      GuiImage::setImageScan();
    }
    else{
      GuiImage::setGeneralScan();
    }
    // erase header bytes and read image size
    line.erase(0,22);
    BUG_DEBUG("erased  " << line);
    std::istringstream is( line );
    int w,h;
    std::string s, scanningScheme = "sawtooth";
    is >> w;
    is >> h;
    if ( is >> s ) scanningScheme = s;
    int numberOfEOS;
    is >> numberOfEOS;

    GuiImage::setEos( false );
    mapper->setScanningScheme( scanningScheme );
    mapper->readFromDatapool(w,h);
    GuiImage::setEos( true );
    getServer()->endOfRead();
  }
  else{
//      BUG_DEBUG("scan line: " << line);

    std::istringstream is( line );
    std::string s;
    int line_nr, channel=0, type;
    is >> type;
    is >> line_nr;
    is >> channel;
    BUG_DEBUG("newLine: type " << type
              << ", line_nr " << line_nr
              << ", channel " << channel);
    if( channel > 99 || channel < 0 ){
      BUG_ERROR("MFMServerSocket invalid channel "
                << channel << "\n" << line.substr(0,20));
    }
    else if( type > 2 || type < 0 ){
      BUG_ERROR("MFMServerSocket invalid type "
                << type << "\n" << line.substr(0,20));
    }
    else{
      int id=type * 100 + channel;
      BUG_DEBUG("newLine: id = " << id);
      mapper->newLine( id, line_nr, is );
    }
    return false; // Da fehlen noch Daten ( EOL )
  }
  return true;
}

#ifdef HAVE_QT
void MFMServerSocket::timerEvent( QTimerEvent *e ){
  int id = e->timerId();
  assert( id == m_timerId_EOS );

  killTimer(m_timerId_EOS);
  m_timerId_EOS = -1;

  MFMImageMapper *mapper=0;
  if( getRequestListener() ){
    mapper=static_cast<MFMImageMapper*>(getRequestListener());
  }
  if( !mapper ){
    return;
  }

  GuiImage::setEos( true );
  mapper->endOfScan();
  if( m_on_eos ){
    getTrigger( m_on_eos )->startJob();
    // Der Trigger löscht sich am Ende selbst ( JobStarter->tryAgain() )
  }
}
#endif
