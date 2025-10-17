
#ifndef GUI_NAVIGATOR_QT_MENU_INCLUDED_H
#define GUI_NAVIGATOR_QT_MENU_INCLUDED_H

#include "gui/GuiNavigatorMenu.h"
#include "gui/qt/GuiQtPopupMenu.h"

class GuiNavigator;
class XferDataItem;

class GuiQtNavigatorMenu : public GuiNavigatorMenu, public GuiQtPopupMenu
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtNavigatorMenu( GuiElement *parent )
    : GuiNavigatorMenu()
    , GuiQtPopupMenu( parent )
  {}
  virtual ~GuiQtNavigatorMenu(){
  }
  void create( GuiNavigator *parent, XferDataItem *xfer );
  void popup(int x, int y, XferDataItem *xferSrc=0);
  void popup(int x, int y, const std::vector<XferDataItem*> xferSrcList);
private:
  // Kein einfacher Konstruktor
  GuiQtNavigatorMenu();
  // Kein Copy-Konstruktor
  GuiQtNavigatorMenu(const GuiQtNavigatorMenu& menu);

  GuiQtNavigatorMenu* clone() { return new GuiQtNavigatorMenu( *this ); }
};

#endif
