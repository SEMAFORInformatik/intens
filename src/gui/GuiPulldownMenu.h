
#if !defined(GUI_PULLDOWNMENU_INCLUDED_H)
#define GUI_PULLDOWNMENU_INCLUDED_H

#include <string>
#include "jsoncpp/json/json.h"
#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

class GuiElement;

class GuiPulldownMenu
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiPulldownMenu(){
  }
  virtual ~GuiPulldownMenu(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setLabel( const std::string &label ) = 0;
  virtual const std::string&getLabel() = 0;
  virtual void setMnemonic( const std::string &mnemonic ) = 0;
  virtual const std::string&getMnemonic() = 0;
  virtual void setAlways() = 0;
  virtual void setTearOff( bool yes ) = 0;
  virtual bool isTearOff() = 0;
  virtual void setRadio( bool yes ) = 0;
  virtual bool isRadio() = 0;
  virtual GuiPulldownMenu *getParentMenu() = 0;

  virtual void attach( GuiElement *e ) = 0;
  virtual void front( GuiElement *e ) = 0;
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  virtual bool serializeContainerElements(in_proto::ElementList* eles, in_proto::PulldownMenu* element, bool onlyUpdated = false) = 0;
#endif
  virtual void serializeContainerElements( std::ostream &os ) = 0;
  virtual bool serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated = false) = 0;
  virtual GuiElement * getElement() = 0;

};

#endif
