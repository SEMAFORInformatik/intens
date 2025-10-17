
#include "gui/GuiElement.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiForm.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiCycleDialog.h"
#include "gui/GuiScrolledText.h"
#include "gui/DialogCopyright.h"
#include "app/QuitApplication.h"
#include "app/HelpFile.h"
#include "job/JobEngine.h"
#include "utils/StringUtils.h"

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiMenuButton::serializeXML( std::ostream &os, bool recursive ){
  std::string actionName;
  JobAction *action = getButtonListener() ? getButtonListener()->getAction() : 0;
  if( action )
    actionName=action->Name();
  if (actionName.empty()) {
    GuiForm::GuiFormButtonListener *lsnr = dynamic_cast<GuiForm::GuiFormButtonListener*>(getButtonListener());
    if (lsnr) {
      actionName="@openForm@" + lsnr->getForm()->getElement()->getName();
    }
  }
  os << "<intens:Option label=\"" << getLabel() << "\" action=\"" << actionName << "\">" << std::endl;
  os << "</intens:Option>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiMenuButton::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  std::string actionName;
  JobAction *action = getButtonListener() ? getButtonListener()->getAction() : 0;
  if( action )
    actionName=action->Name();
  if (actionName.empty()) {
    GuiForm::GuiFormButtonListener *lsnr = dynamic_cast<GuiForm::GuiFormButtonListener*>(getButtonListener());
    if (lsnr) {
      actionName="@openForm@" + lsnr->getForm()->getElement()->getName();
      jsonObj["map_action"] = lsnr->getForm()->getElement()->getName();
    }
    else  {
      if (getButtonListener()) {
        if (dynamic_cast<GuiCycleDialog*>(getButtonListener())) {
          actionName="CycleDialog";
        } else if (dynamic_cast<QuitApplication*>(getButtonListener())) {
          actionName="QuitApplication";
        } else if (dynamic_cast<HelpFile*>(getButtonListener())) {
          actionName="MENU_BUTTON_ACTION";
        } else if (dynamic_cast<GuiMenuButtonListener*>(getButtonListener())) {
          actionName="MENU_BUTTON_ACTION";
        } else if (dynamic_cast<DialogCopyright*>(getButtonListener())) {
          actionName="MENU_BUTTON_ACTION";
        } else {
          if (getButtonListener()->getButton() && getButtonListener()->getButton()->getName().size())
            actionName +=", name: " +  getButtonListener()->getButton()->getName();
          if (getButtonListener()->getAction() && getButtonListener()->getAction()->Name().size())
            actionName +=", actionname: " +  getButtonListener()->getAction()->Name();

          if (actionName.empty() &&
              getButtonListener()->getButton() && getButtonListener()->getButton()->getName().empty()) {
            // variant GuiElement Method
            actionName="GUIELEMENT_METHOD";
          } else if (actionName.empty())
            actionName="MenuButton, not implemented yet";
        }
      }
    }
  }
  jsonObj["label"] = getLabel();
  jsonObj["action"] = actionName;
  jsonObj["disable"] = getButtonListener() ? !getButtonListener()->isAllowed() : false;
  getElement()->writeJsonProperties(jsonObj);
  jsonObj.removeMember("name"); // no transfer ro webtens

  return false; // keine Aenderung nach einem GuiUpdate möglich
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiMenuButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_menu_buttons();
  std::string actionName;
  JobAction *action = getButtonListener() ? getButtonListener()->getAction() : 0;
  if( action )
    actionName=action->Name();
  if (actionName.empty()) {
    GuiForm::GuiFormButtonListener *lsnr = dynamic_cast<GuiForm::GuiFormButtonListener*>(getButtonListener());
    if (lsnr) {
      actionName="@openForm@" + lsnr->getForm()->getElement()->getName();
      element->set_map_action(lsnr->getForm()->getElement()->getName());
    }
    else  {
      if (getButtonListener()) {
        if (dynamic_cast<GuiCycleDialog*>(getButtonListener())) {
          actionName="CycleDialog";
        } else if (dynamic_cast<QuitApplication*>(getButtonListener())) {
          actionName="QuitApplication";
        } else if (dynamic_cast<HelpFile*>(getButtonListener())) {
          actionName="MENU_BUTTON_ACTION";
        } else if (dynamic_cast<GuiMenuButtonListener*>(getButtonListener())) {
          actionName="MENU_BUTTON_ACTION";
        } else if (dynamic_cast<DialogCopyright*>(getButtonListener())) {
          actionName="MENU_BUTTON_ACTION";
        } else {
          if (getButtonListener()->getButton() && getButtonListener()->getButton()->getName().size())
            actionName +=", name: " +  getButtonListener()->getButton()->getName();
          if (getButtonListener()->getAction() && getButtonListener()->getAction()->Name().size())
            actionName +=", actionname: " +  getButtonListener()->getAction()->Name();

          if (actionName.empty() &&
              getButtonListener()->getButton() && getButtonListener()->getButton()->getName().empty()) {
            // variant GuiElement Method
            actionName="GUIELEMENT_METHOD";
          } else if (actionName.empty())
            actionName="MenuButton, not implemented yet";
        }
      }
    }
  }
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_label(getLabel());
  element->set_action(actionName);
  element->mutable_base()->set_enabled(getButtonListener() ? getButtonListener()->isAllowed() : true);

  return false; // keine Aenderung nach einem GuiUpdate möglich
}
#endif

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string GuiMenuButton::variantMethod(const std::string& method,
                                     const Json::Value& jsonArgs, JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);

  // GetActiveCycles
  if (lower(method) == "menu_button_action") {
    if (getButtonListener()) {
      getButtonListener()->ButtonPressed();
      if (getButtonListener()->getAction()) {
        MessageQueueReplyThread *mq_reply = eng->getFunction()->getMessageQueueReplyThread();
        getButtonListener()->getAction()->setMessageQueueReplyThread(mq_reply);
      }
      return "";
    }
  }
  return getElement()->defaultVariantMethod(method, jsonArgs, eng);
}
