
#ifndef INPUT_CHANNEL_EVENT_H
#define INPUT_CHANNEL_EVENT_H

#include <iostream>
#include <stdio.h>
#include <assert.h>

class InputChannel;

/** class for Input Channel events
 */
class InputChannelEvent{
 public:
  InputChannelEvent( InputChannel *channel );
#if defined HAVE_QT
  bool write(std::ostringstream& os);
#else
  std::ostream& getOstream();
  //  FILE* getFilePointer();
#endif
  std::istream& getIstream();

 private:
  InputChannel* m_Channel;
};

#endif
