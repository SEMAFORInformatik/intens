// SPDX-FileCopyrightText: 2026 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#ifndef URIHANDLER_H
#define URIHANDLER_H

#include <map>
#include <string>

class URIHandler {
public:
  URIHandler() = default;
  URIHandler(const std::string &uri);

  void registerHandler();
  void call();

private:
  std::string m_method;
  std::map<std::string, std::string> m_arguments;
};

#endif
