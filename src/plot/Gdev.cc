
#include <iostream>
#include <math.h>
#include <assert.h>
#include <limits>

static const double MAX_DOUBLE = std::numeric_limits<double>::max();
static const double MIN_DOUBLE = MAX_DOUBLE * -1;

#include "utils/Debugger.h"
#include "plot/Gdev.h"
#include "utils/NumLim.h"

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* Gdev --                                                                     */
/* --------------------------------------------------------------------------- */

Gdev::Gdev( double wx0, double wx1
	    , double wy0, double wy1
	    , double vx0, double vx1
	    , double vy0, double vy1 ) :
  m_width( 0.0 )
  , m_height( 0.0 )
  , m_currx( 0.0 )
  , m_curry( 0.0 )
  , m_wx0( wx0 )
  , m_wy0( wy0 )
  , m_wx1( wx1 )
  , m_wy1( wy1 )
  , m_vx0( vx0 )
  , m_vy0( vy0 )
  , m_vx1( vx1 )
  , m_vy1( vy1 )
  , m_xMax( MIN_DOUBLE )
  , m_xMin( MAX_DOUBLE )
  , m_yMax( MIN_DOUBLE )
  , m_yMin( MAX_DOUBLE )
  , m_invert( true )
  , m_textStretch( 1 ){
  // Transformationsparameter berrechnen
}

/* --------------------------------------------------------------------------- */
/* setWindow --                                                                */
/* --------------------------------------------------------------------------- */

void Gdev::setWindow( double wx0, double wy0
		      , double wx1, double wy1 ) {
  BUG(BugPlot,"Gdev::setWindow");
  m_wx0 = wx0;
  m_wy0 = wy0;
  m_wx1 = wx1;
  m_wy1 = wy1;
  // Transformationsparameter neu berrechnen
  calcTransParam();
}

/* --------------------------------------------------------------------------- */
/* setViewport --                                                              */
/* --------------------------------------------------------------------------- */

void Gdev::setViewport( double vx0, double vy0
			, double vx1, double vy1 ) {
 BUG(BugPlot,"Gdev::setViewport");
 BUG_MSG( "vx0 : " << vx0 << " vx1 : " << vx1 << " vy0 : " << vy0 << " vy1 : " << vy1 );
  if( (vx0 <= 1) && (vx1 <= 1) && (vy0 <= 1) && (vy1 <= 1) &&
      (vx0 >= -1) && (vx1 >= -1) && (vy0 >= -1) && (vx1 >= -1)) {
    m_vx0 = vx0;
    m_vy0 = vy0;
    m_vx1 = vx1;
    m_vy1 = vy1;
//     cerr << "Gdev: Viewport-Koordinaten" << endl
// 	 << "  m_vx0 = " << m_vx0 << ", m_vy0 = " << m_vy0
// 	 << "  m_vx1 = " << m_vx1 << ", m_vy1 = " << m_vy1 << endl;
    // Transformationsparameter neu berrechnen
    calcTransParam();
  }
  else {
    std::cerr << "Gdev: Die Koordinaten muessen im Bereich (-1,-1) und (1,1) liegen"
	 << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* setDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */

void Gdev::setDefaultSettings() {
  setWindow( 0, 0, 1, 1);
  setViewport( -1, -1, 1, 1 );
  setLinestyle( 0 );
  setLinewidth( 0 );
  setColor( 0 );
  setFillColor( 0 );
  setEdgeStyle( false );
}

/* --------------------------------------------------------------------------- */
/* setCurrentArea --                                                           */
/* --------------------------------------------------------------------------- */

void Gdev::setCurrentArea( double w, double h ) {
  BUG(BugPlot,"Gdev::setCurrentArea");
  m_width  = w;
  m_height = h;
  calcTransParam();
}

/* --------------------------------------------------------------------------- */
/* getX --                                                                     */
/* --------------------------------------------------------------------------- */

double Gdev::getX( double x ){
  double rslt = m_tx * x + m_qx;
  setXMinMax( x );
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* getY --                                                                     */
/* --------------------------------------------------------------------------- */

double Gdev::getY( double y ){
  double rslt = m_ty * y + m_qy;
  setYMinMax( rslt );
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* setXMinMax --                                                               */
/* --------------------------------------------------------------------------- */

void Gdev::setXMinMax( const double x ){
  double rslt = m_tx * x + m_qx;
  m_xMax = x > m_xMax ? x : m_xMax;
  m_xMin = x < m_xMin ? x : m_xMin;
}

/* --------------------------------------------------------------------------- */
/* setYMinMax --                                                               */
/* --------------------------------------------------------------------------- */

void Gdev::setYMinMax( const double y ){
  m_yMax = y > m_yMax ? y : m_yMax;
  m_yMin = y < m_yMin ? y : m_yMin;
}

/* --------------------------------------------------------------------------- */
/* getTX --                                                                     */
/* --------------------------------------------------------------------------- */

double Gdev::getTX( double l ) const {
  return l * m_tx;
}

/* --------------------------------------------------------------------------- */
/* getTText --                                                                     */
/* --------------------------------------------------------------------------- */

double Gdev::getTText() const {
  return m_tText;
}
/* --------------------------------------------------------------------------- */
/* getTY --                                                                     */
/* --------------------------------------------------------------------------- */

double Gdev::getTY( double l ) const {
  return l * m_ty;
}

/* --------------------------------------------------------------------------- */
/* getTX --                                                                     */
/* --------------------------------------------------------------------------- */

double Gdev::getTX() const {
  return m_tx;
}

/* --------------------------------------------------------------------------- */
/* getTY --                                                                     */
/* --------------------------------------------------------------------------- */

double Gdev::getTY() const {
  return m_ty;
}

/* --------------------------------------------------------------------------- */
/* pointIsVisible --                                                           */
/* --------------------------------------------------------------------------- */

bool Gdev::pointIsVisible( int x, int y ) {
   return (x >= 0) && (x <= m_width)
     && (y >= 0) && (y <= m_height);
}

/* --------------------------------------------------------------------------- */
/* lineIsVisible --                                                            */
/* --------------------------------------------------------------------------- */

bool Gdev::lineIsVisible( int x0, int y0, int x1, int y1 ) {
   return pointIsVisible(x0, y0) && pointIsVisible(x1, y1)
     && ( x0 != x1 || y0 != y1 );
}

/* --------------------------------------------------------------------------- */
/* lineIsVisible --                                                            */
/* --------------------------------------------------------------------------- */

void Gdev::invert( bool status ) {
  m_invert = status;
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* calcTransParam --                                                           */
/* --------------------------------------------------------------------------- */

void Gdev::calcTransParam() {
  BUG(BugPlot,"Gdev::calcTransParam");
  BUG_MSG( "width = " << m_width << " height = " << m_height );
  BUG_MSG( "m_vx1 " << m_vx1 << " m_vx0 " << m_vx0 << "m_wx1 " << m_wx1 << " m_wx0 " << m_wx0 );

  if( m_width == 0 || m_height == 0 ){
    m_qx = 0.0;
    m_qy = 0.0;
    double psize = pixelsize();
    if( psize != 0 ){
      m_tx = 1.0/psize;
      m_ty = 1.0/psize;
    }
    else{
      m_tx = 1.0;
      m_ty = 1.0;
    }
    m_tText = m_ty;
  }
  else{
    if( (m_wx1 - m_wx0) == 0 || (m_wy1 - m_wy0) == 0 )
      return;
    if( (m_wx1 - m_wx0) == 0 ) //!!!!
      m_tx = m_width * (m_vx1 - m_vx0) / 2;
    else{
      m_tx = m_width * (m_vx1 - m_vx0) / (2 * (m_wx1 - m_wx0));
    }
    m_qx = (m_width * (1 + m_vx0) / 2) - (m_tx * m_wx0);
    if( m_invert ) {
      if( (m_wy1 - m_wy0) == 0 )
	m_ty = -1 * m_height * (m_vy1 - m_vy0) / 2;
      else
	m_ty = -1 * m_height * (m_vy1 - m_vy0) / (2 * (m_wy1 - m_wy0));
      m_qy = (m_height * (1 - m_vy0) / 2) - (m_ty * m_wy0);
    }
    else {
      if( (m_wy1 - m_wy0) == 0 )
	m_ty = m_height * (m_vy1 - m_vy0) / 2;
      else
	m_ty = m_height * (m_vy1 - m_vy0) / (2 * (m_wy1 - m_wy0));
      m_qy = (m_height * (1 + m_vy0) / 2) - (m_ty * m_wy0);
    }
    if( m_ty != 0 ){
      m_tText = m_ty / ( ( m_vy1 - m_vy0) / 2 );
    }
    else
      m_tText = 0;
  }
}
