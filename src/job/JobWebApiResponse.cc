
#include "job/JobWebApiResponse.h"
#include "utils/Debugger.h"
#include "gui/GuiForm.h"
#include "gui/GuiFolder.h"
#include "gui/GuiManager.h"
#include "app/DataPoolIntens.h"
#include "app/QuitApplication.h"
#include "app/AppData.h"

#ifdef HAVE_QT
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#endif

INIT_LOGGER();

std::string JobWebApiResponse::s_openURL;
std::map<std::string, JobWebApiResponse::ReplaceData *> JobWebApiResponse::s_guiElementReplaceData;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
bool formOrderSort(GuiElement* a, GuiElement* b) {
  return (a->getFormOrder() < b->getFormOrder());
}
JobWebApiResponse::JobWebApiResponse()
{
  BUG_DEBUG("Constructor JobWebApiResponse" );
  GuiElementList mlist;
  GuiElement::findElementType(m_formList, GuiElement::type_Main);
  m_mainForm = m_formList[0];
  GuiElement::findElementType(mlist, GuiElement::type_Form);
  m_formList.insert(m_formList.end(), mlist.begin(), mlist.end());
  std::sort(m_formList.begin(), m_formList.end(), formOrderSort);
  BUG_DEBUG("LastWebUpdate["<<GuiManager::Instance().LastWebUpdate()
            <<"] CurrentTransaction["<<DataPoolIntens::Instance().CurrentTransaction()
            <<"] DataPoolCurrentTransaction["<<DataPool::getDatapool().GetCurrentTransaction()<<"]");
}

JobWebApiResponse::~JobWebApiResponse() {
  BUG_DEBUG("Destructor JobWebApiResponse LastUpdate["<<GuiManager::Instance().LastWebUpdate()
            <<"] CurrentTransaction["<<DataPoolIntens::Instance().CurrentTransaction()
            <<"] DataPoolCurrentTransaction["<<DataPool::getDatapool().GetCurrentTransaction()<<"]");
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* serializeForms --                                                           */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::serializeForms(Json::Value& jsonElem) {
  GuiElementList::iterator it;
  // replace data
  Json::Value jsonReplaceDataAry = Json::Value(Json::objectValue);

  // only names
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  for( it = m_formList.begin(); it != m_formList.end(); ++it ) {
    if ((*it)->getForm() && (*it)->getForm()->isShown()) {
      jsonAry.append((*it)->getName());

      // gui element replace data
      auto search = s_guiElementReplaceData.find((*it)->getName());
      if ( search != s_guiElementReplaceData.end()){
        std::cout <<  "found: " << (*it)->getName() << std::endl;
        jsonReplaceDataAry[(*it)->getName()] = Json::Value( Json::objectValue);
        Json::Value& d = jsonReplaceDataAry[(*it)->getName()];
        d["old"] = Json::Value(Json::arrayValue);
        d["new"] = Json::Value(Json::arrayValue);
        for (auto itO = search->second->old_element.begin(),
               itN = search->second->new_element.begin();
             itO != search->second->old_element.end() &&
               itN != search->second->new_element.end();
             itO++, itN++) {
          Json::Value jsonRepObjOld = Json::Value( Json::objectValue);
          Json::Value jsonRepObjNew = Json::Value( Json::objectValue);
          (*itO)->writeJsonProperties(jsonRepObjOld);
          (*itN)->writeJsonProperties(jsonRepObjNew);
          d["old"].append(jsonRepObjOld);
          d["new"].append(jsonRepObjNew);
        }
        // remove data
        delete search->second;
        s_guiElementReplaceData.erase(search);
      }
    }
  }
  if (!jsonReplaceDataAry.empty()){
    jsonElem["form_replace"] = jsonReplaceDataAry;
    BUG_DEBUG("form_replace: " << ch_semafor_intens::JsonUtils::value2string(jsonElem, true));
  }
  jsonElem["form"] = jsonAry;
  jsonElem["form_top_name"] = GuiElement::getTopFormName();
  BUG_INFO("TopForm: " << GuiElement::getTopFormName());
}

#if HAVE_PROTOBUF
void JobWebApiResponse::serializeForms(in_proto::WebAPIResponse* reply) {
  GuiElementList::iterator it;
  // only names
  for( it = m_formList.begin(); it != m_formList.end(); ++it ) {
    if ((*it)->getForm() && (*it)->getForm()->isShown()) {
      reply->add_forms((*it)->getName());
      auto search = s_guiElementReplaceData.find((*it)->getName());
      if ( search != s_guiElementReplaceData.end()){
        reply->add_forms_replaced((*it)->getName());
        // remove data
        delete search->second;
        s_guiElementReplaceData.erase(search);
      }
    }
  }
  reply->set_top_form(GuiElement::getTopFormName());
  BUG_INFO("TopForm: " << GuiElement::getTopFormName());
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeCycles --                                                          */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::serializeCycles(Json::Value& jsonElem) {
  DataPoolIntens &dp = DataPoolIntens::Instance();
  std::string name;
  int numcyc = dp.getDataPool().NumCycles();
  Json::Value jsonAry = Json::Value(Json::arrayValue);

  for( int cyc=0; cyc < numcyc; cyc++ ) {
    if( dp.getCycleName( cyc, name ) ){
      if( !name.empty() ){
        jsonAry.append(name);
      }
    }
  }
  jsonElem["cycle"] = jsonAry;
}

#if HAVE_PROTOBUF
void JobWebApiResponse::serializeCycles(in_proto::WebAPIResponse* reply) {
  DataPoolIntens &dp = DataPoolIntens::Instance();
  std::string name;
  int numcyc = dp.getDataPool().NumCycles();

  for( int cyc=0; cyc < numcyc; cyc++ ) {
    if( dp.getCycleName( cyc, name ) ){
      if( !name.empty() ){
        reply->add_cycles(name);
      }
    }
  }
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeElements --                                                        */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::serializeElements( Json::Value& jsonElem,
                                           bool updateAlways ){
  BUG_DEBUG( "serializeElements(): updateAlways = " << updateAlways );
  int cnt=0;
  GuiElementList::iterator it, eit;
  // Form loop
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  for( it = m_formList.begin(); it != m_formList.end(); ++it ) {
    if (!(*it)->isShown()) continue;

    cnt += (*it)->serializeVisibleElements(jsonAry, updateAlways);
  }
  jsonElem["element"] = jsonAry;
  BUG_DEBUG("serializeElements Count: " << cnt << "]");
}

#if HAVE_PROTOBUF
void JobWebApiResponse::serializeElements( in_proto::WebAPIResponse* reply,
                                           bool updateAlways ){
  BUG_DEBUG( "serializeElements(): updateAlways = " << updateAlways );
  int cnt=0;
  GuiElementList::iterator it, eit;
  // Form loop
  for( it = m_formList.begin(); it != m_formList.end(); ++it ) {
    if (!(*it)->isShown()) continue;

    cnt += (*it)->serializeVisibleElements(reply->mutable_elements(), updateAlways);
  }
  BUG_DEBUG("serializeElements Count: " << cnt << "]");
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeFolderTab --                                                       */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::serializeFolderTab(Json::Value& jsonElem) {
  GuiElementList result;
  GuiElement::findElementType( result, GuiElement::type_Folder );

  Json::Value jsonObj = Json::Value(Json::objectValue);
  for( GuiElementList::iterator it = result.begin(); it != result.end(); ++it ) {
    // Es werden alle Folders geliefert, unabhängig davon, ob sie sichtbar sind
    // oder nicht. (hob/23.10.2020)
    // (amg/23.11.2021 wieder drin)
    ///    if (AppData::Instance().HeadlessWebMode() && !(*it)->isShown()) continue;

    GuiFolder *fld = (*it)->getFolder();
    Json::Value jsonVisAry = Json::Value(Json::arrayValue);
    int act = -1;
    int actFirst = -1;
    for (int x=0; x < fld->NumberOfPages(); ++x) {
      bool isHidden = fld->isHiddenPage(x);
      if (fld->PageIsActive(x) && (!isHidden || fld->IsHideButton()))
        act = x;
      if (!fld->IsHideButton()) {
        jsonVisAry.append(!isHidden);
        if (actFirst < 0 && !isHidden)
          actFirst = x;
      }
    }
    Json::Value jsonElem = Json::Value(Json::objectValue);
    jsonElem["active"] = act >= 0 ? act : actFirst;
    jsonElem["visible"] = jsonVisAry;

    // id list
    std::vector<GuiElement*> cList;
    Json::Value jsonAry = Json::Value(Json::arrayValue);
    jsonAry.append((*it)->getElementId());
    (*it)->getCloneList(cList);
    for( GuiElementList::iterator fit = cList.begin(); fit != cList.end(); ++fit ) {
      jsonAry.append((*fit)->getElementId());
    }
    jsonElem["id"] = jsonAry;

    jsonObj[(*it)->getName()] = jsonElem;
    if ((*it)->getName() == "main_window_apc_folder") {
      BUG_INFO("serializeFolderTab main_window_apc_folder: " << ch_semafor_intens::JsonUtils::value2string(jsonElem))
    }
  }

  jsonElem["folder"] = jsonObj;
  BUG_DEBUG("serializeFolderTab Count: " << result.size() << "]");
}

/* --------------------------------------------------------------------------- */
/* serializeFolderTab --                                                       */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
void JobWebApiResponse::serializeFolderTab(in_proto::WebAPIResponse* reply) {
  GuiElementList result;
  GuiElement::findElementType( result, GuiElement::type_Folder );

  for( GuiElementList::iterator it = result.begin(); it != result.end(); ++it ) {
    // Es werden alle Folders geliefert, unabhängig davon, ob sie sichtbar sind
    // oder nicht. (hob/23.10.2020)
    // (amg/23.11.2021 wieder drin)
    ///    if (AppData::Instance().HeadlessWebMode() && !(*it)->isShown()) continue;

    GuiFolder *fld = (*it)->getFolder();
    int act = -1;
    int actFirst = -1;
    auto folder = reply->add_folders();
    for (int x=0; x < fld->NumberOfPages(); ++x) {
      bool isHidden = fld->isHiddenPage(x);
      if (fld->PageIsActive(x) && (!isHidden || fld->IsHideButton()))
        act = x;
      if (!fld->IsHideButton()) {
        folder->add_visible_pages(!isHidden);
        if (actFirst < 0 && !isHidden)
          actFirst = x;
      }
    }
    folder->set_active(act >= 0 ? act : actFirst);

    // id list
    folder->add_ids((*it)->getElementIntId());
    std::vector<GuiElement*> cList;
    (*it)->getCloneList(cList);
    for( GuiElementList::iterator fit = cList.begin(); fit != cList.end(); ++fit ) {
      folder->add_ids((*fit)->getElementIntId());
    }
    folder->set_name((*it)->getName());

  }

  BUG_DEBUG("serializeFolderTab Count: " << result.size() << "]");
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeFunctionStatus --                                                  */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::serializeFunctionStatus(Json::Value& jsonElem, JobAction::JobResult& rslt,
						JobAction& func) {
  std::string status, message;
  Json::Value jsonObj = Json::Value(Json::objectValue);
  switch( rslt ){
  case JobAction::job_Ok:
  case JobAction::job_Ok_ignore:
    status = "OK";
    break;

  case JobAction::job_Canceled:
    message = "Job canceled";
    status = "NOK";
    break;

  case JobAction::job_Aborted:
    message = "Job aborted";
    status = "ABORTED";
    break;

  case JobAction::job_FatalError:
    message = "Fatal error";
    status = "ERROR";
    status = "ABORTED";  // no "Fatal error" in webtens
    break;

  case JobAction::job_Wait:
  case JobAction::job_Nok:
    message = "Nok/Wait";
    status = "NOK";
    break;

  default:
    break;
  }
  jsonObj["exit"] = QuitApplication::Instance()->ExitFlag();
  if (s_openURL.size()) {
    jsonObj["open_url"] =  s_openURL;
    s_openURL.clear();
  }
  jsonObj["status"] = status;
  jsonObj["message"] = message;
  jsonObj["name"] = func.Name();
  jsonElem["function"] = jsonObj;
  BUG_INFO("Function StatusResponse: "<< ch_semafor_intens::JsonUtils::value2string(jsonObj));
  if (QuitApplication::Instance()->ExitFlag()) {
    BUG_INFO("Reset Exit Flag");
    QuitApplication::Instance()->setExitFlag(false);
  }
}

#if HAVE_PROTOBUF
void JobWebApiResponse::serializeFunctionStatus(in_proto::WebAPIResponse* reply, JobAction::JobResult& rslt,
						JobAction& func) {
  std::string status, message;
  switch( rslt ){
  case JobAction::job_Ok:
  case JobAction::job_Ok_ignore:
    status = "OK";
    break;

  case JobAction::job_Canceled:
    message = "Job canceled";
    status = "NOK";
    break;

  case JobAction::job_Aborted:
    message = "Job aborted";
    status = "ABORTED";
    break;

  case JobAction::job_FatalError:
    message = "Fatal error";
    status = "ERROR";
    status = "ABORTED";  // no "Fatal error" in webtens
    break;

  case JobAction::job_Wait:
  case JobAction::job_Nok:
    message = "Nok/Wait";
    status = "NOK";
    break;

  default:
    break;
  }
  auto function = reply->mutable_function();
  function->set_exit(QuitApplication::Instance()->ExitFlag());
  if (s_openURL.size()) {
    function->set_open_url(s_openURL);
    s_openURL.clear();
  }
  function->set_status(status);
  function->set_message(message);
  function->set_name(func.Name());
  if (QuitApplication::Instance()->ExitFlag()) {
    BUG_INFO("Reset Exit Flag");
    QuitApplication::Instance()->setExitFlag(false);
  }
}
#endif

/* --------------------------------------------------------------------------- */
/* serializeForm --                                                            */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::serializeForm( std::ostream &os,
                                       GuiElement* guiForm,
                                       bool& bComma, bool proto ) {
  if (!guiForm) {
    return;
  }

  if (guiForm->getForm()->isShown()) {
    if (bComma)
      os << ",";
    if (proto) {
#if HAVE_PROTOBUF
      auto eles = in_proto::ElementList();
      guiForm->serializeProtobuf(&eles);
      eles.SerializeToOstream(&os);
#endif
    } else {
      Json::Value jsonObj = Json::Value(Json::objectValue);
      guiForm->serializeJson(jsonObj, false);
      os << ch_semafor_intens::JsonUtils::value2string(jsonObj);
      bComma = true;
    }
    BUG_DEBUG("serializeForm Name: " << guiForm->getName() << "]");
  }
}

/* --------------------------------------------------------------------------- */
/* serializeClipboard --                                                       */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::serializeClipboard(Json::Value& jsonElem) {
#ifdef HAVE_QT
  QClipboard *clipboard = QApplication::clipboard();
  Json::Value jsonObj = Json::Value(Json::objectValue);
  if (clipboard->text().size()) {
    if (clipboard->mimeData()->hasText()) {
      jsonObj["text"] = clipboard->text().toStdString();
    }
    clipboard->clear();
    jsonElem["clipboard"] = jsonObj;
  }
  BUG_DEBUG("serializeClipboard");
#endif
}

/* --------------------------------------------------------------------------- */
/* serializeClipboard --                                                       */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
void JobWebApiResponse::serializeClipboard(in_proto::WebAPIResponse* reply) {
#ifdef HAVE_QT
  QClipboard *clipboard = QApplication::clipboard();
  Json::Value jsonObj = Json::Value(Json::objectValue);
  if (clipboard->text().size()) {
    if (clipboard->mimeData()->hasText()) {
      reply->set_clipboard(clipboard->text().toStdString());
    }
    clipboard->clear();
  }
  BUG_DEBUG("serializeClipboard");
#endif
}
#endif

/* --------------------------------------------------------------------------- */
/* add --                                                                      */
/* --------------------------------------------------------------------------- */

void JobWebApiResponse::addGuiElementReplaceData(GuiForm* form, GuiElement *oldElement, GuiElement *newElement){
  std::string formName = form->getElement()->getName();
  auto search = s_guiElementReplaceData.find(formName);
  if ( search == s_guiElementReplaceData.end()){
    s_guiElementReplaceData[formName] = new ReplaceData(oldElement, newElement);
  }else{
    s_guiElementReplaceData[formName]->add(oldElement, newElement);
  }
  std::cout << "GuiElementReplaceData REPLACE form: " << formName << " :: " <<oldElement->getName()<<" => "<<newElement->getName()<<"\n";
}
