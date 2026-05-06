// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#ifndef GZ_STREAM_H
#define GZ_STREAM

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <fstream>

std::istream* getInputStream(std::string fn);
std::ostream* getOutputStream(std::string fn);
#endif
