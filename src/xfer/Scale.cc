
#include <assert.h>
#include <sstream>

#include "xfer/Scale.h"
#include "utils/Debugger.h"
#include "xml/XMLDocumentHandler.h"


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

Scale::Scale()
  : m_value( 1 )
  , m_shift( 0 )
  , m_operator( '*' ){}

Scale::Scale( const Scale &scale ){
  *this = scale;
}

Scale::Scale( double value, char _operator, double shift )
  : m_value( value )
  , m_shift( shift )
  , m_operator( _operator ){
  assert( _operator == '*' || _operator == '/' );
}

Scale::~Scale(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* operator= --                                                                */
/* --------------------------------------------------------------------------- */

Scale &Scale::operator=( const Scale &scale ){
  if( this == &scale )
    return *this;
  m_value = scale.m_value;
  m_shift = scale.m_shift;
  m_operator = scale.m_operator;
  return *this;
}

/* --------------------------------------------------------------------------- */
/* copy --                                                                     */
/* --------------------------------------------------------------------------- */

Scale *Scale::copy(){
  return new Scale( *this );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                     */
/* --------------------------------------------------------------------------- */

double Scale::getValue() const{
  BUG( BugXfer, "Scale::getValue" );
  if( m_operator == '*' ){
    BUG_MSG( "operator is : * , value is : " << m_value );
    return m_value;
  }
  BUG_MSG( "operator is : / , value is : " << m_value );
  return 1./m_value;
}

/* --------------------------------------------------------------------------- */
/* getShift --                                                                 */
/* --------------------------------------------------------------------------- */

double Scale::getShift() const{
  return m_shift;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void Scale::marshal( std::ostream &os ){
  os << "<Scale";
  os << " operator=\"" << m_operator << "\"";
  os << " value=\"" << m_value << "\"";
  os << " shift=\"" << m_shift << "\"";
  os << ">";
  os << "</Scale>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable * Scale::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  std::string s;
  s=attributeList["operator"];
  if( s.empty() ){
  } else {
    char c;
    std::istringstream is( s );
    is >> c;
    m_operator = c;
  }
  s=attributeList["value"];
  if( !s.empty() ){
    std::istringstream is( s );
    is >> m_value;
  }
  s=attributeList["shift"];
  if( !s.empty() ){
    std::istringstream is( s );
    is >> m_shift;
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void Scale::setText( const std::string &text ){
}
