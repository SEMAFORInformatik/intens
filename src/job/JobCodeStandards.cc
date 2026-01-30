#include <QApplication>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "job/JobIncludes.h"

#include "job/JobCodeStandards.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "job/JobStackAddress.h"
#include "job/JobStackReturn.h"
#include "job/JobStackDataVariable.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataComplex.h"
#include "job/JobStackDataString.h"
#include "job/JobCodeOthers.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#include "gui/IconManager.h"
#include "gui/qt/GuiQtElement.h"
#include "app/QuitApplication.h"
#include "app/AppData.h"
#include "app/DataPoolIntens.h"

#include "utils/Date.h"
#include "utils/StringUtils.h"
#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataVector.h"

INIT_LOGGER();
INIT_DESLOGGER();

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpIf::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeOpIf::execute");
  // Auf dem Stack liegt das Resultat der If-Expression
  JobStackDataPtr dat( eng->pop() );
  if( dat.isnt_valid() ) return op_FatalError;

  if( dat->isTrue() ){
    BUG_DEBUG("expression is true: goto " << m_next );
    // Am Ende des If-Code ist ein Branch, der die Sprungadresse vom
    // Stack holt. Die nächste Code-Adresse ist der Beginn des If-Code.
    eng->pushAddress( m_next );
    return op_Ok;
  }

  // Im Else-Fall springen wir einfach zum Start des Else-Code.
  BUG_DEBUG("expression is false: goto " << m_else );
  eng->setCurrentAddr( m_else );
  return op_Branch;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeOpWhile::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeOpWhile::execute");
  // Auf dem Stack liegt das Resultat der While-Expression
  JobStackDataPtr dat( eng->pop() );
  if( dat.isnt_valid() ) return op_FatalError;

  if( dat->isTrue() ){
    BUG_MSG("expression is true");
    // Die Expression ist 'True'. Es geht normal weiter. Am Ende des While-
    // Code ist ein Branch zurück auf den Beginn der While-Expression.
    // Es geht also normal weiter.
    return op_Ok;
  }

  // Fertig mit 'While'. Wir springen zum nächsten Statement nach dem 'While'.
  eng->setCurrentAddr( m_next );
  return op_Branch;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeBranch::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeBranch::execute");

  JobStackItemPtr dat( eng->pop() );
  if( dat.isnt_valid() ){
    BUG_ERROR("JobStackAddress is NULL" );
    return op_FatalError;
  }
  // special case, someone (a ProcessGroup) pushed a return code on stack
  // e.g. RUN(xfer), a ProcessGroup call
  JobStackData* sData = dynamic_cast<JobStackData*>(dat.operator->());
  if( sData && dat->Type() != stck_Address && eng->sizeOfStack() > 1){
    dat = eng->pop();
  }

  BUG_DEBUG("data type = " << dat->Type() );
  if( dat->Type() != stck_Address ){
    BUG_ERROR("JobStackAddress is not a StackAddress...." );
    return op_FatalError;
  }

  eng->setCurrentAddr( dat.address().getAddress() );
  return op_Branch;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeBranchWithAddr::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeBranchWithAddr::execute");
  eng->setCurrentAddr( m_addr );
  return op_Branch;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAbort::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAbort::execute");
  JobStackItemPtr ret( eng->pop() );
  JobStackItemPtr ret2;

  // Unter Umstaenden befinden sich mehrere ReturnControl-Objekte auf dem Stack.
  // Wir interessieren uns nur für das unterste.
  while( ret.is_valid() ){
    if( ret->Type() != stck_ReturnControl ){
      // ignore
    }
    else{
      ret2 = ret;
    }
    ret = eng->pop();
  }

  if( ret2.isnt_valid() ) return op_FatalError;
  return eng->restoreCode( ret2.returncontrol() );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeExitIntens::execute( JobEngine *eng ){
  BUG( BugJobCode,"JobCodeExitIntens::execute force: " << m_force);
  if (m_force || AppData::Instance().hasTestModeFunc()) {
    if (AppData::Instance().getTestModeExitCode() != AppData::successExitCode)
      std::cerr << "ExitCode("<<AppData::Instance().getTestModeExitCode() <<")" << std::endl << std::flush;
    exit( AppData::Instance().getTestModeExitCode() );
  } else {
    if (AppData::Instance().HeadlessWebMode()) {
      QuitApplication::Instance()->setExitFlag(true);
    } else {
    QuitApplication *quit = QuitApplication::Instance();
    quit->confirmYesButtonPressed();
    }
    BUG_DEBUG("EXIT (flag)");
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeBeep::execute( JobEngine *eng ){
  BUG( BugJobCode,"JobCodeBeep::execute" );
#ifdef HAVE_QT
  QApplication::beep();
#endif
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEndOfFunction::execute( JobEngine *eng ){
  BUG_DEBUG( "JobCodeEndOfFunction::execute" );
  JobStackItemPtr ret( eng->pop() );

  while( ret.is_valid() ){
    if( ret->Type() == stck_ReturnControl ){
      BUG_DEBUG("ReturnControl-Object found");
      return eng->restoreCode( ret.returncontrol() );
    }

    BUG_DEBUG("StackItem is NOT a ReturnControl-Object. Try next");
    ret = eng->pop();
  }

  BUG_DEBUG("no ReturnControl-Object");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeNop::execute( JobEngine *eng ){
  BUG_PARA(BugJobCode,"JobCodeNop::execute",m_msg);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRound::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeRound::execute");
  JobStackDataPtr param1( eng->pop() );
  JobStackDataPtr param2( eng->pop() );
  if( param1.isnt_valid() || param2.isnt_valid() )  return op_FatalError;

  return param2->round( eng, param1 );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRound5::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeRound5::execute");
  JobStackDataPtr param( eng->pop() );
  if( param.isnt_valid() ) return op_FatalError;

  return param->round5( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRound10::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeRound10::execute");
  JobStackDataPtr param( eng->pop() );
  if( param.isnt_valid() ) return op_FatalError;

  return param->round10( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSize::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSize::execute");
  JobStackDataPtr d( eng->pop() );
  JobStackDataPtr v( eng->pop() );
  if( d.isnt_valid() || v.isnt_valid() ) return op_FatalError;

  OpStatus status = v->size( eng, d );
  if( status == op_Warning ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: get size of item '";
    v->printFullName( logmsg, true );
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeDataSize::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDataSize::execute");
  JobStackDataPtr d( eng->pop() );
  JobStackDataPtr v( eng->pop() );
  if( d.isnt_valid() || v.isnt_valid() ) return op_FatalError;
  d->clearAllValues();

  if(!v->getDataReference() || v->getDataReference()->hasWildcards()){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: get data size of item '";
    v->printFullName( logmsg, true );
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    return op_Warning;
  }

  std::ostringstream os;
  v->getDataReference()->writeJSON( os, 0, false, HIDEtransient );
  d->setValue(os.str().size(), 0);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeIndex::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeIndex::execute");
  JobStackDataPtr d( eng->pop() );
  JobStackDataPtr v( eng->pop() );
  if( d.isnt_valid() || v.isnt_valid() ) return op_FatalError;

  return v->index( eng, d );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeIcon::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeIndex::execute");
  JobStackDataPtr d( eng->pop() );
  if(d.isnt_valid()) return op_FatalError;
  std::string icon, file, content;
  if(d->getStringValue(icon)){
    IconManager::ICON_TYPE icon_type;
    if (!IconManager::Instance().getDataUrlFileContent(icon_type, icon, content)) {
      std::cerr << "icon '"<<icon<<"' not found.\n";
    }
  }
  eng->push(new JobStackDataString(content));
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeCompare::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeCompare::execute");

  // first stack data item
  JobStackDataPtr dA( eng->pop() );
  if( dA.isnt_valid() ) return op_FatalError;

  // not cycle mode?
  JobStackDataPtr dB;
  if( !m_bCycle ){
    dB = eng->pop();
    if( dB.isnt_valid() ) return op_FatalError;
  }

  // last stack data item
  JobStackDataPtr dS( eng->pop() );
  if( dS.isnt_valid() ) return op_FatalError;

  OpStatus status;
  // not cycle mode?
  if( !m_bCycle ){
    status = dS->compare( eng, dB, dA );
  }
  else{
    status = dS->compare( eng, dA );
  }

  if( status == op_Warning ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: Compare of item '";
    dA->printFullName( logmsg, true );
    logmsg <<  (m_bCycle ? "' to '" : "' and '");
    if( dB.is_valid() ){
      dB->printFullName( logmsg, true );
    }
    else{
      logmsg <<  "<Invalid>";
    }
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }

  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssignConsistency::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeAssignConsistency::execute");

  // first stack data item (right)
  JobStackDataPtr dA(eng->pop());
  if( dA.isnt_valid() ){
    BUG_DEBUG("FATAL: data right not available");
    return op_FatalError;
  }

  // second stack data item (left)
  JobStackDataPtr dB(eng->pop());
  if( dB.isnt_valid() ){
    BUG_DEBUG("FATAL: data left not available");
    return op_FatalError;
  }

  OpStatus status = dB->checkFormat( eng, dA );
  BUG_DEBUG("status of checkFormat = " << status);

  // call from MessageQueueReply.inc
  // right side: check XSS or HTML Injection
  std::string s;
  dA->getStringValue(s);
  if (!XSSValidation(s)) {
    return op_Aborted;
  }

  if( status == op_Warning ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": WARNING: CheckFormat of item '";
    dB->printFullName( logmsg, true );
    logmsg << "' with '";
    std::string s;
    dA->getStringValue(s);
    logmsg << s;
    logmsg << "' not possible" << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    BUG_DEBUG("Warning: " << logmsg.str());
    eng->pushTrue( false );
  }
  else {
    if (status == op_Canceled || status == op_Aborted) {
      // Consistency check, only cancel assign
      status = op_Aborted;
    }
    else {
      status = dB->assign( eng, dA );
    }
    eng->pushTrue( true );
  }

  BUG_DEBUG("return status " << status);
  return status;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeClass::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeClass::execute");
  GuiElement *guiElement(m_guiElement);

  // get GuiElement or DataReference(UserAttr)
  if (!guiElement) {
    // get GuiElement
    JobStackDataPtr data( eng->pop() );
    if( data.isnt_valid() ) return op_FatalError;
    std::string  guiName;
    if(data->getStringValue(guiName)){
      guiElement = GuiElement::findElement(guiName);
    }
    // get DataReference(UserAttr)
    if (!guiElement) {
      DataReference *ref= data->getDataReference();
      if (ref) {
        UserAttr *userAttr =  dynamic_cast<UserAttr*>(ref->getUserAttr());
        if (userAttr) {
          std::string name;
          ref->getFullName(name, true);
          //userAttr->SetClassname(m_className, );
          userAttr->SetClassname(m_className, name);
          return op_Ok;
        }
      }
    }
  }
  if (guiElement) {
    guiElement->setClass(m_className);
    guiElement->setAttributeChangedFlag(true);
    guiElement->getQtElement()->updateWidgetProperty();
    return op_Ok;
  }
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetResource::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeSetResource::execute");

  if (m_useValue) {
    AppData::Instance().setResource(m_key, m_value);
    return op_Ok;
  }
  // stack data item
  JobStackDataPtr dS( eng->pop() );
  if( dS.isnt_valid() ) return op_FatalError;

  switch( dS->getDataType() ){
  case DataDictionary::type_Real:{
    double d;
    dS->getRealValue(d);
    AppData::Instance().setResource(m_key, d);
    break;
  }
  case DataDictionary::type_Integer:{
    int i;
    dS->getIntegerValue(i);
    AppData::Instance().setResource(m_key, i);
    break;
  }
  case DataDictionary::type_CharData:
  case DataDictionary::type_String:{
    std::string s;
    dS->getStringValue(s);
    AppData::Instance().setResource(m_key, s);
    break;
  }
  case DataDictionary::type_StructVariable:{
    std::ostringstream ostr;
    dS->getDataReference()->writeJSON( ostr, 0, false, HIDEhidden );
    std::string s(ostr.str());
    AppData::Instance().setResource(m_key, s );
    break;
  }
  default:
    std::cerr << "unknown resource type.\n";
    break;
  }

  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeTimestamp::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeTimestamp::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;

  return data->getTimestamp( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeDbModified::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDbModified::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;
  if (!data->isValid()) {
    eng->push( new JobStackDataInteger(0) );
    return op_Ok;
  }

  return data->getDbModified( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetDbTimestamp::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetDbTimestamp::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;

  return data->setDbTimestamp( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeClassname::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeClassname::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;

  return data->getClassname( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */
#include "JobDataReference.h"

JobElement::OpStatus JobCodeNodename::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeNodename::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;

  return data->getNodename( eng );
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssignThis::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAssignThis::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;

  DataReference* dataref = 0;
  bool ret = data->getStructureValue( dataref );
  BUG_MSG( "set THIS to '" << dataref->fullName(true) << "'" );
  if( ret && dataref ){
    eng->getFunction()->replaceData(dataref, 0);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssignFuncIndex::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAssignFuncIndex::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;

  int index;
  if(data->getIntegerValue(index)){
    BUG_MSG( "set INDEX '" << data->getDataReference()->fullName(true) << "' to " << index);
    eng->getFunction()->setIndex(index);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssignFuncReason::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAssignFuncIndex::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;

  std::string reason;
  if(data->getStringValue(reason)){
    reason = lower(reason);
    BUG_MSG( "set REASON '" << data->getDataReference()->fullName(true) << "' to " << reason);
    if (reason == "input")
      eng->getFunction()->setReason(JobElement::cll_Input);
    else if (reason == "insert")
      eng->getFunction()->setReason(JobElement::cll_Insert);
    else if (reason == "dupl")
      eng->getFunction()->setReason(JobElement::cll_Dupl);
    else if (reason == "clear")
      eng->getFunction()->setReason(JobElement::cll_Clear);
    else if (reason == "delete")
      eng->getFunction()->setReason(JobElement::cll_Delete);
    else if (reason == "pack")
      eng->getFunction()->setReason(JobElement::cll_Pack);
    else if (reason == "select")
      eng->getFunction()->setReason(JobElement::cll_Select);
    else if (reason == "unselect")
      eng->getFunction()->setReason(JobElement::cll_Unselect);
    else if (reason == "selectpoint")
      eng->getFunction()->setReason(JobElement::cll_SelectPoint);
    else if (reason == "selectrectangle")
      eng->getFunction()->setReason(JobElement::cll_SelectRectangle);
    else if (reason == "activate")
      eng->getFunction()->setReason(JobElement::cll_Activate);
    else if (reason == "open")
      eng->getFunction()->setReason(JobElement::cll_Open);
    else if (reason == "close")
      eng->getFunction()->setReason(JobElement::cll_Close);
    else if (reason == "drop")
      eng->getFunction()->setReason(JobElement::cll_Drop);
    else if (reason == "move")
      eng->getFunction()->setReason(JobElement::cll_Move);
    else if (reason == "new_connection")
      eng->getFunction()->setReason(JobElement::cll_NewConnection);
    else if (reason == "remove_connection")
      eng->getFunction()->setReason(JobElement::cll_RemoveConnection);
    else if (reason == "remove_element")
      eng->getFunction()->setReason(JobElement::cll_RemoveElement);
    else if (reason == "cycle_clear")
      eng->getFunction()->setReason(JobElement::cll_CycleClear);
    else if (reason == "cycle_delete")
      eng->getFunction()->setReason(JobElement::cll_CycleDelete);
    else if (reason == "cycle_new")
      eng->getFunction()->setReason(JobElement::cll_CycleNew);
    else if (reason == "cycle_rename")
      eng->getFunction()->setReason(JobElement::cll_CycleRename);
    else if (reason == "cycle_switch")
      eng->getFunction()->setReason(JobElement::cll_CycleSwitch);
    else if (reason == "function")
      eng->getFunction()->setReason(JobElement::cll_Function);
    else if (reason == "task")
      eng->getFunction()->setReason(JobElement::cll_Task);
    else if (reason == "guiupdate")
      eng->getFunction()->setReason(JobElement::cll_GuiUpdate);
    else if (reason == "callreasonentries")
      eng->getFunction()->setReason(JobElement::cll_CallReasonEntries);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAssignFuncArguments::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAssignFuncArguments::execute");
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() ) return op_FatalError;
  // DebugOuput in file
  if (__debugLogger__) {
    static int nNb = 0;
    std::ostringstream osFn;
    osFn << "webReq_" << std::setfill('0') << std::setw(5) << ++nNb << ".json";
    std::ofstream osf(osFn.str());
    data->print(osf);
    osf.close();
  }

  // get left(this) DataReference
  DataReference *dref_this = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "this");
  if(!dref_this || !dref_this->isValid()) return op_FatalError;
  std::string thisValue;
  dref_this->GetValue(thisValue);
  if(dref_this && dref_this->isValid()) {
    DataReference* dref_left = DataPoolIntens::Instance().getDataReference(thisValue);

    // get old value
    JobStackData* old_data = 0;
    if(dref_left) {
      switch( dref_left->getDataType() ){
      case DataDictionary::type_Integer:
        {
          int i;
          if( dref_left->GetValue(i)) {
            old_data = new JobStackDataInteger(i);
          }
          break;
        }
      case DataDictionary::type_Real:
        {
          double d;
          if( dref_left->GetValue(d)) {
            old_data = new JobStackDataReal(d);
          }
          break;
        }
      case DataDictionary::type_Complex:
        {
          dComplex c;
          if( dref_left->GetValue(c)) {
            old_data = new JobStackDataComplex(c);
          }
          break;
        }
      case DataDictionary::type_String:
      default:
        {
          std::string s;
          if( dref_left->GetValue(s)) {
            old_data = new JobStackDataString(s);
          }
          break;
        }
      }
    }

    // replace this
    BUG_DEBUG( "set FuncArguments DataRef: '" << data->getDataReference()->fullName(true) << "'");
    DataReference *dref_thisValue = DataPoolIntens::Instance().getDataReference(thisValue);
    if (dref_thisValue) {
      // if old_data is null, thisValue is a datastruct DataReference
      eng->getFunction()->replaceData(dref_thisValue, old_data);
    }
  }

  // set index
  DataReference *dref_index = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "index");
  if(!dref_index || !dref_index->isValid()) return op_FatalError;
  if(dref_index != 0){
    int index;
    if (dref_index->GetValue(index)) {
      eng->getFunction()->setIndex(index);
    }
  }

  // set reason
  std::string reason;
  DataReference *dref_reason = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "reason");
  if(!dref_reason || !dref_reason->isValid()) return op_FatalError;
  if(dref_reason->GetValue(reason)){
    reason = lower(reason);
    BUG_DEBUG( "set REASON '" << data->getDataReference()->fullName(true) << "' to " << reason);
    if (reason == "input")
      eng->getFunction()->setReason(JobElement::cll_Input);
    else if (reason == "insert")
      eng->getFunction()->setReason(JobElement::cll_Insert);
    else if (reason == "dupl")
      eng->getFunction()->setReason(JobElement::cll_Dupl);
    else if (reason == "clear")
      eng->getFunction()->setReason(JobElement::cll_Clear);
    else if (reason == "delete")
      eng->getFunction()->setReason(JobElement::cll_Delete);
    else if (reason == "pack")
      eng->getFunction()->setReason(JobElement::cll_Pack);
    else if (reason == "select")
      eng->getFunction()->setReason(JobElement::cll_Select);
    else if (reason == "unselect")
      eng->getFunction()->setReason(JobElement::cll_Unselect);
    else if (reason == "selectpoint")
      eng->getFunction()->setReason(JobElement::cll_SelectPoint);
    else if (reason == "selectrectangle")
      eng->getFunction()->setReason(JobElement::cll_SelectRectangle);
    else if (reason == "activate")
      eng->getFunction()->setReason(JobElement::cll_Activate);
    else if (reason == "open")
      eng->getFunction()->setReason(JobElement::cll_Open);
    else if (reason == "close")
      eng->getFunction()->setReason(JobElement::cll_Close);
    else if (reason == "drop")
      eng->getFunction()->setReason(JobElement::cll_Drop);
    else if (reason == "move")
      eng->getFunction()->setReason(JobElement::cll_Move);
    else if (reason == "new_connection")
      eng->getFunction()->setReason(JobElement::cll_NewConnection);
    else if (reason == "remove_connection")
      eng->getFunction()->setReason(JobElement::cll_RemoveConnection);
    else if (reason == "remove_element")
      eng->getFunction()->setReason(JobElement::cll_RemoveElement);
    else if (reason == "cycle_clear")
      eng->getFunction()->setReason(JobElement::cll_CycleClear);
    else if (reason == "cycle_delete")
      eng->getFunction()->setReason(JobElement::cll_CycleDelete);
    else if (reason == "cycle_new")
      eng->getFunction()->setReason(JobElement::cll_CycleNew);
    else if (reason == "cycle_rename")
      eng->getFunction()->setReason(JobElement::cll_CycleRename);
    else if (reason == "cycle_switch")
      eng->getFunction()->setReason(JobElement::cll_CycleSwitch);
    else if (reason == "function")
      eng->getFunction()->setReason(JobElement::cll_Function);
    else if (reason == "task")
      eng->getFunction()->setReason(JobElement::cll_Task);
    else if (reason == "guiupdate")
      eng->getFunction()->setReason(JobElement::cll_GuiUpdate);
    else if (reason == "callreasonentries")
      eng->getFunction()->setReason(JobElement::cll_CallReasonEntries);
  }

  // set select index
  DataReference *dref_selIndex = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "select_index");
  DataReference *dref_elem = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "guielement");
  int row(0);
  if(dref_selIndex != 0 && dref_elem != 0){
    XferDataItem *xfer = new XferDataItem(dref_selIndex);
    std::string guiElementId;
    if (dref_elem->GetValue(guiElementId) && dref_selIndex->GetValue(row)) {
      JobCodeSelectRows::executeElement(guiElementId, xfer, row);
    }
    // delete xfer; // delete inside JobCodeSelectRows::executeElement
    delete dref_elem;
  }

  // get source DataReference
  DataReference *dref_source = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "source");
  if(dref_source && dref_source->isValid()) {
    int id[1] = {-1};
    DataVector dataVec;
    std::vector<XferDataItem*> vecXfer;
    dref_source->getDataVector(dataVec, 1, id);
    if (dataVec.size()) {
      for(DataVector::iterator i=dataVec.begin(); i < dataVec.end(); i++) {
        std::string sourceValue;
        (*i)->getValue(sourceValue);
        DataReference* dref_src = DataPoolIntens::Instance().getDataReference(sourceValue);
        if (dref_src)
          vecXfer.push_back(new XferDataItem(dref_src));
      }
      eng->getFunction()->setSourceDataItem(vecXfer);
      BUG_DEBUG( "set FuncArguments SourceRef: '" << data->getDataReference()->fullName(true) << "'");
    }
  }

  // get source2 DataReference
  DataReference *dref_source2 = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "source2");
  if(dref_source2 && dref_source2->isValid()) {
    std::string sourceValue;
    if (dref_source2->GetValue(sourceValue)) {
      DataReference* dref_src = DataPoolIntens::Instance().getDataReference(sourceValue);
      if (dref_src)
        eng->getFunction()->setSource2DataItem(new XferDataItem(dref_src));
      BUG_DEBUG( "set FuncArguments Source2Ref: '" << sourceValue  << "'");
    }
  }

  // get x_pos DataReference
  DataReference *dref_xpos = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "x_pos");
  if((reason == "drop" || reason == "move") &&  !dref_xpos || !dref_xpos->isValid()) return op_FatalError;
  double x;
  if (dref_xpos->GetValue(x)) {
    eng->getFunction()->setDiagramXPos(x);
    BUG_DEBUG( "set FuncArguments DiagramXPos: '" << x  << "'");
  }

  // get y_pos DataReference
  DataReference *dref_ypos = DataPoolIntens::Instance().getDataReference(data->getDataReference(), "y_pos");
  if((reason == "drop" || reason == "move") && !dref_ypos || !dref_ypos->isValid()) return op_FatalError;
  double y;
  if (dref_ypos->GetValue(y)) {
    eng->getFunction()->setDiagramYPos(y);
    BUG_DEBUG( "set FuncArguments DiagramYPos: '" << y  << "'");
  }

  return op_Ok;
}
