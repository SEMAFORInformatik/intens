
#ifdef __MINGW32__
#include <winsock2.h>
#endif

#include "ClientSocket.h"
#include "xfer/XferDataItem.h"
#include "streamer/Stream.h"
#include "utils/gettext.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"

  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
ClientSocket::ClientSocket ( Stream *out_stream
			      , Stream *in_stream )
  : m_host( "localhost" )
  , m_ip("")
  , m_port( 2100 )
  , m_portxfer(0)
  , m_hostxfer(0)
  , m_out_stream( out_stream )
  , m_in_stream( in_stream )
  , m_listener( 0 ){
}

ClientSocket::~ClientSocket(){
  delete m_portxfer;
  delete m_hostxfer;
};

bool ClientSocket::checkInput(){
  std::vector<std::string> invaliditems;

  if( getOutStream() ) {
    if( getOutStream()->getInvalidItems(invaliditems) ){
      std::ostringstream errs;
      //       errs << mls.getString( "ML_CH_INCOMP_INP", ML_CONST_STRING );
      //      errs << _("\nIncomplete input for the following items:\n");
      errs << compose(_("\nIncomplete input for the following items \nin stream '%1':\n"), getOutStream()->Name());

      for( int i=0; i<invaliditems.size(); ++i ){
	errs << "   " << invaliditems[i] << std::endl;
      }
      m_message=errs.str();
      GuiFactory::Instance()->getLogWindow()->writeText( errs.str() );
      return false;
    }
  }
  return true;
}


/* --------------------------------------------------------------------------- */
/* getPort --                                                                  */
/* --------------------------------------------------------------------------- */

int ClientSocket::getPort(){
  if( m_portxfer ){
    m_portxfer->getValue( m_port );
  }
  return m_port;
}

/* --------------------------------------------------------------------------- */
/* getHost --                                                                  */
/* --------------------------------------------------------------------------- */
const std::string &ClientSocket::getHost(){
  if( m_hostxfer ){
    m_hostxfer->getValue( m_host );
  }

#ifdef __MINGW32__
  if( m_ip.empty() ){
    hostent *h = NULL;

    h = gethostbyname(m_host.c_str());

    if (h == NULL){
      return m_host;
    }
    m_ip=inet_ntoa(*(reinterpret_cast<in_addr*>(h->h_addr)));
  }
  return m_ip;
#else
  return m_host;
#endif
}
