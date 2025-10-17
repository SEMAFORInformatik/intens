
#include <assert.h>
#include <string>

#include "operator/Channel.h"
#include "operator/ChannelEvent.h"

ChannelEvent::ChannelEvent( InputChannel *channel )
  : m_inChannel( channel )
  , m_outChannel( 0 ) {
  assert( channel != 0 );
}

ChannelEvent::ChannelEvent( OutputChannel *channel )
  : m_outChannel( channel )
  , m_inChannel( 0 ) {
  assert( channel != 0 );
}

ostream& ChannelEvent::getOstream() {
  if( m_inChannel == 0 )
    return m_outChannel->getOstream();
  else
    return m_inChannel->getOstream();
}

FILE* ChannelEvent::getFilePointer() {
  assert( m_inChannel != 0 );
  return m_inChannel->getFilePointer();
}

istream& ChannelEvent::getIstream() {
  assert( m_inChannel != 0 );
  return m_inChannel->getIstream();
}
