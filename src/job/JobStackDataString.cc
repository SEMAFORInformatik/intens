
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobManager.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"
#include "utils/Date.h"

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackData *JobStackDataString::clone(){
  return new JobStackDataString( m_value );
}

/* --------------------------------------------------------------------------- */
/* getRealValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataString::getRealValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  std::istringstream is(m_value);
  bool fail = (is >> val).fail();

  // m_value beginnt nicht mit einer Zahl oder wir haben alles gelesen
  if (fail || is.eof())
    return !fail;

  // maybe an iso date
  if ( m_value.find_first_of("-:",is.tellg()) != std::string::npos ) {
    // rest contains ['-', ':']
    bool ok;
    double date = convertISOToJulianDate(m_value, &ok);
    if ( ok ) {
      val = date;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getIntegerValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataString::getIntegerValue( int &val ){
  if( isInvalid() ){
    return false;
  }
  std::istringstream is(m_value);
  return !(is >> val).fail();
}

/* --------------------------------------------------------------------------- */
/* getStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataString::getStringValue( std::string &val ){
  if( isInvalid() ){
    return false;
  }
  val = m_value;
  return true;
}

/* --------------------------------------------------------------------------- */
/* isTrue --                                                                   */
/* --------------------------------------------------------------------------- */

bool JobStackDataString::isTrue(){
  if( isInvalid() ){
    return false;
  }
  return !m_value.empty();
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::add( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::add");
  if( isInvalid() ){
    if( dat->isInvalid() ){
      eng->pushInvalid();
      BUG_EXIT("Data not valid");
      return op_Warning;
    }
    m_value = "";
  }


  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    if( dat->getStringValue( s ) )
      eng->push( new JobStackDataString( m_value + s ) );
    else
      eng->push( new JobStackDataString( m_value ) );
    return op_Ok;
  }
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: StructVariable");
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* subtract --                                                                 */
/* --------------------------------------------------------------------------- */
#include "utils/Date.h"

JobElement::OpStatus JobStackDataString::subtract( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::subtract");

  // if string is a time or date => try to get the difference
  if (dat->getDataType() == DataDictionary::type_String) {
    std::string s;
    dat->getStringValue(s);
    s = Date::getDateTimeElapsed(m_value, s);
    if (s.size()) {
      eng->push( new JobStackDataString( s ) );
      return op_Ok;
    }
  }
  eng->pushInvalid();
  BUG_EXIT("Warning: not possible");
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* multiply --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::multiply( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::multiply");
  eng->pushInvalid();
  BUG_EXIT("Warning: not possible");
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* divide --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::divide( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::divide");
  eng->pushInvalid();
  BUG_EXIT("Warning: not possible");
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* power --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::power( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::power");
  eng->pushInvalid();
  BUG_EXIT("Warning: not possible");
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* negate --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::negate( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataString::negate");
  if( isInvalid() ){
    eng->pushInvalid();
  }
  else{
    eng->push( new JobStackDataString( m_value ) );
  }
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* greater --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::greater( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::greater");

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, Real, Integer or StructVariable");
    return op_Warning;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    if( dat->getStringValue( s ) )
      if( isValid() )
	eng->pushTrue( m_value > s );
      else
	eng->pushTrue( false );
    else
      if( isInvalid() )
	eng->pushInvalid();
      else
	eng->pushTrue( true );
    return op_Ok;
  }

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* greaterEqual --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::greaterEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::greaterEqual");

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, Real, Integer or StructVariable");
    return op_Warning;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    if( dat->getStringValue( s ) )
      if( isValid() )
	eng->pushTrue( m_value >= s );
      else
	eng->pushTrue( false );
    else
      if( isInvalid() )
	eng->pushInvalid();
      else
	eng->pushTrue( true );
    return op_Ok;
  }
  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* less --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::less( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::less");

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, Real, Integer or StructVariable");
    return op_Warning;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    if( dat->getStringValue( s ) )
      if( isValid() )
	eng->pushTrue( m_value < s );
      else
	eng->pushTrue( true );
    else
      if( isInvalid() )
	eng->pushInvalid();
      else
	eng->pushTrue( false );
    return op_Ok;
  }
  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* lessEqual --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::lessEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::lessEqual");

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, Real, Integer or StructVariable");
    return op_Warning;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    if( dat->getStringValue( s ) )
      if( isValid() )
	eng->pushTrue( m_value <= s );
      else
	eng->pushTrue( true );
    else
      if( isInvalid() )
	eng->pushInvalid();
      else
	eng->pushTrue( false );
    return op_Ok;
  }
  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* equal --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::equal( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::equal");

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, Real, Integer or StructVariable");
    JobManager::Instance().printMessage( "Warning: Equality Check of String and Complex, Real, Integer or StructVariable always fails", GuiElement::msg_Warning );
    return op_Warning;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    if( dat->getStringValue( s ) )
      if( isValid() )
	eng->pushTrue( m_value == s );
      else
	eng->pushTrue( false );
    else
      if( isInvalid() )
	eng->pushInvalid();
      else
	eng->pushTrue( false );
    return op_Ok;
  }
  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* notEqual --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataString::notEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataString::notEqual");

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, Real, Integer or StructVariable");
    JobManager::Instance().printMessage( "Warning: Inequality Check of String and Complex, Real, Integer or StructVariable always fails", GuiElement::msg_Warning );
    return op_Warning;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    if( dat->getStringValue( s ) )
      if( isValid() )
	eng->pushTrue( m_value != s );
      else
	eng->pushTrue( true );
    else
      if( isInvalid() )
	eng->pushInvalid();
      else
	eng->pushTrue( true );
    return op_Ok;
  }
  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* getUnits --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobStackDataString::getUnits( std::string & units ) {
  return false;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataString::print( std::ostream &ostr ){
  ostr << m_value;
}
