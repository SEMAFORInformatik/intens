
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobStackDataStructure.h"
#include "app/UserAttr.h"
#include "app/AppData.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobStackDataStructure::JobStackDataStructure( DataReference* data )
  : JobStackData( false )
  , m_data( data ){
}

JobStackDataStructure::JobStackDataStructure( XferDataItem& data )
  : JobStackData( false )
  , m_data( data ){
}

JobStackDataStructure::~JobStackDataStructure(){
}

/*=============================================================================*/
/* member Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

JobStackDataStructure *JobStackDataStructure::clone(){
  return new JobStackDataStructure( m_data );
}

/* --------------------------------------------------------------------------- */
/* isTrue --                                                                   */
/* --------------------------------------------------------------------------- */

bool JobStackDataStructure::isTrue(){
  return false;
}

/* --------------------------------------------------------------------------- */
/* getStructureValue --                                                        */
/* --------------------------------------------------------------------------- */

bool JobStackDataStructure::getStructureValue( DataReference * &rout, DataReference *rin ){
  BUG(BugJobStack,"JobStackDataStructure::getStructureValue");
  assert( rin->getDataType() == DataDictionary::type_StructVariable );

  DataDictionary *din  = rin->GetDict();
  assert( din->getDataType() == DataDictionary::type_StructVariable );

  din = din->getStructure();
  assert( din->getDataType() == DataDictionary::type_StructDefinition );

  DataDictionary *dout = m_data.Data()->GetDict();
  assert( dout->getDataType() == DataDictionary::type_StructVariable );

  dout = dout->getStructure();
  assert( dout->getDataType() == DataDictionary::type_StructDefinition );

  BUG_MSG("in="  << din->getName());
  BUG_MSG("out=" << dout->getName());
  if( din == dout ){
    rout = m_data.Data();
    BUG_EXIT("same Definition");
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getStringValue --                                                           */
/* --------------------------------------------------------------------------- */

bool JobStackDataStructure::getStringValue( std::string &val ){
  std::ostringstream os;
  m_data.Data()->writeJSON( os, 0, false, HIDEnothing );
  val = os.str();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getDataReference --                                                         */
/* --------------------------------------------------------------------------- */

DataReference *JobStackDataStructure::getDataReference(){
  return m_data.Data();
}

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::add( JobEngine *eng, JobStackDataPtr &dat ){
  if( isInvalid() || dat->isInvalid() ) return op_FatalError; // invalid data

  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* subtract --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::subtract( JobEngine *eng, JobStackDataPtr &dat ){
  if( isInvalid() || dat->isInvalid() ) return op_FatalError; // invalid data

  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* multiply --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::multiply( JobEngine *eng, JobStackDataPtr &dat ){
  if( isInvalid() || dat->isInvalid() ) return op_FatalError; // invalid data

  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* divide --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::divide( JobEngine *eng, JobStackDataPtr &dat ){
  if( isInvalid() || dat->isInvalid() ) return op_FatalError; // invalid data

  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* power --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::power( JobEngine *eng, JobStackDataPtr &dat ){
  if( isInvalid() || dat->isInvalid() ) return op_FatalError; // invalid data

  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* negate --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::negate( JobEngine *eng ){
  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* greater --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::greater( JobEngine *eng, JobStackDataPtr &dat ){
  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* greaterEqual --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::greaterEqual( JobEngine *eng, JobStackDataPtr &dat ){
  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* less --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::less( JobEngine *eng, JobStackDataPtr &dat ){
  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* lessEqual --                                                                */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::lessEqual( JobEngine *eng, JobStackDataPtr &dat ){
  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* equal --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::equal( JobEngine *eng, JobStackDataPtr &dat ){
  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* notEqual --                                                                 */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackDataStructure::notEqual( JobEngine *eng, JobStackDataPtr &dat ){
  eng->pushInvalid();
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void JobStackDataStructure::print( std::ostream &ostr ){
  std::ostringstream os;
  m_data.Data()->writeJSON( os, 0, false, HIDEnothing );
  if (os.str().size() < 400 || !AppData::Instance().HeadlessWebMode()) {
    ostr << os.str();
  } else {
    ostr << os.str().substr(0, 100) << " ... " << os.str().substr(os.str().size() - 50);
  }
}
