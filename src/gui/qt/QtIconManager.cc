
#include<QDir>
#include<QImage>
#include<QPixmap>
#include<QImageReader>
#include<QIcon>
#include<QList>
#define QWebView QSvgWidget
#if QT_VERSION >= 0x060000
#include <QtSvgWidgets/QSvgWidget>
#elif QT_VERSION >= 0x050900
#include <QtSvg/QSvgWidget>
#else
#include <qwebview.h>
#endif
#include<QLabel>
#include <QGridLayout>
#include <QtSvg/QSvgRenderer>
#include<QPainter>

#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

#include "gui/qt/QtIconManager.h"
#include "app/AppData.h"
#include <iostream>

INIT_LOGGER();

QtIconManager* QtIconManager::s_instance = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
QtIconManager::QtIconManager() {
#if 0  // amg 2017-09-12
  QList<QByteArray> slist = QImageReader::supportedImageFormats();

  // test icon type map and remove type if not exists
  IconTypeMap::iterator tit = s_iconTypeMap.begin();
  for( ; tit != s_iconTypeMap.end(); ++tit){
    std::string::size_type pos = tit->second.find_last_of(".");
    if (pos == std::string::npos) continue;
    std::string::size_type posE = tit->second.find_last_of(")");
    if (posE == std::string::npos) posE = tit->second.size();
    std::string str( upper(tit->second.substr(pos+1, posE-pos-1)) );

    int ret = slist.indexOf( QString::fromStdString(str).toLower().toLocal8Bit() );
    if (ret < 0 && str != "JPG") {
      //      std::cerr << compose(_("WARNING: graphic file type [%1] can make problems."), str) << std::endl << std::flush;
///      s_iconTypeMap.erase(tit);
    }
  }
#endif
}
QtIconManager::~QtIconManager() {
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

QtIconManager &QtIconManager::Instance() {
  if (s_instance == 0) {
    s_instance = new QtIconManager;
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* getPixmap --                                                                */
/* --------------------------------------------------------------------------- */

bool QtIconManager::getPixmap( const std::string& iconname,
                               QPixmap& pixmap, int width, int height ){
  BUG_PARA(BugGuiIcon,"QtIconManager::getPixmap",iconname);
  ICON_TYPE icon_type;

  std::string file = locateFile( icon_type, iconname );
  if (file.empty()) {
    if ( QIcon::hasThemeIcon(QString::fromStdString(iconname)) ) {
      pixmap = QIcon::fromTheme(QString::fromStdString(iconname)).pixmap(width, height);
      return true;
    }
    return false;
  }
  return getPixmap(file, icon_type, pixmap, width, height);
}

/* --------------------------------------------------------------------------- */
/* getPixmap --                                                                */
/* --------------------------------------------------------------------------- */

bool QtIconManager::getPixmap(const std::string& filename, ICON_TYPE icon_type,
                              QPixmap& pixmap, int width, int height ){

  BUG_PARA(BugGuiIcon,"QtIconManager::getPixmap",filename);

  switch (icon_type) {
  case SVG:
    {
      QSvgRenderer renderer(QString::fromStdString(filename));
      QPixmap pm(width > 0 && height > 0 ? QSize(width, height) : renderer.defaultSize());
      pm.fill(Qt::transparent);
      QPainter painter;
      painter.begin(&pm);
      renderer.render(&painter, pm.rect());
      painter.end();
      pixmap = pm;
      return true;
    }
    case XPM:
    case BMP:
    case GIF:
    case MNG:
    case JPG:
    case PBM:
    case PGM:
    case PNG:
    case PNM:
    case PPM:
    case EPS:
    case XBM:
      {
        QImage qimg( QString::fromStdString(filename) );
        if( width > 0 && height > 0 ) {  // scale
          qimg = qimg.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        if ( ! qimg.isNull() ) {
          pixmap = QPixmap::fromImage( qimg );
          return true;
        }
        else
          std::cerr << "QtIconManager:   WARNING unknown type of file ["<< filename <<"]\n"  << std::flush;
        break;
      }

  default:
    std::cerr << "QtIconManager:   WARNING unknown type ["<< icon_type <<"]\n"  << std::flush;
    break;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getWebViewWidget --                                                         */
/* --------------------------------------------------------------------------- */
bool QtIconManager::getWebViewWidget( const std::string& iconname,
				      QWebView& webview,
				      int width, int height) {
  BUG_PARA(BugGuiIcon,"QtIconManager::getWebViewWidget",iconname);
  ICON_TYPE icon_type;

  std::string file = locateFile( icon_type, iconname );
  if (file.empty())
    return false;
  switch (icon_type) {
    case SVG: {
      // transparent background
      QSvgRenderer* renderer = new QSvgRenderer( QString::fromStdString(file) );

#if QT_VERSION < 0x050900
      QPalette pal = webview.palette();
      pal.setBrush(QPalette::Base, Qt::transparent);
      webview.page()->setPalette(pal);
#endif
      QString sheet("background:transparent");
      if(webview.styleSheet() != sheet) {
        webview.setStyleSheet("background:transparent");
      }
      webview.setAttribute(Qt::WA_TranslucentBackground);
      webview.setDisabled( true );
#if defined(Q_OS_WIN) || defined(Q_OS_CYGWIN)
      /// evtl. mit älterem QT:  webview.load(QString::fromStdString(compose("file:///%1", file)));
      webview.load(QString::fromStdString(file));
#else
      webview.load(QString::fromStdString(file));
#endif
      // additional 20 pixel for scrollbar
      webview.setMaximumSize(QSize(renderer->defaultSize().width()+20, renderer->defaultSize().height()+20));
      return true;
    }
  default:
    BUG_WARN("File type with name '" << iconname  << "' not supported ");
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getLabelPixmap --                                                           */
/* --------------------------------------------------------------------------- */

QPixmap* QtIconManager::getLabelPixmap( const std::string& label, QPixmap& pm, QFont& font, bool& multiLine ) {
  QPixmap* pixmap;

  // create a label pixmap
  QLabel *lbl = new QLabel();
  QString labelStr = QString::fromStdString(label);
  if ( labelStr.contains('\n') ) {
    multiLine = true;
    QWidget widget;
    QFontMetrics fm(font);
    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(0);
    widget.setLayout( layout );
    QStringList strList = labelStr.split('\n');
    int col = pm.isNull() ? 0 : 1;
    if (!pm.isNull())
      lbl->setPixmap(pm);
    if (col) { layout->addWidget( lbl, 0, 0, -1, 1); }
    for (int i = 0; i < strList.size(); ++i) {
      QLabel *lbl = new QLabel();
      lbl->setFont( font );
      lbl->setText( strList.at(i) );
      layout->addWidget( lbl, i, col);
    }
    pixmap = new QPixmap(widget.sizeHint().width(), widget.sizeHint().height());
    widget.setAutoFillBackground(true);
    widget.render(pixmap, QPoint(), QRegion(), QWidget::DrawChildren);
    return pixmap;
  } else {
    multiLine = false;
    lbl->setFont( font );
    lbl->setText( labelStr );
    pixmap = new QPixmap(lbl->sizeHint().width(), lbl->sizeHint().height());
    lbl->render(pixmap);
  }

  return pixmap;
}
