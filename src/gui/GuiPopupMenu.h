#if !defined(GUI_POPUPMENU_INCLUDED_H)
#define GUI_POPUPMENU_INCLUDED_H

#if HAVE_PROTOBUF
#include "protobuf/IndexMenu.pb.h"
#endif

#include <iostream>
#include "gui/GuiElement.h"

class XferDataItem;
class GuiContainer;

class GuiPopupMenu
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiPopupMenu(std::string title="")
    : m_title(title)
    , m_tear_off( false )
    , m_clear_default_menu( true ) {}
  GuiPopupMenu(const GuiPopupMenu& menu);
  virtual ~GuiPopupMenu(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void setTitle(std::string title) { m_title = title; }
  void setTearOff( bool yes ) { m_tear_off = yes; }
  void clearDefaultMenu( bool yes ) { m_clear_default_menu = yes; }
  virtual void resetMenuPost() {};
  void attach( GuiElement *e );
  void front( GuiElement *e )    ;
  virtual GuiElement * getElement() = 0;
  virtual void popup() = 0;
  void serializeXML(std::ostream &os, bool recursive = false){}
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, in_proto::PopupMenu* element, bool onlyUpdated = false);
#endif
  const std::string& getTitle() { return m_title; }
protected:
  bool isTearOff() { return m_tear_off; }
  bool isClearDefaultMenu() { return m_clear_default_menu; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  GuiElementList m_container;
private:
  std::string m_title;
  bool        m_tear_off;
  bool        m_clear_default_menu;

};

#endif
