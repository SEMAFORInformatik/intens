
#include <assert.h>
#include <string>

#include "operator/Channel.h"
#include "operator/InputChannelEvent.h"

InputChannelEvent::InputChannelEvent( InputChannel *channel )
  : m_Channel( channel ){
  assert( channel != 0 );
}

#if defined HAVE_QT
bool InputChannelEvent::write(std::ostringstream& os) {
  return m_Channel->write( os );
}
#else
std::ostream& InputChannelEvent::getOstream() {
  return m_Channel->getOstream();
}
#endif

std::istream& InputChannelEvent::getIstream() {
  return m_Channel->getIstream();
}
