
#include <sstream>
#include <fstream>
#include <vector>

#include "gui/GuiFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/GuiEventData.h"
#include "gui/GuiPrinterDialog.h"

#include "utils/gettext.h"
#include "utils/base64.h"

#include "plot/qt/Canvas.h"
#include "plot/qt/GdevQt.h"
#include "plot/GdevSVG.h"
#include "plot/qt/QtSimpel.h"

#include "operator/OutputChannelEvent.h"
#include "operator/InputChannelEvent.h"

INIT_LOGGER();

const int ZEICHENFELD    (1);
const int POLYGONZUG     (2);
const int ZAHLEN         (3);
const int MYTEXT           (4);
const int KREISBOGEN     (5);
const int ENDE           (6);
const int GRIECH1        (7);
const int GRIECH2        (8);
const int DREHUNG       (10);
const int MYCHAR          (14);
const int POLYGONFLAECHE(15);
const int KREISFLAECHE  (16);

const int EOS = -1;

const int    MAX_POINTS (500);
const double PI         (3.1415926);
const double DEFHUGE    (1e99);


#define SIHEAD( f, t, n ) \
   if( sihead( (f), (t), (n) ) < 0 ) return EOS
#define SISCALE( f, x, y, ax, ay, xm, ym, m, j )\
   if( siscale( (f), (x),(y),(ax),(ay),(xm),(ym),(m),(j)) < 0 ) return EOS
#define SIPOLY( f, ks, kp, ir, m, x, y ) \
   if( sipoly( (f), (ks), (kp), (ir), (m), (x), (y) ) < 0 ) return EOS
#define SINUM( f, x, y, r, i, j, n, m, h, w, k ) \
   if( sinum( (f), (x), (y), (r), (i), (j), (n), (m), (h), (w), (k) ) < 0 ) return EOS
#define SITEXT( f, x, y, h, w, k, m, t ) \
   if( sitext( (f), (x), (y), (h), (w), (k), (m), (t) ) < 0 ) return EOS
#define SIARC( f, x, y, r, a, e, s, p ) \
   if( siarc( (f), (x), (y), (r), (a), (e), (s), (p) ) < 0 ) return EOS
#define SIARCF( f, x, y, r, a, e, p, i, l, d ) \
   if( siarcf( (f), (x), (y), (r), (a), (e), (p), (i), (l), (d) ) < 0 ) return EOS
#define SIEND( f, n )\
   if( siend( (f), (n) ) < 0 ) return EOS
#define SIGREEK( f, x, y, l, h, p ) \
   if( sigreek( (f), (x), (y), (l), (h), (p) ) < 0 ) return EOS
#define SIROT( f, x, y, w, sx, sy )\
   if( sirot( (f), (x), (y), (w), (sx), (sy) ) < 0 ) return EOS
#define SICHAR( f, x, y, h, b, a, w, k, m, t ) \
   if( sichar( (f), (x), (y), (h), (b), (a), (w), (k), (m), (t) ) < 0 ) return EOS
#define SIPOLYF( f, ks, i, kp, d, ir, m, x, y ) \
   if( sipolyf( (f), (ks), (i), (kp), (d), (ir), (m), (x), (y) ) < 0 ) return EOS

#ifdef MIN
#undef MIN
#endif

#define MIN( a, b ) ((int)(a) < (int)(b) ? (int)(a) : (int)(b))

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* QtSimpel --                                                                   */
/* --------------------------------------------------------------------------- */

QtSimpel::QtSimpel( GuiElement *parent, const std::string &name )
  : BasicStream( name )
  //  , GuiElement( parent )
  , QtListPlot( parent, name )
  , m_name( name )
  , m_title( "" )
  , m_endPage( true )
  , m_xoffset( 0.0 )
  , m_xmax( 0.0 )
  , m_xmin( 0.0 )
  , m_ymax( 0.0 )
  , m_ymin( 0.0 )
  , m_tx0( 0.0 )
  , m_ty0( 0.0 )
  , m_tamx( 0.0 )
  , m_tamy( 0.0 )
  , m_txmax( 0.0 )
  , m_tymax( 0.0 )
  , m_height( 400 )
  , m_width( 600 )
  , m_currentFileFormat( HardCopyListener::Postscript )
  , m_popUpMenu( this )
  , m_currentPage( 1 )
  , m_textStretch( 1 ){

  installData();
  m_supportedFileFormats.push_back( HardCopyListener::HPGL );
  m_supportedFileFormats.push_back( HardCopyListener::Postscript );
  m_supportedFileFormats.push_back( HardCopyListener::PDF );
  m_supportedFileFormats.push_back( HardCopyListener::PNG );
  m_supportedFileFormats.push_back( HardCopyListener::GIF );
  m_supportedFileFormats.push_back( HardCopyListener::JPEG );
  stretchText( 0.7 );
}

/* --------------------------------------------------------------------------- */
/* ~QtSimpel --                                                                  */
/* --------------------------------------------------------------------------- */

QtSimpel::~QtSimpel() {
}

/* --------------------------------------------------------------------------- */
/* Type --                                                                     */
/* --------------------------------------------------------------------------- */

GuiElement::ElementType QtSimpel::Type() {
  return GuiElement::type_Simpel;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void QtSimpel::manage() {
  if (myWidget())
    myWidget()->show();
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void QtSimpel::create() {
//   m_widget = XtVaCreateWidget( "ScrolledWindow"
// 			       , xmScrolledWindowWidgetClass
// 			       , parent
// 			       , XmNscrollingPolicy, XmAUTOMATIC
// 			       , XmNwidth,          m_width
// 			       , XmNheight,         m_height
// 			       , NULL );

   m_canvas->create();
   m_popUpMenu.create();

//   XtVaSetValues( m_widget, XmNuserData, this, 0 );
//   XtManageChild( m_widget );
   setPage( m_canvas->gdev() );
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void QtSimpel::getSize( int &width, int &height ) {
  width = m_width;
  height = m_height;

}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void QtSimpel::update( UpdateReason ur ) {
  BUG(BugPlot,"QtSimpel::update");
  if( ur == reason_Cancel ){
    return;
  }
  if( BasicStream::isDataItemUpdated( GuiQtManager::Instance().LastGuiUpdate() )
      || ur == reason_Cycle
      || ur == reason_Always ){
    m_canvas->repaint();
  }
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* QtSimpel::myWidget() {
  assert( m_canvas != 0 );
  return m_canvas->myWidget();
}

/* --------------------------------------------------------------------------- */
/* getFileFormat --                                                            */
/* --------------------------------------------------------------------------- */
HardCopyListener::FileFormat QtSimpel::getFileFormat() {
  return m_currentFileFormat;
}

/* --------------------------------------------------------------------------- */
/* MenuLabel --                                                                */
/* --------------------------------------------------------------------------- */
const std::string &QtSimpel::MenuLabel() {
  return m_name;
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool QtSimpel::isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::FileFormats::iterator it;
  bool result = false;
  for( it = m_supportedFileFormats.begin(); it != m_supportedFileFormats.end() && !result; ++it )
    if( (*it) == fileFormat )
      result = true;
  return result;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool QtSimpel::write( InputChannelEvent &event ) {
  BUG(BugPlot,"QtSimpel::write");
  return QtListPlot::write( event );
}

/* --------------------------------------------------------------------------- */
/* refresh --                                                                  */
/* --------------------------------------------------------------------------- */

void QtSimpel::refresh( Gdev &gdev ) {
  BUG(BugPlot,"QtSimpel::refresh");
  gdev.copyPixmapIntoDarea();
}

/* --------------------------------------------------------------------------- */
/* repaint --                                                                  */
/* --------------------------------------------------------------------------- */

void QtSimpel::repaint( Gdev &gdev, bool allPages ) {
  BUG(BugPlot,"QtSimpel::repaint");
  std::string s;
  int pages = allPages ? m_pageCount : 1;
  int firstPage = allPages ? 1 : m_currentPage;
  int lastPage = firstPage + pages -1;
  setPage( gdev );
  gdev.stretchText( m_textStretch );
  int i;
  for( i = firstPage; i <= lastPage; ++i ){
    if( readBuffer( s, i - 1 ) ){
      BUG_MSG("readBuffer " << s.size() << " bytes");
      m_plotIter = s.begin();
      plotSimpelFile( gdev, s );
      if( pages > 1 )
	newPage( gdev );
    }
  }
  endPlot( gdev );
}

/* --------------------------------------------------------------------------- */
/* stretchText --                                                              */
/* --------------------------------------------------------------------------- */

void QtSimpel::stretchText( double stretch ){
  m_textStretch = stretch;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool QtSimpel::read( OutputChannelEvent &event ){
  BUG(BugPlot,"QtSimpel::read");

  int idx = 0;
  m_pageCount = 0;
  std::string s = event.getStreamBuffer();
  m_plotIter = m_startPlotIter = s.begin();
  BUG_MSG("read " << s.size() << " bytes");
  m_xmax = 0.0;
  m_xmin = 0.0;
  m_ymax = 0.0;
  m_ymin = 0.0;
  plotSimpelFile( s );
  setDataItemsValid();
  return true;
}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void QtSimpel::clearRange(){
  BUG(BugPlot,"QtSimpel::clearRange");
  setDataItemsInvalid();
  m_canvas->gdev().clear();
}

/* --------------------------------------------------------------------------- */
/* plotSimpelFile --                                                           */
/* --------------------------------------------------------------------------- */

void QtSimpel::plotSimpelFile( Gdev &gdev, std::string &s ) {
  BUG(BugPlot,"QtSimpel::plotSimpelFile");

  gdev.setLinewidth( 1 );
  gdev.setMarkerSymbol( 2 );
  m_endPage = false;

  executePlot( gdev, s );
}

void QtSimpel::plotSimpelFile( std::string &s ) {
  m_endPage = false;
  executePlot( s );
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* executePlot --                                                              */
/* --------------------------------------------------------------------------- */

void QtSimpel::executePlot( Gdev &gdev, std::string &s ){
  BUG(BugPlot,"QtSimpel::executePlot");
  int rslt;
  do{
  rslt = plotSimpel( gdev, s );
  }
  while( rslt != EOS );
}

void QtSimpel::executePlot( std::string &s ){
  int rslt;
  do{
  rslt = plotSimpel( s );
  }
  while( rslt != EOS );
}

/* --------------------------------------------------------------------------- */
/* endPlot --                                                                  */
/* --------------------------------------------------------------------------- */

void QtSimpel::endPlot( Gdev &gdev ) {
  BUG(BugPlot,"QtSimpel::endPlot");
  gdev.finallyWork();
  gdev.copyPixmapIntoDarea();
}

/* --------------------------------------------------------------------------- */
/*  setXMinMax --                                                              */
/* --------------------------------------------------------------------------- */

void QtSimpel::setXMinMax( double x ) {
  if( x > m_xmax )
    m_xmax = x;
  if( x < m_xmin )
    m_xmin = x;
}

/* --------------------------------------------------------------------------- */
/* setYMinMax --                                                               */
/* --------------------------------------------------------------------------- */

void QtSimpel::setYMinMax( double y ) {
  if( y > m_ymax )
    m_ymax = y;
  if( y < m_ymin )
    m_ymin = y;
}

/* --------------------------------------------------------------------------- */
/* plotSimpel --                                                               */
/* --------------------------------------------------------------------------- */

int QtSimpel::plotSimpel( Gdev &gdev, std::string &s ) {

  FLONG   type;
  FLONG   n, i, j, k, m, ia, lstyle, ks, kp, ir, jr, kl, mr, kd, kf;
  float xt, yt, hs, ws, ra, r, xm, ym, ag, eg, bs, as;
  float xh, yl, xl, yh;
  float xk, yk, dx, dy,wd, sfx, sfy;
  char buf[BUFSIZ];
  float x[MAX_POINTS], y[MAX_POINTS];
  std::vector<double> xx, yy;
  char c;

  SIHEAD( s, &type, &k );

  if( m_endPage && (type != ENDE) ) {
    m_endPage = false;
  }

  switch( static_cast<int>(type) ){

  case ZEICHENFELD:

    SISCALE( s, &m_tx0, &m_ty0, &m_tamx, &m_tamy, &m_txmax, &m_tymax, &mr, &jr );

    m_tx0 = m_tx0 + m_xoffset;
    if( mr > 1 ){
      gdev.mark( m_tx0, m_ty0 );
      setXMinMax( m_tx0 );
      setYMinMax( m_ty0 );
    }
    return 1;

  case POLYGONZUG:

    n = MIN( (k-3)/2, MAX_POINTS );
    SIPOLY( s, &ks, &kp, &ir, &n, x, y );

    lstyle = (int)ks-1;
    for( i=0; i < n; i++ ){
      xx.push_back( x[i] * m_tamx + m_tx0 );
      yy.push_back( y[i] * m_tamy + m_ty0 );
      setXMinMax( xx[i] );
      setYMinMax( yy[i] );
    }
    if( lstyle >= 0 ){
      gdev.setLinestyle( lstyle );
      if( kp > 1 )
	gdev.setColor( kp-1 );
      gdev.drawLines( xx, yy );
      if( kp > 1 )
	gdev.setColor( 0 );
    }
    else
      gdev.moveTo( xx[i-1], yy[i-1] );

    return 1;

  case POLYGONFLAECHE:

    n = MIN( (k-3)/2, MAX_POINTS );
    SIPOLYF( s, &ks, &kf, &kp, &kd, &ir, &n, x, y );

    lstyle = (int)ks-1;
    for( i=0; i < n; i++ ){
      xx.push_back( x[i]*m_tamx + m_tx0 );
      setXMinMax( xx[i] );
    }
    for( i=0; i < n; i++ ){
      yy.push_back( y[i]*m_tamy + m_ty0 );
      setYMinMax( yy[i] );
    }
    if( lstyle >= 0 ){
      gdev.setLinestyle( lstyle );
      if( kp > 1 )
	gdev.setColor( kp-1 );
      if( n > 1 ){
	gdev.setEdgeStyle( 1 );
	gdev.setFillColor( 1 );

	gdev.drawPolygone( xx, yy );
      }
      else
	gdev.drawTo( xx[0], yy[0] );
      if( kp > 1 )
        gdev.setColor( 0 );
    }
    else
      gdev.moveTo( xx[i-1], yy[i-1] );
    return 1;

  case ZAHLEN:

    SINUM( s, &xt, &yt, &ra, &ia, &j, &n, &m, &hs, &ws, &kp );

    switch( j ){
    case 1:
      sprintf( buf, "%*.*f", (int)n+1, (int)m, ra );
      break;
    case 2:
      sprintf( buf, "%*.*e", (int)n+1, (int)m, ra );
      break;
    case 3:
      sprintf( buf, "%*d", (int)n+1, (int)ia );
      break;
    default:
      break;
    }
    if( ( j > 0 ) && ( j < 4 ) ) {
      gdev.setTextAttributes( hs/1.5, Gdev::HLEFT, Gdev::VBOTTOM, ws, 0 );
      if (gdev.type() == Gdev::type_Svg) {
        x[0] = (xt + 1.05*m_tx0 -hs - gdev.qx())/gdev.tx();
        y[0] = (-yt + -m_ty0 - gdev.qy() + gdev.getHeight())/gdev.ty();
      } else {
        x[0] = xt + m_tx0 - hs;
        y[0] = yt + m_ty0;
      }
      putString( gdev, x[0], y[0], ws, hs, n+1, buf );
    }
    return 1;

  case MYTEXT:

    SITEXT( s, &xt, &yt, &hs, &ws, &kp, &m, buf );
    buf[m]='\0';
    gdev.setTextAttributes( hs/1.5, Gdev::HLEFT, Gdev::VBOTTOM, ws, 0 );
    if (gdev.type() == Gdev::type_Svg) {
      x[0] = (xt + m_tx0 - gdev.qx())/gdev.tx();
      y[0] = (-yt + -m_ty0 - gdev.qy() + gdev.getHeight())/gdev.ty();
    } else {
      x[0] = m_tx0 + xt;
      y[0] = m_ty0 + yt;
    }

    putString( gdev, x[0], y[0], ws, hs, m, buf );
    return 1;

  case MYCHAR:

    SICHAR( s, &xt, &yt, &hs, &bs, &as, &ws, &kp, &m, buf);
    /* Zeichenbreite: bs*hs */
    /* as: Spacing */
    gdev.setTextAttributes( hs/1.5, Gdev::HLEFT, Gdev::VBOTTOM, ws, 0 );
    if (gdev.type() == Gdev::type_Svg) {
      x[0] = (xt + 1.05*m_tx0 - gdev.qx())/gdev.tx();
      y[0] = (-yt + -m_ty0 - gdev.qy() + gdev.getHeight())/gdev.ty();
    } else {
      x[0] = m_tx0 + xt;
      y[0] = m_ty0 + yt;
    }
    putString( gdev, x[0], y[0], ws, hs, m, buf );
    return 1;

  case KREISBOGEN:

    SIARC( s, &xm, &ym, &r, &ag, &eg, &ks, &kp );

    lstyle = ks-1;
    x[0] = ( m_tamx*xm + m_tx0 );
    y[0] = ( m_tamy*ym + m_ty0 );
    gdev.setLinestyle( lstyle );
    gdev.drawArc( x[0], y[0]       // center
		  , 2*r*m_tamx, 2*r*m_tamy // major and minor axes
		  , ag, eg );        // angles

    xl = x[0]-r*m_tamx;
    xh = x[0]+r*m_tamx;
    yl = y[0]-r*m_tamy;
    yh = y[0]+r*m_tamy;
    if( yh > m_ymax )
      m_ymax = yh;
    if( yl < m_ymin )
      m_ymin = yl;
    if( xh > m_xmax )
      m_xmax = xh;
    if( xl < m_xmin )
      m_xmin = xl;
    return 1;

  case KREISFLAECHE:

    SIARCF( s, &xm, &ym, &r, &ag, &eg, &kp, &kf, &kl, &kd );

    lstyle = kf-1;
    x[0] = ( m_tamx*xm + m_tx0 );
    y[0] = ( m_tamy*ym + m_ty0 );
    gdev.setLinestyle( lstyle );
    gdev.drawArc( x[0], y[0]       // center
		  , 2*r*m_tamx, 2*r*m_tamy // major and minor axes
		  , ag, eg );        // angles

    xl = x[0]-r*m_tamx;
    xh = x[0]+r*m_tamx;
    yl = y[0]-r*m_tamy;
    yh = y[0]+r*m_tamy;
    if( yh > m_ymax )
      m_ymax = yh;
    if( yl < m_ymin )
      m_ymin = yl;
    if( xh > m_xmax )
      m_xmax = xh;
    if( xl < m_xmin )
      m_xmin = xl;
    return 1;

  case ENDE:
    SIEND( s, &n );
    switch( n ){
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    default:
      if( n > 4 ){
      }
    }
    if( m_endPage )
      return 1;

    m_endPage = true;
    gdev.endPage();
    return 1;

  case GRIECH1:
  case GRIECH2:

    SIGREEK( s, &xk, &yk, &kl, &hs, &kp );
    return 1;

  case DREHUNG:
    SIROT( s, &dx, &dy, &wd, &sfx, &sfy );
    return 1;

  default:
    while( streamRead( &c, 1, 1, s )==true );
  }
  return EOS;
}

int QtSimpel::plotSimpel( std::string &s ) {
  FLONG   type;
  FLONG   n, i, j, k, m, ia, lstyle, ks, kp, ir, jr, kl, mr, kd, kf;
  float xt, yt, hs, ws, ra, r, xm, ym, ag, eg, bs, as;
  float xh, yl, xl, yh;
  float xk, yk, dx, dy,wd, sfx, sfy;
  char buf[BUFSIZ];
  float x[MAX_POINTS], y[MAX_POINTS];
  std::vector<double> xx, yy;
  char c;

  SIHEAD( s, &type, &k );

  if( m_endPage && (type != ENDE) ) {
    m_endPage = false;
  }

  switch( static_cast<int>(type) ){

  case ZEICHENFELD:
    SISCALE( s, &m_tx0, &m_ty0, &m_tamx, &m_tamy, &m_txmax, &m_tymax, &mr, &jr );
    m_tx0 = m_tx0 + m_xoffset;
    if( mr > 1 ){
      setXMinMax( m_tx0 );
      setYMinMax( m_ty0 );
    }
    return 1;

  case POLYGONZUG:
    n = MIN( (k-3)/2, MAX_POINTS );
    SIPOLY( s, &ks, &kp, &ir, &n, x, y );

    lstyle = (int)ks-1;
    for( i=0; i < n; i++ ){
      xx.push_back( x[i] * m_tamx + m_tx0 );
      yy.push_back( y[i] * m_tamy + m_ty0 );
      setXMinMax( xx[i] );
      setYMinMax( yy[i] );
    }
    return 1;

  case POLYGONFLAECHE:
    n = MIN( (k-3)/2, MAX_POINTS );
    SIPOLYF( s, &ks, &kf, &kp, &kd, &ir, &n, x, y );

    lstyle = (int)ks-1;
    for( i=0; i < n; i++ ){
      xx.push_back( x[i]*m_tamx + m_tx0 );
      setXMinMax( xx[i] );
    }
    for( i=0; i < n; i++ ){
      yy.push_back( y[i]*m_tamy + m_ty0 );
      setYMinMax( yy[i] );
    }
    return 1;

  case ZAHLEN:
    SINUM( s, &xt, &yt, &ra, &ia, &j, &n, &m, &hs, &ws, &kp );
    return 1;

  case MYTEXT:
    SITEXT( s, &xt, &yt, &hs, &ws, &kp, &m, buf );
    return 1;

  case MYCHAR:
    SICHAR( s, &xt, &yt, &hs, &bs, &as, &ws, &kp, &m, buf);
    return 1;

  case KREISBOGEN:
    SIARC( s, &xm, &ym, &r, &ag, &eg, &ks, &kp );

    lstyle = ks-1;
    x[0] = ( m_tamx*xm + m_tx0 );
    y[0] = ( m_tamy*ym + m_ty0 );

    xl = x[0]-r*m_tamx;
    xh = x[0]+r*m_tamx;
    yl = y[0]-r*m_tamy;
    yh = y[0]+r*m_tamy;
    if( yh > m_ymax )
      m_ymax = yh;
    if( yl < m_ymin )
      m_ymin = yl;
    if( xh > m_xmax )
      m_xmax = xh;
    if( xl < m_xmin )
      m_xmin = xl;
    return 1;

  case KREISFLAECHE:
    SIARCF( s, &xm, &ym, &r, &ag, &eg, &kp, &kf, &kl, &kd );

    lstyle = kf-1;
    x[0] = ( m_tamx*xm + m_tx0 );
    y[0] = ( m_tamy*ym + m_ty0 );

    xl = x[0]-r*m_tamx;
    xh = x[0]+r*m_tamx;
    yl = y[0]-r*m_tamy;
    yh = y[0]+r*m_tamy;
    if( yh > m_ymax )
      m_ymax = yh;
    if( yl < m_ymin )
      m_ymin = yl;
    if( xh > m_xmax )
      m_xmax = xh;
    if( xl < m_xmin )
      m_xmin = xl;
    return 1;

  case ENDE:{
    SIEND( s, &n );
    writeBuffer( std::string( m_startPlotIter, m_plotIter ), m_pageCount );
    m_startPlotIter = m_plotIter;
    ++m_pageCount;
    if( m_endPage )
      return 1;
    m_endPage = true;
    return 1;
  }

  case GRIECH1:
  case GRIECH2:
    SIGREEK( s, &xk, &yk, &kl, &hs, &kp );
    return 1;

  case DREHUNG:
    SIROT( s, &dx, &dy, &wd, &sfx, &sfy );
    return 1;

  default:
    while( streamRead( &c, 1, 1, s )==true );
  }
  return EOS;
}

/* --------------------------------------------------------------------------- */
/* putString --                                                                */
/* --------------------------------------------------------------------------- */

void QtSimpel::putString( Gdev &gdev, double x, double y
		      , double angle, double hs
		      , int len, char* buf ) {
  if (gdev.type() == Gdev::type_Svg) {
    gdev.moveTo(x, y);
    gdev.putText( buf );
  } else {
    int i;
    double cosx, sinx, a;

    a = 2*PI/360*angle;
    cosx = cos( a );
    sinx = sin( a );
    for( i=0; i < len; i++ ) {
      if( !isspace(buf[i]) ) {
        gdev.moveTo( x, y + 0.02 * hs );
        std::ostringstream tt;
        tt << buf[i];
        gdev.putText( tt.str() );
      }
      x += cosx*hs;
      y += sinx*hs;
    }
  }
  setXMinMax( x );
  setYMinMax( y );
}

/* --------------------------------------------------------------------------- */
/* newPage --                                                                  */
/* --------------------------------------------------------------------------- */

void QtSimpel::newPage( Gdev &gdev ) {
  BUG(BugPlot,"QtSimpel::newPage");
  double x0, x1, y0, y1;
  x0 = m_xmin;
  x1 = m_xmax;
  y0 = m_ymin;
  y1 = m_ymax;

  gdev.setWindow( x0, y0, x1, y1 );

  x0 = -0.99;
  x1 = 0.99;
  y0 = -0.99;
  y1 = 0.99;

  gdev.setViewport( x0, y0, x1, y1 );
}

/* --------------------------------------------------------------------------- */
/* setPage --                                                                  */
/* --------------------------------------------------------------------------- */

bool QtSimpel::setPage( Gdev &gdev ) {
  BUG(BugPlot,"QtSimpel::setPage");
  assert( m_canvas != 0 );
  double dx, dy;
  double pixelsize = 1; //72 / 25.4;
  dx = (m_xmax - m_xmin);
  dy = (m_ymax - m_ymin);
  if( ( dx <= 0 ) || ( dy <= 0 ) )
    return false;
  m_canvas->setDrawingAreaSize( (int)(dx * pixelsize), (int)(dy * pixelsize) );
  gdev.setWindow( m_xmin, m_ymin
		  , m_xmax, m_ymax );
  gdev.setViewport( -0.99, -0.99, 0.99, 0.99 );
  return true;
}

/* --------------------------------------------------------------------------- */
/* siopen --                                                                   */
/* --------------------------------------------------------------------------- */

void QtSimpel::siopen() {
  /* do nothing */
}

/* --------------------------------------------------------------------------- */
/* sihead --                                                                   */
/* --------------------------------------------------------------------------- */

int QtSimpel::sihead( std::string &s, FLONG *type, FLONG *num ) {
  int reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( type, s ) )
    return( -1 );
  if( !get( num, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 2 );
}

/* --------------------------------------------------------------------------- */
/* siscale --                                                                  */
/* --------------------------------------------------------------------------- */

int QtSimpel::siscale( std::string &s, float *x0, float *y0, float *amx, float *amy
		   , float *xmax, float *ymax, FLONG *mr, FLONG *jr ) {
  int reclen;
  FLONG  idumy, kxh;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( x0, s ) )
    return( -1 );
  if( !get( y0, s ) )
    return( -1 );
  if( !get( amx, s ) )
    return( -1 );
  if( !get( amy, s ) )
    return( -1 );
  if( !get( xmax,  s ) )
    return( -1 );
  if( !get( ymax,  s ) )
    return( -1 );
  if( !get( mr, s ) )
    return( -1 );
  if( !get( jr, s ) )
    return( -1 );
  if( !get( &idumy, s ) )
    return( -1 );
  if( !get( &kxh, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 8 );
}

/* --------------------------------------------------------------------------- */
/* sipoly --                                                                   */
/* --------------------------------------------------------------------------- */

int QtSimpel::sipoly( std::string &s, FLONG *ks, FLONG *kp, FLONG *ir, FLONG *m
		  , float *x, float *y ) {
  int i, reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( ks, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( ir, s ) )
    return( -1 );
  for( i=0; i<*m; i++ )
    if( !get( &x[i], s ) )
      return( -1 );
  for( i=0; i<*m; i++ )
    if( !get( &y[i], s ) )
      return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 3 + *m * 2 );
}

/* --------------------------------------------------------------------------- */
/* sipolyf --                                                                  */
/* --------------------------------------------------------------------------- */

int QtSimpel::sipolyf( std::string &s, FLONG *ks, FLONG *kf, FLONG *kp, FLONG *kd, FLONG *ir
		   , FLONG *m, float *x, float *y ) {
  int i, reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( ks, s ) )
    return( -1 );
  if( !get( kf, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( kd, s ) )
    return( -1 );
  if( !get( ir, s ) )
    return( -1 );
  for( i=0; i<*m; i++ )
    if( !get( &x[i], s ) )
      return( -1 );
  for( i=0; i<*m; i++ )
    if( !get( &y[i], s ) )
      return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 3 + *m * 2 );
}

/* --------------------------------------------------------------------------- */
/* sinum --                                                                    */
/* --------------------------------------------------------------------------- */

int QtSimpel::sinum( std::string &s, float *xt, float *yt, float *ra
		 , FLONG *ia, FLONG *j, FLONG *n, FLONG *m, float *hs, float *ws
		 , FLONG *kp ) {
  int reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( xt, s ) )
    return( -1 );
  if( !get( yt, s ) )
    return( -1 );
  if( !get( ra, s ) )
    return( -1 );
  if( !get( ia, s ) )
    return( -1 );
  if( !get( j, s ) )
    return( -1 );
  if( !get( n, s ) )
    return( -1 );
  if( !get( m, s ) )
    return( -1 );
  if( !get( hs, s ) )
    return( -1 );
  if( !get( ws, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 10 );
}

/* --------------------------------------------------------------------------- */
/* sitext --                                                                   */
/* --------------------------------------------------------------------------- */

int QtSimpel::sitext( std::string &s, float *xt, float *yt, float *hs, float *ws
		  , FLONG *kp, FLONG *m, char *text ) {
  int reclen, l;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( xt, s ) )
    return( -1 );
  if( !get( yt, s ) )
    return( -1 );
  if( !get( hs, s ) )
    return( -1 );
  if( !get( ws, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( m, s ) )
    return( -1 );
  l = reclen - 4*sizeof( float ) - 2*sizeof( FLONG );
  if( !get( text, l, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 7 );
}

/* --------------------------------------------------------------------------- */
/* siarc --                                                                    */
/* --------------------------------------------------------------------------- */

int QtSimpel::siarc( std::string &s, float *xm, float *ym, float *r, float *ag, float *eg
		 , FLONG *ks, FLONG *kp ) {
  int reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( xm, s ) )
    return( -1 );
  if( !get( ym, s ) )
    return( -1 );
  if( !get( r, s ) )
    return( -1 );
  if( !get( ag, s ) )
    return( -1 );
  if( !get( eg, s ) )
    return( -1 );
  if( !get( ks, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 7 );
}

/* --------------------------------------------------------------------------- */
/* siarcf --                                                                   */
/* --------------------------------------------------------------------------- */

int QtSimpel::siarcf( std::string &s, float *xm, float *ym, float *r, float *ag, float *eg
		  , FLONG *kp, FLONG *kf, FLONG *kl, FLONG *kd ) {
  int reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( xm, s ) )
    return( -1 );
  if( !get( ym, s ) )
    return( -1 );
  if( !get( r, s ) )
    return( -1 );
  if( !get( ag, s ) )
    return( -1 );
  if( !get( eg, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( kf, s ) )
    return( -1 );
  if( !get( kl, s ) )
    return( -1 );
  if( !get( kd, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 9 );
}

/* --------------------------------------------------------------------------- */
/* siend --                                                                    */
/* --------------------------------------------------------------------------- */

int QtSimpel::siend( std::string &s, FLONG *n ) {
  BUG(BugPlot,"QtSimpel::siend");
  int reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( n, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return 1 ;
}

/* --------------------------------------------------------------------------- */
/* sigreek --                                                                  */
/* --------------------------------------------------------------------------- */

int QtSimpel::sigreek( std::string &s, float *xk, float *yk, FLONG *kl, float *hs, FLONG *kp ) {
  int reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( xk, s ) )
    return( -1 );
  if( !get( yk, s ) )
    return( -1 );
  if( !get( kl, s ) )
    return( -1 );
  if( !get( hs, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 5 );
}

/* --------------------------------------------------------------------------- */
/* sirot --                                                                    */
/* --------------------------------------------------------------------------- */

int QtSimpel::sirot( std::string &s, float *dx, float *dy, float *wd, float *sfx, float *sfy ) {
  int reclen;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( dx, s ) )
    return( -1 );
  if( !get( dy, s ) )
    return( -1 );
  if( !get( wd, s ) )
    return( -1 );
  if( !get( sfx, s ) )
    return( -1 );
  if( !get( sfy, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 5 );
}

/* --------------------------------------------------------------------------- */
/* sichar --                                                                   */
/* --------------------------------------------------------------------------- */

int QtSimpel::sichar( std::string &s, float *xt, float *yt, float *hs, float *bs
		  , float *as, float *ws, FLONG *kp, FLONG *m, char *text ) {
  int reclen, l;

  if( !get( &reclen, s ) )
    return( -1 );
  if( !get( xt, s ) )
    return( -1 );
  if( !get( yt, s ) )
    return( -1 );
  if( !get( hs, s ) )
    return( -1 );
  if( !get( bs, s ) )
    return( -1 );
  if( !get( as, s ) )
    return( -1 );
  if( !get( ws, s ) )
    return( -1 );
  if( !get( kp, s ) )
    return( -1 );
  if( !get( m, s ) )
    return( -1 );
  l = reclen - 4*sizeof( float ) - 2*sizeof( FLONG );
  if( !get( text, *m, s ) )
    return( -1 );
  if( !get( &reclen, s ) )
    return( -1 );
  return( 9 );
}

/* --------------------------------------------------------------------------- */
/* streamRead --                                                               */
/* --------------------------------------------------------------------------- */

bool QtSimpel::streamRead(  char *c, int size, int n, std::string &s ){
  for( int i = 0; i < ( n*size ); ++i ){
    if( m_plotIter == s.end() ){
      return false;
    }
    *c = *m_plotIter;
    ++c;
    ++m_plotIter;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* get --                                                                      */
/* --------------------------------------------------------------------------- */

bool QtSimpel::get( int *ip, std::string &s ){
  return streamRead( reinterpret_cast<char *>(ip), sizeof(int), 1, s );
}
bool QtSimpel::get( float *fp, std::string &s ){
  return streamRead( reinterpret_cast<char *>(fp), sizeof(float), 1, s );
}
bool QtSimpel::get( char *cp, int len, std::string &s ){
  return streamRead( cp, sizeof( char ), len, s );
}

/* --------------------------------------------------------------------------- */
/* nextPage --                                                                 */
/* --------------------------------------------------------------------------- */

void QtSimpel::nextPage(){
  if( m_currentPage >= m_pageCount )
    return;
  ++m_currentPage;
  m_canvas->repaint();
}

/* --------------------------------------------------------------------------- */
/* prevPage --                                                                 */
/* --------------------------------------------------------------------------- */

void QtSimpel::prevPage(){
  if( m_currentPage <= 1 )
    return;
  --m_currentPage;
  m_canvas->repaint();
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void QtSimpel::PopUpMenu::create() {
//   MultiLanguage &mls = MultiLanguage::Instance();
  GuiFactory *factory = GuiFactory::Instance();

  m_popUpMenu = new GuiQtPopupMenu( m_simpel );

  GuiQtLabel *menu_title = new GuiQtLabel( m_popUpMenu );
  menu_title->setAlignment( GuiElement::align_Default );
  menu_title->setLabel( "Uniplot Window" );
  m_popUpMenu->attach(menu_title);

  m_popUpMenu->attach( new GuiQtSeparator( m_popUpMenu ) );

  GuiEventData *event = new GuiEventData();
  m_nextPageBtn = new GuiQtMenuButton( m_popUpMenu, this, event );
  m_popUpMenu->attach( m_nextPageBtn );
  m_nextPageBtn->setDialogLabel( _("Next page") );

  event = new GuiEventData();
  m_prevPageBtn = new GuiQtMenuButton( m_popUpMenu, this, event );
  m_popUpMenu->attach( m_prevPageBtn );
  m_prevPageBtn->setLabel( _("Previous page") );

  if (!AppData::Instance().HeadlessWebMode()) {
    event = new GuiEventData();
    m_printPageBtn = new GuiQtMenuButton( m_popUpMenu, this, event );
    m_popUpMenu->attach( m_printPageBtn );
    m_printPageBtn->setLabel( _("Print") );
  }

  m_popUpMenu->create(); //w
  m_simpel->m_canvas->setPopupMenu( m_popUpMenu );
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void QtSimpel::PopUpMenu::ButtonPressed( GuiEventData *event ){
  if( event == 0 )
    return;
  GuiElement *element = event->m_element;
  if( element == 0 )
    return;
  else if( element == m_nextPageBtn->getElement() )
    m_simpel->nextPage();
  else if( element == m_prevPageBtn->getElement() )
    m_simpel->prevPage();
  else if( element == m_printPageBtn->getElement() ){
    GuiPrinterDialog::MyEventData event( ReportGen::PRINT );
    GuiFactory::Instance()->createPrinterDialog()->showDialog( m_simpel, m_simpel, &event );
  }
  else
    assert( false );
}
