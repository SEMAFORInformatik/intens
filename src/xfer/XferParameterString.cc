
#include <sstream>
#include "xfer/XferParameterString.h"
#include "app/DataPoolIntens.h"

#include "utils/StringUtils.h"

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XferParameterString::XferParameterString( DataReference *dref
                                        , int width, char delimiter )
  : XferDataParameter( dref )
  , m_conv( width )
  , m_delimiter( delimiter ){
}

XferParameterString::XferParameterString( XferParameterString &param )
  : XferDataParameter( param )
  , m_conv( param.m_conv ){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

XferDataParameter *XferParameterString::clone() {
  return new XferParameterString( *this );
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool XferParameterString::read( std::istream &is ){
  BUG_DEBUG("read");
  assert( m_dataitem != 0 );
  std::string rslt;
  if( !m_conv.read( is, rslt, m_delimiter ) ){
    BUG_DEBUG("read of converter failed");
    return false;
  }
  m_dataitem->setValue( rslt );
  BUG_DEBUG("Value is '" << rslt << "'");
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool XferParameterString::write( std::ostream &os ){
  BUG_DEBUG("write");
  assert( m_dataitem != 0 );
  std::string rslt;
  if( !m_dataitem->getValue( rslt ) ){
    BUG_DEBUG("invalid value");
    return false;
  }
  return m_conv.write( rslt, os );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XferParameterString::getFormattedValue( std::string &text ){
  BUG_DEBUG("getFormattedValue( string )");
  assert( m_dataitem != 0 );
  std::string rslt;
  if( !m_dataitem->getValue( rslt ) ){
    text.erase();
    BUG_DEBUG("Value cleared");
    return;
  }
  std::ostringstream output;
  m_conv.write( rslt, output );
  text = output.str();
  BUG_DEBUG("Value is " << rslt << ", formatted = '" << text << "'");
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferParameterString::setFormattedValue( const std::string &text ){
  BUG_DEBUG("setFormattedValue( string )");
  assert( m_dataitem != 0 );
  std::istringstream input( text );
  std::string rslt;
  if( !m_conv.readAll( input, rslt ) ){
    return false;
  }
  if (m_dataitem->getFullName(false) != INTERNAL_CYCLE_NAME) {
    DES_INFO("  " << m_dataitem->getFullName(true)  << " = \"" << rslt<< "\";");
    if (AppData::Instance().PyLogMode()) {
      PYLOG_INFO(compose(PYLOG_SETVALUE, removeString2(m_dataitem->getFullName(true), "[0]"), compose("'%1'", rslt)));
      return true;
    }
  }
  m_dataitem->setValue( rslt );
  BUG_DEBUG("Value is " << rslt << ", unformatted = '" << text << "'");
  return true;
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus XferParameterString::checkFormat( const std::string &text ){
  BUG_DEBUG("XferParameterString::checkFormat '" << text << "'" );
  std::string val;
  bool is_valid = m_dataitem->getValue( val );
  if( isEmpty( text ) ){
    if( !is_valid ){
      return status_Unchanged;
    }
    return status_Changed;
  }
  std::istringstream input( text );
  std::string rslt;
  if( !m_conv.readAll( input, rslt ) ){
    return status_Bad;
  }
  if( is_valid ){
      BUG_DEBUG("XferParameterString::checkFormat isValid '"
		    << val << "' == '"
		    << rslt << "'" );
    if( val == rslt ){
      return status_Unchanged;
    }
  }
  return status_Changed;
}

/* --------------------------------------------------------------------------- */
/* convertValue --                                                             */
/* --------------------------------------------------------------------------- */

bool XferParameterString::convertValue( const std::string &text ){
  BUG_DEBUG("convertValue '" << text << "'" );
  if( isEmpty( text ) ) return true;

  std::istringstream input( text );
  std::string rslt;
  if( !m_conv.readAll( input, rslt ) ){
    BUG_DEBUG(  "readAll failed with input '" << text << "'" );
    return false;
  }
  std::ostringstream output;
  m_conv.write( rslt, output );
  BUG_DEBUG("write(" << rslt << ") => '" << output.str() << "'" );
  return text == output.str();
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool XferParameterString::setLength( int len ){
  m_conv.setWidth( len );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool XferParameterString::setScalefactor( Scale *scale ){
  // no sense, delete scale
  delete scale;
  return false;
}
