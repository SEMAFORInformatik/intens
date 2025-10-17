
#if !defined(GUI_CYCLEBUTTON_INCLUDED_H)
#define GUI_CYCLEBUTTON_INCLUDED_H

#include "GuiElement.h"

class UpdateReason;

class GuiCycleButton{

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() = 0;
  virtual void manage() = 0;
  virtual void getSize( int &x, int &y ) = 0;
  virtual void addTabGroup() = 0;
  virtual std::string getLabel( int num ) = 0;
  virtual bool hasHelptext() = 0;
  virtual bool showHelptext() = 0;
  virtual void getHelptext( std::string &text ) = 0;
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual GuiElement * getElement() = 0;

};

#endif
