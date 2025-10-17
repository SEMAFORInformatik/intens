#if !defined(GUI_SEPARATOR_INCLUDED_H)
#define GUI_SEPARATOR_INCLUDED_H

#if HAVE_PROTOBUF
#include "protobuf/GuiElement.pb.h"
#endif

#include "gui/GuiElement.h"

class GuiSeparator
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiSeparator(){
  }
  virtual ~GuiSeparator(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void setOrientation( GuiElement::Orientation ) = 0;
  virtual GuiElement::Orientation getOrientation() = 0;
  virtual GuiElement * getElement() = 0;
  virtual void serializeXML(std::ostream &os, bool recursive = false) {}
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false) {
    jsonObj["type"] = getElement()->StringType();
    jsonObj["orientation"] = getOrientation() == GuiElement::orient_Vertical ? "vertical" : "horizontal";
    return true;
  }
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) {
    auto element = eles->add_separators();
    element->set_id(getElement()->getElementIntId());
    element->set_orientation(getOrientation() == GuiElement::orient_Vertical ?
                              in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical :
                              in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal);
    return true;
  }
#endif
};

#endif
