
#include <string>

#include "gui/GuiElement.h"
#include "gui/GuiMenubar.h"

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiMenubar::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:Menubar>" << std::endl;
  serializeContainerElements(os);
  os << "</intens:Menubar>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiMenubar::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  Json::Value jsonElem = Json::Value(Json::arrayValue);
  bool ret = serializeContainerElements(jsonElem);
  jsonObj["element"] = jsonElem;
  getElement()->writeJsonProperties(jsonObj);
  return ret;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiMenubar::serializeProtobuf(in_proto::ElementList *eles, bool onlyUpdated){
  auto element = eles->add_menubars();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  return serializeContainerElements(eles, element, onlyUpdated);
}
#endif
