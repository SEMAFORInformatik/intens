
#ifndef OUTPUT_CHANNEL_EVENT_H
#define OUTPUT_CHANNEL_EVENT_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <assert.h>

class OutputChannel;

/** class for Input Channel events
 */
class OutputChannelEvent{
 public:
  OutputChannelEvent( OutputChannel *channel );
  std::ostream& getOstream();
  std::string& getStreamBuffer();

 private:
  OutputChannel* m_Channel;
};

#endif
