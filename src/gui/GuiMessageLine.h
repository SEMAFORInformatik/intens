
#if !defined(GUI_MESSAGELINE_INCLUDED_H)
#define GUI_MESSAGELINE_INCLUDED_H

#include <ostream>
#include "jsoncpp/json/value.h"
#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

class GuiElement;

class GuiMessageLine
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiMessageLine(){
  }
  virtual ~GuiMessageLine(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual void setDelay( int delay ) {}
  virtual GuiElement *getElement() = 0;
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
  void serializeXML(std::ostream &os, bool recursive = false);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

};

#endif
