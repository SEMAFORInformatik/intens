
#include "gui/GuiElement.h"
#include "gui/GuiMessageLine.h"

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiMessageLine::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:MessageLine>" << std::endl;
  os << "</intens:MessageLine>"<< std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJSON --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiMessageLine::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  if (onlyUpdated){
    return false;
  }
  getElement()->writeJsonProperties(jsonObj);
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiMessageLine::serializeProtobuf(in_proto::ElementList *eles, bool onlyUpdated){
  if (onlyUpdated){
    return false;
  }
  auto element = eles->add_messagebars();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  return true;
}
#endif
