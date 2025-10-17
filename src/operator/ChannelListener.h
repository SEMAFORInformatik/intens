
#ifndef CHANNEL_LISTENER_H
#define CHANNEL_LISTENER_H

#include <iostream>
#include "operator/InputChannelEvent.h"
#include "operator/OutputChannelEvent.h"

class QProcess;

/** abstract base class for Channel listener
 *  must be implemented by channel customers and providers
 */
class ChannelListener {
 public:
    /// reads from istream
  virtual bool read( OutputChannelEvent & ) = 0;
  /// writes to ostream
  virtual bool write( InputChannelEvent& ) = 0;
  /// terminates an updatable Stream
  virtual void resetContinuousUpdate() = 0;
  virtual bool setContinuousUpdate( bool flag ) = 0;
#if defined HAVE_QT
  virtual QProcess* getQProcess() { assert(false); return 0; }
#endif
};

#endif
