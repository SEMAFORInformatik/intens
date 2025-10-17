
#if !defined(GUI_QT_INDEXMENU_INCLUDED_H)
#define GUI_QT_INDEXMENU_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtDataField.h"
#include "gui/GuiTableActionFunctor.h"
#include "gui/GuiMenuButtonListener.h"
#include "gui/GuiIndexMenu.h"

class GuiQtPopupMenu;
class GuiQtLabel;
class GuiQtIndexLabel;

class GuiQtIndexMenu : public GuiIndexMenu, public GuiQtElement
{
  friend class GuiQtIndexLabel;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtIndexMenu( GuiElement *parent, HideFlag hideFlag )
    : GuiIndexMenu(hideFlag)
    , GuiQtElement( parent )
    , m_labelwidget( 0 )
    ///    , m_menu( 0 )
    ///    , m_menu_title( 0 )
  {}
  virtual ~GuiQtIndexMenu(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_IndexMenu; }
  virtual void create();
  virtual void manage() {}
  virtual bool destroy();
  virtual void update( GuiElement::UpdateReason );
  virtual QWidget*  myWidget();
  virtual void setIndex( const std::string &, int );
  virtual GuiElement * getElement() { return this; }


  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setOrientation( GuiElement::Orientation orient ) {
    GuiIndexMenu::setOrientation(orient);
  }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
 private:
  void createPopupMenu();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiQtIndexLabel    *m_labelwidget;
  //  GuiQtPopupMenu     *m_menu;
  //  GuiQtLabel         *m_menu_title;
};

#endif
