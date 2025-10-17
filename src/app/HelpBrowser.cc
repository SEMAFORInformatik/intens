
#if defined HAVE_QT && !defined HAVE_HEADLESS

#include <iostream>
#include "gui/qt/HelpWindow.h"
#include "app/HelpBrowser.h"
#include "app/UiManager.h"
#include "app/log.h"

HelpBrowser::HelpBrowser(HelpManager *helpManager)
    : HelpFile(helpManager)
    , m_helpwindow(0)
{}

HelpBrowser::~HelpBrowser(){
  if (m_helpwindow)
    delete m_helpwindow;
}

/* --------------------------------------------------------------------------- */
/* help --                                                                     */
/* --------------------------------------------------------------------------- */

void HelpBrowser::help( const std::string &keyName, const int chapterNo){

  std::string parameter = m_FileName.c_str();
  if (keyName != "") parameter = parameter + '#' + keyName;

  if (!m_helpwindow)
    m_helpwindow = new HelpWindow(parameter.c_str(), ".");
  m_helpwindow->show();
  m_helpwindow = NULL;
}

#endif
