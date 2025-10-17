
#include "utils/Debugger.h"

#include "datapool/DataReference.h"
#include "streamer/StringStreamParameter.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"
#include "utils/LaTeXConverter.h"
#include "utils/UrlConverter.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

StringStreamParameter::StringStreamParameter(XferDataItem *dref, int level
					     , StructStreamParameter *p
						 , char delimiter, bool locale)
  : DataStreamParameter(dref,level,p,false,delimiter,locale) {
}

StringStreamParameter::StringStreamParameter(XferDataItem *ditem, int level
					     , int width, char delimiter, bool locale, bool mand )
  : DataStreamParameter(ditem,level,0,mand,delimiter, locale)
  , m_conv(width) {
}

StringStreamParameter::~StringStreamParameter(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool StringStreamParameter::read( std::istream &is ){
  std::string s;
  BUG_PARA(BugStreamer, "StringStreamParameter::read( istream & )" ,getName() );
  if( m_conv.read( is, s, m_delimiter ) ){
    if( s.size() > 0 ){
      // !! append option set ?
      if (hasAppendOption()) {
	std::string oldValue;
	if ( m_dataitem->getValue(oldValue) ) {
	  m_dataitem->setValue( oldValue + s );
	}
      } else
      m_dataitem->setValue( s );
    }
    BUG_MSG( "varname:'" <<  m_dataitem->getFullName(true) << "' val='" << s <<"'" );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool StringStreamParameter::write( std::ostream &os ) {
  std::string s;
  if( m_dataitem->getValue( s ) ){
    if( m_format == HardCopyListener::LaTeX )
      LaTeXConverter::convert2LaTeX( s );
    if( m_format == HardCopyListener::URL )
      UrlConverter::urlQuote( s );
    m_conv.write( s, os );
    int width = m_conv.getWidth();
    if( width == 0 &&
        m_format != HardCopyListener::URL // no delimiter for URLs
        )
      os << (m_delimiter == '\0' ? ' ' : m_delimiter);
  }
  else{
    int width = m_conv.getWidth();
    if( width == 0 ) {
      if ( m_format != HardCopyListener::URL ) // no delimiter for URLs
        os << (m_delimiter == '\0' ? ' ' : m_delimiter);
    } else
      for( int n = 0; n < width; ++n )
	os << ' ';
  }

  return true;
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void StringStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  BUG( BugStreamer, "StringStreamParameter::putValues" );
  if( dest != 0 ){
    BUG_MSG( getName() );
    std::string s;
    createDataItemIndexes();
    setDataItemIndexes();
    if( m_dataitem->getValue(s) ){
      dest -> putValue(*this, s );
    }
    else
      dest -> putValue( *this, "" );
    removeDataItemIndexes();
  }
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void StringStreamParameter::setValue( double v ){
  std::ostringstream os;
  RealConverter( 0, -1 , 0, false, !isLocale() ).write( v, os );
  m_dataitem->setValue(os.str());
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool StringStreamParameter::getValue( double &v )const{
  std::string s;
  if(m_dataitem->getValue(s)){
    std::istringstream is(s);
    return RealConverter( 0, -1 , 0, false, !isLocale() ).read( is, v, m_delimiter );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void StringStreamParameter::marshal( std::ostream &os ){
  os << "<StringStreamParameter";
  if( isMandatory() )
    os << " mand=\"true\"";
  if( isScalar() )
    os << " scalar=\"true\"";
  if( isCell() )
    os << " cell=\"true\"";
  os << " delimiter=\"" << m_delimiter << "\"";
  os << ">\n";
  m_dataitem -> marshal( os );
  m_conv.marshal( os );
  os << "</StringStreamParameter>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *StringStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "StringStreamParameter" ){
    GuiIndex::clearIndexContainer();
    std::string s;
    s = attributeList["mand"];
    if( !s.empty() )
      setMandatory();
    s = attributeList["scalar"];
    if( !s.empty() )
      setScalar( true );
    s = attributeList["cell"];
    if( !s.empty() )
      setCell( true );
    s = attributeList["delimiter"];
    m_delimiter = s.size() ? s[0] : ' ';
    return this;
  }
  else if( element == "StringConverter" ){
    m_conv.unmarshal( element, attributeList );
    return 0;
  }
  else if( element == "XferDataItem" ){
    m_dataitem -> unmarshal( element, attributeList );
    return m_dataitem;
  }
  else if( element == "/StringStreamParameter" ){
    GuiIndex::registerIndexContainer( this );
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void StringStreamParameter::setText( const std::string &text ){
}
