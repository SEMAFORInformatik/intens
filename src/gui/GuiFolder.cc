
#include "gui/GuiContainer.h"
#include "gui/qt/GuiQtOrientationContainer.h"
#include "utils/HTMLConverter.h"

#include "gui/GuiForm.h"
#include "gui/GuiFolder.h"
#include "utils/gettext.h"
#include "utils/JsonUtils.h"

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
GuiFolder::~GuiFolder(){
  GuiNamedElementList::iterator pi;
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi ){
    delete (*pi).second;
  }
}

GuiFolder::GuiFolder( const GuiFolder &fd ) :
   m_button_placement( fd.m_button_placement )
  ,m_label_orientation( fd.m_label_orientation )
  ,m_tab_stretched( fd.m_tab_stretched )
  ,m_tab_expandable( fd.m_tab_expandable )
  ,m_tab_movable( fd.m_tab_movable )
  ,m_hideButtons( fd.m_hideButtons )
{
}

/* --------------------------------------------------------------------------- */
/* setButtonPlacement --                                                       */
/* --------------------------------------------------------------------------- */
void GuiFolder::setButtonPlacement( GuiElement::Alignment align ){
  m_button_placement = align;
}

/* --------------------------------------------------------------------------- */
/* setLabelOrientation --                                                      */
/* --------------------------------------------------------------------------- */
void GuiFolder::setLabelOrientation( GuiElement::Orientation orient ){
  m_label_orientation = orient;
}

/* --------------------------------------------------------------------------- */
/* setTabStretch --                                                            */
/* --------------------------------------------------------------------------- */
void GuiFolder::setTabStretch( bool stretched ){
  m_tab_stretched = stretched;
}
/* --------------------------------------------------------------------------- */
/* tabStretched --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiFolder::tabStretched() {
  return m_tab_stretched;
}

/* --------------------------------------------------------------------------- */
/* setTabExpandable --                                                         */
/* --------------------------------------------------------------------------- */
void GuiFolder::setTabExpandable( bool expand ){
  m_tab_expandable = expand;
}

/* --------------------------------------------------------------------------- */
/* tabExpandable --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiFolder::tabExpandable() {
  return m_tab_expandable;
}

/* --------------------------------------------------------------------------- */
/* setTabMovable --                                                            */
/* --------------------------------------------------------------------------- */
void GuiFolder::setTabMovable( bool move ){
  m_tab_movable = move;
}

/* --------------------------------------------------------------------------- */
/* tabMovable --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiFolder::tabMovable() {
  return m_tab_movable;
}

/* --------------------------------------------------------------------------- */
/* NumberOfPages --                                                            */
/* --------------------------------------------------------------------------- */
int GuiFolder::NumberOfPages(){
  return m_elements.size();
}

/* --------------------------------------------------------------------------- */
/* replace --                                                                  */
/* --------------------------------------------------------------------------- */
bool GuiFolder::replace( GuiElement *old_el, GuiElement *new_el ){
  assert( old_el != 0 );
  assert( new_el != 0 );

  GuiNamedElementList::iterator pi;
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi ){
    if( (*pi).second->replace( old_el, new_el ) ){
      draw();
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiFolder::update( GuiElement::UpdateReason reason ){
  GuiNamedElementList::iterator pi;
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi ){
    (*pi).second->update( reason );
  }
}

/* --------------------------------------------------------------------------- */
/* setTabOrder --                                                              */
/* --------------------------------------------------------------------------- */
void GuiFolder::setTabOrder(){
  GuiNamedElementList::iterator pi;
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi ){
    (*pi).second->addTabGroup();
  }
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi ){
    (*pi).second->setTabOrder();
  }
}

/* --------------------------------------------------------------------------- */
/* unsetTabOrder --                                                            */
/* --------------------------------------------------------------------------- */
void GuiFolder::unsetTabOrder(){
  GuiNamedElementList::iterator pi;
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi ){
    (*pi).second->removeTabGroup();
  }
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi ){
    (*pi).second->unsetTabOrder();
  }
}

#define printKeyJson(os, label, value) \
  os << "\""<<label<<"\":" << value << std::endl;

/* --------------------------------------------------------------------------- */
/* getPage --                                                                  */
/* --------------------------------------------------------------------------- */

GuiElement* GuiFolder::getPage(int page) {
  if (page < m_elements.size()) {
    GuiNamedElementList::iterator iter = m_elements.begin();
    while (--page>=0) ++iter;
    return iter->second;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiFolder::serializeXML(std::ostream &os, bool recursive){
 // if (type == AppData::serialize_JSON) {
 //   Json::Value jsonObj = Json::Value(Json::objectValue);
 //   serializeJson(jsonObj, recursive);
 //   os << ch_semafor_intens::JsonUtils::value2string(jsonObj);
 // }
  os << "<intens:Folder";
  std::string name = getElement()->getName();
  os << " name=\"" << name;
  os << "\" buttons=\"";
  if( m_hideButtons )
    os << "false";
  else
    os << "true";
  os << "\">" << std::endl;
  GuiNamedElementList::iterator pi;
  int i = 0;
  bool active=false;
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi, ++i ){
    os << "<intens:FolderPage";
    os << " pageid=\"" << i << "\"";
    std::string s((*pi).first);
    HTMLConverter::convert2HTML( s );
    os << " label=\"" << s << "\"";
    if ( PageIsActive(i) ) {
      active=true;
      os << " active=\"true\"";
    }
    os << ">"<<std::endl;
    (*pi).second->serializeXML(os, recursive);
    os << "</intens:FolderPage>"<<std::endl;
  }
  //amg  assert( active );
  os<<"</intens:Folder>"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiFolder::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
   jsonObj["buttons"] = !m_hideButtons;

   bool ret(false);
   Json::Value jsonPages = Json::Value(Json::arrayValue);
   GuiNamedElementList::iterator pi;
   int i = 0, active = 0;
   for( pi = m_elements.begin(); pi != m_elements.end(); ++pi, ++i ){
     Json::Value jsonPageObj = Json::Value(Json::objectValue);
     jsonPageObj["pageid"] = i;
     std::string s((*pi).first);
     jsonPageObj["label"] = s;
     jsonPageObj["active"] = PageIsActive(i);
     if (jsonPageObj["active"].asBool()) active = i;
     jsonPageObj["type"] = "foldertab";

     if ((*pi).second->serializeJson(jsonPageObj, onlyUpdated))
       ret = true;
     jsonPages.append(jsonPageObj);
   }
   getElement()->writeJsonProperties(jsonObj);
   jsonObj["pages"] = jsonPages;
   jsonObj["active"] = active;

   std::string buttonAlignment = "top";
   switch(m_button_placement) {
     case GuiElement::align_Left:
      buttonAlignment = "left";
      break;
     case GuiElement::align_Right:
      buttonAlignment = "right";
      break;
     case GuiElement::align_Bottom:
      buttonAlignment = "bottom";
      break;
   default:
     // ignore 'align_Default', 'align_Center', 'align_Top'
     ;
   }
   jsonObj["buttonAlignment"] = buttonAlignment;
   jsonObj["width"] = 0.7 * jsonObj["width"].asInt();
   return ret;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiFolder::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  auto element = eles->add_folders();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_buttons(!m_hideButtons);

  bool ret(false);
  Json::Value jsonPages = Json::Value(Json::arrayValue);
  GuiNamedElementList::iterator pi;
  int i = 0, active = 0;
  for( pi = m_elements.begin(); pi != m_elements.end(); ++pi, ++i ){
    Json::Value jsonPageObj = Json::Value(Json::objectValue);
    auto page = element->add_pages();
    page->set_page_id(i);
    std::string s((*pi).first);
    page->set_label(s);
    auto is_active = PageIsActive(i);
    page->set_active(is_active);
    if (is_active) active = i;
    auto container = static_cast<GuiQtOrientationContainer*>((*pi).second);
    if (container->serializeProtobuf(eles, page, onlyUpdated))
      ret = true;
  }
  element->set_active(active);
  in_proto::GuiElement::Alignment buttonAlignment;
  switch(m_button_placement) {
    case GuiElement::align_Left:
     buttonAlignment = in_proto::GuiElement::Alignment::GuiElement_Alignment_Left;
     break;
    case GuiElement::align_Right:
     buttonAlignment = in_proto::GuiElement::Alignment::GuiElement_Alignment_Right;
     break;
    case GuiElement::align_Bottom:
     buttonAlignment = in_proto::GuiElement::Alignment::GuiElement_Alignment_Bottom;
     break;
  default:
    // ignore 'align_Default', 'align_Center', 'align_Top'
    ;
  }
  element->set_button_alignment(buttonAlignment);
  element->set_width(0.7 * element->base().width());
  return ret;
}
#endif

/* --------------------------------------------------------------------------- */
/* serializePage --                                                            */
/* --------------------------------------------------------------------------- */
void GuiFolder::serializePage( std::ostream &os, const std::string &pageId  ){
  GuiNamedElementList::iterator pi = m_elements.begin();
  while( (*pi).first != pageId  && pi != m_elements.end() ){
    ++pi;
  }
  if( pi != m_elements.end() ){
    os << "<intens:FolderPage";
    std::string s((*pi).first);
    HTMLConverter::convert2HTML( s );
    os << " label=\"" << s << "\">"<<std::endl;
    (*pi).second->serializeXML(os);
    os << "</FolderPage>"<<std::endl;
  }
}
