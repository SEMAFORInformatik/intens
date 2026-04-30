
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#include "streamer/StreamParameter.h"
#include "utils/FileUtilities.h"

#ifdef __MINGW32__
bool StreamParameter::read( const std::string &filename ) {
  std::istream *ifs = FileUtilities::ReadFile(filename);
  if (!ifs) return false;
  bool ret =  read(*ifs);
  delete ifs;
  return ret;
}
#endif