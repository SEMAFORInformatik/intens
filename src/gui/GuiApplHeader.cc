
#include "gui/GuiElement.h"
#include "gui/GuiApplHeader.h"

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiApplHeader::serializeXML( std::ostream &os, bool recursive ){
}

/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiApplHeader::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  std::string s;
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["title"] = getTitle();
  jsonObj["subtitle"] = getSubTitle();
  s = AppData::Instance().LeftTitleIcon();
  jsonObj["left_icon"] = s.size() ? s : "semafor";
  jsonObj["right_icon"] = AppData::Instance().RightTitleIcon();
  return false; // keine Aenderung nach einem GuiUpdate möglich
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiApplHeader::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_headers();
  std::string s;
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_title(getTitle());
  element->set_subtitle(getSubTitle());
  s = AppData::Instance().LeftTitleIcon();
  element->set_left_icon(s.size() ? s : "semafor");
  element->set_right_icon(AppData::Instance().RightTitleIcon());
  return false; // keine Aenderung nach einem GuiUpdate möglich
}
#endif
