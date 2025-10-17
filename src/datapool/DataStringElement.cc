
#include <sstream>
#include <stdio.h>
#include <iomanip>

#include "utils/utils.h"
#include "utils/base64.h"
#include "datapool/DataStringElement.h"
#include "datapool/DataStringValue.h"
#include "datapool/DataLogger.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

// special (xml) characters
#define SPECIAL_CHARACTERS "<>&\"'"

// control characters: ascii 00 - 1f, without 0, 9, a, d (NUL, TAB, LF, CR)
#define CONTROL_CHARACTERS "\x1\x2\x3\x4\x5\x6\x7\x8\xb\xc\xe\xf\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataStringElement::DataStringElement(DataDictionary::DataType dataType)
  : m_value( "" )
  , m_dataType(dataType) {
  LOG_DEBUG("(constructor)");
}

DataStringElement::~DataStringElement(){
  LOG_DEBUG("(destructor)");
}

DataStringElement::DataStringElement( const DataStringElement &ref )
  : DataElement( ref )
  , m_value( ref.m_value ){
  LOG_DEBUG( "m_value=[" << m_value << "]");
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getElementType  --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataStringElement::getElementType() const{
  return m_dataType;
}

/* --------------------------------------------------------------------------- */
/* operator=  --                                                               */
/* --------------------------------------------------------------------------- */

bool DataStringElement::operator=( const DataElement &ref ){
  std::string val;
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

DataContainer *DataStringElement::clone() const{
  return new DataStringElement( *this );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::getValue( std::string &val ) const{
  LOG_DEBUG(  "getValue (string)" );
  if( isValid() ){
    val = m_value;
    LOG_DEBUG( "rslt=true value=[" << val << "]");
    return true;
  }
  LOG_DEBUG(  "rslt=false");
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::setValue( const std::string &val, int rundung ){
  LOG_DEBUG( "val=[" << val << "]  oldVal=["<<m_value<<"]" );
  std::string value(val);
  m_mimeType.clear();
  if (val.substr(0, 5) == "data:") {
    size_t pos = val.find_first_of(',');
    size_t outsize = val.size() - pos;
    char *output = new char[val.size()];
    if( base64decode(val.substr(pos + 1), reinterpret_cast< unsigned char *>( output ), outsize ) ){
      value = std::string(output, outsize);
    }
    size_t pos2 = val.find_first_of(';');
    m_mimeType = val.substr(5, pos2-5);
    LOG_DEBUG("Header ["<<val.substr(0, pos)<<"] mimeType["<<m_mimeType<<"]");
    delete[] output;
  }

  if( !isLocked() ){
    if( isValid() && value == m_value ){
      return false; // Es ändert sich nichts
    }
    if (getElementType() == DataDictionary::type_CharData) {
      m_mimeType = FileUtilities::getDataMimeType(value);
      LOG_DEBUG("new mime type["<<m_mimeType<<"]");
    }
    m_value = value;
    setValid();
    setDataContainerValueUpdated( DataPool::getTransactionNumber() );
    LOG_DEBUG(  "rslt=true");
    return true;
  }
  LOG_DEBUG(  "rslt=true (locked)");
  return false; // Passiert hier komplett still
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::getValue( int &val ) const{
  LOG_DEBUG("getValue (int)");
  std::string d;

  if( !getValue( d ) ){
    LOG_DEBUG("rslt=false");
    return false;
  }
  std::istringstream is( d );
  is >> val;
  if (is.rdstate() & std::ios::failbit) {
    LOG_DEBUG ("rslt=false (conversion problem)");
    return false;
  }
  LOG_DEBUG(  "rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::setValue( int val, int rundung ){
  LOG_DEBUG(" setValue (int) val=" << val);
  char tmp[30];

#if defined(__DP__STANDALONE_VS2013__)
  sprintf_s( tmp, "%d", val );
#else
  sprintf( tmp, "%d", val );
#endif
  std::string stmp(tmp);
  return setValue( stmp, -1 ); // ohne Rundung
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::setValue( unsigned int val, int rundung ){
  LOG_DEBUG(" setValue (int) val=" << val);
  char tmp[30];

#if defined(__DP__STANDALONE_VS2013__)
  sprintf_s( tmp, "%u", val );
#else
  sprintf( tmp, "%u", val );
#endif
  std::string stmp(tmp);
  return setValue( stmp, -1 ); // ohne Rundung
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::getValue( double &val ) const{
  LOG_DEBUG("getValue (double)");
  std::string d;

  if( !getValue( d ) ){
    LOG_DEBUG(  "rslt=false");
    return false;
  }
  std::istringstream is( d  );
  is >> val;
  if (is.rdstate() & std::ios::failbit) {
    LOG_DEBUG("rslt=false (conversion problem)");
    return false;
  }
  LOG_DEBUG( "rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::setValue( double val, int rundung ){
  LOG_DEBUG("setValue (double) val=" << val);
  char tmp[30];

  if( rundung >= 0 ){
    val = runden( val, rundung );
  }
  // todo: write and use common double/int to string method?
  std::ostringstream os;
  os << std::setprecision( 17 ) << val;
  std::string stmp(os.str());
  return setValue( stmp, -1 ); // ohne Rundung
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::getValue( dComplex &val ) const{
  LOG_DEBUG("getValue (complex)");
  double d;

  if( !getValue( d ) ){ // Call the double-Function
    LOG_DEBUG(  "rslt=false");
    return false;
  }
  val = dComplex( d, 0 );
  LOG_DEBUG(  "rslt=true value=" << val);
  return true; // success
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStringElement::setValue( const dComplex &val, int rundung ){
  LOG_DEBUG("setValue (double) val=" << val);

  if( val.imag() > 0 ){
    LOG_DEBUG(  "rslt=false");
    return false;
  }
  return setValue( val.real(), rundung ); // Call the double-Function
}

/* --------------------------------------------------------------------------- */
/* getDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataValue *DataStringElement::getDataValue() const {
  return new DataStringValue( m_value, getAttributes() );
}

/* --------------------------------------------------------------------------- */
/* setDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStringElement::setDataValue( DataValue *d ){
  UpdateStatus status = NoUpdate;
  if( d ){
    if( d->isValid() ){
      std::string val;
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

DataElement::UpdateStatus DataStringElement::clearValue(){
  m_value.erase();
  return setInvalid();
}

/* --------------------------------------------------------------------------- */
/* assignDataElement --                                                        */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStringElement::assignDataElement( const DataElement &el ){
  std::string val;
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

DataContainer *DataStringElement::copyContainer( DataPool &datapool ) const{
  DataStringElement *el = new DataStringElement();
  if( isValid() ){
    el->setValue( m_value, -1 ); // ohne Rundung
  }
  if( isLocked() ){
    el->setLocked();
  }
  return el;
}

/* --------------------------------------------------------------------------- */
/* convert2HTML --                                                             */
/* --------------------------------------------------------------------------- */

void DataStringElement::convert2HTML( std::string &text ){
  std::string::size_type pos = text.find_first_of( SPECIAL_CHARACTERS );
  if( pos != std::string::npos ){
    while( pos != std::string::npos ){
      if( text[pos] == '<' ){
	text.replace( pos, 1, "&lt;" );
	pos += 4;
      }
      else if( text[pos] == '>' ){
	text.replace( pos, 1, "&gt;" );
	pos += 4;
      }
      else if( text[pos] == '&' ){
	text.replace( pos, 1, "&amp;" );
	pos += 5;
      }
      else if( text[pos] == '"' ){
	text.replace( pos, 1, "&quot;" );
	pos += 6;
      }
      else if( text[pos] == '\'' ){
	text.replace( pos, 1, "&apos;" );
	pos += 6;
      }
//       else if( text[pos] == 'ä' ){
// 	text.replace( pos, 1, "&auml;" );
// 	pos += 6;
//       }
//       else if( text[pos] == 'ü' ){
// 	text.replace( pos, 1, "&uuml;" );
// 	pos += 6;
//       }
//       else if( text[pos] == 'ö' ){
// 	text.replace( pos, 1, "&ouml;" );
// 	pos += 6;
//       }
      else
	assert( false );
      pos = text.find_first_of( SPECIAL_CHARACTERS, pos );
    }
  }
}


/* --------------------------------------------------------------------------- */
/* convertControlCharacters --                                                 */
/* --------------------------------------------------------------------------- */

void DataStringElement::convertControlCharacters( std::string &text, bool erase ){
  std::string::size_type pos = text.find_first_of(CONTROL_CHARACTERS);
  if(pos == std::string::npos) {
    return;
  }

  // character 01-1f -> unicode 0x2401-0x241f -> UTF-8 e2 90 81 - e2 90 9f
  char replace[4] = {'\xe2', '\x90', '\x80', '\0'};
  while(pos != std::string::npos) {
    if(erase) {
      text.erase(pos, 1);
      pos += 1;
    } else {
      replace[2] = text[pos] + 0x80;
      text.replace(pos, 1, replace);
      pos += 3;
    }
    pos = text.find_first_of(CONTROL_CHARACTERS, pos);
  }
}


/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataStringElement::writeXML( std::ostream &os
                                , std::vector<std::string> &attrs
                                , bool is_structitem
                                , int level
                                , bool debug
                                , bool isScalar )
{
  LOG_DEBUG(  "write");

  std::string qs;

  if( debug )
    for( int i=0; i<level; i++ ) os << " ";
  if( !isScalar && !isValid() ){
    qs.append( "<v/>" );
  }

  if( !isScalar && isValid() ) {
    qs.append( "<v>" );
  }

  if( isValid() ) {
    // replace special and control characters
    std::string::size_type pos = m_value.find_first_of( SPECIAL_CHARACTERS CONTROL_CHARACTERS );
    if( pos != std::string::npos ){
      std::string value( m_value );
      convert2HTML( value );
      convertControlCharacters( value, false );
      qs.append( value.c_str() );
    }
    else {
      qs.append( m_value.c_str() );
    }
  }

  if( !isScalar && isValid() ){
    qs.append( "</v>" );
  }
  os << qs << '\n';
  return true;
}

/* --------------------------------------------------------------------------- */
/* JSON Functions --                                                           */
/* --------------------------------------------------------------------------- */

// begin json
// The following functions are copied from jsoncpp/json_writer.cpp and jsoncpp/json_tool.h
/// Returns true if ch is a control character (in range [0,32[).

static inline bool isControlCharacter( char ch ){
   return ch > 0 && ch <= 0x1F;
}

static bool containsControlCharacter( const char* str ){
   while ( *str )
   {
      if ( isControlCharacter( *(str++) ) )
         return true;
   }
   return false;
}

std::string valueToQuotedString( const char *value ){
   // Not sure how to handle unicode...
   if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL && !containsControlCharacter( value ))
      return std::string("\"") + value + "\"";
   // We have to walk value and escape any special characters.
   // Appending to std::string is not efficient, but this should be rare.
   // (Note: forward slashes are *not* rare, but I am not escaping them.)
   std::string::size_type maxsize = strlen(value)*2 + 3; // allescaped+quotes+NULL
   std::string result;
   result.reserve(maxsize); // to avoid lots of mallocs
   result += "\"";
   for (const char* c=value; *c != 0; ++c)
   {
      switch(*c)
      {
         case '\"':
            result += "\\\"";
            break;
         case '\\':
            result += "\\\\";
            break;
         case '\b':
            result += "\\b";
            break;
         case '\f':
            result += "\\f";
            break;
         case '\n':
            result += "\\n";
            break;
         case '\r':
            result += "\\r";
            break;
         case '\t':
            result += "\\t";
            break;
         //case '/':
            // Even though \/ is considered a legal escape in JSON, a bare
            // slash is also legal, so I see no reason to escape it.
            // (I hope I am not misunderstanding something.
            // blep notes: actually escaping \/ may be useful in javascript to avoid </
            // sequence.
            // Should add a flag to allow this compatibility mode and prevent this
            // sequence from occurring.
         default:
            if ( isControlCharacter( *c ) )
            {
               std::ostringstream oss;
               oss << "\\u" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<int>(*c);
               result += oss.str();
            }
            else
            {
               result += *c;
            }
            break;
      }
   }
   result += "\"";
   return result;
}

/* --------------------------------------------------------------------------- */
/* writeJSON --                                                                */
/* --------------------------------------------------------------------------- */

bool DataStringElement::writeJSON( std::ostream &ostr,
                                   bool is_structitem,
                                   int level,
                                   int indentation,
                                   bool isScalar,
                                   double scale,
                                   const SerializableMask flags )
{
  if (m_mimeType.size() && m_mimeType != "text/plain" && m_mimeType != "image/svg+xml") {
    std::string sBase64;
    base64encode(reinterpret_cast<const unsigned char*>(m_value.c_str()),
                 m_value.size(), sBase64, false);
    ostr << "\"" << "data:" << m_mimeType << ";base64," << sBase64 << "\"";
  } else {
    ostr << valueToQuotedString( m_value.c_str() );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDPvalue --                                                             */
/* --------------------------------------------------------------------------- */

void DataStringElement::writeDPvalue( std::ostream &ostr ) const{
  if( isValid() ){
    ostr << m_value;
  }
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataStringElement::compare( const DataContainer &cont
                                            , int fall
                                            , int max_faelle )
{
  const DataElement *el = cont.getDataElement();
  std::string val;
  bool equal = false;
  if( el->getValue( val ) ){
    equal = val == m_value;
  }
  return result_of_compare( isValid(), el->isValid(), equal );
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

void *DataStringElement::getPython(){
#if defined(HAVE_PYTHON)
#if PY_MAJOR_VERSION < 3
  if( isValid() ){
    return PyString_FromString( m_value.c_str() );
  }
  return PyString_FromString( "" );
#else
  if( isValid() ){
    return PyBytes_FromString( m_value.c_str() );
    return PyUnicode_FromString( m_value.c_str() );
  }
  return PyUnicode_FromString( "" );
#endif
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStringElement::setPython( const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  LOG_DEBUG("DataStringElement::setPython");
  assert( obj.hasPyObject() );

  const char *c = 0;
#if PY_MAJOR_VERSION < 3
  c = PyString_AsString( obj );
#else
  if( PyUnicode_Check( obj ) ){
    c = PyUnicode_AsUTF8( obj );
  }
  else if( PyBytes_Check( obj ) ){
    c = PyBytes_AsString( obj );
  }
#endif
  if( c != 0 ){
    if( setValue( std::string( c ), -1 ) ){
      return ValueUpdated;
    }
  }
  else{
    return clearValue();
  }
#endif
  return NoUpdate;
}
