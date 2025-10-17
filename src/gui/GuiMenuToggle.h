
#if !defined(GUI_MENUTOGGLE_INCLUDED_H)
#define GUI_MENUTOGGLE_INCLUDED_H
#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

#include "jsoncpp/json/json.h"

class GuiElement;
class GuiToggleListener;
class GuiEventData;

/** Der GuiMenuToggle ist ein Toggle, welcher in einem PulldownMenu installiert
    werden kann. Er gibt den Input über einen GuiToggleListener weiter. Er kann von
    einem GuiListenerController kontrolliert werden.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiMenuToggle.h,v 1.9 2003/10/21 13:54:12 hob Exp $
 */
class GuiMenuToggle
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiMenuToggle(){
  }
  virtual ~GuiMenuToggle(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setLabel( const std::string &text ) = 0;
  /** Mit der Funktion getLabel() wird der Text des Labels des Buttons zurückgegeben.
   */
  virtual const std::string getLabel() = 0;
  virtual void setDialogLabel( const std::string &text ) = 0;
  virtual void setToggleStatus( bool state ) = 0;
  virtual bool getToggleStatus() = 0;
  virtual void setAccelerator( const std::string &acc_key, const std::string &acc_text ) = 0;
  virtual GuiElement * getElement() = 0;

  virtual GuiToggleListener* getToggleListener() = 0;

protected:
  virtual void serializeXML( std::ostream &os, bool recursive = false );
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
};

#endif
