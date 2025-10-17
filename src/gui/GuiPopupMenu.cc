
#include "gui/GuiPopupMenu.h"

/* --------------------------------------------------------------------------- */
/* Constuctor                                                                  */
/* --------------------------------------------------------------------------- */
GuiPopupMenu::GuiPopupMenu(const GuiPopupMenu& menu)
  : m_title(menu.m_title)
  , m_tear_off(menu.m_tear_off)
  , m_clear_default_menu( menu.m_clear_default_menu ) {
  GuiElementList::const_iterator it = menu.m_container.begin();
  for(; it != menu.m_container.end(); ++it) {
    GuiElement* elem=(*it);
    if (elem->cloneable()) {
      elem = elem->clone();
      elem->setParent((GuiElement*)this);
      attach(elem);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* attach                                                                      */
/* --------------------------------------------------------------------------- */
void GuiPopupMenu::attach( GuiElement *e ) {
  m_container.push_back(e);
}

/* --------------------------------------------------------------------------- */
/* front                                                                       */
/* --------------------------------------------------------------------------- */
void GuiPopupMenu::front( GuiElement *e )  {
  m_container.insert(m_container.begin(), e);
}

/* --------------------------------------------------------------------------- */
/* serializeJson                                                                */
/* --------------------------------------------------------------------------- */
bool GuiPopupMenu::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  jsonObj["title"] = m_title;
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  GuiElementList::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it) {
    Json::Value jsonSubElem = Json::Value(Json::objectValue);
    (*it)->serializeJson(jsonSubElem, onlyUpdated);
    jsonAry.append(jsonSubElem);
  }
  jsonObj["element"] = jsonAry;
  jsonObj["tearOff"] = m_tear_off;
  jsonObj["clearDefaultMenu"] = m_clear_default_menu;
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf                                                           */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiPopupMenu::serializeProtobuf(in_proto::ElementList* eles, in_proto::PopupMenu* element, bool onlyUpdated){
  element->set_title(m_title);
  GuiElementList::iterator it = m_container.begin();
  for(; it != m_container.end(); ++it) {
    (*it)->serializeProtobuf(eles, onlyUpdated);
    auto ref = element->add_elements();
    ref->set_id((*it)->getElementIntId());
    ref->set_type((*it)->ProtobufType());
  }
  element->set_tear_off(m_tear_off);
  element->set_clear_default_menu(m_clear_default_menu);
  return false;
}
#endif
