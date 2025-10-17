
#include "gui/GuiElement.h"
#include "gui/GuiMenuToggle.h"
#include "gui/GuiForm.h"
#include "gui/GuiToggleListener.h"

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiMenuToggle::serializeXML( std::ostream &os, bool recursive ){
  os << "<intens:MenuToogle label=\"" << getLabel() << "\">" << std::endl;
  os << "</intens:MenuToogle>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiMenuToggle::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  std::string actionName;

  jsonObj["label"] = getLabel();
  jsonObj["checked"] = getToggleStatus();
  jsonObj["disable"] = getToggleListener() ? !getToggleListener()->isAllowed() : false;
  getElement()->writeJsonProperties(jsonObj);
  if (getToggleListener()) {
    actionName="MENU_BUTTON_ACTION";
    jsonObj["action"] = actionName;
  }

  return false; // keine Aenderung nach einem GuiUpdate möglich
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiMenuToggle::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_toggles();

  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_label(getLabel());
  element->set_checked(getToggleStatus());
  if (getToggleListener()) {
    element->set_action("MENU_BUTTON_ACTION");
  }

  return false; // keine Aenderung nach einem GuiUpdate möglich
}
#endif
