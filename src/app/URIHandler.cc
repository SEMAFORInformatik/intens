// SPDX-FileCopyrightText: 2026 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#include "URIHandler.h"
#include "utils/JsonUtils.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/value.h>
#include <qurl.h>
#include <qurlquery.h>
#include <sstream>
#include <string_view>
#include <zmq.hpp>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#pragma comment(lib, "shlwapi.lib")
#endif

URIHandler::URIHandler(const std::string &uri) {
  QUrl url(QString::fromStdString(uri));

  m_method = url.path().toStdString();
  QUrlQuery query(url.query());

  for (auto &[key, val] : query.queryItems()) {
    m_arguments[key.toStdString()] = val.toStdString();
  }
}

void login(const std::string &jwt) {
  const std::string command = "login_token";
  zmq::context_t ctx;
  zmq::socket_t sock(ctx, zmq::socket_type::req);

  auto args = Json::Value(Json::objectValue);
  args["user"] = "empty";
  args["token"] = jwt;

  auto message = ch_semafor_intens::JsonUtils::value2string(args);
  zmq::message_t zmqMessage(message.size());
  memcpy(zmqMessage.data(), message.data(), message.size());

  sock.connect("tcp://localhost:4000");
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
  sock.send(zmq::buffer(command), zmq::send_flags::sndmore);
  sock.send(zmq::buffer(message));
#else

  {
    zmq::message_t zmqMessage(command.size());
    memcpy(zmqMessage.data(), command.data(), command.size());
    sock.send(zmqMessage, ZMQ_SNDMORE);
  }
  {
    zmq::message_t zmqMessage(message.size());
    memcpy(zmqMessage.data(), message.data(), message.size());
    sock.send(zmqMessage);
  }
#endif
}

void URIHandler::call() {
  assert(!m_method.empty());

  if (m_method == "/LOGIN") {
    login(m_arguments.at("jwt"));
    return;
  }
}

#ifdef _WIN32
void URIHandler::registerHandler() {
  wchar_t exePathC[MAX_PATH];
  GetModuleFileNameW(nullptr, exePathC, MAX_PATH);
  std::wstring exePath = exePathC;

  HKEY hKey = nullptr;
  LONG result;
  std::wstring basePath = L"Software\\Classes\\intens";

  // Create/open the main protocol key
  result = RegCreateKeyExW(HKEY_CURRENT_USER, basePath.c_str(), 0, nullptr,
                           REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey,
                           nullptr);

  if (result != ERROR_SUCCESS)
    return;

  std::wstring description = L"URL:intens Protocol";
  RegSetValueExW(hKey, nullptr, 0, REG_SZ, (const BYTE *)description.c_str(),
                 (DWORD)((description.size() + 1) * sizeof(wchar_t)));

  // Set "URL Protocol" value (empty string required)
  RegSetValueExW(hKey, L"URL Protocol", 0, REG_SZ, (const BYTE *)L"", 0);

  RegCloseKey(hKey);

  // Create shell/open/command subkey
  std::wstring commandKey = basePath + L"\\shell\\open\\command";
  result = RegCreateKeyExW(HKEY_CURRENT_USER, commandKey.c_str(), 0, nullptr,
                           REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey,
                           nullptr);
  if (result != ERROR_SUCCESS)
    return;

  std::wstring commandLine = L"\"" + exePath + L"\" --uriOpener \"%1\"";
  RegSetValueExW(hKey, nullptr, 0, REG_SZ, (const BYTE *)commandLine.c_str(),
                 (DWORD)((commandLine.size() + 1) * sizeof(wchar_t)));

  RegCloseKey(hKey);

  // Notify system of changes
  SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSHNOWAIT, nullptr, nullptr);
}

#else
void URIHandler::registerHandler() {
  std::stringstream applicationFolderStream;
  auto dataHome = std::getenv("XDG_DATA_HOME");
  std::system("xdg-mime default intens-uri.desktop x-scheme-handler/intens");

  if (dataHome) {
    applicationFolderStream << dataHome << "/applications";
  } else {
    auto home = std::getenv("HOME");
    applicationFolderStream << home << "/.local/share/applications";
  }

  auto applicationFolder = applicationFolderStream.str();

  if (!std::filesystem::exists(applicationFolder)) {
    std::filesystem::create_directory(applicationFolder);
  }

  auto filename = applicationFolder + "/intens-uri.desktop";

  if (std::filesystem::exists(filename)) {
    return;
  }

  std::ofstream file(applicationFolder + "/intens-uri.desktop");
  file << R"([Desktop Entry]
Encoding=UTF-8
Type=Application
Name=Intens URI Handler
Exec=intens --uriOpener %u
Terminal=false
Comment=INTENS URL handler
Categories=Application;Network;
MimeType=x-scheme-handler/intens;)";
}
#endif
