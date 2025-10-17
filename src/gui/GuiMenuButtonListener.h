
#if !defined(GUI_MENUBUTTON_LISTENER_INCLUDED_H)
#define GUI_MENUBUTTON_LISTENER_INCLUDED_H

#include <string>
#include "gui/GuiButtonListener.h"

class GuiContainer;
class GuiMenuButton;

class GuiMenuButtonListener : public GuiButtonListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiMenuButtonListener()
    : m_dialog_label( false )
    , m_menu_installed( false ){
  }
  virtual ~GuiMenuButtonListener(){
  }

/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual void setButton( GuiMenuButton *button );
  GuiMenuButton *attach( GuiElement *parent, GuiEventData *event = 0 );
  void setLabel( const std::string &label ) { m_label = label; }
  void setDialogLabel( const std::string &label ) { m_label = label; m_dialog_label=true; }
  void setHelptext( const std::string &help ) { m_helptext = help; }
  void setMenuInstalled() { m_menu_installed = true; }
  bool isMenuInstalled() { return m_menu_installed; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string  m_label;
  bool         m_dialog_label;
  std::string  m_helptext;
  bool         m_menu_installed;

};

#endif
