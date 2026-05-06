// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#include "utils/gzstream.h"

std::istream* getInputStream(std::string fn){
  std::ifstream file(fn.c_str(), std::ios::binary);
  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::gzip_decompressor());
  in.push(file);
  return new std::istream(&in);
}

std::ostream* getOutputStream(std::string fn){
  std::ofstream file(fn.c_str(), std::ios::binary);
  boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
  out.push(boost::iostreams::gzip_compressor());
  out.push(file);
  return new std::ostream(&out);
}
