
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#ifndef CHANNELEVENT_H
#define CHANNELEVENT_H

#include <iostream>
#include <stdio.h>

class InputChannel;
class OutputChannel;

/** class for Channel events
 */
class ChannelEvent {
 public:
  ChannelEvent( InputChannel *channel );
  ChannelEvent( OutputChannel *channel );
  ostream& getOstream();
  FILE* getFilePointer();
  istream& getIstream();

 private:
  InputChannel* m_inChannel;
  OutputChannel* m_outChannel;
};

#endif