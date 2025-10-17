
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>
#include <time.h>
#include <iostream>

static const double PIXELSIZE = 25.4/72;

#include "utils/utils.h"
#include "app/AppData.h"
#include "plot/GdevPS.h"

#include "gui/qt/GuiQtManager.h"
#include <QtCore/QSettings>


/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* GdevPS --                                                                   */
/* --------------------------------------------------------------------------- */

GdevPS::GdevPS( std::ostream &ostr, const std::string& title,
		double x0, double y0, double x1, double y1,
		const std::string &creator, const std::string &user, int pages, Gdev::Orientation orient )
  : PicWidth( 0.0 )
  , PicHeight( 0.0 )
  , XOFF( 0.0 )
  , YOFF( 0.0 )
  , m_path( false )
  , SegmentOpened( false )
  , m_lineWidth( 0 )
  , m_size( 0.01 )
  , m_hjust( HLEFT )
  , m_vjust( VBOTTOM )
  , m_fontno( 0 )
  , m_color( 0 )
  , m_orient( orient )
  , m_numPages( pages )
  , m_currentPage( 1 )
  , m_numCharSets( 0 )
  , m_ostr( ostr ) {
  BUG(BugPlot,"GdevPS::GdevPS");
  invert( false );
  setViewport( -1.0, -1.0, 1.0, 1.0 );
  setCurrentArea( (x1-x0) / PIXELSIZE, (y1-y0) / PIXELSIZE );
  calcTransParam();
  header( title, x0, y0, x1, y1, creator, user );
}

/* --------------------------------------------------------------------------- */
/* ~GdevPS --                                                                  */
/* --------------------------------------------------------------------------- */

GdevPS::~GdevPS() {

}
/* --------------------------------------------------------------------------- */
/* getY --                                                                     */
/*    overloaded transformation function for y-values                          */
/* --------------------------------------------------------------------------- */

double GdevPS::getY( double y ){
  double rslt = ty() * y + qy();
  rslt = m_orient == ORIENT_PORTRAIT ? rslt : rslt-PicHeight;
  setYMinMax( rslt );
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* setLinewidth --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::setLinewidth( int lwidth ) {
  stroke();
  m_lineWidth = lwidth;
  m_ostr << " " << lwidth << " setlinewidth" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* setLinestyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::setLinestyle( int lstyle ) {
  stroke();
  m_ostr << " " << lstyle << " setlinestyle" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* setTextSize --                                                              */
/* --------------------------------------------------------------------------- */

void GdevPS::setTextSize( double size ) {
  setTextAttributes( size, m_hjust, m_vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextHjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::setTextHjust( HJust hjust ) {
  setTextAttributes( m_size, hjust, m_vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextVjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::setTextVjust( VJust vjust ) {
  setTextAttributes( m_size, m_hjust, vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextAngle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::setTextAngle( double angle ) {
  setTextAttributes( m_size, m_hjust, m_vjust, angle, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextFont --                                                              */
/* --------------------------------------------------------------------------- */

void GdevPS::setTextFont( int fontno ) {
  setTextAttributes( m_size, m_hjust, m_vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextAttributes --                                                        */
/* --------------------------------------------------------------------------- */

void GdevPS::setTextAttributes( double size, HJust hjust, VJust vjust
				, double angle, int fno ) {

  m_size = size * textStretch();
  double currentSize = m_size*getTText()/PIXELSIZE > 0.001 ? m_size*getTText()/PIXELSIZE : 0.001;

  m_hjust = hjust;
  m_vjust = vjust;
  m_fontno = fno<m_numCharSets? fno: 0;
  stroke();  // Do we need " ..<< fixed << .."? I dont know.
  m_ostr << " Font" << m_fontno << " [" << currentSize
	 << " 0 0 " << currentSize
	 << " 0 0 ] makefont setfont /fheight " << currentSize
	 << " def /height "  << currentSize << " def" << std::endl
	 << "  /vjust " << m_vjust << " def /hjust " << m_hjust << " def /angle "
	 << angle << " def /color " << m_color<< " def " << std::endl;
}

/* --------------------------------------------------------------------------- */
/* setBackgroundColor --                                                       */
/* --------------------------------------------------------------------------- */

void GdevPS::setBackgroundColor( int bcolor ) {

}

/* --------------------------------------------------------------------------- */
/* setMarkerSymbol --                                                          */
/* --------------------------------------------------------------------------- */

void GdevPS::setMarkerSymbol( int msymbol ) {
  m_ostr << " /msymbol " << msymbol << " def" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevPS::mark( double x, double y ) {
  double xx, yy;
  stroke();
  xx = getX( x );
  yy = getY( y ); // Do we need " ..<<fixed<<.."?
  m_ostr << " " << static_cast<double>(xx)
	 << " " << static_cast<double>(yy)
	 << " draw_marker" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevPS::mark( double x, double y, int msymbol) {
  setMarkerSymbol( msymbol );
  mark( x, y );
}

/* --------------------------------------------------------------------------- */
/* putText --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevPS::putText( const std::string &text ) {
  char *cp, *buf = strdup( text.c_str() );

  if( (cp = strtok( buf, "\n" )) ==  NULL )
    return;

  stroke();
  putString( cp );
  while( (cp = strtok( NULL, "\n" ))!= NULL ){
    m_ostr << " newline" << std::endl;
    if( strlen( cp ) > 0 )
      putString( cp );
  }
  free( buf );
}

/* --------------------------------------------------------------------------- */
/* putText --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevPS::setPolygonAttr( int edge, int style, int col, int pattern ) {
  stroke();
  m_ostr << " /interior " << edge << " def /edge " << style << " def /color "
	 << col << " def" << std::endl;
  //setColor(col);
}

/* --------------------------------------------------------------------------- */
/* drawPolygone --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::drawPolygone( const std::vector<double>&x
			 , const std::vector<double>&y ) {

}

/* --------------------------------------------------------------------------- */
/* setFillColor --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::setFillColor( int col ) {

}

/* --------------------------------------------------------------------------- */
/* setEdgeStyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::setEdgeStyle( bool edge ) {

}

/* --------------------------------------------------------------------------- */
/* drawArc --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevPS::drawArc( double xc, double yc
		      , double a, double b
		      , double a0, double a1) {

  double xx, yy, ang;
  double xstretch = a/b*getTX()/getTY(); //a/b*P_TX/P_TY;

  xx = getX( xc )/xstretch; // (xc*P_TX + P_QX)/xstretch;
  yy = getY( yc ); // yc*P_TY + P_QY;

  ang = a0 + a1;
  if( a1 > 0 )
    m_ostr << " " << xx << " " << yy << " " << b*getTY()/2.0 << " " << a0
	   << " " << ang << " " << xstretch << " draw_arc" << std::endl;
  else {
    if( ang > 0 )
      m_ostr << " " << xx << " " << yy << " " << b*getTY()/2.0 << " " << a0
	     << " " << ang << " " << xstretch << " draw_arcn" << std::endl;
    else
      m_ostr << " " << xx << " " << yy << " " << b*getTY()/2.0 << " " << a0
	     << " " << 360+ang << " " << xstretch << " draw_arcn" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

void GdevPS::setColor( int icol ) {
  m_ostr << " " << icol << " setcolor" <<std::endl;
  m_color = icol;
}

/* --------------------------------------------------------------------------- */
/* endPage --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevPS::endPage() {
  showpage();
}

/* --------------------------------------------------------------------------- */
/* moveTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevPS::moveTo( double x, double y ) {
  stroke();
  m_ostr << " " << getX(x)
	 << " " << getY(y) << " m" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* drawTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevPS::drawTo( double x, double y ) {
  m_path = true;
  m_ostr << " " << getX(x)
	 << " " << getY(y) << " l" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* drawLines --                                                                */
/* --------------------------------------------------------------------------- */

void GdevPS::drawLines( const std::vector<double>& x
			, const std::vector<double>&y ) {

  int i, n;

  if( x.size() != y.size() )
    return;
  n = x.size();
  if( n <= 0 )
    return;

  moveTo( x[0], y[0] );
  for( i=1; i<n; ++i ) {
    m_ostr << " " << getX(x[i])
	   << " " << getY(y[i]) << " l" << std::endl;
  }
  m_path = true;
  stroke();
}

/* --------------------------------------------------------------------------- */
/* flush --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevPS::flush() {

}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevPS::clear() {

}

/* --------------------------------------------------------------------------- */
/* finallyWork --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevPS::finallyWork() {
  close();
}

/* --------------------------------------------------------------------------- */
/* header --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevPS::header( const std::string& title
		     , double x0, double y0, double x1, double y1
		     , const std::string &creator, const std::string &user ){
  static char datestr[80];
  static char timestr[80];
  time_t now;
  int width, height, i;

  (void) time( &now );
  strftime( datestr, sizeof( datestr ), "%y/%m/%d", localtime( &now ) );
  strftime( timestr, sizeof( timestr ), "%H:%M:%S", localtime( &now ) );
  PicWidth = (x1-x0) / PIXELSIZE;
  PicHeight = (y1-y0) / PIXELSIZE;

  if( m_orient == ORIENT_LANDSCAPE ) {
    width = static_cast<int>(PicWidth);
    height = static_cast<int>(PicHeight);
    XOFF = static_cast<int>(y0) / PIXELSIZE;
    YOFF = static_cast<int>(x0) / PIXELSIZE;
  }
  else {
    width = static_cast<int>(PicWidth);
    height = static_cast<int>(PicHeight);
    XOFF = static_cast<int>(x0) / PIXELSIZE;
    YOFF = static_cast<int>(y0) / PIXELSIZE;
  }

  // Fontliste generieren
  char *fontnames[20] = {0};
  char  *flist(0), *cp(0);

#ifdef HAVE_QT
  flist = new char[ AppData::Instance().FontName().size()+1];
  strcpy(flist, AppData::Instance().FontName().c_str());
  ///  fontnames[AppData::Instance().FontName().size()] = NULL;
#endif
  ///  fontnames[19] = NULL;

  if( flist ){
    for( i=0; i<19; i++ ){
      if( (cp=strchr( flist, ',' )) == NULL ){
	fontnames[i]=flist;
	fontnames[i+1] = NULL;
	break;
      }
      else{
	cp[0]='\0';
	fontnames[i]=flist;
	flist = &(cp[1]);
      }
    }
  }

  double tmp1 = x0 / PIXELSIZE;
  double tmp2 = y0 / PIXELSIZE;
  m_ostr << "%!PS-Adobe-3.0" << std::endl;
  if( m_orient == ORIENT_LANDSCAPE )
    m_ostr << "%%BoundingBox: " << tmp2 << " " << tmp1
	   << " " << height+tmp2 << " " << width+tmp1 << std::endl;
  else
    m_ostr << "%%BoundingBox: " << tmp1 << " " << tmp2
	   << " " << width+tmp1 << " " << height+tmp2 << std::endl;
  m_ostr << "%%Title: " << title << std::endl;;
  m_ostr << "%%Creator: " << creator << " (" << user << ")" << std::endl;
  m_ostr << "%%CreationDate: " << datestr << std::endl;
  m_ostr << "%%CreationTime: " << timestr << std::endl;
  if(*fontnames){
    m_ostr << "%%DocumentFonts: ";
    for( i=0; fontnames[i]!=NULL; i++ )
      m_ostr << fontnames[i] << " ";
  }
  m_numCharSets=i;
  m_ostr << std::endl;
  m_ostr << "%%Pages: " << m_numPages << std::endl;
  m_ostr << "%%EndComments" << std::endl;
  m_ostr << "%%BeginProlog" << std::endl;
  if(*fontnames){
    for( i=0; fontnames[i]!=NULL; i++ )
      m_ostr << "/Font" << i << " /" << fontnames[i] << " findfont def" << std::endl;
  }
  m_ostr << "/vjust 1 def /hjust 1 def" << std::endl;
  m_ostr << "/interior 0 def /edge 0 def /color 0.5 def /ncols 10 def" << std::endl;

//   m_ostr << "/setcolor { /c exch def" << std::endl;
//   m_ostr << "    c 0 eq c 8 eq or { 0 setgray }if" << std::endl;
//   m_ostr << "    c 1 eq { 0.175 setgray }if" << std::endl;
//   m_ostr << "    c 2 eq { 0.525 setgray }if" << std::endl;
//   m_ostr << "    c 3 eq { 0.325 setgray }if" << std::endl;
//   m_ostr << "    c 4 eq { 0.9 setgray }if" << std::endl;
//   m_ostr << "    c 5 eq { 0.075 setgray }if" << std::endl;
//   m_ostr << "    c 6 eq { 0.725 setgray }if" << std::endl;
//   m_ostr << "    c 7 eq { 1 setgray }if" << std::endl;
//   m_ostr << "    c 9 eq { 0.975 setgray }if" << std::endl;
//   m_ostr << "    c 9 gt { c ncols div setgray }if} def" << std::endl;
  m_ostr << "/setlinestyle { /s exch def" << std::endl;
  m_ostr << "    s 0 eq s 8 eq or { [] 0 setdash }if" << std::endl;
  m_ostr << "    s 1 eq { [2 5] 0 setdash }if" << std::endl;
  m_ostr << "    s 2 eq { [2 3] 0 setdash }if" << std::endl;
  m_ostr << "    s 3 eq { [3 2] 0 setdash }if" << std::endl;
  m_ostr << "    s 4 eq { [5 2] 0 setdash }if" << std::endl;
  m_ostr << "    s 5 eq { [6 2 1 2] 0 setdash }if" << std::endl;
  m_ostr << "    s 6 eq { [6 6] 0 setdash }if" << std::endl;
  m_ostr << "    s 7 eq { [12 12] 0 setdash }if" << std::endl;
  m_ostr << "    s 9 eq { [10 4 1 4] 0 setdash }if" << std::endl;
  m_ostr << "    s 9 gt { [ s ncols div 2] 0 setdash }if} def" << std::endl;
  m_ostr << "/draw_polygon {" << std::endl;
  m_ostr << "    interior 0 ne { gsave color setcolor fill grestore } if" << std::endl;
  m_ostr << "    edge 0 eq { gsave 0 setgray s grestore newpath }" << std::endl;
  m_ostr << "    { newpath } ifelse } def" << std::endl;
  m_ostr << "/DS {" << std::endl;
  m_ostr << "  /text exch def /x 0 def /y 0 def" << std::endl;
  m_ostr << "  text stringwidth pop /width exch def " << std::endl;
  m_ostr << "  hjust 2 eq { /x x width 2 div sub def } if" << std::endl;
  m_ostr << "  hjust 3 eq { /x x width sub def } if" << std::endl;
  m_ostr << "  vjust 2 eq { /y y height 2 div sub def } if" << std::endl;
  m_ostr << "  vjust 3 eq { /y y height sub def } if" << std::endl;
  m_ostr << "  x y rmoveto text gsave color setcolor angle rotate show grestore" << std::endl;
  m_ostr << "  x neg y neg rmoveto } def" << std::endl;
  m_ostr << "/newline {" << std::endl;
  m_ostr << "  0 1.2 fheight mul neg rmoveto} def" << std::endl;
  m_ostr << "/clippon { clippflag 0 eq {" << std::endl;
  m_ostr << "  gsave /clippflag 1 def x0 y0 moveto x0 y1 lineto" << std::endl;
  m_ostr << "  x1 y1 lineto x1 y0 lineto closepath clip} if } def" << std::endl;
  m_ostr << "/clippoff { " << std::endl;
  m_ostr << "   clippflag 1 eq {grestore /clippflag 0 def} if} def" << std::endl;
  m_ostr << "/draw_marker { /y exch def /x exch def" << std::endl;
  m_ostr << "  msymbol 1 eq { newpath x y 2.5 0 360 arc " << std::endl;
  m_ostr << "    gsave 1 setcolor fill grestore } if" << std::endl;
  m_ostr << "  msymbol 2 eq { newpath x msize sub y moveto" << std::endl;
  m_ostr << "    x msize add y lineto x y msize sub moveto" << std::endl;
  m_ostr << "    x y msize add lineto gsave 0.5 setlinewidth stroke grestore newpath } if" << std::endl;
  m_ostr << "  msymbol 3 eq { newpath x msize sub y moveto" << std::endl;
  m_ostr << "    x msize add y lineto" << std::endl;
  m_ostr << "    x y msize sub moveto x y msize add lineto" << std::endl;
  m_ostr << "    x msize 1 sub sub y msize 1 sub sub moveto" << std::endl;
  m_ostr << "    x msize 1 sub add y msize 1 sub add lineto" << std::endl;
  m_ostr << "    x msize 1 sub add y msize 1 sub sub moveto" << std::endl;
  m_ostr << "    x msize 1 sub sub y msize 1 sub add lineto" << std::endl;
  m_ostr << "    gsave  1 setlinewidth stroke grestore newpath } if" << std::endl;
  m_ostr << "  msymbol 4 eq { newpath x y msize 0 360 arc gsave 0.5 setlinewidth stroke grestore newpath } if" << std::endl;
  m_ostr << "  msymbol 5 eq { newpath x msize sub y msize sub moveto" << std::endl;
  m_ostr << "    x msize add y msize add lineto" << std::endl;
  m_ostr << "    x msize add y msize sub moveto" << std::endl;
  m_ostr << "    x msize sub y msize add lineto gsave 0.5 setlinewidth stroke grestore newpath } if" << std::endl;
  m_ostr << "  msymbol 6 eq { newpath x msize sub y msize sub moveto" << std::endl;
  m_ostr << "    msize 2 mul 0 rlineto" << std::endl;
  m_ostr << "    msize neg msize 1.73 mul rlineto" << std::endl;
  m_ostr << "    msize neg msize 1.73 mul neg rlineto " << std::endl;
  m_ostr << "    gsave  0.5 setlinewidth stroke grestore newpath } if" << std::endl;
  m_ostr << "  msymbol 7 eq { newpath x msize sub y msize sub moveto" << std::endl;
  m_ostr << "    msize 2 mul 0 rlineto" << std::endl;
  m_ostr << "    0 msize 2 mul rlineto" << std::endl;
  m_ostr << "    msize 2 mul neg 0 rlineto" << std::endl;
  m_ostr << "    0 msize 2 mul neg rlineto " << std::endl;
  m_ostr << "    gsave  0.5 setlinewidth stroke grestore newpath } if" << std::endl;
  m_ostr << "  msymbol 8 eq { newpath x 2.5 sub y 2.5 sub moveto" << std::endl;
  m_ostr << "    5 0 rlineto" << std::endl;
  m_ostr << "    0 5 rlineto" << std::endl;
  m_ostr << "    5 neg 0 rlineto" << std::endl;
  m_ostr << "    0 5 neg rlineto gsave 8 setcolor fill grestore newpath } if" << std::endl;
  m_ostr << "  x y moveto } def /msize 3 def" << std::endl;
  m_ostr << "/l {lineto} def /m {moveto} def" << std::endl;
  m_ostr << "/draw_arc { gsave newpath 1 scale arc stroke grestore } def" << std::endl;
  m_ostr << "/draw_arcn { gsave newpath 1 scale arcn stroke grestore } def" << std::endl;
  m_ostr << "%%EndProlog" << std::endl;
  m_ostr << "%%Page: 1 " << m_numPages << std::endl;

  setWindow( 0.0, 0.0, width, height );
  if( SegmentOpened )
    return;

  colorTable();
  openSegment();

  setTextAttributes( m_size, m_hjust, m_vjust, 0, 0 );
  //  setMarkerSymbol( 1 ); // !!!!
  setLinestyle( 0 );
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* putString --                                                                */
/* --------------------------------------------------------------------------- */

void GdevPS::putString( char *cp ) {
  m_ostr << " (";
  for( ; *cp != '\0'; cp++ ) {
    switch( *cp ){
    case '(':
    case ')':
    case '\\':
      m_ostr << '\\' << *cp;
      break;
    case '%':
      m_ostr << "\045";
      break;
    default:
      m_ostr << *cp;
    }
  }
  m_ostr << ") DS\n";
}

/* --------------------------------------------------------------------------- */
/* stroke --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevPS::stroke() {
  if(m_path) {
    m_path = false;
    m_ostr << " stroke" << std::endl;
  }
}

// /* --------------------------------------------------------------------------- */
// /* scaleFactors --                                                             */
// /* --------------------------------------------------------------------------- */

// void GdevPS::scaleFactors( double wx0, double wx1, double wy0, double wy1
// 			 , double vx0, double vx1, double vy0, double vy1 ) {
//   double dwx, dwy, dvx, dvy;

//   dwx = wx1 - wx0;
//   dwy = wy1 - wy0;
//   dvx = vx1 - vx0;
//   dvy = vy1 - vy0;

//   P_QX = ( -wx0/dwx * dvx/2.0 + ( vx0+1.0)/2.0 )* PicWidth + XOFF;
//   P_QY = ( -wy0/dwy * dvy/2.0 + ( vy0+1.0)/2.0 ) * PicHeight - YOFF;
//   if ( m_angle == 90 ) {
//     P_QY = P_QY - PicHeight;
//   }

//   P_TX = dvx/dwx/2.0 * PicWidth;
//   P_TY = dvy/dwy/2.0 * PicHeight;
//   if( SegmentOpened )
//     return;
//   openSegment();
// }

/* --------------------------------------------------------------------------- */
/* openSegment --                                                              */
/* --------------------------------------------------------------------------- */

void GdevPS::openSegment() {
  if( SegmentOpened )
    return;

  m_ostr << " gsave" << std::endl;
  m_ostr << "  " << XOFF << " " << YOFF << " translate" << std::endl;
  m_ostr << "  " << (m_orient==ORIENT_PORTRAIT?0:90) << " rotate" << std::endl;
  m_ostr << "  /clippflag 0 def /msymbol 2 def" << std::endl;

  setLinewidth( m_lineWidth );
  SegmentOpened=true;
}

/* --------------------------------------------------------------------------- */
/* closeSegment --                                                             */
/* --------------------------------------------------------------------------- */

void GdevPS::closeSegment() {
  if( SegmentOpened ){
    stroke();
    m_ostr << " clippoff grestore" << std::endl;
    SegmentOpened=false;
  }
}

/* --------------------------------------------------------------------------- */
/* colorTable --                                                               */
/* --------------------------------------------------------------------------- */

void GdevPS::colorTable() {
  // Farbdefinitionen erst spaeter definieren (da druckertreiber diese evtl.uberschreiben)
  std::string pscolors;
#if defined HAVE_QT && !defined HAVE_HEADLESS
  if( GuiQtManager::Instance().Settings() ) {
    pscolors = GuiQtManager::Instance().Settings()->value
      ( "/DefaultColors/psPlot", "").toString().toStdString();
  }
#endif
  if ( pscolors.empty() ) {
    // set default colors
    pscolors = "FFFFFFFF 00FFFF00 FFFF0000 FF00FF00 C0C0C000 00669900 6699FF00 00FF0000 FF000000 0000FF00 6699FF00 99660000";
  }

  m_ostr << "[/Indexed /DeviceCMYK 11 " << std::endl;
  m_ostr << "<" << pscolors << ">" << std::endl;
  m_ostr << "] setcolorspace" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* close --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevPS::close() {
  m_ostr << "%%EOF" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* showpage --                                                                 */
/* --------------------------------------------------------------------------- */

void GdevPS::showpage() {
  stroke();
  closeSegment();
  m_ostr << " showpage" << std::endl;
  if( m_currentPage < m_numPages ){
    ++m_currentPage;
    m_ostr << "%%Page: " << m_currentPage << " " << m_numPages << std::endl;
    colorTable();
    openSegment();
  }
}

/* --------------------------------------------------------------------------- */
/* pixelsize --                                                                */
/* --------------------------------------------------------------------------- */

double GdevPS::pixelsize() const{
  return PIXELSIZE;
}
