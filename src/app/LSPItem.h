
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#if !defined(_LSP_ITEM_INCLUDED_H)
#define _LSP_ITEM_INCLUDED_H

#include <string>

class LSPItem {
public:
  inline void setLSPFilename(std::string name) { m_LspFilename = name; }
  inline void setLSPLineno(int lineno) { m_LspLineno = lineno; }
  inline std::string LSPFilename() const { return m_LspFilename; }
  inline int LSPLineno() const { return m_LspLineno; }

private:
  std::string m_LspFilename;
  int m_LspLineno;
};

#endif // !defined(_LSP_ITEM_INCLUDED_H)
