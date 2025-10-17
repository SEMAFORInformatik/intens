
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>

#include "utils/gettext.h"
#include "utils/utils.h"
#include "datapool/DataException.h"
#include "datapool/DataRealElement.h"
#include "datapool/DataRealValue.h"
#include "datapool/DataPool.h"
#include "datapool/DataEvents.h"
#include "datapool/DataLogger.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

#define __CLASSNAME__ "DataRealElement"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataRealElement::DataRealElement()
  : m_value(0.0){
  LOG_DEBUG("(constructor)");
}

DataRealElement::~DataRealElement(){
  LOG_DEBUG("(destructor)");
}

DataRealElement::DataRealElement( const DataRealElement &ref )
  : DataElement(ref)
  , m_value(ref.m_value){
  LOG_DEBUG( "m_value=" << m_value );
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getElementType  --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataRealElement::getElementType() const{
  return DataDictionary::type_Real;
}

/* --------------------------------------------------------------------------- */
/* operator=  --                                                               */
/* --------------------------------------------------------------------------- */

bool DataRealElement::operator=( const DataElement &ref ){
  double val = 0.0;
  if( ref.getValue( val ) ){
    setValue( val, -1 ); // ohne Runden
  }
  else{
    clearElement();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

DataContainer *DataRealElement::clone() const{
  return new DataRealElement( *this );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::getValue( double &val ) const{
  LOG_DEBUG(  "getValue (double)" );

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

bool DataRealElement::setValue( double val, int rundung ){
  if( !isLocked() ){
    if( rundung >= 0 ){
      val = runden( val, rundung );
    }
    if( isValid() ){
      double value = m_value;
      if( rundung >= 0 ){
        value = runden( value, rundung );
      }
      if( val == m_value ){
        return false; // Es ändert sich nichts
      }
    }
    m_value = val;
    setValid();
    return true; // update
  }
  return false; // Passiert hier komplett still. Keine Änderung
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::getValue( int &val ) const{
  LOG_DEBUG("getValue (int)");

  double d;
  if( !getValue(d) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
  val = (int)d;
  LOG_DEBUG("rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::setValue( int val, int rundung ){
  LOG_DEBUG("val=" << val);

  return setValue( (double)val, rundung );
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::setValue( unsigned int val, int rundung ){
  LOG_DEBUG("val=" << val);

  return setValue( (double)val, rundung );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::getValue( std::string &val ) const{
  LOG_DEBUG("getValue (string)");
  double d;
  char tmp[20];

  if( !getValue(d) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
  std::ostringstream os;
  os << std::setprecision(17) << d;
  val = std::string(os.str());
  LOG_DEBUG("rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::setValue( const std::string &val, int rundung ){
  LOG_DEBUG( "val=" << val);
  double dbl[3];
  char c;
  int i = 0;
  std::istringstream is( val );

  while( i < 3 && is.rdstate() != std::ios_base::eofbit ){
    if( i ){
      is >> c >> dbl[i];
    }
    else{
      is >> dbl[i];
    }
    if ( ( i && c == '\0') && is.rdstate() & std::ios::eofbit ) {
      is.clear( std::ios::eofbit );
      break;
    }
    if( ( i && c != '/')  || is.rdstate() & std::ios::badbit || is.rdstate() & std::ios::failbit ){
      is.clear( std::ios::failbit );
      break;
    }
    c='\0';
    i++;
  }

  if (is.rdstate() ==  std::ios::eofbit) {
    switch (i) {
      case 1:
        return setValue( dbl[0], rundung );
        break;
      case 2:
        return setValue( dbl[0]/dbl[1], rundung );
        break;
      case 3:
        return setValue( dbl[0]/dbl[1]/dbl[2], rundung );
        break;
    }
  }
  else{
    std::stringstream logmsg;
    logmsg << compose( _("WARNING Cannot convert string '%1' to double."), val) << std::endl;
    DataPool::getDataEvents().writeLogMessage( logmsg.str() );
    return false;
  }
  LOG_DEBUG("rslt=false istringstream problem");
  return false;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::getValue( dComplex &val ) const{
  LOG_DEBUG("getValue (complex)");
  double d;

  if( !getValue( d ) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
  val = dComplex( d, 0.0 );
  LOG_DEBUG("rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::setValue( const dComplex &val, int rundung ){
  LOG_DEBUG( "val=" << val);

  if( val.imag() != 0 ){
    LOG_DEBUG("rslt=false Imaginary-Value != 0");
    return false;
  }
  return setValue( val.real(), rundung );
}

/* --------------------------------------------------------------------------- */
/* getDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataValue *DataRealElement::getDataValue() const {
  return new DataRealValue( m_value, getAttributes() );
}

/* --------------------------------------------------------------------------- */
/* setDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataRealElement::setDataValue( DataValue *d ){
  UpdateStatus status = NoUpdate;
  if( d ){
    if( d->isValid() ){
      double val;
      if( d->getValue( val ) ){
        if( resetAttributes( d->getAttributes() ) ){
          status = DataUpdated;
        }
        if( setValue( val, -1 ) ){ // ohne Runden
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

DataElement::UpdateStatus DataRealElement::clearValue(){
  m_value = 0.0;
  return setInvalid();
}

/* --------------------------------------------------------------------------- */
/* assignDataElement --                                                        */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataRealElement::assignDataElement( const DataElement &el ){
  double val = 0.0;
  if( el.getValue( val ) ){
    if( setValue( val, -1 ) ){ // ohne Runden
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

DataContainer *DataRealElement::copyContainer( DataPool &datapool ) const{
  DataRealElement *el = new DataRealElement();
  if( isValid() ){
    el->setValue( m_value, -1 ); // ohne Runden
  }
  if( isLocked() ){
    el->setLocked();
  }
  return el;
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataRealElement::writeXML( std::ostream &ostr
                              , std::vector<std::string> &attrs
                              , bool is_structitem
                              , int level
                              , bool debug
                              , bool isScalar )
{
  LOG_DEBUG( "writeXML");
  if( debug ){
    for( int i=0; i<level; i++ ) ostr << " ";
  }

  if( !isScalar ) {
    if( isValid() ){
      ostr << "<v>";
    }
    else {
      ostr << "<v/>\n";
      return true;
    }
  }
  if( isValid() ){
    ostr << std::setprecision( 12 ) << m_value;
  }

  if( !isScalar ){
    ostr << "</v>";
  }
  ostr << '\n';
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeJSON --                                                                */
/* --------------------------------------------------------------------------- */

bool DataRealElement::writeJSON( std::ostream &ostr,
                                 bool is_structitem,
                                 int level,
                                 int indentation,
                                 bool isScalar,
                                 double scale,
                                 const SerializableMask flags )
{
  if( isValid() ){
    if ( std::isfinite(m_value) ) {
      std::ostringstream os;
      os << std::setprecision( 12 ) << (m_value * scale);
      std::string val=os.str();
      if( val.find ( '.' )==std::string::npos &&
          val.find ( 'E' )==std::string::npos &&
          val.find ( 'e' )==std::string::npos ){
        val.append(".0");
      }
      ostr << val;
    }
    else{
      if ( m_value < 0 ) ostr << "-";
      ostr << "Infinity";
    }
  }
  else {
    ostr << "null";
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDPvalue --                                                             */
/* --------------------------------------------------------------------------- */

void DataRealElement::writeDPvalue( std::ostream &ostr ) const{
  if( isValid() ){
    ostr << m_value;
  }
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataRealElement::compare( const DataContainer &cont
                                          , int fall
                                          , int max_faelle )
{
  const DataElement *el = cont.getDataElement();
  double val = 0.0;
  bool equal = false;
  if( el->getValue( val ) ){
    equal = val == m_value;
  }
  return result_of_compare( isValid(), el->isValid(), equal );
  ThrowDpException( "compare", "Ein Compare ist fatal" );
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

void *DataRealElement::getPython(){
#if defined(HAVE_PYTHON)
  if( isValid() ){
    return PyFloat_FromDouble( m_value );
  }
  return PythonObject::getEmptyObject();
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataRealElement::setPython( const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  LOG_DEBUG("DataRealElement::setPython");
  assert( obj.hasPyObject() );

  double val = 0.0;
  if( PyFloat_Check( obj ) != 0 ){
    val = PyFloat_AsDouble( obj );
  }
  else
  if( PyLong_Check( obj ) != 0 ){
    val = PyLong_AsDouble( obj );
  }
  else{
    LOG_DEBUG("clear");
    return clearValue();
  }
  LOG_DEBUG("set value " << val );
  if( setValue( val, -1 ) ){
    return ValueUpdated;
  }
#endif
  return NoUpdate;
}
