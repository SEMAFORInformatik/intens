
#if !defined(GUI_MENUBAR_INCLUDED_H)
#define GUI_MENUBAR_INCLUDED_H


class GuiElement;
class GuiPulldownMenu;

#include "jsoncpp/json/json.h"

class GuiMenubar
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiMenubar(){
  }
  virtual ~GuiMenubar(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void attach( GuiElement *e ) = 0;
  virtual void front( GuiElement *e ) = 0;
  virtual void attachHelp( GuiPulldownMenu *menu ) = 0;
  virtual GuiElement * getElement() = 0;
  virtual void serializeXML(std::ostream &os, bool recursive);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  virtual bool serializeContainerElements(in_proto::ElementList* eles, in_proto::Menubar* element, bool onlyUpdated = false)=0;
#endif
  virtual void serializeContainerElements(std::ostream &os)=0;
  virtual bool serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated = false)=0;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};

#endif
