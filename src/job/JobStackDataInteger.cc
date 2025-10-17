
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobManager.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"


/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackDataInteger *JobStackDataInteger::clone(){
  JobStackDataInteger *data = new JobStackDataInteger( m_value );
  if( isInvalid() ){
    data->setInvalid();
  }
  return data;
}

/* --------------------------------------------------------------------------- */
/* getRealValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getRealValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = (double)m_value;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getAbsValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getAbsValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = fabs( (double)m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSinValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getSinValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = sin( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getCosValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getCosValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = cos( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getTanValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getTanValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = tan( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getASinValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getASinValue( double &val ){
  if( isInvalid() || m_value < -1 || m_value > 1 ){
    return false;
  }
  val = asin( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getACosValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getACosValue( double &val ){
  if( isInvalid() || m_value < -1 || m_value > 1 ){
    return false;
  }
  val = acos( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getATanValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getATanValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = atan( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getLog10Value --                                                            */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getLog10Value( double &val ){
  if( isInvalid() || m_value <= 0 ){
    return false;
  }
  val = log10( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getLogValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getLogValue( double &val ){
  if( isInvalid() || m_value <= 0 ){
    return false;
  }
  val = log( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSqrtValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getSqrtValue( double &val ){
  if( isInvalid() || m_value < 0 ){
    return false;
  }
  val = sqrt( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getArgValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getArgValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = std::arg( dComplex( (double)m_value, 0.0 ) );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getComplexValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getComplexValue( dComplex &val ){
  if( isInvalid() ){
    return false;
  }
  val = dComplex( (double)m_value, 0.0 );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getIntegerValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getIntegerValue( int &val ){
  if( isInvalid() ){
    return false;
  }
  val = m_value;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataInteger::getStringValue( std::string &val ){
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

bool JobStackDataInteger::isTrue(){
  if( isInvalid() ){
    return false;
  }
  return m_value != 0;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataInteger::print( std::ostream &ostr ){
  if( isInvalid() )
    ostr << "<invalid>";
  else
    ostr << m_value;
}

/* --------------------------------------------------------------------------- */
/* round --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInteger::round( JobEngine *eng
					       , JobStackDataPtr &dat )
{
  int rundung;
  if( !dat->getIntegerValue( rundung ) ){
    eng->pushInvalid();
  }
  else{
    eng->push( clone() ); // Da gibt es nichts zu tun
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* round5 --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInteger::round5( JobEngine *eng ){
  eng->push( clone() ); // Da gibt es nichts zu tun
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* round10 --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInteger::round10( JobEngine *eng ){
  eng->push( clone() ); // Da gibt es nichts zu tun
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInteger::add( JobEngine *eng
					     , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::add");
  if( isInvalid() ){
    if( dat->isInvalid() ){
      eng->pushInvalid();
      BUG_EXIT("Data not valid");
      return op_Warning;
    }
    m_value = 0;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) )
      eng->push( new JobStackDataReal( (double)m_value + d ) );
    else
      eng->push( new JobStackDataReal( (double)m_value ) );
    return op_Ok;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) )
      eng->push( new JobStackDataInteger( m_value + i ) );
    else
      eng->push( new JobStackDataInteger( m_value ) );
    return op_Ok;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) )
      eng->push( new JobStackDataComplex( c + (double)m_value ) );
    else
      eng->push( new JobStackDataComplex( dComplex( (double)m_value, 0.0 ) ) );
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
/* subtract --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInteger::subtract( JobEngine *eng
						  , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::subtract");
  if( isInvalid() ){
    if( dat->isInvalid() ){
      eng->pushInvalid();
      BUG_EXIT("Data not valid");
      return op_Warning;
    }
    m_value = 0;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) )
      eng->push( new JobStackDataReal( (double)m_value - d ) );
    else
      eng->push( new JobStackDataReal( (double)m_value ) );
    return op_Ok;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) )
      eng->push( new JobStackDataInteger( m_value - i ) );
    else
      eng->push( new JobStackDataInteger( m_value ) );
    return op_Ok;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) )
      eng->push( new JobStackDataComplex( dComplex( (double)m_value, 0.0 ) - c ) );
    else
      eng->push( new JobStackDataComplex( dComplex( (double)m_value, 0.0 ) ) );
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

JobElement::OpStatus JobStackDataInteger::multiply( JobEngine *eng
						  , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::multiply");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->push( new JobStackDataReal( (double)m_value * d ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      eng->push( new JobStackDataInteger( m_value * i ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      eng->push( new JobStackDataComplex( c * (double)m_value ) );
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

JobElement::OpStatus JobStackDataInteger::divide( JobEngine *eng
						, JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::divide");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
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
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      if( i == 0 ){
	eng->pushInvalid();
	BUG_EXIT("Divide by zero");
	return op_Warning;
      }
      else{
	eng->push( new JobStackDataInteger( m_value / i ) );
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
	eng->push( new JobStackDataComplex( dComplex( (double)m_value, 0.0 ) / c ) );
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

JobElement::OpStatus JobStackDataInteger::modulo( JobEngine *eng
						, JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::modulo");
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
	eng->push( new JobStackDataReal( fmod(m_value,r) ) );
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
	eng->push( new JobStackDataInteger( m_value % i ) );
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

JobElement::OpStatus JobStackDataInteger::power( JobEngine *eng
					       , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::power");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }
  double x=static_cast<double>(m_value);
  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->push( new JobStackDataReal( pow( x, d ) ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      // int power negative int always returns real
      if ( i < 0 ) {
        eng->push( new JobStackDataReal( pow( x, i ) ) );
        return op_Ok;
      }
      eng->push( new JobStackDataInteger( (int) floor(pow( x, i ) + 0.5) ) );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      //      eng->push( new JobStackDataComplex( pow( dComplex( x ), c ) ) );
      eng->push( new JobStackDataComplex( dComplex( 0.0,0.0 ) ) );
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

JobElement::OpStatus JobStackDataInteger::negate( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataInteger::negate");
  if( isInvalid() ){
    eng->pushInvalid();
    return op_Ok; // invalid data
  }
  eng->push( new JobStackDataInteger( m_value * -1 ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* greater --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInteger::greater( JobEngine *eng
						 , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::greater");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( (double)m_value > d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      eng->pushTrue( m_value > i );
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

JobElement::OpStatus JobStackDataInteger::greaterEqual( JobEngine *eng
						      , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::greaterEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( (double)m_value >= d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      eng->pushTrue( m_value >= i );
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

JobElement::OpStatus JobStackDataInteger::less( JobEngine *eng
					      , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::less");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( (double)m_value < d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      eng->pushTrue( m_value < i );
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

JobElement::OpStatus JobStackDataInteger::lessEqual( JobEngine *eng
						   , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::lessEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( (double)m_value <= d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      eng->pushTrue( m_value <= i );
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

JobElement::OpStatus JobStackDataInteger::equal( JobEngine *eng
					       , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::equal");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( (double)m_value == d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      eng->pushTrue( m_value == i );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      eng->pushTrue( dComplex( (double)m_value, 0.0 ) == c );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: String or StructVariable");
    JobManager::Instance().printMessage( "Warning: Equality Check of Integer and String or StructVariable always fails", GuiElement::msg_Warning );
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

JobElement::OpStatus JobStackDataInteger::notEqual( JobEngine *eng
						  , JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInteger::notEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    if( dat->getRealValue( d ) ){
      eng->pushTrue( (double)m_value != d );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    if( dat->getIntegerValue( i ) ){
      eng->pushTrue( m_value != i );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      eng->pushTrue( dComplex( (double)m_value, 0.0 ) != c );
      return op_Ok;
    }
    break;
  }
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    JobManager::Instance().printMessage( "Warning: Inequality Check of Integer and String or StructVariable always fails", GuiElement::msg_Warning );
    BUG_EXIT("Warning: String or StructVariable");
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}
