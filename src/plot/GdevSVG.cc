/*
 * Copyright (C) 2021 SEMAFOR Informatik & Energie AG, Basel, Switzerland
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include <iostream>
#include "string.h"

#include "app/AppData.h"
#include "GdevSVG.h"

static const char *ColorTable[]={
  "black"
  , "red"
  , "blue"
  , "green"
  , "navy"
  , "gold"        // statt yellow
  , "#996600"     // statt pink (HG Pabst + Wiener)
  , "magenta"
  , "cyan"
  , "turquoise"
  , "violet"
  , "purple"
  , "wheat"
  , "lime"
  , "orange"
};

static const char *LineTypeTable[] = {
  "5",
  "2 5",
  "2 3",
  "3 2",
  "5 2",
  "6 2 1 2",
  "6 6",
  "12 12",
  "10 4 1 4"
};

static int NLTYPES = sizeof(LineTypeTable) / sizeof( LineTypeTable[0] );

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* GdevSVG --                                                                   */
/* --------------------------------------------------------------------------- */

GdevSVG::GdevSVG( std::ostream &ostr, const std::string& title,
		double x0, double y0, double x1, double y1,
		const std::string &creator, const std::string &user,
                  int pages, Gdev::Orientation orient )
  : PicWidth( 0.0 )
  , PicHeight( 0.0 )
  , XOFF( 0.0 )
  , YOFF( 0.0 )
  , m_path( false )
  , SegmentOpened( false )
  , m_lineWidth( 0 )
  , m_lineStyle( 0 )
  , m_currPosX(0)
  , m_currPosY(0)
  , m_symbol(0)
  , m_size( 0.01 )
  , m_angle( 0 )
  , m_hjust( HLEFT )
  , m_vjust( VBOTTOM )
  , m_fontno( 0 )
  , m_color( 0 )
    //, m_orient( orient )
    //, m_numPages( pages )
  , m_currentPage( 1 )
  , m_numCharSets( 0 )
  , m_ostr(ostr) { // std::ofstream("simple.svg") ) {

  setViewport( -1.0, -1.0, 1.0, 1.0 );
  if(y1<0) {
    PicHeight=-y1;
  }
  setCurrentArea( (x1-x0) / pixelsize(), std::abs(y0-y1) / pixelsize() );

  header( title, x0, y0, x1, y1, creator, user );
}

/* --------------------------------------------------------------------------- */
/* ~GdevSVG --                                                                  */
/* --------------------------------------------------------------------------- */

GdevSVG::~GdevSVG() {

}
/* --------------------------------------------------------------------------- */
/* getY --                                                                     */
/*    overloaded transformation function for y-values                          */
/* --------------------------------------------------------------------------- */
/*
double GdevSVG::getY( double y ){
  double rslt = ty() * y + qy();
  rslt = m_orient == ORIENT_PORTRAIT ? rslt : rslt-PicHeight;
  setYMinMax( rslt );
  return rslt;
  }*/

/* --------------------------------------------------------------------------- */
/* setLinewidth --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::setLinewidth( int lwidth ) {
  m_lineWidth = lwidth;
  //  m_ostr << " " << lwidth << " setlinewidth" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* setLinestyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::setLinestyle( int lstyle ) {
  m_lineStyle = lstyle;
  //m_ostr << " " << lstyle << " setlinestyle" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* setTextSize --                                                              */
/* --------------------------------------------------------------------------- */

void GdevSVG::setTextSize( double size ) {
  setTextAttributes( size, m_hjust, m_vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextHjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::setTextHjust( HJust hjust ) {
  setTextAttributes( m_size, hjust, m_vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextVjust --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::setTextVjust( VJust vjust ) {
  setTextAttributes( m_size, m_hjust, vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextAngle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::setTextAngle( double angle ) {
  setTextAttributes( m_size, m_hjust, m_vjust, angle, m_fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextFont --                                                              */
/* --------------------------------------------------------------------------- */

void GdevSVG::setTextFont( int fontno ) {
  setTextAttributes( m_size, m_hjust, m_vjust
		     , m_orient==ORIENT_PORTRAIT?0.0:90.0, fontno );
}

/* --------------------------------------------------------------------------- */
/* setTextAttributes --                                                        */
/* --------------------------------------------------------------------------- */

void GdevSVG::setTextAttributes( double size, HJust hjust, VJust vjust
				, double angle, int fno ) {

  m_size = size * textStretch();
  double currentSize = m_size*getTText()/pixelsize() > 0.001 ? m_size*getTText()/pixelsize() : 0.001;
  m_angle = angle;
  m_hjust = hjust;
  m_vjust = vjust;
  m_fontno = fno<m_numCharSets? fno: 0;
}

/* --------------------------------------------------------------------------- */
/* setBackgroundColor --                                                       */
/* --------------------------------------------------------------------------- */

void GdevSVG::setBackgroundColor( int bcolor ) {

}

/* --------------------------------------------------------------------------- */
/* setMarkerSymbol --                                                          */
/* --------------------------------------------------------------------------- */

void GdevSVG::setMarkerSymbol( int msymbol ) {
  m_symbol = msymbol;
  // 1: filled circle
  // 2: +
  // 3: *
  // 4: rect
  // 5: triangle
  // 6: circle
  // 7: filled rect
  // 8: filled triangle
  //
  //  m_ostr << " /msymbol " << msymbol << " def" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevSVG::mark( double x, double y ) {
  double xx, yy;
  stroke();
  xx = getX( x );
  yy = getY( y ); // TODO
  switch(m_symbol){
  case 1:
    m_ostr << "<circle cx=\"" << xx <<"\" cy=\"" << yy << "\" r=\"2\" fill=\""
           << ColorTable[m_color] << "\"/>" << std::endl;
    break;
  case 2:
    m_ostr << "<path d=\"M"
           << xx << "," << yy-2 << " L"
           << xx << "," << yy+2 <<" M"<<xx-2<<","<<yy <<" L"<<xx+2<<","<<yy
           <<"\" stroke=\"" << ColorTable[m_color]
      << "\" stroke-width=\"1\" />" << std::endl;
    break;
  case 3:
    m_ostr << "<path d=\"M"
           << xx-2 << "," << yy-2 << " L" << xx+2 << "," << yy+2 <<" M"
           << xx-2 << "," << yy <<" L" << xx+2 <<","<< yy <<" M"
           << xx << "," << yy-2 <<" L" << xx <<","<< yy+2 <<" M"
           << xx+2 << "," << yy-2 <<" L" << xx-2 <<","<< yy+2
           <<"\" stroke=\"" << ColorTable[m_color]
      << "\" stroke-width=\"1\" />" << std::endl;
    break;
  case 4:
    m_ostr << "<circle cx=\"" << xx <<"\" cy=\"" << yy << "\" r=\"2\" fill=\"none\" "
           << "stroke-width=\"1\" stroke=\"" << ColorTable[m_color] <<"\"/>" << std::endl;
    break;
  case 5:
    m_ostr << "<path stroke=\"" << ColorTable[m_color] << "\" stroke-width=\"1\" d=\"M"
           << xx-2 << "," << yy-2 << " L" << xx+2 << "," << yy+2 << " M"
           << xx-2 << "," << yy+2 << " L" << xx+2 << "," << yy-2 << "\"/>" << std::endl;
    break;
  case 6:
    m_ostr << "<path d=\"M"
           << xx-2 << "," << yy-2 << " L" << xx+2 << "," << yy-2 << " L"
           << xx << "," << yy+2
           <<" Z\" stroke=\"" << ColorTable[m_color] << "\" fill=\"none\""
      << " stroke-width=\"1\" />" << std::endl;
    break;
  case 7:
    m_ostr << "<rect stroke=\"" << ColorTable[m_color]
           << "\" width=\"3\" height=\"3\" fill=\"none\" x=\""
           << xx <<"\" y=\"" << yy-1.5 << "\"/>" << std::endl;
    break;
  default:
    m_ostr << "<rect stroke=\"" << ColorTable[m_color]
           << "\" width=\"3\" height=\"3\" fill=\"" << ColorTable[m_color] <<"\" x=\""
           << xx <<"\" y=\"" << yy-1.5 << "\"/>" << std::endl;
  }
  //  m_ostr << " " << static_cast<double>(xx)
  // << " " << static_cast<double>(yy)
  //	 << " draw_marker" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* mark --                                                                     */
/* --------------------------------------------------------------------------- */

void GdevSVG::mark( double x, double y, int msymbol) {
  setMarkerSymbol( msymbol );
  mark( x, y );
}

/* --------------------------------------------------------------------------- */
/* putText --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevSVG::putText( const std::string &text ) {
  double x = m_currPosX;
  double y = m_currPosY;
  const char *alignment[3];
  if(m_hjust < HLEFT) m_hjust = HLEFT;
  if(m_hjust > HRIGHT) m_hjust = HRIGHT;

  alignment[0] = "start";
  alignment[1] = "middle";
  alignment[2] = "end";

  std::istringstream f(text);
  std::string line;
  // 0.0045 * 3000 => 13.5px
  while(std::getline(f, line)) {
    m_ostr << "<text font-size=\"" << 3000*m_size << "\" text-anchor=\"" << alignment[m_hjust-1]
           <<"\" transform=\"translate(" << x <<" " << y << ") "
           << "rotate(" << -m_angle << ")\">"
           << line << "</text>" << std::endl;
    y += 15;
  }
}

/* --------------------------------------------------------------------------- */
/* putPolygonAttr --                                                           */
/* --------------------------------------------------------------------------- */

void GdevSVG::setPolygonAttr( int edge, int style, int col, int pattern ) {
  //m_ostr << " /interior " << edge << " def /edge " << style << " def /color "
  //	 << col << " def" << std::endl;
  //setColor(col);
}

/* --------------------------------------------------------------------------- */
/* drawPolygone --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::drawPolygone( const std::vector<double>&x
			 , const std::vector<double>&y ) {

}

/* --------------------------------------------------------------------------- */
/* setFillColor --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::setFillColor( int col ) {

}

/* --------------------------------------------------------------------------- */
/* setEdgeStyle --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::setEdgeStyle( bool edge ) {

}

/* --------------------------------------------------------------------------- */
/* drawArc --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevSVG::drawArc( double xc, double yc
		      , double a, double b
		      , double a0, double a1) {

  double xx, yy, ang;
  double xstretch = a/b*getTX()/getTY(); //a/b*P_TX/P_TY;
  int laf = 0; // counter clockwise

  /* TODO
  m_ostr << "<path d='M'" << xc << " " << yc
         << " A" << a << " " << b << " 0 " << laf << " 0 " <<
     $        trim(adjustl(CXPOS(1))), ' ', trim(adjustl(CYPOS(1))),
     $        ' A', trim(adjustl(CR)), ' ', trim(adjustl(CR)), ' ',
     $        '0 ', trim(adjustl(CL)),
     $        ' 0 ', trim(adjustl(CXPOS(2))), ' ',
     $        trim(adjustl(CYPOS(2))),
     $        '" inx="', trim(adjustl(CI)),
     $        '" />' */
  xx = getX( xc )/xstretch; // (xc*P_TX + P_QX)/xstretch;
  yy = getY( yc ); // yc*P_TY + P_QY;

  ang = a0 + a1;
  /*  if( a1 > 0 )
    m_ostr << " " << xx << " " << yy << " " << b*getTY()/2.0 << " " << a0
	   << " " << ang << " " << xstretch << " draw_arc" << std::endl;
  else {
    if( ang > 0 )
      m_ostr << " " << xx << " " << yy << " " << b*getTY()/2.0 << " " << a0
	     << " " << ang << " " << xstretch << " draw_arcn" << std::endl;
    else
      m_ostr << " " << xx << " " << yy << " " << b*getTY()/2.0 << " " << a0
	     << " " << 360+ang << " " << xstretch << " draw_arcn" << std::endl;
             } */
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

void GdevSVG::setColor( int icol ) {
  //m_ostr << " " << icol << " setcolor" <<std::endl;
  m_color = icol;
}

/* --------------------------------------------------------------------------- */
/* endPage --                                                                  */
/* --------------------------------------------------------------------------- */

void GdevSVG::endPage() {
  close();
}

void GdevSVG::writeLineAttributes(){
  std::vector<std::string> attrs = {
    "vector-effect=\"non-scaling-stroke\"",
    std::string("stroke=\"")+ColorTable[m_color]+"\"",
    "stroke-width=\"2\""};
  //   stroke-linecap="square" stroke-linejoin="bevel" >
  for( std::vector<std::string>::iterator a=attrs.begin();
       a<attrs.end(); a++){
    m_ostr << *a << " ";
  }
  if(m_lineStyle > 0){
    m_ostr << "stroke-dasharray=\""
           << LineTypeTable[m_lineStyle%NLTYPES] << "\" ";
  }
}
/* --------------------------------------------------------------------------- */
/* moveTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevSVG::moveTo( double x, double y ) {
  m_currPosX = getX(x);
  m_currPosY = getY(y);
  //m_ostr << " " << getX(x)
  //	 << " " << getY(y) << " m" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* drawTo --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevSVG::drawTo( double x, double y ) {
  m_ostr << "<line ";
  writeLineAttributes();
  m_ostr << "x1=\"" << m_currPosX
         << "\" y1=\"" << m_currPosY
         << "\" x2=\"" << getX(x)
	 << "\" y2=\"" << getY(y) << "\"/>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* drawLines --                                                                */
/* --------------------------------------------------------------------------- */

void GdevSVG::drawLines( const std::vector<double>&x
			, const std::vector<double>&y ) {

  if( x.size() < 1 )
    return;

  m_ostr << "<polyline ";
  writeLineAttributes();
  m_ostr << "fill=\"none\" points=\"";
  std::vector<double>::const_iterator i = x.begin();
  std::vector<double>::const_iterator j = y.begin();
  for ( ;
       i < x.end() && j < y.end(); i++, j++) {
    m_ostr <<(*i)<<" "<<-(*j)+PicHeight;
    if( i+1 < x.end() && j+1 < y.end() ){
      m_ostr << ", ";
    }
  }
  m_ostr <<"\"/>\n";
}

/* --------------------------------------------------------------------------- */
/* flush --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevSVG::flush() {

}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevSVG::clear() {

}

/* --------------------------------------------------------------------------- */
/* finallyWork --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevSVG::finallyWork() {
}

/* --------------------------------------------------------------------------- */
/* header --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevSVG::header( const std::string& title
		     , double x0, double y0, double x1, double y1
		     , const std::string &creator, const std::string &user ){
  static char datestr[80];
  static char timestr[80];
  time_t now;
  int width, height, i;

  width = x1;
  height = std::abs(y1);
  (void) time( &now );
  strftime( datestr, sizeof( datestr ), "%y/%m/%d", localtime( &now ) );
  strftime( timestr, sizeof( timestr ), "%H:%M:%S", localtime( &now ) );

  m_ostr << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"" << std::endl;
  if(std::abs(x1-x0) > 0 && std::abs(y1-y0) > 0){
    m_ostr << " width=\"" << width << "\" height=\"" << height << "\"";
      //           << "\" viewBox=\" -40 470 550 370\"";
    //m_ostr << "    viewBox=\" 0 0 " << width << " " << height <<"\"" << std::endl;
  }
  m_ostr << " font-size=\"12\" font-family=\"monospace\">" << std::endl;
  //  m_ostr << "%%Title: " << title << std::endl;;
  //m_ostr << "%%Creator: " << creator << " (" << user << ")" << std::endl;
  //m_ostr << "%%CreationDate: " << datestr << std::endl;
  //m_ostr << "%%CreationTime: " << timestr << std::endl;
  //if(*fontnames){
  //  m_ostr << "%%DocumentFonts: ";
  //  for( i=0; fontnames[i]!=NULL; i++ )
  //    m_ostr << fontnames[i] << " ";
  //}
  //m_numCharSets=i;
  //m_ostr << "%%Pages: " << m_numPages << std::endl;
  //m_ostr << "%%EndComments" << std::endl;
  //m_ostr << "%%BeginProlog" << std::endl;
  //if(*fontnames){
  //  for( i=0; fontnames[i]!=NULL; i++ )
  //    m_ostr << "/Font" << i << " /" << fontnames[i] << " findfont def" << std::endl;
  //}
  //m_ostr << "/vjust 1 def /hjust 1 def" << std::endl;
  //m_ostr << "/interior 0 def /edge 0 def /color 0.5 def /ncols 10 def" << std::endl;

  setWindow( 0.0, 0.0, width, height );
  setTextAttributes( m_size, m_hjust, m_vjust, 0, 0 );
  //  setMarkerSymbol( 1 ); // !!!!
  setLinestyle( 0 );
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* stroke --                                                                   */
/* --------------------------------------------------------------------------- */

void GdevSVG::stroke() {
  if(m_path) {
    m_path = false;
    //    m_ostr << " stroke" << std::endl;
  }
}

// /* --------------------------------------------------------------------------- */
// /* scaleFactors --                                                             */
// /* --------------------------------------------------------------------------- */

// void GdevSVG::scaleFactors( double wx0, double wx1, double wy0, double wy1
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

void GdevSVG::openSegment() {
  if( SegmentOpened )
    return;

  //m_ostr << " gsave" << std::endl;
  //m_ostr << "  " << XOFF << " " << YOFF << " translate" << std::endl;
  //m_ostr << "  " << (m_orient==ORIENT_PORTRAIT?0:90) << " rotate" << std::endl;
  //m_ostr << "  /clippflag 0 def /msymbol 2 def" << std::endl;

  setLinewidth( m_lineWidth );
  SegmentOpened=true;
}

/* --------------------------------------------------------------------------- */
/* closeSegment --                                                             */
/* --------------------------------------------------------------------------- */

void GdevSVG::closeSegment() {
  if( SegmentOpened ){
    stroke();
    //m_ostr << " clippoff grestore" << std::endl;
    SegmentOpened=false;
  }
}


/* --------------------------------------------------------------------------- */
/* close --                                                                    */
/* --------------------------------------------------------------------------- */

void GdevSVG::close() {
  m_ostr << "</svg>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* showpage --                                                                 */
/* --------------------------------------------------------------------------- */

void GdevSVG::showpage() {
  stroke();
  closeSegment();
  //m_ostr << "</svg>" << std::endl;
  if( m_currentPage < m_numPages ){
    ++m_currentPage;
    //m_ostr << "%%Page: " << m_currentPage << " " << m_numPages << std::endl;
    //colorTable();
    openSegment();
  }
}
