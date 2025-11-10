
#include <qglobal.h>
#include <QUrl>

#if QT_WEBENGINE
#if QT_VERSION >= 0x050900
// #if defined(Q_OS_WIN) || defined(Q_OS_CYGWIN)
// #include <QtWebKitWidgets/QWebView>
// #else
#include <QtWebEngineWidgets/QWebEngineView>
// #endif
#endif
#endif

#include "utils/FileUtilities.h"
#include "app/HelpHTML.h"
#include "app/AppData.h"
#include "app/HelpManager.h"

HelpHTML::HelpHTML(HelpManager *helpManager)
  : HelpFile(helpManager){
}

/* --------------------------------------------------------------------------- */
/* helpBrouserCmd --                                                           */
/* --------------------------------------------------------------------------- */

void HelpHTML::helpBrowserCmd( std::string &cmd, const std::string &keyName, bool remote ) const{
#ifdef _WIN32
  cmd = getenv("BROWSER") ? getenv("BROWSER") : "start ";
#else
  cmd = getenv("BROWSER") ? getenv("BROWSER") : "xdg-open";
#endif
  cmd += " ";
  if( remote ){
    cmd += "-remote ";
    cmd += "\"openFile(";
  }
#ifdef HAVE_QT
  QUrl url( QString::fromStdString( FileUtilities::MakeAbsolutPath( m_FileName ) ));
  cmd += url.toEncoded().data();
#else
// TODO
#endif
  if( !keyName.empty() ){
    cmd += "#";
    cmd += keyName;
  }
  if( remote ){
    cmd += ")\"";
  }
  else{
    cmd += " &";
  }
  //  std::cout << "  TRY To start CMD["<<cmd<<"]\n"<<std::flush;
  if ( AppData::Instance().Debug() ){
    fprintf( stdout, "helpBrouserCmd: '%s'\n", cmd.c_str() );
  }
}

/* --------------------------------------------------------------------------- */
/* help --                                                                     */
/* --------------------------------------------------------------------------- */

void HelpHTML::help( const std::string &keyName, const int chapterNo){

#if QT_WEBENGINE
#if QT_VERSION < 0x050900 || defined(Q_OS_WIN) || defined(Q_OS_CYGWIN)
  QWebView* view = new QWebView();
#else
  QWebEngineView* view = new QWebEngineView();
#endif
  view->load(QUrl(QString::fromStdString(m_FileName)));
  view->show();
#endif
}
