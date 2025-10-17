
#include "gui/GuiFactory.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiMenuButtonListener.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/


/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setButton --                                                                */
/* --------------------------------------------------------------------------- */

void GuiMenuButtonListener::setButton( GuiMenuButton *button ) {
  GuiButtonListener::setButton( button );

  if( !m_label.empty() ){
    if( m_dialog_label )
      button->setDialogLabel( m_label );
    else
      button->setLabel( m_label );
  }
  else{
    button->setLabel( "<No Label>" );
  }
  if( !m_helptext.empty() ){
    button->getElement()->setHelptext( m_helptext );
    m_helptext.erase();
  }
}

/* --------------------------------------------------------------------------- */
/* attach --                                                                   */
/* --------------------------------------------------------------------------- */

GuiMenuButton *GuiMenuButtonListener::attach( GuiElement *parent, GuiEventData *event ){
  assert( parent != 0 );

  GuiMenuButton *button = GuiFactory::Instance()->createMenuButton( parent, this, event );
  parent->attach( button->getElement() );
  setMenuInstalled();
  return button;
}
