
#include <string>

#include "gui/GuiElement.h"
#include "gui/GuiPulldownMenu.h"

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiPulldownMenu::serializeXML(std::ostream &os, bool recursive){
#if 0
  if( getParentMenu() == 0 ){
    os << "<intens:PulldownMenu label=\"" << m_label << "\">" << std::endl;
    m_container.serializeXML(os, recursive);
    os << "</intens:PulldownMenu>" << std::endl;
  }
  else{
    os << "<intens:Optgroup label=\"" << m_label << "\">" << std::endl;
    m_container.serializeXML(os, recursive);
    os << "</intens:Optgroup>" << std::endl;
  }
#endif
  os << "<intens:Menubar>" << std::endl;
  serializeContainerElements(os);
  os << "</intens:Menubar>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiPulldownMenu::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  Json::Value jsonElem = Json::Value(Json::arrayValue);
  if (!serializeContainerElements(jsonElem))
    return false;
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["element"] = jsonElem;
  jsonObj["label"] = getLabel();
  jsonObj["mnemonic"] = getMnemonic();
  jsonObj["radio"] = isRadio();
  jsonObj["tearOff"] = isTearOff();
  return jsonElem.size() > 0;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiPulldownMenu::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_pulldown_menus();
  if (!serializeContainerElements(eles, element, onlyUpdated))
    return false;
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_label(getLabel());
  element->set_mnemonic(getMnemonic());
  element->set_radio(isRadio());
  element->set_tearoff(isTearOff());
  return element->elements_size() > 0;
}
#endif
