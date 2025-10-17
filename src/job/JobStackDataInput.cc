
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobDataReference.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"
#include "job/JobStackDataInput.h"
#include "job/JobStackDataStructure.h"
#include "utils/JsonUtils.h"

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackDataInput *JobStackDataInput::clone(){
  return new JobStackDataInput( m_dataitem );
}

/* --------------------------------------------------------------------------- */
/* getDataType --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType JobStackDataInput::getDataType() {
  return m_dataitem.getDataType();
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::isValid(){
  return m_dataitem.isValid();
}

/* --------------------------------------------------------------------------- */
/* isInvalid --                                                                */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::isInvalid(){
  return !m_dataitem.isValid();
}

/* --------------------------------------------------------------------------- */
/* getRealValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getRealValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getRealValue( d );
}

/* --------------------------------------------------------------------------- */
/* getImagValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getImagValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getImagValue( d );
}

/* --------------------------------------------------------------------------- */
/* getAbsValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getAbsValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getAbsValue( d );
}

/* --------------------------------------------------------------------------- */
/* getSinValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getSinValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getSinValue( d );
}

/* --------------------------------------------------------------------------- */
/* getCosValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getCosValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getCosValue( d );
}

/* --------------------------------------------------------------------------- */
/* getTanValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getTanValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getTanValue( d );
}

/* --------------------------------------------------------------------------- */
/* getASinValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getASinValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getASinValue( d );
}

/* --------------------------------------------------------------------------- */
/* getACosValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getACosValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getACosValue( d );
}

/* --------------------------------------------------------------------------- */
/* getATanValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getATanValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getATanValue( d );
}

/* --------------------------------------------------------------------------- */
/* getLogValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getLogValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getLogValue( d );
}

/* --------------------------------------------------------------------------- */
/* getLog10Value --                                                            */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getLog10Value( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getLog10Value( d );
}

/* --------------------------------------------------------------------------- */
/* getSqrtValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getSqrtValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getSqrtValue( d );
}

/* --------------------------------------------------------------------------- */
/* getArgValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getArgValue( double &d ){
  JobStackDataPtr data( getJobStackData() );
  return data->getArgValue( d );
}

/* --------------------------------------------------------------------------- */
/* getComplexValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getComplexValue( dComplex &c ){
  JobStackDataPtr data( getJobStackData() );
  return data->getComplexValue( c );
}

/* --------------------------------------------------------------------------- */
/* getIntegerValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getIntegerValue( int &i ){
  JobStackDataPtr data( getJobStackData() );
  return data->getIntegerValue( i );
}

/* --------------------------------------------------------------------------- */
/* getStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getStringValue( std::string &s ){
  JobStackDataPtr data( getJobStackData() );
  return data->getStringValue( s );
}

/* --------------------------------------------------------------------------- */
/* getStructureValue --                                                        */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getStructureValue( DataReference * &rout ){
  BUG(BugJobStack,"JobStackDataInput::getStructureValue");
  JobStackDataPtr data( getJobStackData() );
  DataReference *ref;
  bool valid = data->getStructureValue( ref, m_dataitem.Data() );
  rout = DataPool::newDataReference( *ref );
  return valid;
}

/* --------------------------------------------------------------------------- */
/* getStructureValue --                                                        */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::getStructureValue( DataReference * &rout, DataReference *rin ){
  BUG(BugJobStack,"JobStackDataInput::getStructureValue");
  rout = m_dataitem.Data();
  assert( rout != 0 );
  if( rin->isSameDataType( rout ) ){
    BUG_EXIT("same Definition");
    return true;
  }
  rout = 0;
  return false;
}

/* --------------------------------------------------------------------------- */
/* isTrue --                                                                   */
/* --------------------------------------------------------------------------- */

bool JobStackDataInput::isTrue(){
  JobStackDataPtr data( getJobStackData() );
  return data->isTrue();
}

/* --------------------------------------------------------------------------- */
/* getNodename --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::getNodename( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataInput::getNodename");
  eng->push( new JobStackDataString(m_dataitem.getName()) );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataInput::print( std::ostream &ostr ){
  JobStackDataPtr data( getJobStackData() );
  data->print( ostr );
}

/* --------------------------------------------------------------------------- */
/* round --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::round( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataInput::round");
  JobStackDataPtr data( getJobStackData() );
  return data->round( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* round5 --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::round5( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataInput::round5");
  JobStackDataPtr data( getJobStackData() );
  return data->round5( eng );
}

/* --------------------------------------------------------------------------- */
/* round10 --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::round10( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataInput::round10");
  JobStackDataPtr data( getJobStackData() );
  return data->round10( eng );
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::add( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->add( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* subtract --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::subtract( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->subtract( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* multiply --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::multiply( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->multiply( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* divide --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::divide( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->divide( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* power --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::power( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->power( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* negate --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::negate( JobEngine *eng ){
  JobStackDataPtr data( getJobStackData() );
  return data->negate( eng );
}

/* --------------------------------------------------------------------------- */
/* greater --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::greater( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->greater( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* greaterEqual --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::greaterEqual( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->greaterEqual( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* less --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::less( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->less( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* lessEqual --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::lessEqual( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->lessEqual( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* equal --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::equal( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->equal( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* notEqual --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::notEqual( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( getJobStackData() );
  return data->notEqual( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* assign --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::assign( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataInput::assign");

  if( m_dataitem.isReadOnly() ){
    BUG_EXIT("item is readonly");
    return op_Warning;
  }

  switch( m_dataitem.getDataType() ){
  case DataDictionary::type_Integer:{
      int i;
      if( dat->getIntegerValue( i ) ){
        m_dataitem.setValue( i );
        BUG_MSG("Integer Value " << i << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Real:{
      double d;
      if( dat->getRealValue( d ) ){
        m_dataitem.setValue( d );
        BUG_MSG("Real Value " << d << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_Complex:{
      dComplex c;
      if( dat->getComplexValue( c ) ){
        m_dataitem.setValue( c );
        BUG_MSG("Complex Value " << c << " set");
        return op_Ok;
      }
    }
    break;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
      std::string s;
      if( dat->getStringValue( s ) ){
        std::string leftValue;
        m_dataitem.getValue(leftValue);
        bool replaced = ch_semafor_intens::JsonUtils::assignJsonObjectComboBox(leftValue,s);
        assert ( replaced );
        m_dataitem.setValue( leftValue );
        BUG_MSG("String Value " << leftValue << " set");
        return op_Ok;
      }
      if( dat->isInvalid() ){
        std::string leftValue, rightValue("<INVALID>");
        m_dataitem.getValue(leftValue);
        bool replaced = ch_semafor_intens::JsonUtils::assignJsonObjectComboBox(leftValue,rightValue,false);
        if ( replaced ) {
          m_dataitem.setValue( leftValue );
          BUG_MSG("String Value " << leftValue << " set");
          return op_Ok;
        }
      }
    }
    break;

  case DataDictionary::type_StructVariable:{
      DataReference *ref = 0;
      if( dat->getStructureValue( ref, m_dataitem.Data() ) ){
	m_dataitem.Data()->AssignDataElement( *ref );
	BUG_MSG("Structure Value " << ref->fullName( true ) << " set");
	return op_Ok;
      }
      m_dataitem.Data()->clearAllElements();
      return op_Ok;
    }

  default:
    BUG_MSG("unknown Data Type");
    break;
  }
  m_dataitem.setInvalid();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* assignCorresponding --                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::assignCorresponding( JobEngine *eng,
                                                             JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataInput::assignCorresponding");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::setColor( JobEngine *eng,
						  JobStackDataPtr &dat )
{
  BUG(BugJobStack,"JobStackDataInput::setColor");

  int color;
  if( !dat->getIntegerValue( color ) ){
    color = 0;
  }

  DataReference *ref = m_dataitem.Data();
  if( !ref ){
    return op_FatalError;
  }
  DataReference *r = DataPool::newDataReference( *ref );
  JobDataReference data;
  data.newDataReference(r);
  data.updateDataReference( eng );
  data.setColor( eng, color );

  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* setColorBit --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataInput::setColorBit( JobEngine *eng,
						     JobStackDataPtr &dat,
						     bool unset )
{
  BUG(BugJobStack,"JobStackDataInput::setColorBit");

  int color;
  if( !dat->getIntegerValue( color ) ){
    color = 0;
  }

  DataReference *ref = m_dataitem.Data();
  if( !ref ){
    return op_FatalError;
  }
  DataReference *r = DataPool::newDataReference( *ref );
  JobDataReference data;
  data.newDataReference(r);
  data.updateDataReference( eng );
  data.setColorBit( eng, color, unset );

  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* getJobStackData --                                                          */
/* --------------------------------------------------------------------------- */

JobStackData *JobStackDataInput::getJobStackData(){
  BUG(BugJobStack,"JobStackDataInput::getJobStackData");

  JobStackData *item = 0;

  switch( getDataType() ){
  case DataDictionary::type_Integer:
    {
      int i;
      if( m_dataitem.getValue( i ) ){
        item = new JobStackDataInteger( i );
        BUG_MSG("Integer Value " << i);
      }
    }
    break;

  case DataDictionary::type_Real:
    {
      double d;
      if( m_dataitem.getValue( d ) ){
        item = new JobStackDataReal( d );
        BUG_MSG("Real Value " << d);
      }
    }
    break;

  case DataDictionary::type_Complex:
    {
      dComplex c;
      if( m_dataitem.getValue( c ) ){
	item = new JobStackDataComplex( c );
	BUG_MSG("Complex Value " << c);
      }
    }
    break;

  case DataDictionary::type_CharData:
  case DataDictionary::type_String:
    {
      std::string s;
      if( m_dataitem.getValue( s ) ){
        item = new JobStackDataString( s );
        BUG_MSG("String Value " << s);
      }
    }
    break;

  case DataDictionary::type_StructVariable:
    {
      item = new JobStackDataStructure( m_dataitem );
      BUG_MSG("Struct Value " << m_dataitem.Data()->fullName( true ) );
    }
    break;

  default:
    BUG_MSG("unknown Data Type");
    break;
  }
  if( item == 0 ){
    item = new JobStackDataInteger();
  }
  return item;
}
