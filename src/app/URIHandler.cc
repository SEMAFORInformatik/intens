// SPDX-FileCopyrightText: 2026 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#include "URIHandler.h"
#include "utils/JsonUtils.h"
#include <jsoncpp/json/value.h>
#include <qurl.h>
#include <qurlquery.h>
#include <zmq.hpp>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
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
  zmq::context_t ctx;
  zmq::socket_t sock(ctx, zmq::socket_type::req);

  auto jsonElem = Json::Value(Json::objectValue);
  jsonElem["command"] = "LOGIN";

  auto array = Json::Value(Json::arrayValue);
  array.append("__token__");
  array.append(jwt);
  array.append("en-US");

  jsonElem["argument"] = array;

  auto message = ch_semafor_intens::JsonUtils::value2string(jsonElem);

  sock.connect("tcp://localhost:15560");
  sock.send(zmq::str_buffer("control"), zmq::send_flags::sndmore);
  sock.send(zmq::buffer(message));
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
    result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        basePath.c_str(),
        0, nullptr, REG_OPTION_NON_VOLATILE,
        KEY_WRITE, nullptr, &hKey, nullptr
    );

    if (result != ERROR_SUCCESS) return;

    std::wstring description = L"URL:intens Protocol";
    RegSetValueExW(hKey, nullptr, 0, REG_SZ, 
                   (const BYTE*)description.c_str(), 
                   (DWORD)((description.size() + 1) * sizeof(wchar_t)));

    // Set "URL Protocol" value (empty string required)
    RegSetValueExW(hKey, L"URL Protocol", 0, REG_SZ, 
                   (const BYTE*)L"", 0);

    RegCloseKey(hKey);

    // Create shell/open/command subkey
    std::wstring commandKey = basePath + L"\\shell\\open\\command";
    result = RegCreateKeyExW(HKEY_CURRENT_USER, commandKey.c_str(), 0, nullptr,
                             REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (result != ERROR_SUCCESS) return;

    std::wstring commandLine = L"\"" + exePath + L"\" --uriOpener \"%1\"";
    RegSetValueExW(hKey, nullptr, 0, REG_SZ,
                   (const BYTE*)commandLine.c_str(),
                   (DWORD)((commandLine.size() + 1) * sizeof(wchar_t)));

    RegCloseKey(hKey);

    // Notify system of changes
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSHNOWAIT, nullptr, nullptr);

}

#endif
