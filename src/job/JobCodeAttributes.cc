
#include "job/JobIncludes.h"

#include "job/JobCodeAttributes.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "job/JobStackDataVariable.h"
#include "gui/GuiElement.h"
#include "gui/GuiManager.h"
#include "gui/GuiFieldgroup.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrEditable::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrEditable::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setEditable( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrLock::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrLock::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setLocked( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrColor::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrColor::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setColor( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrColorSetBit::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrColorSetBit::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setColorBit( eng, b, false );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrColorUnsetBit::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrColorSetBit::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setColorBit( eng, b, true );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrSetDataTimestamp::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrTimestamp::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setDataTimestamp( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAttrSetValueTimestamp::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAttrTimestamp::execute");
  JobStackDataPtr b( eng->pop() );
  if( b.isnt_valid() ) return op_FatalError;
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->setValueTimestamp( eng, b );
  BUG_EXIT("Status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetAccordionExpanded::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeSetAccordionExpanded::execute");
  int expanded(0);

  // get value (expanded)
  JobStackDataPtr dat_int( eng->pop() );
  if (dat_int.isnt_valid()) return op_FatalError;
  if (!dat_int->getIntegerValue(expanded)) {
    // discard
    // return op_FatalError;
  }

  GuiElement *guiElement(m_element);
  if (!guiElement) {
    // get GuiElement
    JobStackDataPtr data( eng->pop() );
    if( data.isnt_valid() ) return op_FatalError;
    std::string  guiName;
    if(data->getStringValue(guiName)){
      guiElement = GuiElement::findElement(guiName);
    }
    if(!guiElement){
      return op_FatalError;  // TODO: message??
    }
    if( guiElement->Type() != GuiElement::type_Fieldgroup ){
      return op_FatalError;  // TODO: message??
    }
  }

  auto fieldgroup = guiElement->getFieldgroup();

  // set accordion state (expanded or collapsed)
  if (expanded >= 0){
    fieldgroup->setAccordionExpanded(expanded);
  }

  BUG_DEBUG("normal exit");
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetFieldgroupTitle::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeSetFieldgroupTitle::execute");
  std::string title;

  // get title
  JobStackDataPtr dat_str( eng->pop() );
  if (dat_str.isnt_valid()) return op_FatalError;
  if (!dat_str->getStringValue(title)){
    return op_FatalError;
  }

  GuiElement *guiElement(m_element);
  if (!guiElement) {
    // get GuiElement
    JobStackDataPtr data( eng->pop() );
    if( data.isnt_valid() ) return op_FatalError;
    std::string  guiName;
    if(data->getStringValue(guiName)){
      guiElement = GuiElement::findElement(guiName);
    }
    if(!guiElement){
      return op_FatalError;  // TODO: message??
    }
    if( guiElement->Type() != GuiElement::type_Fieldgroup ){
      return op_FatalError;  // TODO: message??
    }
  }

  auto fieldgroup = guiElement->getFieldgroup();

  // set fieldgroup title
  if (!title.empty()){  // TODO: why may it not be empty??
    fieldgroup->setFieldgroupTitle(title);
  }

  BUG_DEBUG("normal exit");
  return op_Ok;
}
