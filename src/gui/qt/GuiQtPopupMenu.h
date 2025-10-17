
#if !defined(GUI_QT_POPUPMENU_INCLUDED_H)
#define GUI_QT_POPUPMENU_INCLUDED_H
#if HAVE_PROTOBUF
#include "protobuf/IndexMenu.pb.h"
#endif

#include <vector>

#include <QMenu>
#include "gui/GuiPopupMenu.h"
#include "gui/qt/GuiQtElement.h"

class QMenu;
class GuiQtPulldownMenu;

class GuiQtPopupMenu : public GuiPopupMenu, public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtPopupMenu( GuiElement *parent, std::string title="", QMenu* menu=0 )
    : GuiQtElement( parent )
    , GuiPopupMenu(title)
    , m_menu( menu )
  {
  }
  virtual ~GuiQtPopupMenu(){
  }
protected:
  GuiQtPopupMenu(const GuiQtPopupMenu& menu);

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiPopupMenu *getPopupMenu(){ return this; }
  virtual GuiElement::ElementType Type() { return GuiElement::type_PopupMenu; }
  virtual void create();
  virtual void manage() {}

  virtual void update( UpdateReason r) {}
  virtual QWidget* myWidget();
  virtual GuiElement * getElement() { return this; }
  virtual void setTitle(std::string title) { GuiPopupMenu::setTitle(title); }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, in_proto::PopupMenu* element, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* public member functions (GuiContainer)                                      */
/*=============================================================================*/
public:
  virtual void attach( GuiElement *e ) { GuiPopupMenu::attach(e); }
  virtual void front( GuiElement *e )  {GuiPopupMenu::front(e); }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void popup();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QMenu*              m_menu;
};

#endif
