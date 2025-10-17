
#include <iostream>
#include <limits>
#include <assert.h>

#include "utils/Debugger.h"
#include "plot/qt/GdevQt.h"
#include "plot/qt/qtcanvas.h"
#include "gui/qt/GuiQtManager.h"

INIT_LOGGER();

/* File scope variables */
static const char       *ColorTable[]={
  "Black"
, "red"
, "blue"
, "green"
, "navy"
, "gold"        // statt yellow
, "#996600"     // statt pink (HG Pabst + Wiener)
, "magenta"
, "cyan"
, "Turquoise"
, "Violet"
, "Purple"
, "Wheat"
, "LimeGreen"
, "Orange"
};

static int NCOLORS = sizeof(ColorTable) / sizeof( ColorTable[0] );

static char line_types[][8] = {
  {1, 15},
  {2, 4 },
  {8, 8},
  {4, 2 },
  {4, 2, 2, 4 },
  {12, 1, 2, 1},
  {9, 1, 2, 1, 2, 1}
};

static int line_type_elements[] = {2, 2, 2, 2, 4, 4, 6};
static int number_of_line_types = sizeof( line_type_elements ) / sizeof( line_type_elements[0] );

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* GdevQt --                                                                   */
/* --------------------------------------------------------------------------- */

GdevQt::GdevQt( QtCanvas *w, const std::string &xfontlist ) :
  m_widget( w )
  , FONTPATTERN( "-*-courier-*-r-*" )
  , GDncharsets( 0 )
  , m_fillcolor( 0 )
  , m_edge( false )
  , m_charset(0)
  , m_fontno(0)
  , m_angle(0)
  , m_align(0)

  , m_currx(0)
  , m_curry(0)
  , m_xmin( std::numeric_limits<int>::max() )
  , m_xmax( std::numeric_limits<int>::min() )
  , m_ymin( std::numeric_limits<int>::max() )
  , m_ymax( std::numeric_limits<int>::min() )

  , m_currColor(0)
  , m_currLineWidth(1)
{
  calcTransParam();
  int	k,n,s,i;
  int   size;
  char  *xflist, *cp;
  int   nfonts;

//   Display *dpy = XtDisplay(m_widget);

//   xflist = xfontlist.size() > 0 ?
//     XtNewString( xfontlist.c_str() ) :
//     XtNewString( FONTPATTERN.c_str() );

//   GDncharsets=MAXCHARSETS;
//   for( s=0; (s < MAXCHARSETS) && (xflist != NULL); s++ ) {

//     if( (cp=strchr( xflist, ',' )) == NULL ) {
//       DAfont[s].fontlist=XListFonts( dpy
// 				     , xflist, MAXFONTS
// 				     , &(DAfont[s].nfonts) );
//       GDncharsets=s+1;
//       xflist=NULL;
//     }
//     else{
//       cp[0]='\0';
//       DAfont[s].fontlist=XListFonts( dpy
// 				     , xflist, MAXFONTS
// 				     , &(DAfont[s].nfonts) );
//       xflist = &(cp[1]);
//     }
//     if( DAfont[s].nfonts == 0) {
//       /* lets try another font */
//       DAfont[s].fontlist = XListFonts( dpy, "-*-courier-*", MAXFONTS
// 				       , &(DAfont[s].nfonts) );
//       if( DAfont[s].nfonts == 0) {
// 	/* lets try another font */
// 	DAfont[s].fontlist = XListFonts( dpy, "fixed", MAXFONTS
// 					 , &(DAfont[s].nfonts) );
// 	if( DAfont[s].nfonts == 0) {
// 	  std::cerr << "GdevQt: No fonts found for \"" << FONTPATTERN
// 	       << "\" (\"*courier*\" and \"fixed\" neither)!" << std::endl;
// 	  return;
// 	}
//       }
//     }
//     for ( n=k=0; n<DAfont[s].nfonts; n++ ){
//       DAfont[s].font[k] = XLoadQueryFont( dpy, DAfont[s].fontlist[n] );
//       if(DAfont[s].font[k] == 0) {
// 	std::cerr << "GdevQt  Cannot load font \"" << DAfont[s].fontlist[n] << "\" !" << std::endl;
//       }
//       else {
// 	if( (size = DAfont[s].font[k]->ascent +
// 	     DAfont[s].font[k]->descent) > 8 ){
// 	  for( i=0; i < k+1; i++ )
// 	    if( size == DAfont[s].size[i] )
// 	      break;
// 	  if( i == k+1 ){
// 	    DAfont[s].size[k] = size;
// 	    k++;
// 	  }
// 	  else
// 	    XFreeFont( dpy, DAfont[s].font[k] );
// 	}
// 	else
// 	  XFreeFont( dpy, DAfont[s].font[k] );
//       }
//     }
//     DAfont[s].nfonts = k;

//     nfonts = 0;
//     for( k=0; k<DAfont[s].nfonts; k++ ){
//       for( n=0; n<k; n++ ){
// 	if( DAfont[s].size[n] > DAfont[s].size[k] ){
// 	  XFontStruct *xf;
// 	  char *flist;
// 	  xf    = DAfont[s].font[n];
// 	  flist = DAfont[s].fontlist[n];
// 	  size  = DAfont[s].size[n];
// 	  DAfont[s].fontlist[n] = DAfont[s].fontlist[k];
// 	  DAfont[s].font[n]     = DAfont[s].font[k];
// 	  DAfont[s].size[n]     = DAfont[s].size[k];
// 	  DAfont[s].fontlist[k] = flist;
// 	  DAfont[s].font[k]     = xf;
// 	  DAfont[s].size[k]     = size;
// 	}
//       }
//     }
//   }
//   XGCValues	 xGCv;
//   Window         wd;
//   Screen        *screen;
//   Colormap      cmap;
//   XColor        color, exact;
 int           c;

//   dpy = XtDisplay( w );
//   wd = XDefaultRootWindow( dpy );
//   screen = XtScreen( w );
//   XtVaGetValues( w
// 		 , XmNcolormap, &cmap
// 		 , XmNforeground, &(xGCv.foreground)
// 		 , XmNbackground, &(xGCv.background)
// 		 , NULL );
//   xGCv.line_width = 0;
//   xGCv.function = GXcopy;
//   xGCv.fill_style = FillSolid;
//   m_fillgc = XCreateGC( dpy
// 			, RootWindowOfScreen( screen )
// 			, GCFunction |GCForeground | GCBackground  |
// 			GCLineWidth | GCFillStyle
// 			, &xGCv);

 m_ncolors = NCOLORS;
 for ( c=0; c < NCOLORS; c++ ){
   m_pixcol.push_back( QColor(ColorTable[c]) );
 }

}

/* --------------------------------------------------------------------------- */
/* ~GdevQt --                                                                  */
/* --------------------------------------------------------------------------- */

GdevQt::~GdevQt() {
}


/* --------------------------------------------------------------------------- */
/* setLinewidth --                                                            */
/* --------------------------------------------------------------------------- */

void GdevQt::setLinewidth( int lwidth ) {
  m_currLineWidth = lwidth;
}

/* --------------------------------------------------------------------------- */
/* setLinestyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevQt::setLinestyle ( int lstyle ) {

  switch (lstyle) {
    case 2:
      m_currLineStyle = Qt::DashLine;
      break;
    case 1:
      m_currLineStyle = Qt::DotLine;
      break;
    case 3:
      m_currLineStyle = Qt::DashDotLine;
      break;
    case 4:
      m_currLineStyle = Qt::DashDotDotLine;
      break;
    case 5:
      m_currLineStyle = Qt::CustomDashLine;
      break;
    case 0:
    default:
      m_currLineStyle = Qt::SolidLine;
      break;
  }
}

/* --------------------------------------------------------------------------- */
/* setTextSize --                                                              */
/* --------------------------------------------------------------------------- */

void GdevQt::setTextSize( double size ) {
  std::cout <<  "  === GdevQt::setTextSize ===> not implemented yet!\n" <<std::flush;
  setTextAttributes( size, m_hjust, m_vjust, m_angle, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextHjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevQt::setTextHjust( HJust hjust ) {
  setTextAttributes( m_size, hjust, m_vjust, m_angle, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextVjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevQt::setTextVjust( VJust vjust ) {
  setTextAttributes( m_size, m_hjust, vjust, m_angle, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextAngle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevQt::setTextAngle( double angle ) {
  std::cout <<  "  === GdevQt::setTextAngle ===> not implemented yet!\n" <<std::flush;
  setTextAttributes( m_size, m_hjust, m_vjust, angle, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextFont --                                                              */
/* --------------------------------------------------------------------------- */

void GdevQt::setTextFont( int fontno ) {
  std::cout <<  "  === GdevQt::setTextFont ===> not implemented yet!\n" <<std::flush;
  setTextAttributes( m_size, m_hjust, m_vjust, m_angle, fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextAttributes --                                                        */
/* --------------------------------------------------------------------------- */

void GdevQt::setTextAttributes( double size, HJust hjust, VJust vjust
			       , double angle, int fno )
/*
 * sets the current text-attributes :
 *
 *	SIZE    	: Height of the character-box.
 *
 *      ANGLE           : angle in degrees between direction of
 *                        text and horizontal
 *      FNO             : font
 *
 */
{
  int dy, f;
  int s;

  m_size   = size;
  m_hjust  = hjust;
  m_vjust  = vjust;
  m_angle  = angle;

  m_align = Qt::AlignLeft|Qt::AlignBottom;
  switch( hjust ) {
    case Gdev::HLEFT:
      m_align = Qt::AlignLeft;
      break;
    case Gdev::HCENTER:
      m_align = Qt::AlignCenter;
      break;
    case Gdev::HRIGHT:
      m_align = Qt::AlignRight;
      break;
  }
  switch( vjust ) {
    case Gdev::VBOTTOM:
      m_align |= Qt::AlignBottom;
      break;
    case Gdev::VCENTER:
      m_align |= Qt::AlignVCenter;
      break;
    case  Gdev::VTOP:
      m_align |= Qt::AlignTop;
      break;
  }

  if( m_widget != NULL ){
//     s = fno < GDncharsets ? fno : 0;
//     if( DAfont[s].nfonts == 0 )
//       return;
//     //    dy = static_cast<int>(-m_size*getHeight()+5.0);
    dy = static_cast<int>(-m_size*getTText());
//     std::cout << "   DY INFO  ["<<dy<<"]\n";

//     for( f=0; (f<DAfont[s].nfonts-1) && (dy > DAfont[s].size[f]); f++ );
//     m_fontno = f;
//     m_charset = s;
  }
}

/* --------------------------------------------------------------------------- */
/* setBackgroundColor --                                                       */
/* --------------------------------------------------------------------------- */

void GdevQt::setBackgroundColor( int bcolor ) {
  // keine Implementaion !!!!
}

/* --------------------------------------------------------------------------- */
/* setMarkerSymbol --                                                          */
/* --------------------------------------------------------------------------- */

void GdevQt::setMarkerSymbol( int msymbol ) {
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevQt::mark( double x, double y ) {
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevQt::mark( double x, double y, int msymbol ) {
  const int PERIODSIZE   = 3;
  const int PLUSSIZE	 = 4;
  const int ASTERISKSIZE = 3;
  const int OSIZE	 = 4;
  const int XSIZE	 = 3;
  const int TSIZE        = 6;
  const int RSIZE        = 4;

  int xx, yy;

  if ( m_widget == NULL ) return;
  if ( m_currColor == NULL) return; // keine Farbe gesetzt ==> Return

  m_currx = xx = static_cast<int>(getX( x )); //x*m_width;
  m_curry = yy = static_cast<int>(getY( y )); //y*m_height;


  switch( (msymbol-1) % 8 ) {
    case 0:
      { // punkt ausgefuellt
  	QtCanvasPolygonalItem* i = new QtCanvasEllipse(2*PERIODSIZE, 2*PERIODSIZE, m_widget);
	i->setBrush(*m_currColor); // Background
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->move(xx-0.5*PERIODSIZE, yy-0.5*PERIODSIZE);
 	i->show();
 	break;
      }
    case 1:
      { // kreuz
	QtCanvasLine* i = new QtCanvasLine(m_widget);
	i->setPoints(xx-PLUSSIZE, yy, xx+PLUSSIZE, yy);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->pen().setStyle( m_currLineStyle );
 	i->show();

	i = new QtCanvasLine(m_widget);
	i->setPoints(xx, yy-PLUSSIZE, xx, yy+PLUSSIZE);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->pen().setStyle( m_currLineStyle );
 	i->show();
	break;
      }
    case 2:
      { //stern
	QtCanvasLine* i = new QtCanvasLine(m_widget);
	i->setPoints(xx-ASTERISKSIZE, yy-ASTERISKSIZE, xx+ASTERISKSIZE, yy+ASTERISKSIZE);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->pen().setStyle( m_currLineStyle );
	i->show();

	i = new QtCanvasLine(m_widget);
	i->setPoints(xx-ASTERISKSIZE, yy+ASTERISKSIZE, xx+ASTERISKSIZE, yy-ASTERISKSIZE);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->pen().setStyle( m_currLineStyle );
	i->show();

	i = new QtCanvasLine(m_widget);
	i->setPoints(xx-ASTERISKSIZE, yy, xx+ASTERISKSIZE, yy);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->pen().setStyle( m_currLineStyle );
	i->show();

	i = new QtCanvasLine(m_widget);
	i->setPoints(xx, yy-ASTERISKSIZE, xx, yy+ASTERISKSIZE);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->pen().setStyle( m_currLineStyle );
	i->show();
	break;
      }
    case 3:
      { // punkt
 	QtCanvasPolygonalItem* i = new QtCanvasEllipse(2*OSIZE, 2*OSIZE, m_widget);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->move(xx-OSIZE, yy-OSIZE);
 	i->show();
      }
    case 4:
      { // x
	QtCanvasLine* i = new QtCanvasLine(m_widget);
	i->setPoints(xx-XSIZE, yy-XSIZE, xx+XSIZE, yy+XSIZE);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->show();

	i = new QtCanvasLine(m_widget);
	i->setPoints(xx-XSIZE, yy+XSIZE, xx+XSIZE, yy-XSIZE);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->show();
	break;
      }
    case 5:
      { // dreieck
	int x0, y0, y1;
	x0 = xx-XSIZE/2;
	y0 = yy-(int)(0.43*TSIZE);
	y1 = y0+(int)(0.86*TSIZE);

	QtCanvasLine* i = new QtCanvasLine(m_widget);
	i->setPoints(x0, y0, x0+TSIZE, y0);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->show();

	i = new QtCanvasLine(m_widget);
	i->setPoints(x0+TSIZE, y0, x0+TSIZE/2, y1);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->show();

	i = new QtCanvasLine(m_widget);
	i->setPoints(x0, y0, x0+TSIZE/2, y1);
	i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
	i->show();
	break;
      }
    case 6:
      {  // rechteck
	int x0, y0;
	x0 = xx-RSIZE/2;
	y0 = yy-RSIZE/2;
	QtCanvasPolygonalItem *i = new QtCanvasRectangle(x0, y0, RSIZE, RSIZE, m_widget);
	i->setPen(QPen(*m_currColor, m_currLineWidth));
	i->show();
	break;
      }
    case 7:
      {
	int x0, y0;
	x0 = xx-RSIZE/2;
	y0 = yy-RSIZE/2;
	QtCanvasPolygonalItem *i = new QtCanvasRectangle(x0, y0, RSIZE, RSIZE, m_widget);
	i->setBrush( *m_currColor );
	i->setPen(QPen(*m_currColor,  m_currLineWidth));
	i->show();
	break;
      }
  };
  setBoundingBox();
}

/* --------------------------------------------------------------------------- */
/* putText --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevQt::putText( const std::string &text ) {
  int x, y, s;

  if ( m_currColor == NULL) return; // keine Farbe gesetzt ==> Return
  x = m_currx;
  y = m_curry;
  s = m_charset;
  char *c = const_cast<char*>(text.c_str());
  if( m_widget != NULL ){
    QtCanvasText* i = new QtCanvasText(m_widget);
    i->setText( QString::fromStdString(text) );
    i->setColor( *m_currColor );
    i->move(x, y+i->font().pointSize()/3);
    i->setTextFlags( m_align );
    i->show();

    // ein bisschen Spielerei hier (bei Ueberschneidungen wird einfach der Font verkleinert)
    {
      QFont font(i->font()); font.setPointSize(6);
      i->setFont( font  ) ;
      int ps = i->font().pointSize();
      while (i->collisions(false).size() && ps > 6) {
	QFont font(i->font()); font.setPointSize(--ps);
	i->setFont( font  ) ;
      }
      QRect r=i->boundingRect();
      while (m_widget->rect().width() < (i->boundingRect().x()+i->boundingRect().width()) && ps > 6) {
	QFont font(i->font()); font.setPointSize(--ps);
	i->setFont( font  ) ;
      }
    }
//     if( DAfont[s].nfonts == 0 )
//       return;
//     if ( XtIsRealized( m_widget ) ){
//       int asciiCode = (int)*c < 0 ? (int)*c + 255 : (int)*c;
//       Drawable d = XtWindow( m_widget );
//       XFontStruct* xfs = DAfont[s].font[m_fontno];
//       char *cp = const_cast<char*>(asciiCode > xfs->max_char_or_byte2 ? "?" : c);
//       XRotDrawAlignedString( XtDisplay( m_widget )
// 			     , xfs
// 			     , m_angle
// 			     , d
// 			     , m_gc
// 			     , x, y+DAfont[s].size[m_fontno]/3
// 			     , cp, m_align );
//     if( m_pixmap != 0 )
//       assert( false );
// 	XRotDrawAlignedString( XtDisplay( m_widget )
// 			       , xfs
// 			       , m_angle
// 			       , m_pixmap
// 			       , m_gc
// 			       , x, y+DAfont[s].size[m_fontno]/3
// 			       , cp, m_align );
//     }
  }
  setBoundingBox();
}

/* --------------------------------------------------------------------------- */
/* drawPolygone --                                                             */
/* --------------------------------------------------------------------------- */

void GdevQt::drawPolygone( const std::vector<double>&x,
			    const std::vector<double>&y ) {
  std::cout <<  "  === GdevQt::drawPolygone ===\n" <<std::flush;
  assert( false );
  //   XPoint    *pts;
  //   XGCValues xGCv;
  int	    n, i;

  if( x.size() != y.size() )
    return;
  if( m_widget == NULL )
    return;

  n = x.size();
  QPolygon  pts(n+1);

  for( i=0; i < n ; i++ ) {
    pts.setPoint(i, (int)getX(x[i]), (int)getY(y[i]) );
  }
  pts.setPoint(i, pts.point(0));
//   i++;

  // draw polygon
  QtCanvasPolygon* poly = new QtCanvasPolygon(m_widget);
  poly->setPoints(pts);
  if (m_fillcolor >= 0)
    poly->setBrush(QColor( m_pixcol[ m_fillcolor % m_ncolors ] ));
  if (m_edge && m_currColor)
    poly->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
  poly->move(0, 0);
  poly->show();

//   /* filled polygon ? */
//   if ( m_fillcolor >= 0 ) {
//     xGCv.foreground = m_pixcol[ m_fillcolor % m_ncolors ];
//     XChangeGC( XtDisplay( m_widget ), m_fillgc,
// 	       GCForeground, &xGCv);
//     XFillPolygon( XtDisplay( m_widget),XtWindow( m_widget ) ,
// 		 m_fillgc, pts, i, Nonconvex, CoordModeOrigin);
//     if( m_pixmap != 0 )
//       XFillPolygon( XtDisplay( m_widget), m_pixmap ,
// 		    m_fillgc, pts, i, Nonconvex, CoordModeOrigin);
//   }
//   /* draw border ? */
//   // neu m_edge statt edge, muss irgendwo gesetzt werden!!!!
//   if( m_edge ) {
//     XDrawLines( XtDisplay( m_widget ), XtWindow( m_widget ),
// 	       m_gc, pts, i, CoordModeOrigin);
//     if( m_pixmap != 0 )
//       XDrawLines( XtDisplay( m_widget ), m_pixmap,
// 		  m_gc, pts, i, CoordModeOrigin);
//   }
}

/* --------------------------------------------------------------------------- */
/* setFillColor --                                                             */
/* --------------------------------------------------------------------------- */

void GdevQt::setFillColor( int col ) {
  m_fillcolor = col;
}

/* --------------------------------------------------------------------------- */
/* setEdgeStyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevQt::setEdgeStyle( bool edge ) {
  m_edge = edge;
}

/* --------------------------------------------------------------------------- */
/* drawArc --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevQt::drawArc( double xc, double yc
		       , double a, double b
		       , double a0, double a1) {
  int angle1 = static_cast<int>(16*a0);
  int angle2 = static_cast<int>(16*a1);

  int x = static_cast<int>(xc - (a / 2.));
  int y = static_cast<int>(yc + (b / 2.));
  int x1 = static_cast<int>( getX(x) );
  int y1 = static_cast<int>( getY(y) );
  unsigned int width  = static_cast<unsigned int>(getTX( a ));
  unsigned int height = static_cast<unsigned int>(getTY( -b ));

  if( m_widget != NULL ){
    QtCanvasPolygonalItem* i = new QtCanvasEllipse(width, height,angle1, angle2, m_widget);
//   i->setBrush(*m_currColor); // Background
    if (m_currColor)
      i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
    i->move(x1, y1);
    i->show();
  }

//   if( m_widget != NULL ){
//     if ( XtIsRealized( m_widget ) ){
//       XDrawArc( XtDisplay( m_widget ), XtWindow( m_widget ),
// 		m_gc, x1, y1, width, height, angle1, angle2 );
//       if( m_pixmap != 0 )
// 	XDrawArc( XtDisplay( m_widget ), m_pixmap,
// 		  m_gc, x1, y1, width, height, angle1, angle2 );
//     }
//   }
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

void GdevQt::setColor( int icol ) {
  m_currColor = &(m_pixcol[ icol % m_ncolors ]);
}

/* --------------------------------------------------------------------------- */
/* endPage --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevQt::endPage() {
}

/* --------------------------------------------------------------------------- */
/* moveTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevQt::moveTo( double x, double y ) {
  m_currx =  static_cast<int>(getX( x )); //x*m_width;
  m_curry =  static_cast<int>(getY( y )); //y*m_height;
  setBoundingBox();
}

/* --------------------------------------------------------------------------- */
/* drawTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevQt::drawTo( double x, double y ) {
  int xx, yy;
  if ( m_currColor == NULL) return; // keine Farbe gesetzt ==> Return

  xx = static_cast<int>(getX( x )); //x*m_width;
  yy = static_cast<int>(getY( y )); //y*m_height;
  if( m_widget != NULL ){

    if( lineIsVisible(m_currx, m_curry, xx, yy) ){
      QtCanvasLine* i = new QtCanvasLine(m_widget);
      i->setPoints(m_currx, m_curry, xx, yy);
      i->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
      i->show();
    }
  }
  m_currx = xx;
  m_curry = yy;
  setBoundingBox();
}

/* --------------------------------------------------------------------------- */
/* drawLines --                                                                */
/* --------------------------------------------------------------------------- */

// void GdrawLines( Gdev *d, int n, double *x, double *y ) {
void GdevQt::drawLines( const std::vector<double>& x, const std::vector<double>&y ) {
  int		 n, i, j;
  double	*px,*py;

   if( x.size() != y.size() )
     return;
   n = x.size();
   if( n <= 0 )
     return;
   if( m_widget == NULL )
     return;

   QPolygon  pts(n);
   std::vector<QtCanvasLine*> lines;

   for( j=i=0; i < n ; i++ ) {
     pts.setPoint(j, (int)getX( x[i] ), (int)getY( y[i] ));
     if( j > 0 ){
       if( lineIsVisible( pts[j-1].x(), pts[j-1].y(), pts[j].x(), pts[j].y() ) ){
	 j++;
       }
     }
     else {
       if( pointIsVisible( pts[j].x(), pts[j].y() ) ){
	 j++;
       }
     }
   }

   if( j > 0 ){
     // draw lines
     for( i=1; i < j; i++ ) {
       QtCanvasLine* line = new QtCanvasLine(m_widget);
       line->setPoints(pts.point(i-1).x(), pts.point(i-1).y(), pts.point(i).x(), pts.point(i).y());
       line->setPen( QPen(*m_currColor, m_currLineWidth, m_currLineStyle) );
       line->show();
     }
   }
   m_currx = static_cast<int>(getX( x[n-1] )); //x[n-1]*m_width;
   m_curry = static_cast<int>(getY( y[n-1] )); //y[n-1]*m_height;
   setBoundingBox();
}

/* --------------------------------------------------------------------------- */
/* flush --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevQt::flush() {
//   XFlush( XtDisplay(m_widget) );
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevQt::clear() {
  BUG_DEBUG( "GdevQt::clear");
  QtCanvasItemList list = m_widget->allItems();
  qDeleteAll(list);
}

/* --------------------------------------------------------------------------- */
/* finallyWork --                                                              */
/* --------------------------------------------------------------------------- */

void GdevQt::finallyWork() {
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* setBoundingBox --                                                           */
/* --------------------------------------------------------------------------- */

void GdevQt::setBoundingBox() {
  if( m_currx > m_xmax )
    m_xmax = m_currx;
  else
    if( m_currx < m_xmin )
      m_xmin = m_currx;
  if( m_curry > m_ymax )
    m_ymax = m_curry;
  else
    if( m_curry < m_ymin )
      m_ymin = m_curry;
}
