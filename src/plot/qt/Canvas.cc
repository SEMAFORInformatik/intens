
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QResizeEvent>
#include "gui/GuiPopupMenu.h"

#include <assert.h>

#include "utils/Debugger.h"
#include "app/AppData.h"
#include "gui/qt/QtMultiFontString.h"
#include "plot/qt/GdevQt.h"
#include "plot/GdevSVG.h"
#include "plot/GdevPS.h"
#include "plot/RedisplayListener.h"
#include "plot/qt/Canvas.h"
#include <unistd.h>

static const double mmToPixel = 72/25.4;
static const double PixelTomm = 1/mmToPixel;

//******************************
//
//   FACILITY CLASS
//
//******************************

PlotCanvasView::PlotCanvasView(QtCanvas* c, QWidget* par, bool resizeable)
  : QtCanvasView(c, par), m_menu(0)
{
  setWidgetResizable( resizeable );
}

void PlotCanvasView::setPopupMenu(GuiPopupMenu* menu) {
  m_menu = menu;
}

GuiPopupMenu* PlotCanvasView::getPopupMenu() {
  return m_menu;
}

void PlotCanvasView::contextMenuEvent(QContextMenuEvent *e) {
  if (m_menu)
    m_menu->popup();
  else
    QtCanvasView::contextMenuEvent( e );
}

void PlotCanvasView::resizeEvent(QResizeEvent *e) {
  if ( widgetResizable() )
    canvas()->resize( e->size().width(), e->size().height());
  QtCanvasView::resizeEvent(e);
}

//******************************
//
//   Canvas CLASS
//
//******************************

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* Canvas --                                                                   */
/* --------------------------------------------------------------------------- */

iCanvas::iCanvas( const std::string &name, RedisplayListener *rlistener, int width, int height )
  : m_gdev(0)
  , m_form(0)
  , m_darea(0)
  , m_canvasView(0)
  , m_pixmap( 0 )
  , m_width( (int)(width*mmToPixel) )
  , m_height( (int)(height*mmToPixel) )
  , m_xmin(0)
  , m_ymin(0)
  , m_xmax(0)
  , m_ymax(0)
  , m_rlistener( rlistener ) {

}

/* --------------------------------------------------------------------------- */
/* ~Canvas --                                                                  */
/* --------------------------------------------------------------------------- */

iCanvas::~iCanvas() {
  delete m_gdev;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void iCanvas::create() {
  BUG(BugPlot,"Canvas::create");

  // create drawing area
  m_darea = new QtCanvas(m_width, m_height);
  m_darea->setAdvancePeriod(30);

  if( m_title.empty() ) {
    m_form = m_canvasView = new PlotCanvasView(m_darea, NULL, true);
    m_canvasView->resize(m_width, m_height);
    m_form->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding)  );
  }
  else {
    m_form  = new QWidget(); //QScrollArea();
    m_form->setWindowTitle( QString::fromStdString(m_title) );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5,5,5,5);
    layout->setSpacing( 5 );
    m_form->setLayout( layout );

    QString s = QtMultiFontString::getQString( m_title );
    QLabel *tw = new QLabel( QtMultiFontString::getQString( m_title ) );
    QFont font = tw->font();
    tw->setFont( QtMultiFontString::getQFont( "@title@", font ) );
    layout->addWidget(tw, 0, Qt::AlignHCenter);

    m_canvasView = new PlotCanvasView(m_darea, NULL, true);
    m_canvasView->resize(m_width, m_height);
    layout->addWidget(m_canvasView, 100);
    m_form->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding)  );
  }

  QObject::connect( m_darea, SIGNAL(resized()),
		    this, SLOT(resizedSlot() ) );

  m_gdev = new GdevQt( m_darea, (AppData::Instance()).XFontList() );
  /* for debugging purposes
  char filename[] = "/tmp/listplot.XXXXXX"; // template for our file.
  int fd = mkstemp(filename);    // Creates and opens a new temp file r/w.
  close(fd);
  std::ofstream *ofs = new std::ofstream(filename);
  m_gdev = new GdevSVG(*ofs, "SVGTEST", 0, 0, m_width, -m_height);  */
  //m_gdev = new GdevPS( ofs, "SVGTEST", 0, 0, 210, 297, "creator", "user", 1 );
  //realName, userName, pages(), orient );
  //repaint();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void iCanvas::getSize( int &w, int &h ) {
  w = m_width;
  h = m_height;
}

/* --------------------------------------------------------------------------- */
/* getOrientation --                                                                  */
/* --------------------------------------------------------------------------- */

Gdev::Orientation iCanvas::getOrientation() {
  return m_height > m_width ? Gdev::ORIENT_PORTRAIT : Gdev::ORIENT_LANDSCAPE;
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* iCanvas::myWidget() {
  return m_form;
}

/* --------------------------------------------------------------------------- */
/* setDrawingAreaSize --                                                       */
/* --------------------------------------------------------------------------- */

void iCanvas::setDrawingAreaSize( int w, int h ) {
  BUG(BugPlot,"Canvas::setDrawingAreaSize");
  //  std::cout << "  Set setDrawingAreaSize w["<<w<<"] h["<<h<<"]\n";
  if (m_darea)
    m_darea->resize(w, h);
  m_width  = w;
  m_height = h;
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* resizeCallback --                                                           */
/* --------------------------------------------------------------------------- */

void iCanvas::resizedSlot() {
  repaint();
}

/* --------------------------------------------------------------------------- */
/* repaint --                                                                  */
/* --------------------------------------------------------------------------- */

void iCanvas::repaint() {
  BUG(BugPlot,"iCanvas::repaint");
  if (!m_gdev) return;

  // get the new window size:
  QSize dsize =  m_darea->size();
  m_width = dsize.width();
  m_height = dsize.height();

  // clear the window: durch XClearArea wird automatisch ein redisplayCallback
  // ausgeloest)
  m_gdev->clearPixmap();
  QtCanvasItemList list = m_darea->allItems();
  qDeleteAll(list);

  // set the new frame factors:
  m_xmin = m_ymin = SHRT_MAX;
  m_xmax = m_ymax = 0;
  m_gdev->setCurrentArea( m_width, m_height );
  if( m_rlistener != 0 ) {
     m_gdev->setDefaultSettings();
     m_rlistener->repaint( *m_gdev );
   }
}

/* --------------------------------------------------------------------------- */
/* setPopupMenu --                                                             */
/* --------------------------------------------------------------------------- */

void iCanvas::setPopupMenu(GuiPopupMenu* menu) {
  if (m_canvasView)
    m_canvasView->setPopupMenu( menu );
}

/* --------------------------------------------------------------------------- */
/* setPopupMenu --                                                             */
/* --------------------------------------------------------------------------- */

GuiPopupMenu* iCanvas::getPopupMenu() {
  if (m_canvasView)
    return m_canvasView->getPopupMenu();
  return 0;
}
