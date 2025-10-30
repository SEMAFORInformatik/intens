
#if defined __CYGWIN__ || defined __MINGW32__
#include "utils/FileUtilities.h"
#else
#include <libgen.h>
#endif
#include <iomanip>

#include "app/App.h"

#include "job/JobIncludes.h"

#include "job/JobClientSocket.h"
#include "job/JobRequest.h"
#include "job/JobSubscribe.h"
#include "job/JobMsgQueuePublish.h"
#include "job/JobDataReference.h"
#include "operator/ClientSocket.h"
#include "operator/MessageQueue.h"
#include "operator/MessageQueueSubscriber.h"
#include "job/JobCodeOthers.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackAddress.h"
#include "job/JobStackReturn.h"
#include "job/JobStackDataVariable.h"
#include "job/JobConfirmation.h"
#include "job/JobMessage.h"
#include "gui/GuiElement.h"
#include "gui/GuiForm.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiManager.h"
#include "gui/GuiFolderGroup.h"
#include "gui/GuiList.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiTable.h"
#include "gui/Gui2dPlot.h"
#include "job/JobStackDataString.h"
#include "xfer/XferDataItem.h"

#include "job/JobStarter.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiFactory.h"
#include "datapool/DataVector.h"
#include "datapool/DataValue.h"
#include "utils/gettext.h"
#include "utils/Date.h"
#include "app/UserAttr.h"

#if defined __CYGWIN__  // || defined __MINGW32__
std::string basename(char *__path) {
  std::string path(__path);
  if (path.empty()) return std::string();
  char sep = '/';
  std::string::size_type pos = path.find_last_of( sep );
  if (pos == std::string::npos)
    return path;
  else
    return path.substr(pos+1);
}

std::string dirname(char * __path) {
  std::string path(__path);
  if (path.empty()) return std::string();

  char sep = '/';
  std::string::size_type pos = path.find_last_of( sep );
  if (pos == std::string::npos)
    return std::string("./");
  else
    return path.substr(0, pos);
}
#endif

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeUpdateGuiElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeUpdateGuiElement::execute");
  m_element->update( GuiElement::reason_Always );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEnableGuiElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEnableGuiElement::execute");
  m_element->enable();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeDisableGuiElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDisableGuiElement::execute");
  m_element->disable();
  BUG_EXIT("normal exit");
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeDisableDragGuiElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDisableDragGuiElement::execute");
  m_element->disable_drag();
  BUG_EXIT("normal exit");
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetStylesheet::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDisableGuiElement::execute");
  std::string stylesheet;

  // get stylesheet string
  JobStackDataPtr dat_str( eng->pop() );
  if (dat_str.isnt_valid()) return op_FatalError;
  if (!dat_str->getStringValue(stylesheet)) return op_FatalError;

  // set stylesheet string
  if (!m_element) {
    // set UserAttr stylesheet Attribute
    JobStackDataPtr dat_ptr( eng->pop() );
    if (dat_ptr.isnt_valid()) return op_FatalError;
    OpStatus status = dat_ptr->setStylesheet(eng, stylesheet);
    BUG_EXIT("Status = " << status );
    return status;
  } else {
    // if gui element, call GuiElement::setStylesheet
    m_element->setStylesheet(stylesheet);
  }
  BUG_EXIT("normal exit");
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeReplaceGuiElement::execute( JobEngine *eng ){
  BUG_INFO("JobCodeReplaceGuiElement::execute "
           << m_old_element->getName() << " by " << m_new_element->getName());
  if( !GuiFactory::Instance()->replace( m_old_element, m_new_element ) ){
    eng->ErrorStream() << "Replace of a GuiElement failed. Old Element not found";
    eng->flushErrorStream();
    return op_Warning;
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGuiElementMethod::execute( JobEngine *eng ){
  BUG_DEBUG("JobCodeGuiElementMethod::execute");
  std::string method, args;

  // get arguments
  JobStackDataPtr argsData( eng->pop() );
  if(argsData.isnt_valid()) return op_FatalError;

  // get method
  JobStackDataPtr methodData( eng->pop() );
  if(methodData.isnt_valid()) return op_FatalError;

  GuiElement *elem = m_gui_element;
  // GuiElement on stack
  if (!elem) {
    std::string s;
    JobStackDataPtr e( eng->pop() );
    if(e.isnt_valid()) return op_FatalError;
    e->getStringValue( s );
    elem = GuiElement::findElement(s);

    // search for element id
    if (!elem) {
      elem = GuiElement::findElementId(s);
    }
    if( !elem ) {
      BUG_ERROR("GuiElement not found, named: " << s);
      return op_FatalError;
    }
  }

  // get method
  methodData->getStringValue(method);

  // get arguments
  argsData->getStringValue(args);

  Json::Value jsonArg = Json::Value(Json::objectValue);
  try {
    if (args.size())
      jsonArg = ch_semafor_intens::JsonUtils::parseJson(args);
  }
  catch ( ...) {
    BUG_INFO("parser error");
  }
  std::string sret = elem->variantMethod(method, jsonArg, eng);
  eng->push(new JobStackDataString(sret));

  // if( !GuiFactory::Instance()->replace( m_old_element, m_new_element ) ){
  //   eng->ErrorStream() << "Replace of a GuiElement failed. Old Element not found";
  //   eng->flushErrorStream();
  //   return op_Warning;
  // }
  BUG_DEBUG("normal exit");
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetGuiFieldgroupRange::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetGuiFieldgroupRange::execute");
  JobStackDataPtr t( eng->pop() );
  JobStackDataPtr f( eng->pop() );

  int from = 0;
  if( f->getIntegerValue( from ) ){
    int to = 0;
    if( !t->getIntegerValue( to ) ){
      to = 0;
    }
    m_fieldgroup->setTableIndexRange( from, to );
    BUG_EXIT("Range set to (" << from << "," << to << ")");
  }
  else{
    BUG_EXIT("From-Value not valid");
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeAllow::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeAllow::execute");
  m_controller->allow();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeDisallow::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeDisallow::execute");
  m_controller->disallow();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --   JobCodeSerializeForm                                           */
/* --------------------------------------------------------------------------- */
JobElement::OpStatus JobCodeSerializeForm::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSerializeForm::execute");
  serializeForm();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* serialize --   JobCodeSerializeForm                                         */
/* --------------------------------------------------------------------------- */
bool JobCodeSerializeForm::serializeForm(){
  BUG(BugJobCode,"JobCodeSerializeForm::serialize");
//   std::string fn ( "./serializedForm_"+m_form->Title()+".xml" );
  std::string fn ( "./serializedForm.xml" );
  std::ofstream serializeStream;
  serializeStream.open( fn.c_str() );
  if( serializeStream.is_open() ){
    serializeStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<std::endl;
    serializeStream << "<intens:Document xmlns:intens=\"http://www.semafor.ch/XML/2003/intens/1.0\">" << std::endl;
    m_form->serializeXML(serializeStream);
    serializeStream << "</intens:Document>" << std::endl;
  }
  serializeStream.close();
  return true;
}

/* --------------------------------------------------------------------------- */
/* execute --   JobCodeSerializeGuiElement                                     */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSerializeGuiElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSerialize::execute");
  std::string fn( m_filename );

  if( m_readStack ){
    JobStackDataPtr f( eng->pop() );
    f->getStringValue( fn );
  }

  if( fn.empty() ){
    GuiFactory::Instance()->showDialogWarning( 0,  _("Error"), _("Empty file name"), 0 );
    eng->setError();
    return op_FatalError; //op_Aborted;
  }

  std::ofstream serializeStream;
  serializeStream.open( fn.c_str() );
  if (m_type == AppData::serialize_XML) {
    m_guiElem->serializeXML(serializeStream, true);
  }
  else if( m_type == AppData::serialize_JSON ){
      Json::Value jsonObj = Json::Value(Json::objectValue);
      m_guiElem->serializeJson(jsonObj, false);
      serializeStream << ch_semafor_intens::JsonUtils::value2string(jsonObj);
      // DebugOuput in file
      if (__debugLogger__) {
        static int nNb = 0;
        std::ostringstream osFn;
        osFn << "webGui_" << std::setfill('0') << std::setw(5) << ++nNb << ".json";
        std::ofstream osf(osFn.str());
        osf << ch_semafor_intens::JsonUtils::value2string(jsonObj, true);
        osf.close();
      }
  } else if(m_type == AppData::serialize_PROTO) {
#if HAVE_PROTOBUF
    auto eles = in_proto::ElementList();
    m_guiElem->serializeProtobuf(&eles);
    eles.SerializeToOstream(&serializeStream);
#endif
  }

  serializeStream.close();
  return op_Ok;
}
/* --------------------------------------------------------------------------- */
/* JobCodeSerializeElementHelper --                                            */
/* --------------------------------------------------------------------------- */

static void JobCodeSerializeElementHelper( GuiElement* elem
					 , std::ostream& os
					 , AppData::SerializeType type )
{
  if( type == AppData::serialize_XML ){
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    os << "<intens:Document xmlns:intens=\"http://www.semafor.ch/XML/2003/intens/1.0\">" << std::endl;
    elem->serializeXML(os, true);
    os << "</intens:Document>" << std::endl;
  }
  else if( type == AppData::serialize_JSON ){
      Json::Value jsonObj = Json::Value(Json::objectValue);
      elem->serializeJson(jsonObj, false);
      os << ch_semafor_intens::JsonUtils::value2string(jsonObj);
      // DebugOuput in file
      if (__debugLogger__) {
        static int nNb = 0;
        std::ostringstream osFn;
        osFn << "webGui__" << std::setfill('0') << std::setw(5) << ++nNb << ".json";
        std::ofstream osf(osFn.str());
        osf << ch_semafor_intens::JsonUtils::value2string(jsonObj, true);
        osf.close();
      }
  } else if(type == AppData::serialize_PROTO) {
#if HAVE_PROTOBUF
    auto eles = in_proto::ElementList();
    elem->serializeProtobuf(&eles);
    eles.SerializeToOstream(&os);
#endif
  }
}

/* --------------------------------------------------------------------------- */
/* execute --   JobCodeSerializeElement                                        */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSerializeElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSerialize::execute");
  std::string fn( m_filename );
  //  JobStackData *f;

  // get filename
  JobStackDataPtr f;
  if( m_filename.empty() ){
    f = eng->pop();
    f->getStringValue( fn );
  }

  // get element
  GuiElement* elem(m_guiElem);
  if( !elem ){
    std::string s;
    JobStackDataPtr e( eng->pop() );
    e->getStringValue( s );
    elem = GuiElement::findElement(s);
    if( !elem ){
#if 1
      // default GuiElement is MainForm
      GuiElementList mlist;
      GuiElement::findElementType(mlist, GuiElement::type_Main);
      elem = mlist[0];
#else
      GuiFactory::Instance() -> showDialogWarning
		( 0
		  , _("Error")
		  , compose(_("Unknown element named '%1'"), s)
		  , 0 );
      eng->setError();
      return op_FatalError; //op_Aborted;
#endif
    }
  }

  if( m_filename.empty() ){
    std::ostringstream os;
    JobCodeSerializeElementHelper( elem, os, m_type );
    f->setStringValue( os.str(), 0 );
  }
  else{
    std::ofstream os;
    os.open( fn.c_str() );
    JobCodeSerializeElementHelper( elem, os, m_type );
    os.close();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --   JobCodeWriteSettings                                           */
/* --------------------------------------------------------------------------- */
JobElement::OpStatus JobCodeWriteSettings::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeWriteSettings::execute");
  GuiManager::Instance().writeSettings();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */
#include "gui/GuiFolder.h"

JobElement::OpStatus JobCodeMap::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMap::execute");

  // used for foldertab only
  int index(-1);
  if(m_nargs == -1) {
    // none: no foldertab given
  } else if (m_nargs == -2) {
    // job_data_reference: foldertab given
    JobStackDataPtr a( eng->pop() );
    if(a.isnt_valid()) return op_FatalError;
    if (!a->getIntegerValue(index))
      index = 0;
    if (index < 0) {
      // tab index is never negative
      std::stringstream logmsg;
      logmsg << DateAndTime()
             << compose(_(": MAP: negative tab index %1 is ignored (nothing is mapped)."),
                        index
                        )
             << std::endl;
      GuiFactory::Instance()->getLogWindow()->writeText(logmsg.str());
      return op_Warning;
    }
  } else {
    // INT_CONSTANT: foldertab given
    index = m_nargs;
  }

  GuiElement *guiElem(m_elem);
  std::string name;
  if( m_xfer && m_xfer->getValue( name ) ) {
    guiElem = GuiElement::findElement( name );
  }
  if (guiElem) {
    switch (guiElem->Type()) {
    case GuiElement::type_Form:
      // map Form
      guiElem->manage();
      break;
    case GuiElement::type_Folder:
      if (index != -1) {
        if(guiElem->getFolder()->getPage(index) == 0) {
          // index exceeds defined pages
          std::stringstream logmsg;
          logmsg << DateAndTime()
                 << compose(_(": MAP: not existing tab index %1 is ignored (nothing is mapped)."),
                            index
                            )
                 << std::endl;
          GuiFactory::Instance()->getLogWindow()->writeText(logmsg.str());
          return op_Warning;
        }

        // activate folder tab by index
        if (AppData::Instance().HeadlessWebMode()) {
          BUG_INFO("?? FOLDERTAB activePage=" << index);
          ///          guiElem->getFolder()->getHiddenIndex(index);
          BUG_INFO("=> FOLDERTAB activePage=" << index);
        }
        guiElem->getFolder()->activatePage(index, GuiFolderGroup::omit_Default, false );
        // GuiUpdate bzw. serialize erwzingen
        guiElem->getFolder()->getPage(index)->setLastGuiUpdated(0);
      } else {
        // map GuiFolder
        guiElem->map();
      }
      break;
    default:
      // map GuiElement
      guiElem->map();
      break;
    }

  } else {
    // activate folder tab
    GuiFolderGroup::activateByName( 0, 0, name, GuiFolderGroup::omit_Default );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeUnmap::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeUnmap::execute");
  std::string name;
  GuiElement *guiElem = 0;
  JobStackDataPtr data( eng->pop() );
  if (data->reference().getDataReference()->GetValue_PreInx( name )){
    guiElem = GuiElement::findElement( name );
  }
  BUG_INFO("JobCodeUnmap name: " << name << ", guiElem: " << guiElem);
  if (guiElem) {
    switch (guiElem->Type()) {
    case GuiElement::type_Form:
      // close Form
      guiElem->unmanage();
      break;
    default:
      // unmap GuiElement
      guiElem->unmap();
      break;
    }

  } else if (name.size()) {
    // hide folder tab
    GuiFolderGroup::hideByName(name);
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeVisible::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeVisible::execute");
  std::string name;
  if( m_xfer && m_xfer->getValue( name ) ) {

    int value(0);
    GuiElement *guiElem = GuiElement::findElement( name );
    if (guiElem) {
      switch (guiElem->Type()) {
      case GuiElement::type_Form:
        // return values
        // 0 => invisible
        // 1 => visible
        // 2 => iconic
        value = guiElem->getForm()->isShown() ? (guiElem->getForm()->isIconic() ? 2 : 1) : 0;
        break;
      default:
        // other GuiElement
        break;
      }

    } else {
      // folder tab
      value = GuiFolderGroup::isActive( name );
    }
    eng->push( new JobStackDataInteger( value ) );
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeEditable::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeEditable::execute");
  bool ret;
  DATAAttributeMask mask;
  if (m_element){
    ret = m_element->isEnabled();
  }else{
    JobStackDataPtr data( eng->pop() );
    mask = data->reference().getDataReference()->getAttributes( GuiManager::Instance().LastGuiUpdate() );
    if(mask & DATAIsEditable && mask & DATAIsReadOnly )
      ret =  false;
    else
      if( mask & DATAeditable ){
        ret =  (mask & DATAIsReadOnly ) == 0;
      }
      else
        ret = ( mask & DATAIsEditable ) != 0;
  }
  eng->push(new JobStackDataInteger(ret));
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetCurrentForm::execute( JobEngine *eng ){
  BUG(BugJobCode,"JJobCodeSetCurrentForm::execute");
  std::string name;
  if( m_xfer && m_xfer->getValue( name ) ) {

    int value(0);
    GuiElement *guiElem = GuiElement::findElement( name );
    if (guiElem && guiElem->getForm()) {
      GuiElement::setTopForm(guiElem->getForm());
    }
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeMapForm::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMapForm::execute");
  if( m_form->getListenerController()->sensitive() ){
    m_form->getElement()->manage();
  }
  else{
    eng->ErrorStream() << "MAP of Form '" << m_form->getTitle() << "' is disallowed";
    eng->flushErrorStream();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeUnmapForm::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeUnmapForm::execute");
  m_form->getElement()->unmanage();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeVisibleForm::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeVisibleForm::execute");
  // return values
  // 0 => invisible
  // 1 => visible
  // 2 => iconic
  int value = m_form->isShown() ? (m_form->isIconic() ? 2 : 1) : 0;
  eng->push( new JobStackDataInteger( value ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeMapFolderGroup::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMapFolderGroup::execute");
  GuiFolderGroup::activateByName( 0, 0, m_groupname, m_omit_map );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeUnmapFolderGroup::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeUnmapFolderGroup::execute");
  GuiFolderGroup::hideByName( m_groupname );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeVisibleFolderGroup::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeVisibleFolderGroup::execute");
  bool ret = GuiFolderGroup::isActive( m_groupname );
  eng->pushTrue(ret);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeMapGuiElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMapGuiElement::execute");
  GuiElement *e = GuiElement::findElement( m_elemname );
  if (e)
    e->map();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeUnmapGuiElement::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeUnmapGuiElement::execute");
  GuiElement *e = GuiElement::findElement( m_elemname );
  if (e)
    e->unmap();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeMapTableLine::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMapTableLine::execute");
  GuiElement *e = GuiElement::findElement( m_tablename );
  GuiTable *table = e->getTable();
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() || table == 0) return op_FatalError;

  // process
  table->mapTableLine(data->getDataReference());
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeUnmapTableLine::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeUnmapTableLine::execute");
  GuiElement *e = GuiElement::findElement( m_tablename );
  GuiTable *table = e->getTable();
  JobStackDataPtr data( eng->pop() );
  if( data.isnt_valid() || table == 0) return op_FatalError;

  // process
  table->unmapTableLine(data->getDataReference());
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSelectRows::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSelectRows::execute");
  DataReference * dref(0);
  XferDataItem *xfer(0);
  int row(0);
  JobStackDataPtr dat_int( eng->pop() );
  if( dat_int.isnt_valid() ) return op_FatalError;
  dat_int->getStructureValue(dref);
  xfer = dref ? new XferDataItem(dref) : 0;
  if (m_dataIsInt) {
    if (!dat_int->getIntegerValue(row)) {
      xfer->getValue(row);
    }
  }
  return executeElement(m_guiElementId, xfer, row);
}

/* --------------------------------------------------------------------------- */
/* executeElement --                                                           */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSelectRows::executeElement(const std::string& guiElementId,  XferDataItem *xfer,
                                                       int row){
  ///  DataReference *dref(0);
  GuiElement *e = GuiElement::findElement(guiElementId);
  assert( e != 0 );

  // Liste
  if ( e->Type() == GuiElement::type_List ) {
    GuiList *list = e->getList();
    assert( list != 0 );

    // special case: multiple selection list
    if (list->isMultipleSelection()) {
      if (xfer) {
        DataVector dVec;
        if (xfer->Data()->hasWildcards()) {
          xfer->getValues(dVec);
        } else {
          int id[1] = {-1};
          xfer->Data()->getDataVector(dVec, 1, id);
        }
        delete xfer;
        std::vector<int> rows;
        int r;
        for (DataVector::iterator it = dVec.begin();it != dVec.end(); ++it) {
          if ((*it)->isValid() && (*it)->getValue(r)){
            rows.push_back(r);
          }
        }
        if (!list->selectRows(rows)){
          return op_FatalError;
        }
        return op_Ok;
      }
    }

    return list->selectRow(row) ? op_Ok : op_FatalError;
  }

  // Table
  if ( e->Type() == GuiElement::type_Table ) {
    GuiTable *table = e->getTable();
    assert( table != 0 );

    // special case: multiple selection table
    if (table->isMultipleSelection()) {
      if (xfer) {
        DataVector dVec;
        if (xfer->Data()->hasWildcards()) {
          xfer->getValues(dVec);
        } else {
          int id[1] = {-1};
          xfer->Data()->getDataVector(dVec, 1, id);
        }
        delete xfer;
        std::vector<int> rows;
        int r;
        for (DataVector::iterator it = dVec.begin();it != dVec.end(); ++it) {
          if ((*it)->isValid() && (*it)->getValue(r)){
            rows.push_back(r);
          }
        }
        if (!table->selectRows(rows)){
          return op_FatalError;
        }
        return op_Ok;
      }
    }

    // general case: select simple row
    return table->selectRow(row) ? op_Ok : op_FatalError;
  }

  // Navigator
  if ( e->Type() == GuiElement::type_NavDiagram ) {
    GuiNavigator *navi = e->getNavigator();
    assert( navi != 0 );
    std::string cid;

    if (xfer) {
      navi->clearSelection();
      DataVector dVec;
      if (!xfer->getValues(dVec)) {
        // no wildcard?
        if (xfer->getValue(cid)) {
          navi->setItemSelection(cid);
        }
      }
      delete xfer;

      std::vector<std::string> vec;
      for (DataVector::iterator it = dVec.begin();it != dVec.end(); ++it) {
        if ((*it)->isValid() && (*it)->getValue(cid)){
          vec.push_back(cid);
        }
        navi->setItemSelection(cid);
      }
    }
    return op_Ok;
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetSelection::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetSelection::execute");
  GuiElement *e = GuiElement::findElement( m_guiElementId );
  assert( e != 0 );

  // Liste
  if ( e->Type() == GuiElement::type_List ) {
    GuiList *list = e->getList();
    assert( list != 0 );

    JobStackDataPtr dat_int( eng->pop() );
    if( dat_int.isnt_valid() ) return op_FatalError;

    dat_int->clearAllValues();
    int row;
    std::vector<int> idxs;
    list->getSelectedRows(idxs);
    std::vector<int>::iterator it = idxs.begin();
    for (long i=0; it != idxs.end(); ++i, ++it){
      dat_int->setValue( (*it), i );
    }
    return op_Ok;
  }

  // Table
  if ( e->Type() == GuiElement::type_Table ) {
    GuiTable *table = e->getTable();
    assert( table != 0 );

    JobStackDataPtr dat_cint( eng->pop() );
    if( dat_cint.isnt_valid() ) return op_FatalError;
    dat_cint->clearAllValues();

    JobStackDataPtr dat_rint( eng->pop() );
    if( dat_rint.isnt_valid() ) return op_FatalError;
    dat_rint->clearAllValues();

    std::vector<int> ridxs;
    std::vector<int> cidxs;
    table->getSelectedItems(ridxs,cidxs);

    std::vector<int>::iterator rit = ridxs.begin();
    std::vector<int>::iterator cit = cidxs.begin();
    for (long i=0; rit != ridxs.end() || cit != cidxs.end(); ++i) {
      if (rit != ridxs.end()) {
	dat_rint->setValue( (*rit), i );
	++rit;
      }
      if (cit != cidxs.end()) {
	dat_cint->setValue( (*cit), i );
	++cit;
      }
    }
    return op_Ok;
  }

  // Navigator
  if ( e->Type() == GuiElement::type_Navigator ) {
    GuiNavigator *navigator = e->getNavigator();
    assert( navigator != 0 );

    JobStackDataPtr dat_str( eng->pop() );
    if( dat_str.isnt_valid() ) return op_FatalError;
    dat_str->clearAllValues();

    int row;
    std::vector<std::string> selectedList;
    navigator->getItemsSelected(selectedList);
    std::vector<std::string>::iterator it = selectedList.begin();
    for (long i=0; it != selectedList.end(); ++i, ++it){
      dat_str->setStringValue( (*it), i );
    }
    return op_Ok;
  }

  // QWTPlot
  if ( e->Type() == GuiElement::type_QWTPlot ) {
    Gui2dPlot *plot = e->get2dPlot();
    assert( plot != 0 );

    JobStackDataPtr dat_str( eng->pop() );
    if( dat_str.isnt_valid() ) return op_FatalError;
    dat_str->clearAllValues();

    JobStackDataPtr dat_int( eng->pop() );
    if( dat_int.isnt_valid() ) return op_FatalError;
    dat_int->clearAllValues();

    JobStackDataPtr dat_y( eng->pop() );
    if( dat_y.isnt_valid() ) return op_FatalError;
    dat_y->clearAllValues();

    JobStackDataPtr dat_x( eng->pop() );
    if( dat_x.isnt_valid() ) return op_FatalError;
    dat_x->clearAllValues();

    // get selection points from Gui2dPlot
    Gui2dPlot::tPointVector pts;
    std::vector<int> axisType;
    std::vector<std::string> axisTitle;
    plot->getSelectionPoints(pts, axisType, axisTitle);
    for (long i=0; i < pts.size() && i < axisType.size(); ++i) {
      dat_x->setRealValue( pts[i].y, i );
      dat_y->setRealValue( pts[i].y, i );
      dat_int->setValue( axisType[i], i );
      dat_str->setStringValue( axisTitle[i], i );
    }
    return op_Ok;
  }

  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetSortCriteria::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetSortCriteria::execute");
  GuiElement *e = GuiElement::findElement( m_guiElementId );
  assert( e != 0 );

  // Liste
  if ( e->Type() == GuiElement::type_List ) {
    GuiList *list = e->getList();
    assert( list != 0 );

    JobStackDataPtr dat_int( eng->pop() );
    if( dat_int.isnt_valid() ) return op_FatalError;

    dat_int->clearAllValues();
    int row;
    std::string sortCriteria;
    if (list->getSortCriteria(sortCriteria)){
      dat_int->setStringValue(sortCriteria, 0);
    } else {
      dat_int->setInvalid();
     }
    return op_Ok;
  }
  return op_Ok;
};

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeClearSelection::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeClearSelection::execute");
  GuiElement *e = GuiElement::findElement( m_guiElementId );
  assert( e != 0 );

  // Liste
  if ( e->Type() == GuiElement::type_List ) {
    GuiList *list = e->getList();
    assert( list != 0 );

    list->clearSelection();
  }

  // Table
  if ( e->Type() == GuiElement::type_Table ) {
    GuiTable *table = e->getTable();
    assert( table != 0 );

    table->clearSelection();
  }

  // Navigator
  if ( e->Type() == GuiElement::type_Navigator ||
       e->Type() == GuiElement::type_NavDiagram ||
       e->Type() == GuiElement::type_NavIconView) {
    GuiNavigator *navigator = e->getNavigator();
    assert( navigator != 0 );

    navigator->clearSelection();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeCopy::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeCopy::execute");
  bool ret = GuiFactory::Instance()->doCopy(m_stream, m_element);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePaste::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePaste::execute");
  bool ret = GuiFactory::Instance()->doPaste(m_stream);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeParse::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeParse::execute");

  // from file
  if( !m_filename.empty() ){
    if (App::Instance().parse(m_filename)) {
      eng->pushTrue(true);
    }
    else{
      eng->pushTrue(false);
    }
    return op_Ok;
  }

  std::string str;
  // from xfer
  if( m_xfer && !m_xfer->getValue( str ) ){
    BUG_MSG("emtpy string to parse.");
  }

  // parse
  bool ret = App::Instance().parse(str);
  eng->pushTrue(ret);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeConfirm::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeConfirm::execute");
  JobStackDataPtr param( eng->pop() );

  JobConfirmation *confirm = new JobConfirmation();
  confirm->setElement( eng->getFunction()->getGuiElement() );

  std::string msg;
  if( !param->getStringValue( msg ) ){
    msg = "no message available";
  }

  confirm->setMessage( msg );
  confirm->addCancelButton( m_cancelBtn );
  confirm->setButtonText( m_buttonText );

  BUG_MSG("start Controller" );
  eng->getFunction()->nextJobAction( confirm );
  return op_Wait;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetText::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetText::execute");
  JobStackDataPtr param( eng->pop() );
  std::string msg;

  if( !param->getStringValue( msg ) ){
    msg = "no message available";
  }

  bool ok;
  std::string s;
  s = GuiFactory::Instance()->showDialogTextInput(0,
                                                  "GetInput",
                                                  msg, "",
                                                  ok,
                                                  0);
  BUG_DEBUG("getText: " << s << ", ok: " << ok);
  if (ok)
    eng->push( new JobStackDataString(s) );
  else
    eng->pushInvalid();
  return op_Ok;

}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeMessageBox::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMessageBox::execute");

  BUG_MSG( "new Message" );
  JobMessage *message = new JobMessage();
  message->setElement( eng->getFunction()->getGuiElement() );

  // get title
  if (m_with_title){
    BUG_MSG("get Title" );
    std::string title;
    JobStackDataPtr paramTitle( eng->pop() );
    assert( paramTitle.is_valid() );
    if( paramTitle->getStringValue( title ) ){
      message->setTitle( title );
    }
  }

  // get text
  BUG_MSG("get Text" );
  std::string msg;
  JobStackDataPtr param( eng->pop() );
  assert( param.is_valid() );
  if( !param->getStringValue( msg ) ){
    msg = "no message available";
  }

  BUG_MSG("set Text" );
  message->setMessage( msg );

  BUG_MSG("start Controller" );
  eng->getFunction()->nextJobAction( message );
  return op_Wait;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeMessage::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeMessage::execute");
  JobStackDataPtr param( eng->pop() );
  if( param.isnt_valid() ) return op_FatalError;

  param->print( eng->MessageStream() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetMessage::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetMessage::execute");
  eng->setMessage();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetFileName::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetFileName::execute");
  // Auf dem Stack liegt der file stream
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  std::string name;
  if( !v->getStringValue( name ) ){
    name="<INVALID>";
  }

  eng->push( new JobStackDataString( name ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetBaseName::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetBaseName::execute");
  // Auf dem Stack liegt der file stream
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  std::string name;
  if( !v->getStringValue( name ) ){
    name="<INVALID>";
  }
  else {
    if( name.size() > 0 ){
      char *buf = new char [name.size()+1];
      for( int i=0; i<=name.size(); ++i ) buf[i]= name[i];
      name=basename( buf );
      delete[] buf;
    }
  }
  eng->push( new JobStackDataString( name ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeGetDirName::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeGetDirName::execute");
  // Auf dem Stack liegt der file stream
  JobStackDataPtr v( eng->pop() );
  if( v.isnt_valid() ) return op_FatalError;

  std::string name;
  if( !v->getStringValue( name ) ){
    name="<INVALID>";
  }
  else {
    if( name.size() > 0 ){
      char *buf = new char [ name.size()+1 ];
      for( int i=0; i<=name.size(); ++i ) buf[i]= name[i];
      name = dirname(buf);
      delete[] buf;
    }
  }
  eng->push( new JobStackDataString( name ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSend::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSend::execute");

  if( m_job -> checkInput() ){
    eng->getFunction()->nextJobAction( m_job );
    return op_Wait;
  }
  GuiFactory::Instance() -> showDialogWarning
    ( 0
      , _("Error")
      , m_job -> getErrorMessage()
      , 0 );
  eng -> setError();
  return op_FatalError; //op_Aborted;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeRequest::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeRequest::execute");

  if( m_job->checkMessageQueue() ){
    eng->getFunction()->nextJobAction( m_job );
    return op_Wait;
  }
  GuiFactory::Instance() -> showDialogWarning
    ( 0
      , _("Error")
      , m_job->getErrorMessage()
      , 0 );
  eng -> setError();
  return op_FatalError; //op_Aborted;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSubscribe::execute( JobEngine* eng ){
  BUG(BugJobCode,"JobCodeSubscribe::execute");

  if( m_job->checkSubscribe() ){
    eng->getFunction()->nextJobAction( m_job );
    return op_Wait;
  }
  GuiFactory::Instance() -> showDialogWarning
    ( 0
      , _("Error")
      , m_job->getErrorMessage()
      , 0 );
  eng -> setError();
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePublish::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePublish::execute");

  if( m_job->checkMessageQueue() ){
    eng->getFunction()->nextJobAction( m_job );
    return op_Wait;
  }
  return op_FatalError; //op_Aborted;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetMessageQueueHost::execute( JobEngine *eng ){
  BUG(BugJobCode, "JobCodeSetMessageQueueHost::execute");
  JobStackDataPtr dat_host( eng->pop() );
  JobStackDataPtr dat_name( eng->pop() );

  if( dat_name.isnt_valid() || dat_host.isnt_valid() ){
    BUG_DEBUG("--> Name or Host missing");
    return op_FatalError;
  }

  std::string name;
  std::string host;
  if( !dat_name->getStringValue( name ) ){
    BUG_DEBUG("--> No String as Name available");
    return op_FatalError; //op_Aborted;
  }
  if( !dat_host->getStringValue( host ) ){
    BUG_DEBUG("--> No String as Hostname available");
    return op_FatalError; //op_Aborted;
  }

  MessageQueueSubscriber *sub = MessageQueue::Instance().getSubscriber( name );
  if( !sub ){
    BUG_DEBUG("--> No Subscriber " << name << " available");
    return op_FatalError; //op_Aborted;
  }

  if( !sub->setHost( host ) ){
    BUG_DEBUG("--> failed to set hostname to " << host);
    return op_FatalError; //op_Aborted;
  }

  BUG_DEBUG("--> Hostname is " << host);
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePrintLog::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePrintLog::execute");
  App::Instance().printLog();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeIsoTime::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeIsoTime::execute");
  eng->push( new JobStackDataString( Date::currentIsoTime() ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeIsoDate::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeIsoDate::execute");
  eng->push( new JobStackDataString( Date::currentIsoDate() ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeIsoDateTime::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeIsoDateTime::execute");
  eng->push( new JobStackDataString( Date::currentIsoDateTime() ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeCompose::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeIsoDateTime::execute");
  std::vector<std::string> valueVec;
  std::string s;

  // get values
  int i = m_numArgs;
  while (--i >= 0) {
    JobStackDataPtr d( eng->pop() );
    if( d.is_valid() ){
      if (d->getStringValue( s )){
	valueVec.push_back(s);
      }
      else{
	valueVec.push_back(" "); // !!! Ein Leerzeichen muss wegen compose sein
      }
    }
  }

  // get formatString
  std::string format(valueVec.back());
  valueVec.pop_back();

  // compose up to 15 parameter.
  switch(valueVec.size()) {
    case 0:
	  s = format;
    case 1:
	  s = compose(format, valueVec[0]);
	  break;
    case 2:
	  s = compose(format, valueVec[1], valueVec[0]);
	  break;
    case 3:
	  s = compose(format, valueVec[2], valueVec[1], valueVec[0]);
	  break;
    case 4:
	  s = compose(format, valueVec[3], valueVec[2], valueVec[1], valueVec[0]);
	  break;
    case 5:
	  s = compose(format, valueVec[4], valueVec[3], valueVec[2], valueVec[1],
				  valueVec[0]);
	  break;
    case 6:
	  s = compose(format, valueVec[5], valueVec[4], valueVec[3], valueVec[2],
				  valueVec[1], valueVec[0]);
	  break;
    case 7:
	  s = compose(format, valueVec[6], valueVec[5], valueVec[4], valueVec[3],
				  valueVec[2], valueVec[1], valueVec[0]);
	  break;
    case 8:
	  s = compose(format, valueVec[7], valueVec[6], valueVec[5], valueVec[4],
				  valueVec[3], valueVec[2], valueVec[1], valueVec[0]);
	  break;
    case 9:
	  s = compose(format, valueVec[8], valueVec[7], valueVec[6], valueVec[5],
				  valueVec[4], valueVec[3], valueVec[2], valueVec[1], valueVec[0]);
	  break;
    case 10:
	  s = compose(format, valueVec[9], valueVec[8], valueVec[7], valueVec[6],
				  valueVec[5], valueVec[4], valueVec[3], valueVec[2], valueVec[1],
				  valueVec[0]);
	  break;
    case 11:
	  s = compose(format, valueVec[10], valueVec[9], valueVec[8], valueVec[7],
				  valueVec[6], valueVec[5], valueVec[4], valueVec[3], valueVec[2],
				  valueVec[1], valueVec[0]);
	  break;
    case 12:
	  s = compose(format, valueVec[11], valueVec[10], valueVec[9], valueVec[8],
				  valueVec[7], valueVec[6], valueVec[5], valueVec[4], valueVec[3],
				  valueVec[2], valueVec[1], valueVec[0]);
	  break;
    case 13:
	  s = compose(format, valueVec[12], valueVec[11], valueVec[10], valueVec[9],
				  valueVec[8], valueVec[7], valueVec[6], valueVec[5], valueVec[4],
				  valueVec[3], valueVec[2], valueVec[1], valueVec[0]);
	  break;
    case 14:
	  s = compose(format, valueVec[13], valueVec[12], valueVec[11], valueVec[10],
				  valueVec[9], valueVec[8], valueVec[7], valueVec[6], valueVec[5],
				  valueVec[4], valueVec[3], valueVec[2], valueVec[1], valueVec[0]);
	  break;
    default:
    case 15:
	  s = compose(format, valueVec[14], valueVec[13], valueVec[12], valueVec[11],
				  valueVec[10], valueVec[9], valueVec[8], valueVec[7], valueVec[6],
				  valueVec[5], valueVec[4], valueVec[3], valueVec[2], valueVec[1],
				  valueVec[0]);
	  break;
  }

  eng->push(new JobStackDataString(s));
  return op_Ok;
}
