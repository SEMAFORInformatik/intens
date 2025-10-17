
#include <iomanip>
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobManager.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackDataReal *JobStackDataReal::clone(){
  JobStackDataReal *data = new JobStackDataReal( m_value );
  if( isInvalid() ){
    data->setInvalid();
  }
  return data;
}

/* --------------------------------------------------------------------------- */
/* getRealValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getRealValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = m_value;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getAbsValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getAbsValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = fabs( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSinValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getSinValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = sin( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getCosValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getCosValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = cos( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getTanValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getTanValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = tan( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getASinValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getASinValue( double &val ){
  if( isInvalid() || m_value < -1 || m_value > 1 ){
    return false;
  }
  val = asin( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getACosValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getACosValue( double &val ){
  if( isInvalid() || m_value < -1 || m_value > 1 ){
    return false;
  }
  val = acos( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getATanValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getATanValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = atan( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getLog10Value --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getLog10Value( double &val ){
  if( isInvalid() || m_value <= 0 ){
    return false;
  }
  val = log10( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getLogValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getLogValue( double &val ){
  if( isInvalid() || m_value <= 0 ){
    return false;
  }
  val = log( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSqrtValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getSqrtValue( double &val ){
  if( isInvalid() || m_value < 0 ){
    return false;
  }
  val = sqrt( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getArgValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getArgValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = std::arg( dComplex( m_value, 0.0 ) );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getComplexValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getComplexValue( dComplex &val ){
  if( isInvalid() ){
    return false;
  }
  val = dComplex( m_value, 0.0 );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getIntegerValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getIntegerValue( int &val ){
  if( isInvalid() ){
    return false;
  }
  val = (int)m_value;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::getStringValue( std::string &val ){
  if( isInvalid() ){
    return false;
  }
  std::ostringstream os;
  os << m_value;
  val = os.str();
  return true;
}

/* --------------------------------------------------------------------------- */
/* isTrue --                                                                   */
/* --------------------------------------------------------------------------- */

bool JobStackDataReal::isTrue(){
  if( isInvalid() ){
    return false;
  }
  return m_value != 0.0;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataReal::print( std::ostream &ostr ){
  if( isInvalid() )
    ostr << "<invalid>";
  else
    ostr << m_value;
}

/* --------------------------------------------------------------------------- */
/* round --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::round( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::round");
  int rundung;
  if( !dat->getIntegerValue( rundung ) ){
    eng->pushInvalid();
  }
  else
  if( isValid() ){
    eng->push( new JobStackDataReal( runden( m_value, rundung ) ) );
  }
  else{
    eng->pushInvalid();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* round5 --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::round5( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataReal::round5");
  if( isValid() ){
    eng->push( new JobStackDataReal( runden5( m_value ) ) );
  }
  else{
    eng->pushInvalid();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* round10 --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::round10( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataReal::round10");
  if( isValid() ){
    eng->push( new JobStackDataReal( runden( m_value, 1 ) ) );
  }
  else{
    eng->pushInvalid();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::add( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::add");
  if( isInvalid() ){
    if( dat->isInvalid() ){
      eng->pushInvalid();
      BUG_EXIT("Data not valid");
      return op_Warning;
    }
    m_value = 0.0;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) )
      eng->push( new JobStackDataReal( m_value + d ) );
    else
      eng->push( new JobStackDataReal( m_value ) );
    return op_Ok;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) )
      eng->push( new JobStackDataComplex( c + m_value ) );
    else
      eng->push( new JobStackDataComplex( dComplex( m_value,0.0 ) ) );
    return op_Ok;
  }
  case DataDictionary::type_String:{
    std::ostringstream os;
    os << m_value;
    std::string s;
    if( dat->getStringValue( s ) )
      eng->push( new JobStackDataString( os.str() + s ) );
    else
      eng->push( new JobStackDataString( os.str() ) );
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

JobElement::OpStatus JobStackDataReal::subtract( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::subtract");
  if( isInvalid() ){
    if( dat->isInvalid() ){
      eng->pushInvalid();
      BUG_EXIT("Data not valid");
      return op_Warning;
    }
    m_value = 0.0;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) )
      eng->push( new JobStackDataReal( m_value - d ) );
    else
      eng->push( new JobStackDataReal( m_value ) );
    return op_Ok;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) )
      eng->push( new JobStackDataComplex( c - m_value ) );
    else
      eng->push( new JobStackDataComplex( dComplex(m_value,0.0) ) );
    return op_Ok;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* multiply --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::multiply( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::multiply");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->push( new JobStackDataReal( m_value * d ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      eng->push( new JobStackDataComplex( c * m_value ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* divide --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::divide( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::divide");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      if( d == 0.0 ){
	eng->pushInvalid();
	BUG_EXIT("Divide by zero");
	return op_Warning;
      }
      else{
	eng->push( new JobStackDataReal( m_value / d ) );
	return op_Ok;
      }
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      if( c == 0.0 ){
	eng->pushInvalid();
	BUG_EXIT("Divide by zero");
	return op_Warning;
      }
      else{
	eng->push( new JobStackDataComplex( dComplex( m_value,0.0 ) / c ) );
	return op_Ok;
      }
    }
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* modulo --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::modulo( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::modulo");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double r;
    if( dat->getRealValue( r ) ){
      if( r == 0 ){
	eng->pushInvalid();
	BUG_EXIT("Modulo by zero");
	return op_Warning;
      }
      else{
	eng->push( new JobStackDataReal( fmod(m_value, r) ) );
	return op_Ok;
      }
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      if( i == 0 ){
	eng->pushInvalid();
	BUG_EXIT("Modulo by zero");
	return op_Warning;
      }
      else{
	eng->push( new JobStackDataReal( fmod(m_value, i) ) );
	return op_Ok;
      }
    }
    break;
  }
  case DataDictionary::type_Complex:{
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex");
    return op_Warning;
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* power --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::power( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::power");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->push( new JobStackDataReal( pow( m_value, d ) ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      //      eng->push( new JobStackDataComplex( pow( dComplex( m_value ), c ) ) );
      eng->push( new JobStackDataComplex( dComplex( 0.0, 0.0 ) ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* negate --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::negate( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataReal::negate");
  if( isInvalid() ){
    eng->pushInvalid();
    return op_Ok; // invalid data
  }
  eng->push( new JobStackDataReal( m_value * -1.0 ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* greater --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataReal::greater( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::greater");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( m_value > d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, String or StructVariable");
    return op_Warning;

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

JobElement::OpStatus JobStackDataReal::greaterEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::greaterEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( m_value >= d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, String or StructVariable");
    return op_Warning;

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

JobElement::OpStatus JobStackDataReal::less( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::less");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( m_value < d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, String or StructVariable");
    return op_Warning;

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

JobElement::OpStatus JobStackDataReal::lessEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::lessEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( m_value <= d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: Complex, String or StructVariable");
    return op_Warning;

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

JobElement::OpStatus JobStackDataReal::equal( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::equal");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( m_value == d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      eng->pushTrue( m_value == c );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    JobManager::Instance().printMessage( "Warning: Equality Check of Real and String or StructVariable always fails", GuiElement::msg_Warning );
    return op_Warning;

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

JobElement::OpStatus JobStackDataReal::notEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataReal::notEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( m_value != d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      eng->pushTrue( m_value != c );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    JobManager::Instance().printMessage( "Warning: Inequality Check of Real and String or StructVariable always fails", GuiElement::msg_Warning );
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}
