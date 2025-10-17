
#if !defined(GUI_QT_MENUBAR_INCLUDED_H)
#define GUI_QT_MENUBAR_INCLUDED_H

#include <vector>

#include "gui/GuiContainer.h"
#include "gui/GuiMenubar.h"
#include "gui/qt/GuiQtElement.h"

class GuiQtPulldownMenu;
class QMenuBar;

class GuiQtMenubar : public GuiMenubar, public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtMenubar( GuiElement *parent );
  virtual ~GuiQtMenubar();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Menubar; }
  virtual void create();
  virtual void manage();
  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason r);
  virtual QWidget* myWidget();
/*   virtual Widget myParentWidget() { return m_container.myWidget(); } */
  virtual void serializeXML(std::ostream &os, bool recursive);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  virtual bool serializeContainerElements(in_proto::ElementList* eles, in_proto::Menubar* element, bool onlyUpdated);
#endif
  virtual void serializeContainerElements( std::ostream &os );
  virtual bool serializeContainerElements(Json::Value& jsonObj, bool onlyUpdated = false);
  virtual GuiElement * getElement();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

/*=============================================================================*/
/* public member functions (GuiContainer)                                      */
/*=============================================================================*/
public:
  virtual void attach( GuiElement *e ) { m_container.push_back(e); }
  virtual void front( GuiElement *e )  { m_container.insert(m_container.begin(), e); }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void attachHelp( GuiPulldownMenu *menu );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
//   typedef std::vector< GuiElement*> GuiQtContainer;
  GuiQtPulldownMenu   *m_helpmenu;
  GuiContainer       m_container;
  QMenuBar*            m_qmenubar;
};

#endif
