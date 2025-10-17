
#include "operator/SubscribeInterface.h"

//-------------------------------------------------
// Constructor
//-------------------------------------------------
SubscribeInterface::SubscribeInterface() : m_header(""), m_function(0) {
}

//-------------------------------------------------
// setSubscribeOutStreams
//-------------------------------------------------
void SubscribeInterface::setSubscribeOutStreams( const std::vector<Stream*>& out_streams )  {
  m_out_streams = out_streams;
}

//-------------------------------------------------
// setSubscribeFunction
//-------------------------------------------------
void SubscribeInterface::setSubscribeFunction( JobFunction* func )  {
  m_function = func;
}

//-------------------------------------------------
// setSubscribeHeader
//-------------------------------------------------
#include <iostream>
void SubscribeInterface::setSubscribeHeader( const std::string header )  {
  m_header = header;
}

//-------------------------------------------------
// setSubscribeListener
//-------------------------------------------------
void SubscribeInterface::setSubscribeListener( ConnectionListener *listener ) {
  m_listener = listener;
}

//-------------------------------------------------
// getSubscribeFunction
//-------------------------------------------------
JobFunction* SubscribeInterface::getSubscribeFunction() {
  return m_function;
}

//-------------------------------------------------
// getSubscribeOutStream
//-------------------------------------------------
std::vector<Stream*>& SubscribeInterface::getSubscribeOutStreams() {
  return m_out_streams;
}

//-------------------------------------------------
// getSubscribeHeader
//-------------------------------------------------
std::string& SubscribeInterface::getSubscribeHeader() {
  return m_header;
}

//-------------------------------------------------
// setSubscribeListener
//-------------------------------------------------
ConnectionListener* SubscribeInterface::getSubscribeListener() {
  return m_listener;
}
