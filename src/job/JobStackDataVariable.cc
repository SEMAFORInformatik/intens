
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobDataReference.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataVariable.h"

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackDataVariable *JobStackDataVariable::clone(){
  JobStackDataVariable *data = new JobStackDataVariable( m_data );
  if( isInvalid() ){
    data->setInvalid();
  }
  return data;
}

/* --------------------------------------------------------------------------- */
/* getDataType --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType JobStackDataVariable::getDataType() {
  return m_data->getDataType();
}

/* --------------------------------------------------------------------------- */
/* reference --                                                                */
/* --------------------------------------------------------------------------- */

JobDataReference &JobStackDataVariable::reference(){
  assert( m_data );
  return *m_data;
}

/* --------------------------------------------------------------------------- */
/* getUnits --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getUnits( std::string & units ) {
  return m_data->getUnit( units );
}

/* --------------------------------------------------------------------------- */
/* getLabel --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getLabel( std::string & label ) {
  return m_data->getLabel( label );
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::isValid(){
  return m_data->isValid();
}

/* --------------------------------------------------------------------------- */
/* isInvalid --                                                                */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::isInvalid(){
  return m_data->isInvalid();
}

/* --------------------------------------------------------------------------- */
/* getRealValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getRealValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getRealValue( d );
}

/* --------------------------------------------------------------------------- */
/* getImagValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getImagValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getImagValue( d );
}

/* --------------------------------------------------------------------------- */
/* getAbsValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getAbsValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getAbsValue( d );
}

/* --------------------------------------------------------------------------- */
/* getSinValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getSinValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getSinValue( d );
}

/* --------------------------------------------------------------------------- */
/* getCosValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getCosValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getCosValue( d );
}

/* --------------------------------------------------------------------------- */
/* getTanValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getTanValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getTanValue( d );
}

/* --------------------------------------------------------------------------- */
/* getASinValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getASinValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getASinValue( d );
}

/* --------------------------------------------------------------------------- */
/* getACosValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getACosValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getACosValue( d );
}

/* --------------------------------------------------------------------------- */
/* getATanValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getATanValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getATanValue( d );
}

/* --------------------------------------------------------------------------- */
/* getLogValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getLogValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getLogValue( d );
}

/* --------------------------------------------------------------------------- */
/* getLog10Value --                                                            */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getLog10Value( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getLog10Value( d );
}

/* --------------------------------------------------------------------------- */
/* getSqrtValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getSqrtValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getSqrtValue( d );
}

/* --------------------------------------------------------------------------- */
/* getArgValue --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getArgValue( double &d ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getArgValue( d );
}

/* --------------------------------------------------------------------------- */
/* getComplexValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getComplexValue( dComplex &c ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getComplexValue( c );
}

/* --------------------------------------------------------------------------- */
/* getIntegerValue --                                                          */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getIntegerValue( int &i ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getIntegerValue( i );
}

/* --------------------------------------------------------------------------- */
/* getStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getStringValue( std::string &s ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->getStringValue( s );
}

/* --------------------------------------------------------------------------- */
/* getStructureValue --                                                        */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getStructureValue( DataReference * &dat ){
  BUG(BugJobStack,"JobStackDataVariable::getStructureValue");
  dat = DataPool::newDataReference( m_data->Data() );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getStructureValue --                                                        */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::getStructureValue( DataReference * &rout, DataReference *rin ){
  BUG(BugJobStack,"JobStackDataVariable::getStructureValue");
  rout = &(m_data->Data());
  if( rin->isSameDataType( rout ) ){
    BUG_EXIT("same Definition");
    return true;
  }
  rout = 0;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getDataReference --                                                         */
/* --------------------------------------------------------------------------- */

DataReference *JobStackDataVariable::getDataReference(){
  return m_data->getDataReference();
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::setValue( int i, int inx ){
  m_data->setValue( i, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setRealValue --                                                             */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::setRealValue( double d, int inx ){
  m_data->setRealValue( d, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::setStringValue( const std::string& s, int inx ) {
  m_data->setStringValue( s, inx );
  return true;
}

/* --------------------------------------------------------------------------- */
/* clearValues --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::clearValues(){
  return m_data->clearValues();
}

/* --------------------------------------------------------------------------- */
/* clearAllValues --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::clearAllValues(){
  return m_data->clearAllValues();
}

/* --------------------------------------------------------------------------- */
/* eraseValues --                                                              */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::eraseValues(){
  return m_data->eraseValues();
}

/* --------------------------------------------------------------------------- */
/* packValues --                                                               */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::packValues( bool packRow ){
  return m_data->packValues( packRow );
}

/* --------------------------------------------------------------------------- */
/* isTrue --                                                                   */
/* --------------------------------------------------------------------------- */

bool JobStackDataVariable::isTrue(){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->isTrue();
}

/* --------------------------------------------------------------------------- */
/* setCycleNumber --                                                           */
/* --------------------------------------------------------------------------- */

void JobStackDataVariable::setCycleNumber( JobStackDataPtr &data ){
  int i = 0;
  if( data->getIntegerValue( i ) ){
    m_data->setCycleNumber( i );
  }
  else{
    m_data->setCycleNumber( -99 );
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataVariable::print( std::ostream &ostr ){
  JobStackDataPtr data( m_data->getJobStackData() );
  data->print( ostr );
}

/* --------------------------------------------------------------------------- */
/* round --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::round( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataVariable::round");
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->round( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* round5 --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::round5( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataVariable::round5");
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->round5( eng );
}

/* --------------------------------------------------------------------------- */
/* round10 --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::round10( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataVariable::round10");
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->round10( eng );
}

/* --------------------------------------------------------------------------- */
/* size --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::size( JobEngine *eng, JobStackDataPtr &dim ){
  return m_data->size( eng, dim );
}

/* --------------------------------------------------------------------------- */
/* index --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::index( JobEngine *eng, JobStackDataPtr &dim ){
  return m_data->index( eng, dim );
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::compare( JobEngine *eng, JobStackDataPtr &dat )
{
  return m_data->compare( eng, &dat.reference() );
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::compare( JobEngine *eng
						  , JobStackDataPtr &dataLeft
						  , JobStackDataPtr &dataRight )
{
  return m_data->compare( eng, &dataLeft.reference(), &dataRight.reference() );
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::add( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->add( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* subtract --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::subtract( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->subtract( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* multiply --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::multiply( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->multiply( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* divide --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::divide( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->divide( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* modulo --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::modulo( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->modulo( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* power --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::power( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->power( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* negate --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::negate( JobEngine *eng ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->negate( eng );
}

/* --------------------------------------------------------------------------- */
/* greater --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::greater( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->greater( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* greaterEqual --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::greaterEqual( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->greaterEqual( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* less --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::less( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->less( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* lessEqual --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::lessEqual( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->lessEqual( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* equal --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::equal( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->equal( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* notEqual --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::notEqual( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  return data->notEqual( eng, dat );
}

/* --------------------------------------------------------------------------- */
/* assign --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::assign( JobEngine *eng, JobStackDataPtr &data ){
  BUG(BugJobStack,"JobStackDataVariable::assign");
  return m_data->assign( data );
}

/* --------------------------------------------------------------------------- */
/* assignCorresponding --                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::assignCorresponding( JobEngine *eng, JobStackDataPtr &dat ){
  BUG(BugJobStack,"JobStackDataVariable::assignCorresponding");
  return m_data->assignCorresponding( dat );
}

/* --------------------------------------------------------------------------- */
/* getTimestamp --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::getTimestamp( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataVariable::getTimestamp");
  return m_data->getTimestamp( eng  );
}

/* --------------------------------------------------------------------------- */
/* getDbModified --                                                            */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::getDbModified( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataVariable::getDbModified");
  return m_data->getDbModified( eng  );
}

/* --------------------------------------------------------------------------- */
/* setDbTimestamp --                                                            */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setDbTimestamp( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataVariable::setDbTimestamp");
  return m_data->setDbTimestamp( eng  );
}

/* --------------------------------------------------------------------------- */
/* getClassname --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::getClassname( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataVariable::getClassname");
  return m_data->getClassname( eng  );
}

/* --------------------------------------------------------------------------- */
/* getNodename --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::getNodename( JobEngine *eng ){
  BUG(BugJobStack,"JobStackDataVariable::getNodename");
  return m_data->getNodename( eng  );
}

/* --------------------------------------------------------------------------- */
/* accumulate --                                                               */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::accumulate( JobEngine *eng, JobStackDataPtr &dat ){
  JobStackDataPtr data( m_data->getJobStackData() );
  OpStatus status = data->add( eng, dat );

  // Das Resultat von add() landet auf dem Stack.
  data = eng->pop();
  return m_data->assign( data );
}

/* --------------------------------------------------------------------------- */
/* increment --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::increment( JobEngine *eng ){
  return m_data->increment();
}

/* --------------------------------------------------------------------------- */
/* decrement --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::decrement( JobEngine *eng ){
  return m_data->decrement();
}

/* --------------------------------------------------------------------------- */
/* setEditable --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setEditable( JobEngine *eng, JobStackDataPtr &dat ){
  int i;
  if( !dat->getIntegerValue( i ) ){
    i = 0;
  }
  return m_data->setEditable( eng, i > 0 );
}

/* --------------------------------------------------------------------------- */
/* setLocked --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setLocked( JobEngine *eng, JobStackDataPtr &dat ){
  int i;
  if( !dat->getIntegerValue( i ) ){
    i = 0;
  }
  return m_data->setLocked( eng, i > 0 );
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setColor( JobEngine *eng, JobStackDataPtr &dat ){
  int i;
  if( !dat->getIntegerValue( i ) ){
    i = 0;
  }
  return m_data->setColor( eng, i );
}

/* --------------------------------------------------------------------------- */
/* setColorBit --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setColorBit( JobEngine *eng
						      , JobStackDataPtr &dat
						      , bool unset )
{
  int i;
  if( !dat->getIntegerValue( i ) ){
    i = 0;
  }
  return m_data->setColorBit( eng, i, unset );
}

/* --------------------------------------------------------------------------- */
/* setStylesheet --                                                            */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setStylesheet(JobEngine *eng
						      , const std::string & stylesheet)
{
  return m_data->setStylesheet(eng, stylesheet);
}

/* --------------------------------------------------------------------------- */
/* setDataTimestamp --                                                         */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setDataTimestamp( JobEngine *eng
							   , JobStackDataPtr &dat )
{
  int i;
  if( !dat->getIntegerValue( i ) ){
    i = 0;
  }
  return m_data->setDataTimestamp( eng, i );
}

/* --------------------------------------------------------------------------- */
/* setValueTimestamp --                                                        */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataVariable::setValueTimestamp( JobEngine *eng
							    , JobStackDataPtr &dat )
{
  int i;
  if( !dat->getIntegerValue( i ) ){
    i = 0;
  }
  return m_data->setValueTimestamp( eng, i );
}

/* --------------------------------------------------------------------------- */
/* printFullName --                                                            */
/* --------------------------------------------------------------------------- */

void JobStackDataVariable::printFullName( std::ostream &ostr, bool withIndizes ){
  m_data->printFullName( ostr, withIndizes );
}
