
#include <sstream>
#include "xfer/XferParameterComplex.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XferParameterComplex::XferParameterComplex( DataReference *dref
		                          , int width, int prec, Scale *scale, bool default_radix_char )
  : XferDataParameter( dref )
  , m_conv( width, prec, scale, false, default_radix_char ){
}

XferParameterComplex::XferParameterComplex( XferParameterComplex &param )
  : XferDataParameter( param )
  , m_conv( param.m_conv ){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

XferDataParameter *XferParameterComplex::clone(){
  return new XferParameterComplex( *this );
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool XferParameterComplex::read( std::istream &is ){
  BUG_DEBUG("read");
  assert( m_dataitem != 0 );
  std::complex<double> rslt;
  // converterSetScaleIndexes( m_conv );
  if( !m_conv.read( is, rslt ) ){
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

bool XferParameterComplex::write( std::ostream &os ){
  BUG_DEBUG("write");
  assert( m_dataitem != 0 );
  std::complex<double> rslt;
  if( !m_dataitem->getValue( rslt ) ){
    BUG_DEBUG("invalid value");
    return false;
  }
  // converterSetScaleIndexes( m_conv );
  return m_conv.write( rslt, os );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XferParameterComplex::getFormattedValue( std::string &text ){
  BUG_DEBUG("getFormattedValue");
  assert( m_dataitem != 0 );
  std::complex<double> rslt;
  if( !m_dataitem->getValue( rslt ) ){
    text.erase();
    return;
  }

  std::ostringstream output;
  // converterSetScaleIndexes( m_conv );
  m_conv.write( rslt, output );
  text = output.str();
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferParameterComplex::setFormattedValue( const std::string &text ){
  BUG_DEBUG("setFormattedValue");
  assert( m_dataitem != 0 );
  std::istringstream input( text );
  std::complex<double> rslt;
  // converterSetScaleIndexes( m_conv );
  if( !m_conv.readAll( input, rslt ) ){
    return false;
  }
  m_dataitem->setValue( rslt );
  return true;
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

XferParameter::InputStatus XferParameterComplex::checkFormat( const std::string &text ){
  std::complex<double> val;
  bool is_valid = m_dataitem->getValue( val );
  if( isEmpty( text ) ){
    if( !is_valid ){
      return status_Unchanged;
    }
    return status_Changed;
  }
  std::istringstream input( text );
  std::complex<double> rslt;
  // converterSetScaleIndexes( m_conv );
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

bool XferParameterComplex::convertValue( const std::string &text ){
  std::istringstream input( text );
  std::complex<double> rslt;
  if( !m_conv.readAll( input, rslt ) ){
    return false;
  }
  std::ostringstream output;
  m_conv.write( rslt, output );
  return text == output.str();
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */

bool XferParameterComplex::setLength( int len ){
  m_conv.setWidth( len );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setPrecision --                                                             */
/* --------------------------------------------------------------------------- */

bool XferParameterComplex::setPrecision( int prec ){
  m_conv.setPrecision( prec );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */

bool XferParameterComplex::setScalefactor( Scale *scale ){
  m_conv.setScale( scale );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getAttributes --                                                            */
/* --------------------------------------------------------------------------- */

DATAAttributeMask XferParameterComplex::getAttributes( TransactionNumber t ){
  converterSetScaleIndexes( m_conv );
  DATAAttributeMask mask = m_conv.getAttributes( t );
  if( mask & DATAisGuiUpdated )
    return XferDataParameter::getAttributes( t ) | mask;
  return XferDataParameter::getAttributes( t );
}
