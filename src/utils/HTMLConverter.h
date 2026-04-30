// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#if !defined(HTML_CONVERTER_H)
#define HTML_CONVERTER_H

#include <string>

class HTMLConverter{
public:
  static void convert2HTML( std::string &text );
  static void convertFromHTML( std::string &text );
};

#endif
