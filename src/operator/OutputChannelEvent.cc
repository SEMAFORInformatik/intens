
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#include <assert.h>
#include <string>

#include "operator/Channel.h"
#include "operator/OutputChannelEvent.h"

OutputChannelEvent::OutputChannelEvent( OutputChannel *channel )
  : m_Channel( channel ){
  assert( channel != 0 );
}

std::ostream& OutputChannelEvent::getOstream() {
  return m_Channel->getOstream();
}

std::string& OutputChannelEvent::getStreamBuffer(){
 return m_Channel->getStreamBuffer();
}