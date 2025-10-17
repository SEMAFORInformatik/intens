
#include <sstream>

#include "utils/HTMLConverter.h"
#include "gui/IconManager.h"
#include "gui/GuiElement.h"
#include "gui/GuiLabel.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* serialize --                                                                */
/* --------------------------------------------------------------------------- */
void GuiLabel::serializeXML( std::ostream &os, bool recursive ){
  std::string s(getSimpleLabel());
  HTMLConverter::convert2HTML(s);
  os << "<intens:Label>" << std::endl;
  os << "<intens:Value>" << s << "</intens:Value>" << std::endl;
  os << "</intens:Label>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiLabel::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  std::string s(getSimpleLabel());
  if (isLabelPixmap()) {
    IconManager::ICON_TYPE icon_type;
    std::string icon, file, content;
    if (!IconManager::Instance().getDataUrlFileContent(icon_type, s, content)) {
      std::cerr << "icon '"<<icon<<"' not found.\n";
      BUG_INFO("GuiLabel icon '"<<icon<<"' not found.");
    } else {
      BUG_DEBUG("GuiLabel icon '"<< s<<"' Found, icon_type: '"<<icon_type<<"'.");
      jsonObj["icon"] = content;
      jsonObj["name"] = s;
    }
  } else {
    jsonObj["label"] = s;
  }
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["type"] = getElement()->StringType();
  jsonObj["colspan"] = getElement()->getColSpan();
  switch (getElement()->getAlignment()) {
    case GuiElement::align_Right:
      jsonObj["alignment"] = "right";
      break;
    case GuiElement::align_Center:
      jsonObj["alignment"] = "center";
      break;
    case GuiElement::align_Stretch:
      jsonObj["alignment"] = "justify";
      break;
    case GuiElement::align_Default:
    case GuiElement::align_Left:
    default:
      jsonObj["alignment"] = "left";
      break;
  }
  return false; // keine Aenderung
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiLabel::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_labels();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  std::string s(getSimpleLabel());
  if (isLabelPixmap()) {
    IconManager::ICON_TYPE icon_type;
    std::string icon, file, content;
    if (!IconManager::Instance().getDataUrlFileContent(icon_type, s, content)) {
      std::cerr << "icon '"<<icon<<"' not found.\n";
      BUG_INFO("GuiLabel icon '"<<icon<<"' not found.");
    } else {
      BUG_DEBUG("GuiLabel icon '"<< s<<"' Found, icon_type: '"<<icon_type<<"'.");
      element->set_icon(content);
      element->mutable_base()->set_name(s);
    }
  } else {
    element->set_label(s);
  }
  element->mutable_base()->set_colspan(getElement()->getColSpan());
  switch (getElement()->getAlignment()) {
    case GuiElement::align_Right:
      element->mutable_base()->set_alignment(in_proto::GuiElement_Alignment::GuiElement_Alignment_Right);
      break;
    case GuiElement::align_Center:
      element->mutable_base()->set_alignment(in_proto::GuiElement_Alignment::GuiElement_Alignment_Center);
      break;
    case GuiElement::align_Stretch:
      element->mutable_base()->set_alignment(in_proto::GuiElement_Alignment::GuiElement_Alignment_Justify);
      break;
    case GuiElement::align_Default:
    case GuiElement::align_Left:
    default:
      element->mutable_base()->set_alignment(in_proto::GuiElement_Alignment::GuiElement_Alignment_Left);
      break;
  }
  return false; // keine Aenderung
}
#endif
