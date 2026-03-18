#include "gui/GuiVoid.h"
#include "gui/GuiElement.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiVoid::serializeXML(std::ostream &os, bool recursive) {
  os << "<intens:Void/>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiVoid::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  if(onlyUpdated){
    return false;
  }
  jsonObj["type"] = getElement()->StringType();
  jsonObj["width"] = m_width;
  jsonObj["height"] = m_height;
  jsonObj["colspan"] = getElement()->getColSpan();
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiVoid::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  if(onlyUpdated){
    return false;
  }
  auto element = eles->add_voids();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_width(m_width);
  element->set_height(m_height);
  element->set_colspan(getElement()->getColSpan());
  return false;
}
#endif

/* --------------------------------------------------------------------------- */
/* setDisplayPercentWidth --                                                   */
/* --------------------------------------------------------------------------- */
void GuiVoid::setDisplayPercentWidth( int w ) {
  m_displayPercentWidth = w;
}

/* --------------------------------------------------------------------------- */
/* setDisplayPercentHeight --                                                  */
/* --------------------------------------------------------------------------- */
void GuiVoid::setDisplayPercentHeight( int h ) {
  m_displayPercentHeight = h;
}
