
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#include <iomanip>
#include <sstream>
#include <limits>
#include <cmath>
#include <assert.h>

#ifdef HAVE_QT
#include <qglobal.h>
#include <qapplication.h>
#if defined Q_OS_WIN || defined Q_OS_CYGWIN
#include <QString>
#include <windows.h>
#include <qlocale.h>
#include<QApplication>
#else
#undef Void
#undef Long
#include <qlocale.h>
#endif
#endif

#include "xml/XMLDocumentHandler.h"
#include "xfer/XferConverter.h"
#include "xfer/gdtoa/gdtoa.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferScale.h"
#include "app/AppData.h"
#include "utils/Debugger.h"
#include "utils/NumLim.h"

INIT_LOGGER();

static char _decimalSymbol('0');
char RealConverter::decimalPoint() {
  if ( _decimalSymbol == '0' ) {
    // try environment variable DECIMAL_POINT
    char * decimalPoint = getenv("DECIMAL_POINT");
    if ( decimalPoint != NULL && strlen(decimalPoint) > 0 ) {
      _decimalSymbol = decimalPoint[0];
      return _decimalSymbol;
    }

    // use keypad decimal
#ifdef HAVE_QT
    if (AppData::Instance().KeypadDecimalPoint() &&
	AppData::Instance().isWindows()) {
#if defined Q_OS_WIN || defined Q_OS_CYGWIN
      _decimalSymbol = char(::MapVirtualKey( VK_DECIMAL, 2 /* MAPVK_VK_TO_CHAR */));
      return _decimalSymbol;
#endif
    }
#endif
    _decimalSymbol = defaultDecimalPoint();
    BUG_DEBUG("decimalSymbol: "<< _decimalSymbol);
  }
  return _decimalSymbol;
}

char RealConverter::getDecimalPoint() const {
  if ( m_default_radix_char ) {
    return m_decimal_point;  // "."
  }
  return decimalPoint();
}

void RealConverter::resetDecimalPoint() {
  _decimalSymbol = '0';
  BUG_DEBUG("decimalSymbol RESET");
}

char RealConverter::defaultDecimalPoint() {
#ifdef HAVE_QT
  if (AppData::Instance().HeadlessWebMode()) {
#if QT_VERSION >= 0x060000
    return QLocale().decimalPoint().toLatin1().back();
#else
    return QLocale().decimalPoint().toLatin1();
#endif
  } else if (qApp) {
#if QT_VERSION >= 0x060000
    return QGuiApplication::inputMethod()->locale().system().decimalPoint().toLatin1().back();
#else
    return QGuiApplication::inputMethod()->locale().system().decimalPoint().toLatin1();
#endif
  }
#endif
  return  '.';
}

const int MAX_ZEROS = 3;
const int MAX_NINES = 3;


static char * smart_round( char * s, char *se, int& decpt) {

  // do smart rounding (something like 4.180000001 to 4.18 ..
  int n;
  char *cp;
  n=0;
  if( s == 0 || se == 0 )
    return se;
  if( s >=se-2 )
    return se;

  int breakPt= decpt > 0 ? decpt-1 : 0;
  for( cp=se-2; cp>(s+breakPt) && *cp=='0'; --cp, ++n );
  if( n>MAX_ZEROS ){
    if (n > 2*MAX_ZEROS) --cp;
    return smart_round( s, cp+2, decpt);
  }
  // .. and 2.49999x to 2.5)
  n=0;
  for( cp=se-2; cp>(s+breakPt) && *cp=='9'; --cp, ++n );
  if( n>MAX_NINES ) {
    while ( *cp == '9' && cp !=s) --cp;
    if( *cp == '9' ){
      *cp='1';
      ++se;
      decpt+=1;
    }
    else {
      *cp=*cp + 1;
    }
    for( ++cp; cp != se; ++cp )
      *cp='0';

    return smart_round( s, se, decpt);
  }
  return se;
}

/**
  binary to decimal ascii conversion of double values
  @param d value to convert
  @param width field width
  @param nprec number of digits after decimal point
  If width is equal 0 and nprec is negative
  the shortest string that yields d is returned.
  The string is left (if width>0) or right padded with
  blanks on return
  @param decimal_point decimal point
  @param thousep
  Thousands are separated if a valid thousep ( != 0x00 ) is given,
  @author Ronald Tanner
*/

/* --------------------------------------------------------------------------- */
/* dfmt --                                                                     */
/* --------------------------------------------------------------------------- */
static std::string dfmt( double x, int width, int nprec, char decimal_point, char thousep, bool appendDot ){
  char buf[2000], *cp;
  int w;

  char *s, *se;
  int decpt, i, j, k, sign, len;
  int addDot = 0;
  if( appendDot && nprec <= 0 && fabs( floor( x + 0.5  ) - x ) <= NumLim::EPSILON_DOUBLE )
    addDot = 1;

  cp = buf;
  // -----------------------------------------------
  // x == 0; Die Formatierung wird separat erledigt.
  // -----------------------------------------------
  if (!x) { // x == 0
    if( width > 0 ){ // output is right adjusted
      int w = nprec<0 ? width-1 : width-2-nprec;
      if( addDot )
	--w;
      for( int i=0; i<w; ++i )
	*cp++=' ';
    }
    *cp++ = '0';
    if( nprec > 0 ){
      *cp++ = decimal_point;
      for( int i=0; i<nprec; ++i ){
	*cp++='0';
      }
    }
    else if( addDot && abs(width) > 1)
      *cp++ = decimal_point;
    if( width < 0 ){ // output is left adjusted
      int w = nprec<0 ? -width-1 : -width-2-nprec;
      if( addDot )
	--w;
      for( int i=0; i<w; ++i )
	*cp++=' ';
    }
    *cp = 0;
    return std::string(buf);
  }

  // -----------------------------------------------
  // x != 0;
  // -----------------------------------------------
  if( nprec < 0 ){
    s = dtoa(x, 0, 0, &decpt, &sign, &se);
  }
  else {
    s = dtoa(x, 3, nprec, &decpt, &sign, &se);
  }
  // -----------------------------------------------
  // Infinity
  // -----------------------------------------------
  if (decpt == 9999) /* Infinity or Nan */ {
//     cerr << "I N F I N I T Y (or NaN) ";
    if( width > 0 ){
      for( int i=0; i<width-3; ++i )
	*cp++=' ';
    }
    *cp++='i';
    *cp++='n';
    *cp++='f';

    if( width < 0 ){
      for( int i=0; i<-width-1; ++i )
	*cp++=' ';
    }
    *cp = 0;
    return std::string(buf);
  }


  int sepCount = 0;
  if( thousep != 0x00 ){
    assert( x != 0 );
    sepCount = static_cast<int>(floor( log10( fabs(x) ) / 3. ));
  }

  // -----------------------------------------------
  // Länge bestimmen
  // -----------------------------------------------
  len = se-s;
  // for( int i=0; i<len; ++i )
  //   std::cerr << s[i];
  // std::cerr << std::endl;
  if( decpt < len ){
    se=smart_round( s, se, decpt);
//     cerr << " new LEN: " << se-s <<std::endl;
//     if( se-s > len  && decpt > 0 )
//      ++decpt;
//   for( int i=0; i<se-s; ++i )
//     cerr << s[i];
//   cerr << std::endl;
  }

  w=width>=0?width:-width;
  if( decpt<=0 ){
    if( nprec>0 ){
      len = nprec + 2;
    }
    else{
      len = se-s-decpt+2;
    }
  }
  else {
    len = se-s;
    if( len == 0 ) ++len; // we want at least a 0
    if( decpt > len )
      len = decpt;
    if( nprec>=0 ){
      len=decpt + nprec;
    }
    if( decpt < len ) // do we have a decimal point ?
      len++;
  }
  if( sign ) // do we have a minus sign ?
    ++len;

  //   cerr << "LEN: " << len << " W: " << w << " SIGN: " << sign << " DECPT: " << decpt << std::endl;
  if (width!=0 && len+sepCount+addDot>w ){ /*
			      should we use exponential form:
			      [-]d.ddde+/-dd
			   */
    //        cerr << "WIDTH: " << width << " DECPT: " << decpt << " X: " << x << std::endl;
    int ndec=decpt;
    if( ndec > 0  && (ndec+sign < w && nprec < 0) ){
      return dfmt( x, width, w-sign-ndec-1, decimal_point, thousep, appendDot );
    }
    if( ndec < 0 && 1-ndec < 4  && nprec < 0 ){
      return dfmt( x, width, w-sign+ndec-1, decimal_point, thousep, appendDot );
//      return dfmt( x, width, w-sign+ndec+1, decimal_point, thousep, appendDot ); //???? amg
    }
    if( ndec == 0 && 1-ndec < 5  && nprec < 0 ){
      return dfmt( x, width, w-sign+ndec-2, decimal_point, thousep, appendDot );
    }

    int mw;
    s = dtoa(x, 0, 0, &decpt, &sign, &se);
    ndec = decpt-1;
    if( ndec < 0 ){
      ndec = -ndec;
    }
    for(j = 2, k = 10; 10*k <= ndec; j++, k *= 10){};
    mw = w-2-j;
    if (sign){
      mw--;
      *cp++ = '-';
    }
    if (*s) {
      *cp++ = *s++;
      int m=1;
      if( *s && m<mw ){
	--mw;
	*cp++ = decimal_point;
      }
      while((*cp = *s++)&&(m<mw)){
	cp++;
	++m;
      }
    }
    else {
      *cp++='0';
    }

    // increase one eg. 2.45(78)e04 => 2.46e04
    if (*cp > '4') {
      char *t = cp;
      while (--t >= (buf+sign)) {
	if (*t == decimal_point) continue;
	if (*t != '9') {
	  ++(*t);
	  break;
	} else {
	  if (t == (buf+sign)) {
	    *t = '1';
	    ++decpt;
	  }else
 	  *t = '0';
	}
      }
    }

    *cp++ = 'e';
    if (--decpt < 0) {
      *cp++ = '-';
      decpt = -decpt;
    }
    else
      *cp++ = '+';
    for(;;) {
      i = decpt / k;
      *cp++ = i + '0';
      if (--j <= 0)
	break;
      decpt -= i*k;
      decpt *= 10;
    }
    *cp = 0;
    return std::string(buf);
  }
  else {
    width-=sepCount;
    width -= addDot;
    if( width > 0 ){
      int l;
      for( l=0; l<width-len; ++l ){
	*cp++=' ';
      }
//       if( nprec>=0 )
// 	len-=l;
    }
    if (sign){
      --len;
      *cp++ = '-';
    }
    if (decpt <= 0) {
      *cp++ = '0';
      *cp++ = decimal_point;
      len -= 2;
      for(; decpt < 0; decpt++){
	*cp++ = '0';
	--len;
      }
      while((*cp = *s++) && (len > 0)){
	--len;
	cp++;
      }
      for(; len > 0; --len){
	*cp++ = '0';
      }
      *cp = 0;
    }
    else {
      while( (*cp = *s++) && (len > 0 )){
	--len;
	cp++;
	if (--decpt == 0 && len>1){
	  *cp++ = decimal_point;
	  --len;
	}
      }

      for(; len > 0; --len){
	*cp++ = '0';
	if (--decpt == 0 && len > 1){
	  *cp++ = decimal_point;
	  --len;
	}
      }

      *cp = 0;
    }
  }

  std::string str(buf);
  std::string::size_type pos = str.find( decimal_point );
  if( pos == std::string::npos ){
    pos = str.size();
  }
  int spos = pos - 3;
  for(;sepCount > 0; --sepCount, spos-=3)
    str.insert( spos, 1, thousep );
  if( addDot == 1  && (pos+1) < abs(width) && pos > 0)
    str += decimal_point;
  return str;
}

/* --------------------------------------------------------------------------- */
/* stringToDouble --                                                           */
/* --------------------------------------------------------------------------- */

static bool stringToDouble( char *str, double &d, double scale, double shift, char thousep, bool default_radix_char ){
  BUG_PARA(BugXfer,"stringToDouble","'" << str << "'");
  double dval;
  d = 0.0;
  char decimalpoint = default_radix_char ? '.' : RealConverter::decimalPoint();
  // WEBAPI
  if (_decimalSymbol == '0') {
    decimalpoint = RealConverter::decimalPoint();
    BUG_DEBUG("New decimalSymbol: "<< decimalpoint
              << ", default_radix_char: " <<  default_radix_char
#if QT_VERSION >= 0x060000
             << " - " << QGuiApplication::inputMethod()->locale().decimalPoint().toLatin1().back()
             << " - " << QGuiApplication::inputMethod()->locale().system().decimalPoint().toLatin1().back()
#else
             << " - " << QGuiApplication::inputMethod()->locale().decimalPoint().toLatin1()
             << " - " << QGuiApplication::inputMethod()->locale().system().decimalPoint().toLatin1()
#endif
             );
  }
  char *tmpbuf=str;
  if( thousep || decimalpoint != '.' ){ // eliminate all thousand separators and replace decimalpoint by '.'
    tmpbuf=strdup(str);
    char *dp=tmpbuf;
    for( char *cp=str; *cp!=0x00; ++cp,++dp ){
      if( *cp == thousep ){
        ++cp;
        if(!*cp) break;
      }
      if( *cp == decimalpoint )
        *dp = '.'; // don't change str; thousep is never just before decimalpoint
      else
        *dp=*cp;
    }
    *dp=0x00;
  }
#ifdef HAVE_QT
  // avoid istringstream if possible (inf, nan)
  bool ok = false;
  dval = QString( tmpbuf ).toDouble( &ok );
#else
  // else, old intens code from 2012
  std::istringstream is(tmpbuf);
  if ( !default_radix_char ) {
    std::locale loc( "" );
    is.imbue( loc );
  }
  is >> dval;
  bool ok = !(is.rdstate() & std::ios::failbit);
#endif
  if( thousep || decimalpoint != '.' ) free(tmpbuf);
  if (ok) {
    if( NumLim::isFinite( dval ) )
      d = (dval- shift) / scale ;
    else
      d = dval - shift;
    return true;
  }
  return false;
}

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

Converter::Converter( bool ignore_newline ):
  m_ignore_newline(ignore_newline)
  , m_width(0)
  , m_adjustleft(false){}


RealConverter::RealConverter( int width, int prec, Scale *scale, bool ignore_newline, bool default_radix_char )
  : Converter(ignore_newline)
  , m_prec(prec)
  , m_appendDot(false)
  , m_thousand_sep(0x00)
  , m_scale( scale )
  , m_default_radix_char( default_radix_char )
{
  setWidth( width );
  if ( m_default_radix_char ) {
    m_decimal_point = '.';
    return;
  }
  m_decimal_point = decimalPoint();
}

ComplexConverter::ComplexConverter( int width, int prec, Scale *scale, bool ignore_newline, bool default_radix_char )
  : RealConverter(width,prec,scale,ignore_newline, default_radix_char)
  , m_default_radix_char(default_radix_char) {
}

IntConverter::IntConverter( int width, Scale *scale, bool ignore_newline )
  : Converter(ignore_newline)
  , m_scale( scale ){
  setWidth( width );
}

StringConverter::StringConverter(int width, bool ignore_newline)
  : Converter(ignore_newline){
  setWidth(width);
}

/* --------------------------------------------------------------------------- */
/* setWidth --                                                                 */
/* --------------------------------------------------------------------------- */

void Converter::setWidth( int w ) {
  if( w<0 ){
    m_width = -w;
    m_adjustleft = true;
  }
  else {
    m_width = w;
    m_adjustleft = false;
  }
}

/* --------------------------------------------------------------------------- */
/* writeInvalid --                                                             */
/* --------------------------------------------------------------------------- */

bool Converter::writeInvalid( std::ostream &os ) const {
  std::string s("<NULL>");
  int width=getWidth();
  if(width > 0 ) {
    if( s.size() > width )
      os << s;
    else {
      int i=0;
      if( !m_adjustleft ){
	for( ; i<width-s.size(); ++i ) os << ' ';
      }
      for( i=0; i<s.size(); ++i ) os << s[i];
      if( m_adjustleft ){
	for( ; i<width; ++i ) os << ' ';
      }
    }
    return os.good();
  }
  return (os << s).good();
}

/* --------------------------------------------------------------------------- */
/* setScaleIndex --                                                            */
/* --------------------------------------------------------------------------- */
void Converter::setScaleIndex( int wildcardNumberBackwards, int value ) {
  if( scale() != 0 ) scale() -> setIndex( wildcardNumberBackwards, value );
}

/* --------------------------------------------------------------------------- */
/* readPattern --                                                              */
/* --------------------------------------------------------------------------- */

bool Converter::readPattern( char c, std::istream &is, const char *pattern, int len )const{
  // Der erste Character ist bereits gelesen
  if( toupper(c) != pattern[0] ){
    return false;
  }
  if( len == 1 ) return true;


  int i = 1;
  assert( len < 10 );

#if ( __GNUC__ < 3 )
  std::streampos pos = is.tellg();
#else
  std::istream::pos_type pos = is.tellg();
#endif

  c = is.get();
  while( c != EOF ){
    if( toupper(c) == pattern[i] ){
      ++i;
      if( i>=len ){
	return true; // pattern has matched
      }
    }
    else{
      break;
    }
    c = is.get();
  }

  // pattern has not matched
  // Der als Parameter uebergebene Char wird nicht zurueckgeladen. Dies soll
  // bei Bedarf der Aufrufer uebernehmen.
  is.seekg( pos );

  return false;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool StringConverter::read( std::istream &is, std::string &s, const char delimiter ) const{
  BUG( BugXfer, "StringConverter::read()" );
  char c;
  // Die Länge ist vorgegeben
  int width=getWidth();
  if( width > 0 ){
    bool ok=true;
    char *cp = new char [width+1];
    int p;
    for( p=0; p<width; ++p ){
      if( (cp[p]=is.get()) == EOF ){
	ok = false;
	break; // maybe this is wrong?
      }
      if( cp[p]=='\n' ){
	is.putback( cp[p] );
	if (p && cp[p-1] == '\r') --p; // remove existing carriage return '\r'
	ok = (p>0); // this means: it is ok if there is at least 1 char
	break;
      }
    }
    cp[p]='\0';
    BUG_MSG( "p= " << p << " s='" << cp <<"'" );
    s=cp;
    std::string::size_type pos=s.find_first_not_of( " \t" );
    if( pos == std::string::npos ){
      s="";
    }
    else {
      if( pos > 0 )
	s.erase(0, pos);  // remove first pos whitespaces
      pos = s.find_last_not_of( " \t" );
      if( pos > 0 )
	s.erase(pos+1);    // remove trailing whitespaces
    }

    BUG_EXIT("return value '"<<s<<"' :: gelesen char '"<<cp
	     <<"' width='"<<width<<"'");
    delete [] cp;
    return ok;
  }

// Ein Delimiter ist definiert
  if( delimiter != '\0' ){
    if( !isspace(delimiter) ){
      while( (c=is.get())!=delimiter ){
	if( c==EOF ) {
	  BUG_EXIT("EOF");
	  return false;
	}
      break;
      }
    } // delimiter is space
    else {
      c=deblank(is, delimiter,
		((!getWidth() && delimiter==' ') ? true : false));
      if( c==EOF ){
	BUG_EXIT("EOF");
	return false;
      }
      // // leerer Wert
      if( c==delimiter ){
	s="";
	BUG_EXIT("true: empty string");
	return true;
      }
    }
  }
  else{
    c=is.get();
  }

  std::ostringstream os;
  while( c != EOF ){
    if( (delimiter==' ' && c=='\n') || c==delimiter ){
      break;
    }
	if( c=='\n' || c==delimiter )
		break;
    os << c;
    c = is.get();
  }
  if( c=='\n' )
    is.putback( c );
  if(!os.str().size() && c==delimiter) {
    s="";
    BUG_EXIT("true: empty string");
    return true;
  }

  // maybe remove existing carriage return '\r'
  const std::string& str = os.str();
  size_t pos = str.find_last_of('\r');
  if (pos ==str.size()-1)
    s=os.str().substr(0,pos);
  s=str;
  BUG_EXIT("return string '"<<s<<"'");
  return s.size()>0;
}


/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool MatrixStringConverter::read( std::istream &is, std::string &s, const char delimiter ) const{
  BUG( BugXfer, "StringConverter::read()" );
  char c;
  // Die Länge ist vorgegeben
  int width=getWidth();
  if( width > 0 ){
    bool ok=true;
    char *cp = new char [width+1];
    int p;
    for( p=0; p<width; ++p ){
      if( (cp[p]=is.get()) == EOF ){
	ok = false;
	break; // maybe this is wrong?
      }
      if( cp[p]=='\n' ){
	is.putback( cp[p] );
	ok = (p>0); // this means: it is ok if there is at least 1 char
	break;
      }
    }
    cp[p]='\0';
    BUG_MSG( "p= " << p << " s='" << cp <<"'" );
    s=cp;
    std::string::size_type pos=s.find_first_not_of( " \t" );
    if( pos == std::string::npos ){
      s="";
    }
    else {
      if( pos > 0 )
	s.erase(0, pos);  // remove first pos whitespaces
      pos = s.find_last_not_of( " \t" );
      if( pos > 0 )
	s.erase(pos+1);    // remove trailing whitespaces
    }
    delete [] cp;
    return ok;
  }

  std::ostringstream os;
  c=is.get();
  while( c != EOF ){
    if( c==delimiter || c == '\n' ){
      break;
    }
    os << c;
    c = is.get();
  }
  if( c=='\n' )
    is.putback( c );

  s=os.str();
  return s.size()>0;
}

/* --------------------------------------------------------------------------- */
/* readAll --                                                                  */
/* --------------------------------------------------------------------------- */

bool StringConverter::readAll( std::istream &is, std::string &s,
			       const char delimiter) const {
  char c = deblank( is, delimiter,
		    ((!getWidth() && delimiter==' ') ? true : false));
  if( c == (char) EOF ){
    s = "";
    return true;
  }
  std::ostringstream os;
  while( c != (char) EOF ){
    os << c;
    c = is.get();
  }
  s = os.str();
  std::string::size_type pos = s.find_last_not_of(' ');
  if( pos != std::string::npos ){
    s = s.substr(0,pos+1);
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool RealConverter::read( std::istream &is, double &d, const char delimiter ) const {
  BUG_PARA(BugXfer,"RealConverter::read","width=" << getWidth());
  int width=getWidth();
  if( width == 0 ){ // skip blanks
    char c;
    if( (c=deblank( is, delimiter, ((!getWidth() && delimiter == ' ') ? true : false) )) == EOF ){
      BUG_EXIT("EOF");
      return false;
    }
    if( c=='\n' ){
      is.putback( c );
      d = std::numeric_limits<double>::quiet_NaN();
      BUG_EXIT("empty value => nan  last char("<<c<<")");
      return false;
    }

    if( !isdigit(c) ){
      if(c==delimiter ){
	d=std::numeric_limits<double>::quiet_NaN();
	BUG_EXIT("empty value => nan  last char("<<c<<")");
	return true;
      }
      if( readInfinite( c, is ) ){
	d=NumLim::infinity();
	BUG_EXIT("return value '"<<d<<"'");
        return true;
      }
      if( readInvalid( c, is ) ){
	d = 0.0;
	BUG_EXIT("return False & Invalid '"<<d<<"'");
        return false;
      }
    }
    char buf[BUFSIZ];
    int bufpos=0;
    bool has_exponent=false;
    bool has_decimal=false;
    bool has_sign=false;
    bool has_exsign=false;
    int  z=0;

    while( c != EOF && !isspace(c) && c != delimiter){
      if( !isdigit(c) ){
	if( c == getDecimalPoint() ) {
	  if( has_decimal ) { is.putback(c); break; }
	  has_decimal = true;
	}
	else if( (c == '-') || (c == '+') ) {
	  if( has_exponent ){
	    if( has_exsign ){ is.putback(c); break; }
	    has_exsign = true;
	  }
	  else {
	    if( has_sign ){ is.putback(c); break; }
	    has_sign = true;
	  }
	}
	else if( toupper(c) == 'E' ){
	  if( has_exponent || z==0 ){ is.putback(c); break; }
	  has_exponent=true;
	}
	else {
	  is.putback(c);
	  break;
	}
      }
      else{ // is digit
	++z;
      }
      buf[bufpos++]=c;
      c=is.get();
    }
    // if( isspace(c) )
    if( (delimiter == ' ' && isspace(c)) || c=='\n')
    //   if (c=='\n')
      is.putback( c );
    if( z<1 ){
      BUG_EXIT("return False z<1 buf '"<<buf<<"'");
      return false;
    }
    buf[bufpos]='\0';
    bool ret = stringToDouble( buf, d, getScale(), getShift(), m_thousand_sep, m_default_radix_char );
    BUG_EXIT("return d '"<<d<<"' buf '"<<buf<<"' return '"<<ret
	     <<"' scale '"<<getScale()<<"' width = 0");
    return ret;
  }

#if ( __GNUC__ < 3 )
  std::streampos pos = is.tellg();
#else
  std::istream::pos_type pos = is.tellg();
#endif

  char c;
  int p;
  char buf[BUFSIZ];
  int bufpos=0;

  for( p=0; p<width; ++p ){
    if( (c=is.get()) == EOF ){
      break;
    }
    buf[bufpos++]=c;
  }
  buf[bufpos]='\0';
  if( stringToDouble( buf, d, getScale(), getShift(), m_thousand_sep
		      , m_default_radix_char ) ) {
    BUG_EXIT("return value '"<<d<<"' :: gelesen real buf '"<<buf
	     <<"' width='"<<width<<"'");
    return true;
  }

  // Conversion failed: let somebody else handle it
  is.clear();
  is.seekg( pos );
  BUG_EXIT("conversion Fails gelesen real  buf '"<<buf
	   <<"' width='"<<width<<"'");

  return false;

}

/* --------------------------------------------------------------------------- */
/* readAll --                                                                  */
/* --------------------------------------------------------------------------- */

bool RealConverter::readAll( std::istream &is, double &d, const char delimiter ) const {
  BUG_PARA(BugXfer,"RealConverter::readAll","width=" << getWidth());
  char c;
  double dval;
#if ( __GNUC__ < 3 )
  std::streampos pos = is.tellg();
#else
  std::istream::pos_type pos = is.tellg();
#endif

  if( (c=deblank( is, delimiter,
		  ((!getWidth() && delimiter==' ') ? true : false))) == EOF ){
    return false;
  }

  char buf[BUFSIZ];
  int bufpos=0;
  while( c != (char) EOF && bufpos<BUFSIZ-1 ){
    buf[bufpos++]=c;
    c=is.get();
  }
  buf[bufpos]='\0';
  if( stringToDouble( buf, d, getScale(), getShift(), m_thousand_sep, m_default_radix_char ) )
    return true;

  // Conversion failed: let somebody else handle it
//CHECK IT: This statement causes core dump on Linux: (TAR 01-01-02)
  is.clear();
  is.seekg( pos );

  return false;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool ComplexConverter::read( std::istream &is, std::complex<double> &dc ) const {
  BUG(BugXfer,"ComplexConverter::read");
  int width=getWidth();
  if( width == 0 ){ // skip blanks
    return readAll( is, dc, ' ' );
  }

  char c;
  int p;
  std::ostringstream os;
  for( p=0; p<width; ++p ){
    if( (c=is.get()) == EOF ){
      break;
    }
    os << c;
  }

  std::istringstream istr( os.str() );
  return readAll( istr, dc );
}

/* --------------------------------------------------------------------------- */
/* readAll --                                                                  */
/* --------------------------------------------------------------------------- */

 bool ComplexConverter::readAll( std::istream &is, std::complex<double> &dc, char delimiter ) const {
  BUG(BugXfer,"ComplexConverter::readAll" );
  char c;
  double re,im;
  char real_img_delimeter = getDecimalPoint() == ',' ? ';'  : ',';

  if( (c=deblank( is, delimiter,
		  ((!getWidth() && delimiter==' ') ? true : false))) == EOF ){
    BUG_EXIT("EOF => empty");
    return false;
  }

  if( c != '(' ){
    // Keine komplexe Zahl
    is.putback(c);
    if( !RealConverter::read( is, re, delimiter ) )
      return false;
    dc = std::complex<double>( re, 0.0 );
    BUG_EXIT("nur Realteil => OK");
    return true;
  }

  // Real-Teil
  RealConverter realconv( 0, getPrecision(), !scale() ? 0 : scale()->copy(), false, m_default_radix_char );

  if( !realconv.read( is, re, delimiter) ){
    BUG_EXIT("Realteil falsch");
    return false;
  }
  BUG_MSG("Show '" << c << "'" );
  while( c != EOF ){
    BUG_MSG("Show '" << c << "'" );
    if( c == real_img_delimeter ) break;
    c=is.get();
  }
  if( c == EOF ){
    BUG_EXIT("kein Komma (or Delimeter)");
    return false;
  }

  // Imaginaer-Teil
  if( !realconv.read( is, im, delimiter ) ){
    BUG_EXIT("Imagteil falsch");
    return false;
  }
  while( c != EOF ){
    if( c == ')' ) break;
    c=is.get();
  }
  if( c == EOF ){
    BUG_EXIT("keine Klammer ')'");
    return false; // Syntax falsch
  }
  dc = std::complex<double>( re, im );
  BUG_EXIT("ok");
  return true;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool IntConverter::read( std::istream &is, int &i, const char delimiter ){
  BUG_PARA( BugXfer, "IntConverter::read"," : scale= " << getScale() << " : shift= " << getShift());
  int width=getWidth();
  if( width == 0 ){
    return readAll( is, i, delimiter );
  }

#if ( __GNUC__ < 3 )
  std::streampos pos = is.tellg();
#else
  std::istream::pos_type pos = is.tellg();
#endif

  char c;
  int p;
  char buf[BUFSIZ];
  int bufpos=0;
  for( p=0; p<width; ++p ){
    if( (c=is.get()) == EOF ){
      break;
    }
    buf[bufpos++]=c;
  }
  buf[bufpos]='\0';
  std::istringstream istr( buf );
  istr >> i;
  if( !istr.fail() ){
    double rslt = (i - getShift())/getScale();
    if( fabs( floor( rslt + 0.5  ) - rslt ) > NumLim::EPSILON_DOUBLE ){
      BUG_EXIT("conversion Mismatch rslt '"<<rslt<<"' gelesen int '"
	       <<buf<<"' width='"<<width<<"'");
      return false;
    }
    i = static_cast<int>((i - getShift())/getScale());
    BUG_EXIT("return value '"<<i<<"' :: gelesen int '"<<buf
	     <<"' width='"<<width<<"'");
    return true;
  }
  // Conversion failed: let somebody else handle it
  is.clear();
  is.seekg( pos );
  BUG_EXIT("conversion Fails gelesen int '"<<buf<<"' width='"<<width<<"'");

  return false;
}
/* --------------------------------------------------------------------------- */
/* readAll --                                                                  */
/* --------------------------------------------------------------------------- */

bool IntConverter::readAll( std::istream &is, int &i, const char delimiter ){
  BUG( BugXfer, "IntConverter::readAll" );

#if ( __GNUC__ < 3 )
  std::streampos pos = is.tellg();
#else
  std::istream::pos_type pos = is.tellg();
#endif

  int c;
  if( (c=deblank( is, delimiter, ((!getWidth() && delimiter==' ') ? true : false) )) == EOF ){
    BUG_EXIT("EOF");
    return false;
  }
  if(c == '\n'){
    is.putback(c);
    i = std::numeric_limits<int>::min();
    BUG_EXIT("empty value => min  last char("<<c<<")");
    return false;
  }
  if(c == delimiter){
    i = std::numeric_limits<int>::min();
    BUG_EXIT("empty value => min  last char("<<c<<")");
    return true;
  }
//   cerr << " OK: first non-blank: " << c << std::endl;
  BUG_MSG( " first non-blank: " << c );
  // Zahl lesen
  char buf[BUFSIZ];
  int bufpos=0;
  if( c == '-' ){
    buf[0]='-';
    ++bufpos;
    c=is.get();
  }
  while( (c != EOF) && isdigit(c) && c != delimiter ){
    buf[bufpos++]=c;
    c=is.get();
  }
  buf[bufpos]='\0';
  BUG_MSG( "last: " << c << " buf: '" << buf << "'");
  if( !isdigit(c) && c != delimiter ){
    is.putback(c);
  }

  std::istringstream istr(buf);
  istr >> i;
  if( !istr.fail() ){
    double rslt = (i - getShift()) / getScale();
    if( fabs( floor( rslt + 0.5  ) - rslt ) > NumLim::EPSILON_DOUBLE ){
      BUG_EXIT("conversion Mismatch rslt '"<<rslt<<"' gelesen int '"
	       <<buf<<"' width='0'");
      return false;
    }
    i = static_cast<int>((i  - getShift())/ getScale());
    BUG_EXIT("return value '"<<i<<"' :: read int buf '"<<buf
	     <<"' scale '"<<getScale()<<"' width='0'");
    return true;
  }

  // Conversion failed: let somebody else handle it
  is.clear();
  is.seekg( pos );
  BUG_EXIT("conversion Fails read int buf '"<<buf<<"' width='0'");

  return false;
}


/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool StringConverter::write( const std::string& str, std::ostream &os ) const {
  BUG_PARA(BugXfer,"StringConverter::write","'" << str << "' w=" << getWidth());

  int width=getWidth();
  if(width > 0 ) {
    if( str.size() > width )
      os << str.substr(0,width);
    else {
      int i=0;
      if( !isLeftAdjusted() ){
	BUG_MSG("just right");
	for( ; i<width-str.size(); ++i ) os << ' ';
      }
      for( i=0; i<str.size(); ++i ) os << str[i];
      if( isLeftAdjusted() ){
	BUG_MSG("just left");
	for( ; i<width; ++i ) os << ' ';
      }
    }
    //    os << ' ';
    return os.good();
  }
  // suppress leading and trailing blanks
  std::string::size_type bpos=str.find_first_not_of( ' ' );
  std::string::size_type epos=str.find_last_not_of( ' ' );
  if( bpos == std::string::npos ) bpos=0;
  if( epos == std::string::npos )
    epos=str.size();
  else
    epos=epos+1;
  for( std::string::size_type i=bpos; i < epos; i++ ){
//     cerr << str[i];
    os << str[i];
  }
//   cerr << std::endl;
  return os.good();
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool RealConverter::write( double d, std::ostream &os ) const {
  int width = isLeftAdjusted() ? -getWidth() : getWidth();
  BUG_PARA( BugXfer, "RealConverter::write",
	    d << "(width= " << width << " scale= " << getScale() << " shift= " << getShift() << " prec=" << m_prec <<")" );
  std::string s;
  if( NumLim::isFinite(d) ){
    s=dfmt( (getScale()*d)+getShift(), width, m_prec, getDecimalPoint(), m_thousand_sep, m_appendDot );
  }
  else {
    if (std::isnan(d))
      s =""; //"nan";
    else
      s="inf";
  }
  // std::cout << "----> " << s << std::std::endl;
  // remove trailing zeros after decimal point if no precision is requested
  std::string::size_type zpos=s.size()-1;
  // std::cout << "remove trailing zeros: " << s << std::endl;
  if( zpos > s.find( getDecimalPoint() ) && m_prec == -1 && s.find( 'e' ) == std::string::npos ){
    std::string::size_type z;
    for( z=zpos; z>0 && s[z]=='0'; --z );
    if( s[z]==getDecimalPoint()) ++z;  // leave at least one zero
    s.erase( z+1 );
  }
  BUG_MSG( "'" << s << "'" );
  // remove trailing zeros before an exponent
  zpos=s.rfind( std::string("0e") );
  if( zpos != std::string::npos ){
    std::string::size_type z;
    for( z=zpos; z>0 && s[z]=='0'; --z );
    s.erase( z+1, zpos-z );
  }

  if( isLeftAdjusted() )
    os.setf(std::ios::left,std::ios::adjustfield);

  //  cerr << getWidth() << "|" << std::setw(getWidth()) << std::setfill(' ') << s.c_str() << "|" << std::endl;
  os << std::setw(getWidth()) << std::setfill(' ') << s.c_str();
  return os.good();
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool ComplexConverter::write( std::complex<double> &comp, std::ostream &os ) const {
  int width = isLeftAdjusted() ? -getWidth() : getWidth();
  char real_img_delimeter = getDecimalPoint() == ',' ? ';'  : ',';
  std::complex<double> cx;
  if( NumLim::isFinite(comp.real()) && NumLim::isFinite(comp.imag() ))
    cx = getScale() * comp + getShift();
  else
    cx = comp;

  std::ostringstream ostr;
  char c;
  int w = 0;
  if( width > 0 ){
    w = width-3;
    w = w>1 ? w/2 : 1;
  }
  ostr << "(";
  std::istringstream sr( dfmt( cx.real(), w, getPrecision(), getDecimalPoint(), 0x00, false ) );
  if( (c=deblank( sr, ' ', true )) == EOF ) return false;
  while( c != EOF ){
    ostr << c;
    c = sr.get();
  }
  ostr << real_img_delimeter;
  std::istringstream si( dfmt( cx.imag(), w, getPrecision(), getDecimalPoint(), 0x00, false ) );
  if( (c=deblank( si, ' ', true )) == EOF ) return false;
  while( c != EOF ){
    ostr << c;
    c = si.get();
  }
  ostr << ")";
  if( !ostr.good() ){
    return false;
  }

  os.fill(' ');
  os.width(width);
  if( isLeftAdjusted() ){
    os.setf(std::ios::left,std::ios::adjustfield);
  }
  os << ostr.str();
  return os.good();
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool IntConverter::write( int i, std::ostream &os ){
  BUG_PARA( BugXfer, "IntConverter::write", i << "(scale= " << getScale() << ")"
            << "(shift= " << getShift() << ")");
  int width=getWidth();
  if( width == 0 ){
    BUG_EXIT("width=0: unformatted");
    double rslt = i*getScale()+getShift();
    if( fabs( floor( rslt + 0.5  ) - rslt ) > NumLim::EPSILON_DOUBLE ){
      return false;
    }
    return (os << static_cast<int>(i*getScale()+getShift())).good();
  }

  os.fill(' ');
  os.width(width);
  if( isLeftAdjusted() ){
    os.setf(std::ios::left,std::ios::adjustfield);
  }
  double rslt = i*getScale()+getShift();
  if( fabs( floor( rslt + 0.5  ) - rslt ) > NumLim::EPSILON_DOUBLE ){
    return false;
  }
  os << static_cast<int>(i*getScale()+getShift());
  BUG_EXIT("width=" << width);
  return os.good();
}

/* --------------------------------------------------------------------------- */
/*  getAttributes--                                                            */
/* --------------------------------------------------------------------------- */

DATAAttributeMask RealConverter::getAttributes( TransactionNumber t ){
  BUG_PARA( BugXfer, "RealConverter::getAttributes", "this :" << this );
  return m_scale != 0 ? m_scale->getAttributes( t ) : 0;
}

/* --------------------------------------------------------------------------- */
/* deblank --                                                                  */
/* --------------------------------------------------------------------------- */

int Converter::deblank( std::istream &is, const char delimiter
			, bool remove_multiple_delimiter) const {
  int c;
  while( (c=is.get()) != EOF ) { //std::istream::traits_type::eof() ){
    if (remove_multiple_delimiter && c==delimiter)
      continue;

    if( (!m_ignore_newline && c=='\n') || !isspace(c) || c==delimiter ){
      break;
    }
  }
//   if( c == std::istream::traits_type::eof() ) {
//     cerr << "Converter::deblank has EOF \n";
//   }
//   else {
//     cerr << "Converter::deblank " << (char)c << std::endl;
//   }
  return c;
}

/* --------------------------------------------------------------------------- */
/* registerIndex --                                                            */
/* --------------------------------------------------------------------------- */

void RealConverter::registerIndex( GuiIndexListener *listener ){
  if( m_scale != 0 ){
    m_scale->registerIndex( listener );
  }
}

/* --------------------------------------------------------------------------- */
/* registerIndex --                                                            */
/* --------------------------------------------------------------------------- */

void IntConverter::registerIndex( GuiIndexListener *listener ){
  if( m_scale != 0 ){
    m_scale->registerIndex( listener );
  }
}

/* --------------------------------------------------------------------------- */
/* unregisterIndex --                                                          */
/* --------------------------------------------------------------------------- */

void RealConverter::unregisterIndex( GuiIndexListener *listener ){
  if( m_scale != 0 ){
    m_scale->unregisterIndex( listener );
  }
}

/* --------------------------------------------------------------------------- */
/* unregisterIndex --                                                          */
/* --------------------------------------------------------------------------- */

void IntConverter::unregisterIndex( GuiIndexListener *listener ){
  if( m_scale != 0 ){
    m_scale->unregisterIndex( listener );
  }
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool RealConverter::acceptIndex( const std::string &name, int inx ){
  if( m_scale == 0 ) return true;
  return m_scale->acceptIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool IntConverter::acceptIndex( const std::string &name, int inx ){
  if( m_scale == 0 ) return true;
  return m_scale->acceptIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool RealConverter::setIndex( const std::string &name, int inx ){
  if( m_scale == 0 ) return false;
  return m_scale->setIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool IntConverter::setIndex( const std::string &name, int inx ){
  if( m_scale == 0 ) return false;
  return m_scale->setIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void Converter::marshal( std::ostream &os ){
  os << "<Converter";
  os << " width=\"" << m_width << "\"";
  if( m_ignore_newline )
    os << " ignoreNewLine=\"true\"";
  if( m_adjustleft )
    os << " adjustleft=\"true\"";
  os << ">";
  os << "</Converter>\n";
}
void RealConverter::marshal( std::ostream &os ){
  os << "<RealConverter";
  os << " prec=\"" << m_prec << "\"";
  if( m_thousand_sep != 0x00 )
    os << " TSep=\"" << m_thousand_sep << "\"";
  os << " width=\"" << getWidth() << "\"";
  if( m_appendDot )
    os << " appendDot=\"true\"";
  os << ">\n";
  if( m_scale != 0 )
    m_scale->marshal( os );
  Converter::marshal( os );
  os << "</RealConverter>\n";
}
void ComplexConverter::marshal( std::ostream &os ){
  os << "<ComplexConverter";
  os << " width=\"" << getWidth() << "\"";
  os << ">\n";
  RealConverter::marshal( os );
  os << "</ComplexConverter>\n";
}
void IntConverter::marshal( std::ostream &os ){
  os << "<IntConverter";
  os << " width=\"" << getWidth() << "\"";
  os << ">\n";
  if( m_scale != 0 )
    m_scale->marshal( os );
  os << "</IntConverter>\n";
}
void StringConverter::marshal( std::ostream &os ){
  os << "<StringConverter";
  os << " width=\"" << getWidth() << "\"";
  os << ">";
  os << "</StringConverter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable * Converter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "Converter" ){
    std::string s;
    s=attributeList["width"];
    if( !s.empty() ){
      std::istringstream is( s );
      is >> m_width;
    }
    s=attributeList["ignoreNewLine"];
    if( !s.empty() ){
      m_ignore_newline = true;
    }
    s=attributeList["adjustleft"];
    if( !s.empty() ){
      m_adjustleft = true;
    }
  }
  return this;
}
Serializable * RealConverter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "RealConverter" ){
    std::string s;
    s=attributeList["prec"];
    if( !s.empty() ){
      std::istringstream is( s );
      is >> m_prec;
    }
    s=attributeList["TSep"];
    if( s.empty() ){
      m_thousand_sep = 0x00;
    } else {
      char c;
      std::istringstream is( s );
      is >> c;
      m_thousand_sep = c;
    }
    s=attributeList["width"];
    if( !s.empty() ){
      int iwidth;
      std::istringstream is( s );
      is >> iwidth;
      setWidth( iwidth );
    }
    s=attributeList["appendDot"];
    if( !s.empty() ){
      setAppendDot();
    }
    return this;
  }
  else if( element == "Scale" ){
    m_scale = new Scale();
    m_scale->unmarshal( element, attributeList );
    return this;
  }
  else if( element == "XferScale" ){
    m_scale = new XferScale();
    return m_scale->unmarshal( element, attributeList );
  }
  else if( element == "Converter" ){
    return Converter::unmarshal( element, attributeList );
  }
  return this;
}
Serializable * ComplexConverter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "ComplexConverter" ){
    std::string s;
    s=attributeList["width"];
    if( !s.empty() ){
      int iwidth;
      std::istringstream is( s );
      is >> iwidth;
      setWidth( iwidth );
    }
    return this;
  }
  else if( element == "RealConverter" ){
    return RealConverter::unmarshal( element, attributeList );
  }
  else if( element == "Converter" ){
    return Converter::unmarshal( element, attributeList );
  }
  else if( element == "Scale" ){
    // RealConverter instanziert new Scale
    return RealConverter::unmarshal( element, attributeList );
  }
  return this;
}
Serializable * IntConverter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "IntConverter" ){
    std::string s;
    s=attributeList["width"];
    if( !s.empty() ){
      int iwidth;
      std::istringstream is( s );
      is >> iwidth;
      setWidth( iwidth );
    }
    return this;
  }
  else if( element == "Scale" ){
    m_scale = new Scale();
    m_scale->unmarshal( element, attributeList );
    return this;
  }
  else if( element == "XferScale" ){
    m_scale = new XferScale();
    m_scale->unmarshal( element, attributeList );
    return this;
  }
  return this;
}
Serializable * StringConverter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  std::string s;
  s=attributeList["width"];
  if( !s.empty() ){
    int iwidth;
    std::istringstream is( s );
    is >> iwidth;
    setWidth( iwidth );
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void Converter::setText( const std::string &text ){
}
void RealConverter::setText( const std::string &text ){
}
void ComplexConverter::setText( const std::string &text ){
}
void IntConverter::setText( const std::string &text ){
}
void StringConverter::setText( const std::string &text ){
}
