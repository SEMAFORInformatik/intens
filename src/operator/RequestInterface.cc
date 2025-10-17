
#include "operator/RequestInterface.h"
#include "operator/MessageQueue.h"
#include "app/AppData.h"
#include "streamer/StreamManager.h"

//-------------------------------------------------
// Constructor
//-------------------------------------------------
RequestInterface::RequestInterface() : m_listener(0), m_header("initialize"), m_timeout(-1) {
}

//-------------------------------------------------
// setRequestOutStreams
//-------------------------------------------------
void RequestInterface::setRequestOutStreams( const std::vector<Stream*>& out_streams )  {
  m_out_streams = out_streams;
  if (AppData::Instance().OpenTelemetryMetadata()) {
    Stream *out = StreamManager::Instance().getStream(MessageQueue::OPENTELEMETRY_METADATA_OUT_STREAM);
    if (out)
      m_out_streams.push_back(out);
  }
}

//-------------------------------------------------
// setRequestInStreams
//-------------------------------------------------
void RequestInterface::setRequestInStreams( const std::vector<Stream*>& in_streams )  {
  m_in_streams = in_streams;
  if (AppData::Instance().OpenTelemetryMetadata()) {
    Stream *in = StreamManager::Instance().getStream(MessageQueue::OPENTELEMETRY_METADATA_IN_STREAM);
    if (in)
      m_in_streams.push_back(in);
  }
}

//-------------------------------------------------
// setRequestHeader
//-------------------------------------------------
#include <iostream>
void RequestInterface::setRequestHeader( const std::string header )  {
  m_header = header;
}

//-------------------------------------------------
// setRequestTimeout
//-------------------------------------------------
void RequestInterface::setRequestTimeout( const int timeout )  {
  m_timeout = timeout;
}

//-------------------------------------------------
// setRequestListener
//-------------------------------------------------
void RequestInterface::setRequestListener( ConnectionListener *listener ) {
  m_listener = listener;
}

//-------------------------------------------------
// getRequestInStreams
//-------------------------------------------------
const std::vector<Stream*>& RequestInterface::getRequestInStreams() const {
  return m_in_streams;
}

//-------------------------------------------------
// getRequestOutStreams
//-------------------------------------------------
const std::vector<Stream*>& RequestInterface::getRequestOutStreams() const {
  return m_out_streams;
}

//-------------------------------------------------
// getRequestHeader
//-------------------------------------------------
const std::string& RequestInterface::getRequestHeader() const {
  return m_header;
}

//-------------------------------------------------
// getRequestTimeout
//-------------------------------------------------
int RequestInterface::getRequestTimeout() const {
  return m_timeout;
}

//-------------------------------------------------
// setRequestListener
//-------------------------------------------------
ConnectionListener* RequestInterface::getRequestListener() {
  return m_listener;
}
