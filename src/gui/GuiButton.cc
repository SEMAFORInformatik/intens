
#include "gui/GuiElement.h"
#include "gui/GuiButton.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiEventData.h"
#include "gui/GuiForm.h"
#include "job/JobAction.h"
#include "utils/gettext.h"

GuiButton::GuiButton( GuiButtonListener *listener, GuiEventData *event )
  : m_listener( listener )
  , m_event( event )
  , m_label_pixmap(false)
{
}

GuiButton::~GuiButton(){
  delete m_event;
}

/* --------------------------------------------------------------------------- */
/* Activate --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiButton::Activate(){
  if( m_listener == 0 )
    return;
  if( m_event != 0  )
    m_listener->ButtonPressed( m_event );
  else
    m_listener->ButtonPressed();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiButton::serializeXML(std::ostream &os, bool recursive){

  std::string disabled = "false";
  os << "<intens:Button ";
  // if (m_buttonwidget) {
  //   if (m_buttonwidget->isEnabled())
  //     disabled = "true";
  // }
//   os << " disabled=\""<<disabled<<"\"";
  std::string pix = m_label_pixmap ? "true" : "false";
  os << " pixmap=\""<<pix<<"\"";

  os << " label=\"" << m_label << "\"";

  os << " action=\"" << "func->Name()" << "\"";

  os << "/>"<< std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiButton::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
///TODO amg2020-10-12  if (onlyUpdated) return false;
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["pixmap"] = m_label_pixmap;
  jsonObj["label"] = m_label;
  jsonObj["editable"] =  m_listener ? m_listener->sensitive() : true;
  jsonObj["type"] = compose("Dialog_%1",getElement()->StringType());
  if (m_listener && m_listener->getAction()) {
    jsonObj["action"] = m_listener->getAction()->Name();
  } else if(m_label != "Close") {
    jsonObj["guielement"] = getElement()->getMyForm() ? getElement()->getMyForm()->getElement()->getName()
      : getElement()->getMainForm()->getElement()->getName();
    jsonObj["action"] = "GUIELEMENT_METHOD";
  }
  return true;
}

#if HAVE_PROTOBUF
bool GuiButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  auto button = eles->add_buttons();
  button->set_allocated_base(getElement()->writeProtobufProperties());
  button->set_pixmap(m_label_pixmap);
  button->set_label(m_label);
  button->set_editable(m_listener ? m_listener->sensitive() : true);
  if (m_listener && m_listener->getAction()) {
    button->set_action(m_listener->getAction()->Name());
  } else if(m_label != "Close") {
    button->set_action("GUIELEMENT_METHOD");
  }
  return true;
}
#endif
