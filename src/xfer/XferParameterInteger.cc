
#include <sstream>
#include "xfer/XferParameterInteger.h"

#include "app/AppData.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XferParameterInteger::XferParameterInteger( DataReference *dref, int width, Scale *scale, char delimiter )
  : XferDataParameter( dref )
  , m_conv( width, scale, false )
  , m_delimiter( delimiter) {
}

XferParameterInteger::XferParameterInteger( XferParameterInteger &param )
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

XferDataParameter *XferParameterInteger::clone(){
  return new XferParameterInteger( *this );
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool XferParameterInteger::read( std::istream &is ){
  BUG_DEBUG("read");
  assert( m_dataitem != 0 );
  int rslt;
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

bool XferParameterInteger::write( std::ostream &os ){
  BUG_DEBUG("write");
  assert( m_dataitem != 0 );
  int rslt;
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

void XferParameterInteger::getFormattedValue( std::string &text ){
  assert( m_dataitem != 0 );
  int rslt;
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

bool XferParameterInteger::setFormattedValue( const std::string &text ){
  assert( m_dataitem != 0 );
  std::istringstream input( text );
  int rslt;
  converterSetScaleIndexes( m_conv );
  if( !m_conv.readAll( input, rslt, m_delimiter ) ){
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
/* checkValue --                                                               */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus XferParameterInteger::checkFormat( const std::string &text ){
  int val;
  bool is_valid = m_dataitem->getValue( val );
  if( isEmpty( text ) ){
    if( !is_valid ){
      return status_Unchanged;
    }
    return status_Changed;
  }
  std::istringstream input( text );
  int rslt;
  converterSetScaleIndexes( m_conv );
  if( !m_conv.readAll( input, rslt, m_delimiter ) ){
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

bool XferParameterInteger::convertValue( const std::string &text ){
  BUG_DEBUG("convertValue '" << text << "'" );
  if( isEmpty( text ) ) return true;

  std::istringstream input( text );
  int rslt;
  if( !m_conv.readAll( input, rslt, m_delimiter ) ){
    BUG_DEBUG(  "readAll failed with input '" << text << "'" );
    return false;
  }
  std::ostringstream output;
  m_conv.write( rslt, output );
  BUG_DEBUG("write(" << rslt << ") => '" << output.str() << "'" );
  return text == output.str();
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

// used for gui index
bool XferParameterInteger::read( std::istream &istr, int &i ){
  converterSetScaleIndexes( m_conv );
  return m_conv.read( istr, i, m_delimiter );
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool XferParameterInteger::setLength( int len ){
  m_conv.setWidth( len );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool XferParameterInteger::setScalefactor( Scale *scale ){
  m_conv.setScale( scale );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

Scale* XferParameterInteger::getScalefactor(){
  return m_conv.scale();
}

/* --------------------------------------------------------------------------- */
/* getAttributes --                                                            */
/* --------------------------------------------------------------------------- */

DATAAttributeMask XferParameterInteger::getAttributes( TransactionNumber t ){
  converterSetScaleIndexes( m_conv );
  DATAAttributeMask mask = m_conv.getAttributes( t );
  if( mask & DATAisGuiUpdated )
    return XferDataParameter::getAttributes( t ) | DATAisGuiUpdated;
  return XferDataParameter::getAttributes( t );
}

/* --------------------------------------------------------------------------- */
/* registerIndex --                                                            */
/* --------------------------------------------------------------------------- */

void XferParameterInteger::registerIndex( GuiIndexListener *listener ){
  m_conv.registerIndex( listener );
}

/* --------------------------------------------------------------------------- */
/* unregisterIndex --                                                          */
/* --------------------------------------------------------------------------- */

void XferParameterInteger::unregisterIndex( GuiIndexListener *listener ){
  m_conv.unregisterIndex( listener );
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool XferParameterInteger::acceptIndex( const std::string &name, int inx ){
  return m_conv.acceptIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferParameterInteger::setIndex( const std::string &name, int inx ){
  return m_conv.setIndex( name, inx );
}
