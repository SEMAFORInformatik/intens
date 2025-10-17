
#include "utils/Debugger.h"

#include "gui/GuiManager.h"
#include "gui/qt/GuiQtPixmap.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/GuiQtSvgView.h"

#include <QStackedWidget>
#include <QtSvg/QSvgRenderer>
#if QT_VERSION >= 0x060000
#include <QtSvgWidgets/QSvgWidget>
#elif QT_VERSION >= 0x050900
#include <QtSvg/QSvgWidget>
#else
#include <qwebview.h>
#include <QWebFrame>
#endif
#include <QUrl>
#include <QDir>
#include <qpixmap.h>
#include <qimage.h>
#include <qlabel.h>
#include <qfile.h>

#define PIXMAP_MIN_WIDTH  100
#define PIXMAP_MIN_HEIGHT 100

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtPixmap::GuiQtPixmap( GuiElement *parent )
  : GuiQtDataField( parent )
  , BasicStream( "" )
  , m_widgetStack( 0 )
  , m_expandable( false )
  , m_webview( 0 )
  , m_myLabel( 0 )
  , m_width( 0 )
  , m_height( 0 ){
}

GuiQtPixmap::~GuiQtPixmap(){
}

GuiQtPixmap::GuiQtPixmap( const GuiQtPixmap &pixmap )
  : GuiQtDataField( pixmap )
  , BasicStream( "" )
  , m_widgetStack( 0 )
  , m_expandable( pixmap.m_expandable )
  , m_webview( 0 )
  , m_myLabel( 0 )
  , m_width( pixmap.m_width )
  , m_height( pixmap.m_height ){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

QWidget* GuiQtPixmap::myWidget() {
  return m_widgetStack; //m_myLabel;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtPixmap::create(){
  BUG(BugGui,"GuiQtPixmap::create");

  // create stacked widget
  m_widgetStack = new QStackedWidget();

  QWidget *parent = getParent()->getQtElement()->myWidget();
  m_myLabel = new QLabel( parent );
  m_widgetStack->addWidget( m_myLabel );

#if QT_VERSION < 0x050900
  m_webview = new QWebView();
  if (m_width == 0) {
    m_webview->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
					   QSizePolicy::Expanding ));
  }
#else
  m_webview = new QSvgWidget();
  // m_webview->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,
  //                                        QSizePolicy::Fixed ));
#endif
  m_widgetStack->addWidget( m_webview );

  m_widgetStack->setCurrentWidget(m_myLabel);

  if (m_width) {
    m_widgetStack->setMinimumSize(m_width, m_height);
    if (!m_expandable) {
      m_widgetStack->setMaximumSize(m_width, m_height);
    }
  }
  else{
    m_widgetStack->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding,
					      QSizePolicy::MinimumExpanding) );
  }

  m_myLabel->setAlignment( getQtAlignment(getAlignment()) );

  m_param->DataItem()->setDimensionIndizes();

  updateWidgetProperty();
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */
bool GuiQtPixmap::destroy(){
  if (m_widgetStack) {
    m_widgetStack->deleteLater();
    m_widgetStack = 0;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setSize --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtPixmap::setSize( int w, int h ){
  m_width  = w;
  m_height = h;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtPixmap::getSize( int &w, int &h, bool hint){
  BUG(BugGui,"GuiQtPixmap::getSize");
  if (m_width != 0 || m_height != 0) {
    w = m_width;
    h = m_height;
  } else {
    w = m_widgetStack->width();
    h = m_widgetStack->height();
  }
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtPixmap::update( UpdateReason reason ){
  BUG(BugGui,"GuiQtPixmap::update");
  BUG_DEBUG("GuiQtPixmap::update, vn " << m_param->DataItem()->getFullName(true));
  updateWidgetProperty();

  if( m_param == 0 ||  m_widgetStack == 0 ) return;
  switch( reason ){
  case reason_FieldInput:
  case reason_Process:
    if( !m_param->isUpdated( GuiManager::Instance().LastGuiUpdate(), true ) ){
      return;
    }
    break;
  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
    setUpdated();
    break;
  default:
    BUG_MSG("Unhandled Update Reason");
    break;
  }
  // m_widgetStack->removeWidget(m_webview);
  if (m_width == 0) {
    m_widgetStack->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  }

  // load pixmap
  std::string pixName;
  QPixmap pixmap;
  m_param->getFormattedValue( pixName );
  // direct svg load
  BUG_DEBUG("GuiQtPixmap::update pixName len: " << pixName.size() << ", Data: " << pixName.substr(0, std::min(80, (int)pixName.size())));
  if (isTypeSvg(pixName)) {
    m_widgetStack->setCurrentWidget(m_webview);
    QString qtext = QString::fromStdString(pixName);
    QByteArray bArray(qtext.toStdString().c_str());
    QSvgRenderer renderer(bArray);
    BUG_DEBUG("ViewBox w: " << renderer.viewBox().width()
	    << "  h: " << renderer.viewBox().height()
	    << "  x: " << renderer.viewBox().x()
	    << "  y: " << renderer.viewBox().y());
    BUG_DEBUG("DefaultSize w: " << renderer.defaultSize().width()
              <<"  h: " << renderer.defaultSize().height());
    BUG_DEBUG("WidgetStack w: " << m_widgetStack->size().width()
              <<"  h: " << m_widgetStack->size().height());
    QSize vbSize = renderer.viewBox().size();
    double yfac = 1.0*m_widgetStack->size().height() / vbSize.height();
    double xfac = 1.0*m_widgetStack->size().width() / vbSize.width();
    double fac = std::min(yfac ? yfac : 1, xfac ? xfac : 1);
    BUG_DEBUG("== yfac: " << yfac
              <<"  xfac: " << xfac << " ==>  fac: " << fac);

    m_webview->setMinimumSize(QSize(fac*vbSize.width(),
				    fac*vbSize.height()));
    m_webview->setMaximumSize(QSize(fac*vbSize.width(),
				    fac*vbSize.height()));
#if QT_VERSION > 0x050900
    m_webview->load(bArray);
#elif QT_VERSION >= 0x050000
    m_webview->setContent(bArray);
#else
    QUrl url("file://"+QDir::currentPath()+QDir::separator());
    m_webview->setHtml(bArray, url);
#endif

#if QT_VERSION < 0x050900
    // transparent background
    QPalette pal = m_webview->palette();
    pal.setBrush(QPalette::Base, Qt::transparent);
    m_webview->page()->setPalette(pal);
#endif
    QString sheet("background-color:transparent");
    if(m_webview->styleSheet() != sheet) {
      m_webview->setStyleSheet(sheet);
    }
    m_webview->setAttribute(Qt::WA_TranslucentBackground);
  }
  else {
    // svg load via filename
    if( pixName.size() < 1000 && QtIconManager::Instance().getWebViewWidget( pixName, *m_webview ) ){
      if (m_width == 0 &&
	  m_webview->maximumSize().width() > 0 &&
	  m_webview->maximumSize().height() > 0) {
        m_widgetStack->setMaximumSize(m_webview->maximumSize().width(),
                                      m_webview->maximumSize().height());
      }
      m_widgetStack->setCurrentWidget(m_webview);
    }
    else {
      // other pixmaps load via filename
      if( pixName.size() ) {
	// is pixmap name size > 1K => pixmap data
	if (pixName.size() > 1000) {
	  QByteArray bArray(pixName.c_str());
	  pixmap.loadFromData((const uchar*) pixName.c_str(), pixName.size());
    if (m_width && !pixmap.isNull()) {
      pixmap = pixmap.scaled(m_width, m_height);
    }
  }
	else {
	  QtIconManager::Instance().getPixmap( pixName, pixmap, m_width, m_height );
	}
	m_myLabel->setPixmap( pixmap );
      }
      else {
	m_myLabel->setPixmap( pixmap );
      }
      m_widgetStack->setCurrentWidget(m_myLabel);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtPixmap::getExpandPolicy() {
  return myWidget()->sizePolicy().expandingDirections();
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtPixmap::getDialogExpandPolicy() {
  return getGuiOrientation(getExpandPolicy());
}

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtPixmap::getVisibleDataPoolValues( GuiValueList& vmap ) {
  if (m_param) {
    std::string s;
    m_param->getFormattedValue( s );
    std::string::size_type pos;
    pos = s.rfind('/');
    if( pos != std::string::npos )
      s = s.substr( ++pos );
    s = "/images/"+s;

    vmap.insert( GuiValueList::value_type(m_param->getFullName(), s ) );
  }
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool GuiQtPixmap::write( InputChannelEvent &event ) {
  BUG(BugPlot,"GuiQtPixmap::write");
  std::string s;
  std::ostringstream os;
  getValue(s);
  os << s;
  event.write( os );
  return true;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool GuiQtPixmap::read( OutputChannelEvent &event ) {
  BUG(BugPlot,"GuiQtPixmap::read");

  std::string s = event.getStreamBuffer();
  if (s.size())
    setValue( s );
  else
    setInvalid();
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtPixmap::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:Pixmap ";
  GuiDataField::serializeXML(os, recursive);
  os << "</intens:Pixmap>" << std::endl;
}

#if HAVE_PROTOBUF
bool GuiQtPixmap::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiDataField::serializeProtobuf(eles->add_data_fields());
}
#endif
/* --------------------------------------------------------------------------- */
/* setExpandable --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtPixmap::setExpandable( FlagStatus status ){
  m_expandable = status == status_ON ? true : false;
}
