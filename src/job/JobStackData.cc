
#include "job/JobIncludes.h"

#include "job/JobStackData.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataString.h"
#include "job/JobEngine.h"
#include "job/JobDataReference.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "xfer/XferParameterComplex.h"
#include "xfer/XferParameterString.h"
#include "app/DataPoolIntens.h"
#include "gui/GuiDataField.h"
#include "gui/GuiElement.h"
#include "gui/GuiFactory.h"
#include "operator/MessageQueueThreads.h"
#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

#define TOLERANZ 0.000001

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor of JobStackData                                    */
/*=============================================================================*/

JobStackData::JobStackData( bool valid )
  : m_valid( valid ){
}

JobStackData::~JobStackData(){
}

/*=============================================================================*/
/* Functions of JobStackData                                                   */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* reference --                                                                */
/* --------------------------------------------------------------------------- */

JobDataReference &JobStackData::reference(){
  assert( false );
  // the following dummy code is never executed (because of above assert)
  // but it avoids the compiler warning: no return statement
  JobDataReference *ref = new JobDataReference();
  return *ref;
}

/* --------------------------------------------------------------------------- */
/* runden --                                                                   */
/* --------------------------------------------------------------------------- */

double JobStackData::runden( double d, int i ){
  return abrunden( d * pow(10.,i) + 0.5 ) / pow(10.,i);
}

/* --------------------------------------------------------------------------- */
/* runden5 --                                                                  */
/* --------------------------------------------------------------------------- */

double JobStackData::runden5( double d ){
  double x = runden( d * 20, 0 );
  return runden( x / 20, 2 );
}

/* --------------------------------------------------------------------------- */
/* aufrunden --                                                                */
/* --------------------------------------------------------------------------- */

double JobStackData::aufrunden( double d ){
  return ceil( d - TOLERANZ );
}

/* --------------------------------------------------------------------------- */
/* abrunden --                                                                 */
/* --------------------------------------------------------------------------- */

double JobStackData::abrunden( double d ){
  return floor( d + TOLERANZ );
}

/* --------------------------------------------------------------------------- */
/* isInteger --                                                                */
/* --------------------------------------------------------------------------- */

bool JobStackData::isInteger( double d ){
  return ( aufrunden( d ) - d <= TOLERANZ );
}

/* --------------------------------------------------------------------------- */
/* size --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::size( JobEngine *eng, JobStackDataPtr &dim ){
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* index --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::index( JobEngine *eng, JobStackDataPtr &dim ){
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::compare( JobEngine *eng, JobStackDataPtr &data ){
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::compare( JobEngine *eng
					  , JobStackDataPtr &dataLeft
					  , JobStackDataPtr &dataRight )
{
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* diff --                                                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::diff( JobEngine *eng, JobStackDataPtr &dim, int mode ){
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::checkFormat( JobEngine *eng, JobStackDataPtr &data ){
  JobElement::OpStatus ret(op_Ok);
  XferDataParameter* param(0);
  DataReference* ref = getDataReference();

  if( !ref ){
    BUG_ERROR("DataReference is null");
    return op_Warning;
  }
  XferDataItem* xfer = new XferDataItem(DataReference::newDataReference(*ref));

  switch( getDataType() ){
  case DataDictionary::type_Integer:
    param = new XferParameterInteger( 0, 0, 0 );
    break;
  case DataDictionary::type_Real:
    param = new XferParameterReal( 0, 0, -1 , 0, false );
    break;
  case DataDictionary::type_Complex:
    switch(xfer->getParamType() ){
    case XferDataItem::param_Default:
      param = new XferParameterComplex( 0, 0, -1, 0, false );
      break;
    case XferDataItem::param_ComplexReal:
    case XferDataItem::param_ComplexImag:
    case XferDataItem::param_ComplexAbs:
    case XferDataItem::param_ComplexArg:
      param = new XferParameterReal( 0, 0, -1 , 0, false );
      break;
    default:
      assert( false );
    }
    break;
  case DataDictionary::type_String:
    param = new XferParameterString( 0, 0, '\0' );
    break;
  case DataDictionary::type_CharData:
    param = new XferParameterString();
    break;

  default:
    return op_FatalError;
  }
  param->setDataItem(xfer);

  // checkFormat call
  std::string s;
  data->getStringValue(s);
  switch( param->checkFormat(s) ){
  case XferParameter::status_Bad:
    BUG_DEBUG("Data Bad Format");
    ret = op_Warning;
    break;

  case XferParameter::status_Unchanged:
    BUG_DEBUG("Data Unchanged");
    break;

  case XferParameter::status_Changed:
     std::string error_msg;
     bool targetsAreCleared = param->DataItem()->StreamTargetsAreCleared(error_msg);
     BUG_DEBUG("Data Changed, targetAreCleared: " << targetsAreCleared);
     if (error_msg.size())
       BUG_INFO("ConsistencyDialog msg: " << error_msg);
     if(!targetsAreCleared ) { // Targets sind nicht ok

       // WebReply Aufruf, do WebQuery
       MessageQueueReplyThread *mq_reply = eng->getFunction()->getBaseCallingAction()->getMessageQueueReplyThread();
       BUG_DEBUG("Query user mq_reply: " << mq_reply);
       if (mq_reply) {
         Json::Value jsonObj = Json::Value(Json::objectValue);
         jsonObj["command"] = "consistency_check";
         jsonObj["message"] =  _("The results are not consistent anymore.\nDo you want to delete them?");
         jsonObj["title"] = _("Consistency Query");
         BUG_DEBUG("ConsistencyDialog do query["<<ch_semafor_intens::JsonUtils::value2string(jsonObj)<<"]");
         std::vector<std::string> expected;
         expected.push_back("return");
         Json::Value retObj = mq_reply->doQuery(jsonObj, expected);
         BUG_DEBUG("ConsistencyDialog mq reply return["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
         s = retObj.isMember("return") ? retObj["return"].asString() : "no";
         if (lower(s) == "yes") {
           param->DataItem()->clearTargetStreams();
         } else {
           ret = op_Aborted;
         }
       } else {
         GuiElement::ButtonType buttonClicked = GuiFactory::Instance()->showDialogConfirmation
           ( 0
             , _("OK")
             , _("The results are not consistent anymore.\nDo you want to delete them?"), 0
             );
         if (buttonClicked == GuiElement::button_Yes) {
           // default, clear target streams
           param->DataItem()->clearTargetStreams();
           BUG_DEBUG("Clear Target Streams");
         } else {
           ret = op_Aborted;
         }
       }
     }
     break;
  }
  delete param;
  return ret;
}

/* --------------------------------------------------------------------------- */
/* round --                                                                    */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::round( JobEngine *eng, JobStackDataPtr &dat )
{
  JobStackData* copy = clone();
  copy->setInvalid();
  eng->push( copy );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* round5 --                                                                   */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::round5( JobEngine *eng ){
  JobStackData* copy = clone();
  copy->setInvalid();
  eng->push( copy );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* round10 --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::round10( JobEngine *eng ){
  JobStackData* copy = clone();
  copy->setInvalid();
  eng->push( copy );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* printFullName --                                                            */
/* --------------------------------------------------------------------------- */

void JobStackData::printFullName( std::ostream &ostr, bool withIndizes ){
  ostr << "<no data item>";
}

/* --------------------------------------------------------------------------- */
/* fullName --                                                                 */
/* --------------------------------------------------------------------------- */

std::string JobStackData::fullName( bool withIndizes ){
  std::ostringstream name;
  printFullName( name, withIndizes );
  return name.str();
}

/* --------------------------------------------------------------------------- */
/* getTimestamp --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::getTimestamp( JobEngine *eng ){
  eng->push( new JobStackDataInteger() );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* getDbModified --                                                            */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::getDbModified( JobEngine *eng ){
  eng->push( new JobStackDataInteger() );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* setDbTimestamp --                                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::setDbTimestamp( JobEngine *eng ){
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* getClassname --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::getClassname( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeClassname::execute");
  eng->push( new JobStackDataString("") );
  return op_Warning;
}

/* --------------------------------------------------------------------------- */
/* getClassname --                                                             */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobStackData::getNodename( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeNodename::execute");
  eng->push( new JobStackDataString("") );
  return op_Warning;
}

/*=============================================================================*/
/* Constructor / Destructor of JobStackDataPtr                                 */
/*=============================================================================*/

JobStackDataPtr::JobStackDataPtr()
  : m_item_ptr(){
}

JobStackDataPtr::JobStackDataPtr( JobStackData *pt )
  : m_item_ptr( pt ){
}

JobStackDataPtr::JobStackDataPtr( const JobStackItemPtr &ptr )
  : m_item_ptr( ptr.get_ptr() ){
  assert( m_item_ptr->Type() == JobStackData::stck_Data );
}

JobStackDataPtr::JobStackDataPtr( const JobStackDataPtr &ptr )
: m_item_ptr( ptr.get_ptr() ){
}

JobStackDataPtr::~JobStackDataPtr(){
}

/*=============================================================================*/
/* Functions of JobStackDataPtr                                                */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* is_valid --                                                                 */
/* --------------------------------------------------------------------------- */

bool JobStackDataPtr::is_valid() const{
  return m_item_ptr.is_valid();
}

/* --------------------------------------------------------------------------- */
/* isnt_valid --                                                               */
/* --------------------------------------------------------------------------- */

bool JobStackDataPtr::isnt_valid() const{
  return m_item_ptr.isnt_valid();
}

/* --------------------------------------------------------------------------- */
/* reference --                                                                */
/* --------------------------------------------------------------------------- */

JobDataReference &JobStackDataPtr::reference(){
  assert( is_valid() );
  return get_ptr()->reference();
}

/* --------------------------------------------------------------------------- */
/* itemPtr --                                                                  */
/* --------------------------------------------------------------------------- */

const JobStackItemPtr &JobStackDataPtr::itemPtr() const{
  return m_item_ptr;
}

/* --------------------------------------------------------------------------- */
/* get_ptr --                                                                  */
/* --------------------------------------------------------------------------- */

JobStackData *JobStackDataPtr::get_ptr() const{
  return (JobStackData*)m_item_ptr.get_ptr();
}

/* --------------------------------------------------------------------------- */
/* operator-> --                                                               */
/* --------------------------------------------------------------------------- */

JobStackData *JobStackDataPtr::operator->() const{
  return (JobStackData*)m_item_ptr.get_ptr();
}

/* --------------------------------------------------------------------------- */
/* operator= --                                                                */
/* --------------------------------------------------------------------------- */

JobStackDataPtr& JobStackDataPtr::operator=( const JobStackDataPtr &p ){
  m_item_ptr = p.get_ptr();
  return *this;
}

/* --------------------------------------------------------------------------- */
/* operator= --                                                                */
/* --------------------------------------------------------------------------- */

JobStackDataPtr& JobStackDataPtr::operator=( JobStackData *pt ){
  m_item_ptr = pt;
  return *this;
}

/* --------------------------------------------------------------------------- */
/* operator* --                                                                */
/* --------------------------------------------------------------------------- */

JobStackData& JobStackDataPtr::operator*(){
  assert( is_valid() );
  return (JobStackData&)*m_item_ptr;
}
