
#include <iostream>
#include <stdio.h>

#include "utils/StringUtils.h"
#include "utils/gettext.h"
#include "utils/utils.h"
#include "datapool/DataComplexElement.h"
#include "datapool/DataComplexValue.h"
#include "datapool/DataPool.h"
#include "datapool/DataEvents.h"
#include "datapool/DataLogger.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataComplexElement::DataComplexElement()
  : m_value(0.0,0.0){
  LOG_DEBUG("(constructor)");
}

DataComplexElement::~DataComplexElement(){
  LOG_DEBUG("(destructor)");
}

DataComplexElement::DataComplexElement( const DataComplexElement &ref )
  : DataElement(ref)
  , m_value(ref.m_value){
  LOG_DEBUG("value=" << m_value);
}

/* --------------------------------------------------------------------------- */
/* operator=  --                                                               */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::operator=( const DataElement &ref ){
  dComplex val = dComplex(0.0,0.0);
  if( ref.getValue( val ) ){
    setValue( val, -1 ); // ohne Rundung
  }
  else{
    clearElement();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

DataContainer *DataComplexElement::clone() const{
  return new DataComplexElement( *this );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::getValue( dComplex &val ) const{
  LOG_DEBUG("getValue(complex)" );

  if( isValid() ){
    val = m_value;
    LOG_DEBUG("rslt=true val=" << val);
    return true;
  }
  LOG_DEBUG("rslt=false");
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::setValue( const dComplex &val, int rundung ){
  if( !isLocked() ){
    dComplex cval = val;

    if( rundung >= 0 ){
      double real = runden( val.real(), rundung );
      double imag = runden( val.imag(), rundung );
      cval = dComplex( real, imag );
    }
    if( isValid() ){
      dComplex cvalue = m_value;
      if( rundung >= 0 ){
        double real = runden( m_value.real(), rundung );
        double imag = runden( m_value.imag(), rundung );
        cvalue = dComplex( real, imag );
      }
      if( cval == cvalue ){
        return false; // Es ändert sich nichts
      }
    }
    m_value = cval;
    setValid();
    return true; // update
  }
  return false;  // Passiert hier komplett still. Keine Änderung
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::getValue( int &val ) const{
  LOG_DEBUG("getValue(int)");
  dComplex d;

  if( !getValue(d) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
  if( d.imag() != 0 ){
    LOG_DEBUG("rslt=false (Imag != 0)");
    return false;
  }
  val = (int)d.real();
  LOG_DEBUG("rslt=true val=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::setValue( int val, int rundung ){
  LOG_DEBUG("val=" << val);

  dComplex d = dComplex( val, 0.0 );
  return setValue( d, rundung );
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::setValue( unsigned int val, int rundung ){
  LOG_DEBUG("val=" << val);

  dComplex d = dComplex( val, 0.0 );
  return setValue( d, rundung );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::getValue( double &val ) const{
  LOG_DEBUG("getValue(double)");
  dComplex d;

  if( !getValue(d) ){
    LOG_DEBUG( "rslt=false");
    return false;
  }
  if( d.imag() != 0 ){
    LOG_DEBUG("rslt=false (Imag != 0)");
    return false;
  }
  val = d.real();
  LOG_DEBUG("rslt=true val=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::setValue( double val, int rundung ){
  LOG_DEBUG("val=" << val);

  dComplex d = dComplex( val ,0.0 );
  return setValue( d, rundung );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::getValue( std::string &val ) const{
  LOG_DEBUG("getValue(string)");
  dComplex d;
  char tmp[45];

  if( !getValue(d) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
#if defined(__DP__STANDALONE_VS2013__)
  sprintf_s( tmp, "complex(%g,%g)", d.real(), d.imag() );
#else
  sprintf( tmp, "complex(%g,%g)", d.real(), d.imag() );
#endif
  val = std::string(tmp);
  LOG_DEBUG("rslt=true val=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataComplexElement::setValue( const std::string &val, int rundung ){
  LOG_DEBUG("val=" << val);
  double d1,d2;

  /*********************************************************/
  /* Jetzt werde ich versuchen, auf verschiedene Arten eine*/
  /* complexe Zahl zu scannen.                             */
  /*********************************************************/

  // Die Hüllen um die Zahlen loeschen, wenn vorhanden
  // möglich (Case insenitiv)
  //      complex(%1,%2)
  //      cplx(%1,%2)
  // das innere (%1,%2) kann mit oder ohne Komma sein
  //
  std::vector<std::string> leading_strs;
  leading_strs.push_back("complex");
  leading_strs.push_back("cplx");
  std::string str( lower( val ) );

  // blanks entfernen
  while (str[0] == ' ')  // remove leading blanks
    str = str.substr(1);
  while (str[str.size()-1] == ' ')  // remove ending blanks
    str = str.substr( 0, str.size()-1);

  // Hüllen entfernen
 std::vector<std::string>::iterator itl = leading_strs.begin();
  for (; itl!= leading_strs.end(); ++itl) {
    if ( str.find( (*itl) ) == 0) {
      str = str.substr( std::string( (*itl) ).size() );
      if (str.empty()) break;
      while (str[0] == ' ')  // remove leading blanks
        str = str.substr(1);
      break;
    }
  }

  // Klammer entfernen
  if ( str[0] == '(' && str[str.size()-1] == ')' ) { // remove brakets
   str = str.substr(1, str.size()-2);
     while (str[str.size()-1] == ' ')  // remove ending blanks
       str = str.substr( 0, str.size()-1);
  }
  else{
//     if (itl != leading_strs.end()) {
      // hier muessten Klammer stehen!!!
    LOG_DEBUG("rslt=false (cannot see Complex in String) [" << val << "]");
    std::stringstream logmsg;
    logmsg << compose( _("WARNING Cannot convert string '%1' to complex number."), val) << std::endl;
    DataPool::getDataEvents().writeLogMessage( logmsg.str() );
    return false;
//     }
  }

  int i = 0;
  char co;
  std::istringstream isA( str  );
  std::istringstream isB( str  );
  isA >> d1 >> co >> d2;
  if (isA.rdstate() !=  std::ios_base::eofbit) {
    isA >> d1 >> d2;
    if (isB.rdstate() !=  std::ios_base::eofbit) {
      std::stringstream logmsg;
      logmsg << compose( _("WARNING Cannot convert string '%1' to complex number."), val) << std::endl;
      DataPool::getDataEvents().writeLogMessage( logmsg.str() );
      return false;
    }
  }
  return setValue( dComplex( d1, d2 ), rundung );


//   if (sscanf(val.c_str(),"%lg,%lg"           , &d1, &d2) == 2    ||
//       sscanf(val.c_str(),"%lg %lg"           , &d1, &d2) == 2    ||
//       sscanf(val.c_str(),"complex(%lg,%lg)"  , &d1, &d2) == 2    ||
//       sscanf(val.c_str(),"COMPLEX(%lg,%lg)"  , &d1, &d2) == 2    ||
//       sscanf(val.c_str(),"cplx(%lg,%lg)"     , &d1, &d2) == 2    ||
//       sscanf(val.c_str(),"CPLX(%lg,%lg)"     , &d1, &d2) == 2      ){
//     return setValue(dComplex(d1,d2));
//   }
  LOG_DEBUG("rslt=false (cannot see Complex in String) [" << val << "]");
  std::stringstream logmsg;
  logmsg << compose( _("WARNING Cannot convert string '%1' to complex number."), val) << std::endl;
  DataPool::getDataEvents().writeLogMessage( logmsg.str() );
  return false; // no luck
}

/* --------------------------------------------------------------------------- */
/* getDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataValue *DataComplexElement::getDataValue() const {
  return new DataComplexValue( m_value, getAttributes() );
}

/* --------------------------------------------------------------------------- */
/* setDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataComplexElement::setDataValue( DataValue *d ){
  UpdateStatus status = NoUpdate;
  if( d ){
    if( d->isValid() ){
      dComplex val;
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

DataElement::UpdateStatus DataComplexElement::clearValue(){
  m_value = dComplex( 0, 0 );
  return setInvalid();
}

/* --------------------------------------------------------------------------- */
/* assignDataElement --                                                        */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataComplexElement::assignDataElement( const DataElement &el ){
  dComplex val = dComplex( 0.0 ,0.0 );
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

DataContainer *DataComplexElement::copyContainer( DataPool &datapool ) const{
  DataComplexElement *el = new DataComplexElement();
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

bool DataComplexElement::writeXML( std::ostream &ostr
                                 , std::vector<std::string> &attrs
                                 , bool is_structitem
                                 , int level
                                 , bool debug
                                 , bool isScalar )
{
  LOG_DEBUG( "write");
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

bool DataComplexElement::writeJSON( std::ostream &ostr,
                                    bool is_structitem,
                                    int level,
                                    int indentation,
                                    bool isScalar,
                                    double scale,
                                    const SerializableMask flags )
{
  // WARNING: Complex Type is not JSON serializable
  if( isValid() ){
    ostr << "{\"re\":" << m_value.real() * scale
         << ",\"im\":" << m_value.imag() * scale <<"}";
  }
  else {
    ostr << "{\"re\":null,\"im\":null}";
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDPvalue --                                                             */
/* --------------------------------------------------------------------------- */

void DataComplexElement::writeDPvalue( std::ostream &ostr ) const{
  if( isValid() ){
    ostr << m_value;
  }
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataComplexElement::compare( const DataContainer &cont
                                             , int fall
                                             , int max_faelle )
{
  const DataElement *el = cont.getDataElement();
  dComplex val;
  bool equal = false;
  if( el->getValue( val ) ){
    equal = val == m_value;
  }
  return result_of_compare( isValid(), el->isValid(), equal );
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

void *DataComplexElement::getPython(){
#if defined(HAVE_PYTHON)
  if( isValid() ){
    return PyComplex_FromDoubles( m_value.real(), m_value.imag() );
  }
  return PythonObject::getEmptyObject();
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataComplexElement::setPython( const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  LOG_DEBUG("DataComplexElement::setPython");
  assert( obj.hasPyObject() );

  dComplex val( 0.0, 0.0 );
  if( PyComplex_Check( obj ) != 0 ){
    Py_complex complex = PyComplex_AsCComplex( obj );
    val = dComplex( complex.real, complex.imag );
  }
  else
  if( PyFloat_Check( obj ) != 0 ){
    val = dComplex( PyFloat_AsDouble( obj ), 0.0 );
  }
  else
  if( PyLong_Check( obj ) != 0 ){
    val = dComplex( PyLong_AsDouble( obj ), 0.0 );
  }
  else{
    return clearValue();
  }

  if( setValue( val, -1 ) ){
    return ValueUpdated;
  }
#endif
  return NoUpdate;
}
