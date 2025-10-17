
#include <string>
#include "utils/Debugger.h"
#include "XMLValue.h"

INIT_LOGGER();

//========================================================================
// end
//========================================================================

void XMLValue::end(){
  std::string value;
  if( getValue( value ) ){
    m_parent->setValue( value );
  }
  m_value = "";
}

//========================================================================
// newElement
//========================================================================

XMLElement *XMLValue::newElement( const std::string &element ){
  return new XMLElement( this );
}

//========================================================================
// characters
//========================================================================

void XMLValue::characters( const std::string &value ){
  BUG_DEBUG("characters(" << value << ")");

  if( value.empty() ){
    return;
  }

  std::string::size_type begin = m_value.size();
  m_value += value;
  m_isValue = true;
}
