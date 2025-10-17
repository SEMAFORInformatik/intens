
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackDataComplex *JobStackDataComplex::clone(){
  JobStackDataComplex *data = new JobStackDataComplex( m_value );
  if( isInvalid() ){
    data->setInvalid();
  }
  return data;
}

/* --------------------------------------------------------------------------- */
/* getRealValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::getRealValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = m_value.real();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getImagValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::getImagValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = m_value.imag();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getAbsValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::getAbsValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = std::abs( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getArgValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::getArgValue( double &val ){
  if( isInvalid() ){
    return false;
  }
  val = std::arg( m_value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getComplexValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::getComplexValue( dComplex &val ){
  if( isInvalid() ){
    return false;
  }
  val = m_value;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getIntegerValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::getIntegerValue( int &val ){
  if( isInvalid() ){
    return false;
  }
  val = (int)m_value.real();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::getStringValue( std::string &val ){
  if( isInvalid() ){
    return false;
  }
  std::ostringstream os;
  os << m_value;
  val = os.str();
  return true;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataComplex::print( std::ostream &ostr ){
  if( isInvalid() )
    ostr << "<invalid>";
  else
    ostr << m_value;
}

/* --------------------------------------------------------------------------- */
/* isTrue --                                                                   */
/* --------------------------------------------------------------------------- */

bool JobStackDataComplex::isTrue(){
  if( isInvalid() ){
    return false;
  }
  return m_value != dComplex( 0.0, 0.0 );
}

/* --------------------------------------------------------------------------- */
/* round --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataComplex::round( JobEngine *eng, JobStackDataPtr &dat ){
  int rundung;
  if( !dat->getIntegerValue( rundung ) ){
    eng->pushInvalid();
  }
  else
  if( isValid() ){
    double real = runden( m_value.real(), rundung ); // Runden
    double imag = runden( m_value.imag(), rundung ); // Runden
    eng->push( new JobStackDataComplex( dComplex( real, imag ) ) );
  }
  else{
    eng->push( this );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* round5 --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataComplex::round5( JobEngine *eng ){
  if( isValid() ){
    double real = runden5( m_value.real() ); // Runden
    double imag = runden5( m_value.imag() ); // Runden
    m_value = dComplex( real, imag );
  }
  eng->push( this );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* round10 --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataComplex::round10( JobEngine *eng ){
  if( isValid() ){
    double real = runden( m_value.real(), 1 ); // Runden
    double imag = runden( m_value.imag(), 1 ); // Runden
    m_value = dComplex( real, imag );
  }
  eng->push( this );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataComplex::add( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::add");
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
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) )
      eng->push( new JobStackDataComplex( c + m_value ) );
    else
      eng->push( new JobStackDataComplex( m_value ) );
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

JobElement::OpStatus JobStackDataComplex::subtract( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::subtract");
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
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) )
      eng->push( new JobStackDataComplex( c - m_value ) );
    else
      eng->push( new JobStackDataComplex( m_value ) );
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

JobElement::OpStatus JobStackDataComplex::multiply( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::multiply");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
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

JobElement::OpStatus JobStackDataComplex::divide( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::divide");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      if( c == 0.0 ){
	eng->pushInvalid();
	BUG_EXIT("Divide by zero");
	return op_Warning;
      }
      else{
	eng->push( new JobStackDataComplex( m_value / c ) );
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
/* power --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataComplex::power( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::power");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:{
    dComplex c;
    if( dat->getComplexValue( c ) ){
      //      eng->push( new JobStackDataComplex( pow( m_value, c ) ) );
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

JobElement::OpStatus JobStackDataComplex::negate( JobEngine *eng ){
  if( isInvalid() ){
    eng->pushInvalid();
    return op_Ok; // invalid data
  }
  eng->push( new JobStackDataComplex( m_value * -1.0 ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* greater --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataComplex::greater( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::greater");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: not possible");
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

JobElement::OpStatus JobStackDataComplex::greaterEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::greaterEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: not possible");
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

JobElement::OpStatus JobStackDataComplex::less( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::less");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: not possible");
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

JobElement::OpStatus JobStackDataComplex::lessEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::lessEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
  case DataDictionary::type_Complex:
  case DataDictionary::type_String:
  case DataDictionary::type_StructVariable:
    eng->pushInvalid();
    BUG_EXIT("Warning: not possible");
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

JobElement::OpStatus JobStackDataComplex::equal( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::equal");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
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

JobElement::OpStatus JobStackDataComplex::notEqual( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataComplex::notEqual");
  if( isInvalid() || dat->isInvalid() ){
    eng->pushInvalid();
    BUG_EXIT("Data not valid");
    return op_Warning;
  }

  switch( dat->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
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
    return op_Warning;

  default:
    break;
  }
  eng->pushInvalid();
  BUG_EXIT("Fatal Error");
  return op_FatalError;
}
