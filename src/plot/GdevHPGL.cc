
#include <stdio.h>
#include <math.h>
#include <string>
#include <iomanip>
#include "plot/GdevHPGL.h"
#include "utils/Debugger.h"


// initialization string
const std::string GdevHPGL::INIT_STRING = "IN;IP0,0,2000,2000;SP1;LT;PU;PA;\n";

static const double PIXELSIZE = 0.025;
static const double PI = 3.1415926;
static const char ETX = static_cast<char>(3);

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* GdevHPGL --                                                                 */
/* --------------------------------------------------------------------------- */

GdevHPGL::GdevHPGL( std::ostream &ostr, const std::string& title,
		    double x0, double y0, double x1, double y1,
		    const std::string &creator, const std::string &user, int pages, Orientation orient )
  : m_hjust( HLEFT )
  , m_vjust( VBOTTOM )
  , m_xsiz( 0.01 )
  , m_ysiz( 0.01 )
  , m_penup( true )
  , m_cmarker( '+' )
  , m_orient( orient )
  , m_ostr( ostr )
  , m_pageCount( pages )
  , m_currentPage( 1 ) {
  invert( false );
  setViewport( -1.0, -1.0, 1.0, 1.0 );
  setCurrentArea( (x1-x0) / PIXELSIZE, (y1-y0) / PIXELSIZE );
  setWindow( 0.0, 0.0, (x1-x0) / PIXELSIZE, (y1-y0) / PIXELSIZE );
  calcTransParam();
  m_ostr.setf(std::ios::fixed);
  m_ostr << INIT_STRING;
}

/* --------------------------------------------------------------------------- */
/* ~GdevHPGL --                                                                */
/* --------------------------------------------------------------------------- */

GdevHPGL::~GdevHPGL() {

}

/* --------------------------------------------------------------------------- */
/* setLinewidth --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setLinewidth( int lwidth ) {

}

/* --------------------------------------------------------------------------- */
/* setLinestyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setLinestyle( int lstyle ) {
  if( lstyle ) {
    double pct;

    switch( lstyle ) {
    case 1:
      pct = 0.8;
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      pct = 4.0;
      break;
    default:
      lstyle = 2;
      pct = 1.0;
      break;
    }
    m_ostr << "LT" << lstyle << "," << pct << ";";
  }
  else
    m_ostr << "LT;";
}

/* --------------------------------------------------------------------------- */
/* setTextSize --                                                              */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setTextSize( double size ) {

}

/* --------------------------------------------------------------------------- */
/* setTextHjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setTextHjust( HJust hjust ) {

}

/* --------------------------------------------------------------------------- */
/* setTextVjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setTextVjust( VJust vjust ) {

}

/* --------------------------------------------------------------------------- */
/* setTextAngle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setTextAngle( double angle ) {

}

/* --------------------------------------------------------------------------- */
/* setTextFont --                                                              */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setTextFont( int fontno ) {

}

/* --------------------------------------------------------------------------- */
/* setTextAttributes --                                                        */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setTextAttributes( double size, HJust hjust, VJust vjust
				  , double angle, int fno ) {
  BUG(BugPlot,"GdevHPGL::setTextAttributes");
  BUG_MSG( "size = " << size );
  double a = PI*angle/180;
  m_hjust = hjust;
  m_vjust = vjust;
  m_xsiz = size;
  m_ysiz = size;
  if( angle != 0.0 )
    m_ostr << "DI" << cos(a) << "," << sin(a) << ";";
  else
    m_ostr << "DI1,0;";
  // m_xsiz*getTTextX() = Zeichengroesse PIXEL.
  // Umrechnung in cm mit Korrekturfaktor 0.8
  m_ostr << "SI" << m_xsiz*getTText()*PIXELSIZE/8*textStretch() << "," << m_ysiz*getTText()*PIXELSIZE/8*textStretch() << ";";
}

/* --------------------------------------------------------------------------- */
/* setBackgroundColor --                                                       */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setBackgroundColor( int bcolor ) {

}

/* --------------------------------------------------------------------------- */
/* setMarkerSymbol --                                                          */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setMarkerSymbol( int msymbol ) {
  switch( msymbol ) {
  case 1:
    m_cmarker = '.';
    break;
  case 2:
    m_cmarker = '+';
    break;
  case 3:
    m_cmarker = '*';
    break;
  case 4:
    m_cmarker = 'O';
    break;
  default:
    m_cmarker = 'X';
  }
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevHPGL::mark( double x, double y ) {
  double ix, iy;

  ix=getX( x );
  iy=getY( y );
//   fprintf( m_fileid, "PU;SI0.2,0.2;SM%c;PA%d,%d;", m_cmarker, ix, iy );
//   fprintf( m_fileid, "SI%g,%g;", m_xsiz*PIXELSIZE/10., m_ysiz*PIXELSIZE/10. );
  m_ostr << "PU;SI0.2,0.2;SM" << m_cmarker << ";PA" << ix << "," << iy << ";" << std::endl;
  m_ostr << "SI" << m_xsiz*getTText()*PIXELSIZE/8 << "," << m_ysiz*getTText()*PIXELSIZE/8 << ";" << std::endl;
  m_penup = true;
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevHPGL::mark( double x, double y, int msymbol) {
  double ix, iy;
  setMarkerSymbol( msymbol );

  ix=getX( x );
  iy=getY( y );
//   fprintf( m_fileid, "PU;SI0.2,0.2;SM%c;PA%d,%d;", m_cmarker, ix, iy );
//   fprintf( m_fileid, "SI%g,%g;", m_xsiz*PIXELSIZE/10., m_ysiz*PIXELSIZE/10. );
  m_ostr << "PU;SI0.2,0.2;SM" << m_cmarker << ";PA" << ix << "," << iy << ";" << std::endl;
  m_ostr << "SI" << m_xsiz*getTText()*PIXELSIZE/8 << "," << m_ysiz*getTText()*PIXELSIZE/8 << ";" << std::endl;
  m_penup = true;
}

/* --------------------------------------------------------------------------- */
/* putText --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevHPGL::putText( const std::string &text ) {
  int xr, yr, strwidth, strheight;

  xr = 0;
  yr = 0;
  strwidth = static_cast<int>(text.size()*m_xsiz*getTText()*PIXELSIZE/8*10/PIXELSIZE/0.8);
  strheight = static_cast<int>(m_ysiz*getTText()*PIXELSIZE/8*10/PIXELSIZE/0.8);
  switch( m_hjust ) {
  case HCENTER:
    xr = -strwidth/2;
    break;
  case HRIGHT:
    xr = -strwidth;
    break;
  default:
    break;
  }
  switch( m_vjust ) {
  case VCENTER:
    yr = -strheight/2;
    break;
  case VTOP:
    yr = -strheight;
    break;
  default:
    break;
  }
  m_ostr << "PR;PU" << xr << "," << yr << ";LB" << text << ETX << ";PA;" << std::endl;
  m_penup = true;
}

/* --------------------------------------------------------------------------- */
/* drawPolygone --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::drawPolygone( const std::vector<double>&x
			     , const std::vector<double>&y ) {

}

/* --------------------------------------------------------------------------- */
/* setFillColor --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setFillColor( int col ) {

}

/* --------------------------------------------------------------------------- */
/* setEdgeStyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setEdgeStyle( bool edge ) {

}

/* --------------------------------------------------------------------------- */
/* drawArc --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevHPGL::drawArc( double xc, double yc
			, double a, double b
			, double a0, double a1) {
  double ix, iy, ixc, iyc;
  double dx, dy;
  double alpha = PI*a0/180;

  dx = a/2*cos( alpha );
  dy = a/2*sin( alpha );
  ix=getX( xc + dx );
  iy=getY( yc + dy );
  ixc=getX( xc );
  iyc=getY( yc );
//   fprintf( m_fileid, "PU%d,%d;PD;AA%d,%d,%d;"
// 	  , ix, iy
// 	  , ixc, iyc
// 	  , (int)(a1) );
  m_ostr << "PU" << ix << "," << iy << std::endl << ";PD;AA" << ixc << "," << iyc << ","
	 << static_cast<int>(a1) << ";" << std::endl;
  m_penup = false;
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

void GdevHPGL::setColor( int icol ) {

}

/* --------------------------------------------------------------------------- */
/* endPage --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevHPGL::endPage() {
  showpage();
}

/* --------------------------------------------------------------------------- */
/* moveTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevHPGL::moveTo( double x, double y ) {
  double ix, iy;

  ix=getX( x );
  iy=getY( y );
//   fprintf( m_fileid, "PU%d,%d;", ix, iy );
  m_ostr << "PU" << ix << "," << iy << ";" << std::endl;
  m_penup = true;
}

/* --------------------------------------------------------------------------- */
/* drawTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevHPGL::drawTo( double x, double y ) {
  double ix, iy;

  ix=getX( x );
  iy=getY( y );
  if( m_penup ){
    m_ostr << "PD;";
    m_penup = false;
  }
//   fprintf( m_fileid, "PA%d,%d;", ix, iy );
  m_ostr << "PA" << ix << "," << iy << ";" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* drawLines --                                                                */
/* --------------------------------------------------------------------------- */

void GdevHPGL::drawLines( const std::vector<double> &x
			  , const std::vector<double> &y ) {
  double ix, iy;
  int i, n;

  if( x.size() != y.size() )
    return;
  n = x.size();
  if( n <= 0 )
    return;

  m_ostr << "PU" << getX(x[0]) << "," << getY(y[0]) << ";PD";
  for( i=1; i<n; i++ ){
    ix=getX( x[i] );
    iy=getY( y[i] );
//     fprintf( m_fileid, "%d,%d", ix, iy );
    m_ostr << ix << "," << iy;
    if( i < n-1 )
//       fputs( ",", m_fileid );
      m_ostr << ",";
  }
//   fputs( ";", m_fileid );
  m_ostr << ";";
  m_penup = false;
}

/* --------------------------------------------------------------------------- */
/* flush --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevHPGL::flush() {

}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevHPGL::clear() {

}

/* --------------------------------------------------------------------------- */
/* finallyWork --                                                              */
/* --------------------------------------------------------------------------- */

void GdevHPGL::finallyWork() {
  close();
}

/* --------------------------------------------------------------------------- */
/* close --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevHPGL::close() {
}

/* --------------------------------------------------------------------------- */
/* showpage --                                                                 */
/* --------------------------------------------------------------------------- */

void GdevHPGL::showpage() {
  m_ostr << "PG;IP0,0,2000,2000;" << std::endl;
  if( m_currentPage < m_pageCount ){
    ++m_currentPage;
    m_ostr << "SP1;" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* pixelsize --                                                                */
/* --------------------------------------------------------------------------- */

double GdevHPGL::pixelsize() const{
  return PIXELSIZE;
}
