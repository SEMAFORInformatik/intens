
#ifndef MESSAGING_QUEUE_REQUEST_INCLUDED_H
#define MESSAGING_QUEUE_REQUEST_INCLUDED_H

#include <string>

#include <QtCore/QObject>

#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "gui/Timer.h"
#include "gui/TimerTask.h"
#include "operator/RequestInterface.h"
#include "operator/MessageQueue.h"

class Stream;
class JobFunction;
class MessageQueueRequestThread;

#ifdef HAVE_QT
class MessageQueueRequest : public QObject, public RequestInterface, public LSPItem
#else
class MessageQueueRequest : public RequestInterface, public LSPItem
#endif
{
public:
  //-------------------------------------------------
  // Constructor / Destructor
  //-------------------------------------------------
  MessageQueueRequest(const std::string& host, int port, int timeout);
  virtual ~MessageQueueRequest();

  //-------------------------------------------------
  // member functions of RequestInterface
  //-------------------------------------------------

  /** set request output Streams for next operation
   */
  // void setRequestOutStreams( const std::vector<Stream*>& out_streams );
  /** set request input Streams for next operation
   */
  // void setRequestInStreams( const std::vector<Stream*>& in_streams );
  /** set request header for next operation
   */
  /* void setRequestHeader( const std::string& header ); */

  /** start a single request to message queue publisher
   */
  void startRequest();
  /** check input for a single request to message queue publisher
   */
  bool checkRequest();

  /** set connection listener for a single request to message queue publisher
   */
  /** get error message
   */
  std::string getErrorMessage() const;

  /** terminate thread
   */
  void terminateRequest();

  /** get default receive timeout
   */
  int getTimeout();

#ifdef HAVE_QT
  /** timer event for GuiUpdates
   */
  void timerEvent ( QTimerEvent * event );


  Q_OBJECT
private slots:
  void slot_error(const std::string& messageShort, const std::string& messageLong);
  void slot_receivedRequestData(const std::vector<std::string>& dataList);
#endif

// -----------------------------------------------------
// private definitions
// -----------------------------------------------------

private:
  std::string   m_host;
  int           m_requestPort;
  MessageQueueRequestThread* m_requestThread;
  int           m_defaultTimeout;
  std::string   m_errMsg;

};

#endif
