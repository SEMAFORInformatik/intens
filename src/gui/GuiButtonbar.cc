
#include "gui/GuiButtonbar.h"

GuiButtonbar::GuiButtonbar(){
}

GuiButtonbar::~GuiButtonbar(){
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiButtonbar::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:Buttonbar>" << std::endl;
  GuiElementList::iterator btn;
  for( btn = m_elements.begin(); btn != m_elements.end(); ++btn ){
    (*btn)->serializeXML(os, recursive);
  }
  os << "</intens:Buttonbar>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiButtonbar::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  getElement()->writeJsonProperties(jsonObj);
  GuiElementList::iterator btn;
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  for( btn = m_elements.begin(); btn != m_elements.end(); ++btn ){
    Json::Value jsonElem = Json::Value(Json::objectValue);
    (*btn)->serializeJson(jsonElem, onlyUpdated);
    jsonAry.append(jsonElem);
  }
  jsonObj["element"] = jsonAry;

  return onlyUpdated ? false : true;
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiButtonbar::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto element = eles->add_buttonbars();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  GuiElementList::iterator btn;
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  for( btn = m_elements.begin(); btn != m_elements.end(); ++btn ){
    Json::Value jsonElem = Json::Value(Json::objectValue);
    (*btn)->serializeProtobuf(eles, onlyUpdated);
    auto ref = element->add_elements();
    ref->set_id((*btn)->getElementIntId());
    ref->set_type((*btn)->ProtobufType());
  }

  return onlyUpdated ? false : true;
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleElement --                                                        */
/* --------------------------------------------------------------------------- */
void GuiButtonbar::getVisibleElement(GuiElementList& res) {
  for( GuiElementList::iterator iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if (GuiElement::isVisibleElementType( (*iter)->Type() )) {
      res.push_back(*iter);
    }
  }
}
