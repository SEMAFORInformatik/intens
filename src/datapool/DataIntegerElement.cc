
#include <iostream>

#include "utils/StringUtils.h"
#include "utils/gettext.h"
#include "utils/utils.h"
#include "datapool/DataIntegerElement.h"
#include "datapool/DataIntegerValue.h"
#include "datapool/DataPool.h"
#include "datapool/DataEvents.h"
#include "datapool/DataLogger.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataIntegerElement::DataIntegerElement()
  : m_value( 0 ){
  LOG_DEBUG("(constructor)");
}

DataIntegerElement::~DataIntegerElement(){
  LOG_DEBUG("(destructor)");
}

DataIntegerElement::DataIntegerElement( const DataIntegerElement &ref )
  : DataElement( ref )
  , m_value( ref.m_value ){
  LOG_DEBUG("m_value=" << m_value);
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getElementType  --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataIntegerElement::getElementType() const{
  return DataDictionary::type_Integer;
}

/* --------------------------------------------------------------------------- */
/* operator=  --                                                               */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::operator=( const DataElement &ref ){
  int val = 0;
  if( ref.getValue( val ) ){
    setValue( val, -1 ); // ohne Rundung (logisch)
  }
  else{
    clearElement();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

DataContainer *DataIntegerElement::clone() const{
  return new DataIntegerElement( *this );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::getValue( int &val ) const{
  LOG_DEBUG(  "getValue (int)" );
  if( isValid() ){
    val = m_value;
    LOG_DEBUG( "rslt=true value=" << val);
    return true;
  }
  LOG_DEBUG("rslt=false");
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::setValue( int val, int rundung ){
  if( !isLocked() ){
    if( isValid() && val == m_value ){
      return false; // Es ändert sich nichts
    }
    m_value = val;
    setValid();
    return true;
  }
  return false;  // Passiert hier komplett still
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::setValue( unsigned int val, int rundung ){
  if( !isLocked() ){
    if( isValid() && val == m_value ){
      return false; // Es ändert sich nichts
    }
    m_value = val;
    setValid();
    return true;
  }
  return false;  // Passiert hier komplett still
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::getValue( double &val ) const{
  LOG_DEBUG("getValue (double)");
  int d;
  if( !getValue(d) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
  val = d;
  LOG_DEBUG("rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::setValue( double val, int rundung ){
  LOG_DEBUG("val=" << val);

  int i = (int)val;
  return setValue( i, -1 ); // ohne Rundung
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::getValue( std::string &val ) const{
  LOG_DEBUG("getValue (string)");
  int i = 0;

  if( !getValue( i ) ){
    LOG_DEBUG( "rslt=false" );
    return false;
  }
  std::ostringstream txt;
  txt << i;
  val = txt.str();
  LOG_DEBUG( "rslt=true value=" << val );
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::setValue( const std::string &val, int rundung ){
  LOG_DEBUG("val=" << val);
  int d;

  std::istringstream is( val  );
  is >> d;
  if (is.rdstate() & std::ios::failbit) {
    LOG_DEBUG("rslt=false conversion problem");
    std::stringstream logmsg;
    logmsg << compose( _("WARNING Cannot convert string '%1' to natural number."), val)
           << std::endl;
    DataPool::getDataEvents().writeLogMessage( logmsg.str() );
    return false;
  }
  return setValue( d, -1 ); // ohne Rundung
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::getValue( dComplex &val ) const{
  LOG_DEBUG("getValue (complex)");
  int d;

  if( !getValue(d) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
  val = dComplex(d,0);
  LOG_DEBUG("rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::setValue( const dComplex &val, int rundung ){
  LOG_DEBUG("val=" << val);

  int i = (int)val.real();

  if( val.imag() > 0 ){
    LOG_DEBUG("rslt=false Imaginaeranteil > 0");
    return false;
  }
  return setValue( i, -1 ); // ohne Rundung
}

/* --------------------------------------------------------------------------- */
/* getDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataValue *DataIntegerElement::getDataValue() const {
  return new DataIntegerValue( m_value, getAttributes() );
}

/* --------------------------------------------------------------------------- */
/* setDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataIntegerElement::setDataValue( DataValue *d ){
  UpdateStatus status = NoUpdate;
  if( d ){
    if( d->isValid() ){
      int val;
      if( d->getValue( val ) ){
        if( resetAttributes( d->getAttributes() ) ){
          status = DataUpdated;
        }
        if( setValue( val, -1 ) ){ // ohne Rundung
          status = ValueUpdated;
        }
        return status;
      }
    }

    // Das valid-Bit wird bei einem reset nicht verändert.
    if( resetAttributes( d->getAttributes() ) ){
      status = DataUpdated;
    }
  }
  return maximum( status, clearValue() );
}

/* --------------------------------------------------------------------------- */
/* clearValue --                                                               */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataIntegerElement::clearValue(){
  m_value = 0;
  return setInvalid();
}

/* --------------------------------------------------------------------------- */
/* assignDataElement --                                                        */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataIntegerElement::assignDataElement( const DataElement &el ){
  int val = 0;
  if( el.getValue( val ) ){
    if( setValue( val, -1 ) ){ // ohne Rundung
      return ValueUpdated;
    }
  }
  else{
    return clearElement();
  }
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* copyContainer --                                                            */
/* --------------------------------------------------------------------------- */

DataContainer *DataIntegerElement::copyContainer( DataPool &datapool ) const{
  DataIntegerElement *el = new DataIntegerElement();
  if( isValid() ){
    el->setValue( m_value, -1 ); // ohne Rundung
  }
  if( isLocked() ){
    el->setLocked();
  }
  return el;
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::writeXML( std::ostream &ostr
                                 , std::vector<std::string> &attrs
                                 , bool is_structitem
                                 , int level
                                 , bool debug
                                 , bool isScalar )
{
  LOG_DEBUG(  "write");
  if( debug )
    for( int i=0; i<level; i++ ) ostr << " ";
  if( !isScalar ) {
    if( isValid() ){ ostr << "<v>"; }
    else {
      ostr << "<v/>\n";
      return true;
    }
  }
  if( isValid() ) ostr << m_value;
  if( !isScalar ) ostr << "</v>";
  ostr << '\n';

  return true;
}

/* --------------------------------------------------------------------------- */
/* writeJSON --                                                                */
/* --------------------------------------------------------------------------- */

bool DataIntegerElement::writeJSON( std::ostream &ostr,
                                    bool is_structitem,
                                    int level,
                                    int indentation,
                                    bool isScalar,
                                    double scale,
                                    const SerializableMask flags )
{
  if( isValid() ){
    ostr << static_cast<int>(std::round(m_value * scale));
  }
  else{
    ostr << "null";
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDPvalue --                                                             */
/* --------------------------------------------------------------------------- */

void DataIntegerElement::writeDPvalue( std::ostream &ostr ) const{
  if( isValid() ){
    ostr << m_value;
  }
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataIntegerElement::compare( const DataContainer &cont
                                             , int fall
                                             , int max_faelle )
{
  const DataElement *el = cont.getDataElement();
  int val = 0;
  bool equal = false;
  if( el->getValue( val ) ){
    equal = val == m_value;
  }
  return result_of_compare( isValid(), el->isValid(), equal );
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

void *DataIntegerElement::getPython(){
#if defined(HAVE_PYTHON)
  if( isValid() ){
    return PyLong_FromLong( m_value );
  }
  return PythonObject::getEmptyObject();
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataIntegerElement::setPython( const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  LOG_DEBUG("DataIntegerElement::setPython");
  assert( obj.hasPyObject() );

  if( PyLong_Check( obj ) != 0 ){
    long val = PyLong_AsLong( obj );
    if( setValue( (int)val, 0 ) ){
      return ValueUpdated;
    }
  }
  else{
    return clearValue();
  }
#endif
  return NoUpdate;
}
