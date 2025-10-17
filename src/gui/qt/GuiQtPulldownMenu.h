
#if !defined(GUI_QT_PULLDOWNMENU_INCLUDED_H)
#define GUI_QT_PULLDOWNMENU_INCLUDED_H

#include <vector>

#include "gui/GuiContainer.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/GuiPulldownMenu.h"

class QMenu;

class GuiQtPulldownMenu : public GuiPulldownMenu, public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtPulldownMenu( GuiElement *parent, const std::string &name );
  virtual ~GuiQtPulldownMenu();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_PulldownMenu; }
  virtual void create();
  virtual void manage() {}
  virtual void unmap();
/*   virtual void enable(); */
/*   virtual void disable(); */
  virtual GuiPulldownMenu *getPulldownMenu(){ return this; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setLabel( const std::string &label );
  virtual const std::string& getLabel() { return m_label; }
  void setMnemonic( const std::string &mnemonic ) { m_mnemonic = mnemonic; };
  virtual const std::string& getMnemonic() { return m_mnemonic; }
  void setAlways() { m_show_always = true; }
  void setTearOff( bool yes ) { m_tear_off = yes; }
  virtual bool isTearOff() { return m_tear_off; }
  virtual void setRadio( bool yes ) { m_radio = yes; }
  virtual bool isRadio() { return m_radio; }
  GuiPulldownMenu *getParentMenu();
/*   Widget getCascadeButtonWidget() { return m_buttonwidget; } */

  virtual void update( UpdateReason r);
  virtual QWidget* myWidget();
/*   virtual Widget myParentWidget() { return m_container.myWidget(); } */
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  virtual bool serializeContainerElements(in_proto::ElementList* eles, in_proto::PulldownMenu* element, bool onlyUpdated = false);
#endif
  virtual void serializeContainerElements( std::ostream &os );
  virtual bool serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated = false);
  virtual GuiElement * getElement();

/*=============================================================================*/
/* public member functions (GuiContainer)                                      */
/*=============================================================================*/
public:
  virtual void attach( GuiElement *e ) { m_container.push_back(e); }
  virtual void front( GuiElement *e )  { m_container.insert(m_container.begin(), e); }

/*=============================================================================*/
/* private member                                                                */
/*=============================================================================*/
private:
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string        m_name;
  std::string        m_label;
  std::string        m_mnemonic;
  bool               m_show_always;
  bool               m_tear_off;
  bool               m_radio;
  GuiContainer     m_container;
  QMenu*             m_menu;
};

#endif
