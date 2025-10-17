
#include <sstream>

#include "xfer/XferParameterReal.h"

#include "app/AppData.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XferParameterReal::XferParameterReal( DataReference *dref
				      , int width, int prec, Scale *scale, bool default_radix_char, char delimiter )
  : XferDataParameter( dref )
  , m_conv( width, prec, scale, false, default_radix_char )
  , m_delimiter( delimiter) {
}

XferParameterReal::XferParameterReal( XferParameterReal &param )
  : XferDataParameter( param )
  , m_conv( param.m_conv )
  , m_delimiter( param.m_delimiter ) {
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

XferDataParameter *XferParameterReal::clone(){
  return new XferParameterReal( *this );
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::read( std::istream &is ){
  BUG_DEBUG("read");
  assert( m_dataitem != 0 );
  double rslt;
  converterSetScaleIndexes( m_conv );
  if( !m_conv.read( is, rslt, m_delimiter ) ){
    BUG_DEBUG("read of converter failed");
    return false;
  }
  m_dataitem->setValue( rslt );
  BUG_DEBUG("Value is " << rslt);
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::write( std::ostream &os ){
  BUG_DEBUG("write");
  assert( m_dataitem != 0 );
  double rslt;
  if( !m_dataitem->getValue( rslt ) ){
    BUG_DEBUG("invalid value");
    return false;
  }
  converterSetScaleIndexes( m_conv );
  return m_conv.write( rslt, os );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XferParameterReal::getFormattedValue( std::string &text ){
  BUG_DEBUG("getFormattedValue( string )");
  assert( m_dataitem != 0 );
  double rslt;
  if( !m_dataitem->getValue( rslt ) ){
    // value is INVALID
    text.erase();
    return;
  }

  std::ostringstream output;
  converterSetScaleIndexes( m_conv );
  m_conv.write( rslt, output );
  text = output.str();
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::setFormattedValue( const std::string &text ){
  BUG_DEBUG("setFormattedValue( string )");
  assert( m_dataitem != 0 );
  std::istringstream input( text );
  double rslt;
  converterSetScaleIndexes( m_conv );
  if( !m_conv.readAll( input, rslt ) ){
    return false;
  }
  DES_INFO("  " << m_dataitem->getFullName(true)  << " = " << rslt<< ";");
  if (AppData::Instance().PyLogMode()) {
    double scale = m_conv.getScale();
    if(std::fabs(scale - 1.0) < 1e-6) {  // scale factor 1 is not logged
      PYLOG_INFO(compose(PYLOG_SETVALUE,
                         removeString2(m_dataitem->getFullName(true), "[0]"),
                         rslt));
    } else {
      PYLOG_INFO(compose(PYLOG_SETVALUE_WITHFACTOR,
                         removeString2(m_dataitem->getFullName(true), "[0]"),
                         rslt * scale,
                         1.0 / scale));
    }
    return true;
  }
  m_dataitem->setValue( rslt );
  return true;
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus XferParameterReal::checkFormat( const std::string &text ){
  double val;
  bool is_valid = m_dataitem->getValue( val );
  if( isEmpty( text ) ){
    if( !is_valid ){
      return status_Unchanged;
    }
    return status_Changed;
  }
  std::istringstream input( text );
  double rslt;
  converterSetScaleIndexes( m_conv );
  if( !m_conv.readAll( input, rslt ) ){
    return status_Bad;
  }
  if( is_valid ){
    if( val == rslt ){
      return status_Unchanged;
    }
  }
  return status_Changed;
}

/* --------------------------------------------------------------------------- */
/* convertValue --                                                             */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::convertValue( const std::string &text ){
  BUG_DEBUG("convertValue '" << text << "'" );
  if( isEmpty( text ) ) return true;

  std::istringstream input( text );
  double rslt;
  if( !m_conv.readAll( input, rslt ) ){
    BUG_DEBUG(  "readAll failed with input '" << text << "'" );
    return false;
  }
  std::ostringstream output;
  m_conv.write( rslt, output );
  BUG_DEBUG( "write(" << rslt << ") => '" << output.str() << "'" );
  return text == output.str();
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::setLength( int len ){
  m_conv.setWidth( len );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setPrecision --                                                             */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::setPrecision( int prec ){
  m_conv.setPrecision( prec );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getPrecision --                                                             */
/* --------------------------------------------------------------------------- */

int XferParameterReal::getPrecision() {
  return m_conv.getPrecision();
}

/* --------------------------------------------------------------------------- */
/* setThousandSep --                                                           */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::setThousandSep(){
  m_conv.setThousandSep();
  return true;
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::setScalefactor( Scale *scale ){
  m_conv.setScale( scale );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

Scale* XferParameterReal::getScalefactor(){
  return m_conv.scale();
}

/* --------------------------------------------------------------------------- */
/* getAttributes --                                                            */
/* --------------------------------------------------------------------------- */

DATAAttributeMask XferParameterReal::getAttributes( TransactionNumber t ){
  converterSetScaleIndexes( m_conv );
  DATAAttributeMask mask = m_conv.getAttributes( t );
  if( mask & DATAisGuiUpdated )
    return XferDataParameter::getAttributes( t ) | DATAisGuiUpdated;
  return XferDataParameter::getAttributes( t );
}

/* --------------------------------------------------------------------------- */
/* registerIndex --                                                            */
/* --------------------------------------------------------------------------- */

void XferParameterReal::registerIndex( GuiIndexListener *listener ){
  m_conv.registerIndex( listener );
}

/* --------------------------------------------------------------------------- */
/* unregisterIndex --                                                          */
/* --------------------------------------------------------------------------- */

void XferParameterReal::unregisterIndex( GuiIndexListener *listener ){
  m_conv.unregisterIndex( listener );
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::acceptIndex( const std::string &name, int inx ){
  return m_conv.acceptIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferParameterReal::setIndex( const std::string &name, int inx ){
  return m_conv.setIndex( name, inx );
}
