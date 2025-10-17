
#include "gui/GuiButtonListener.h"
#include "gui/GuiElement.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiButton.h"

std::vector<GuiButtonListener*> GuiButtonListener::s_listeners;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiButtonListener::GuiButtonListener()
  : m_button( 0 ), m_allow(true) {
  s_listeners.push_back( this );
}

/* --------------------------------------------------------------------------- */
/* getButtonListenerById --                                                    */
/* --------------------------------------------------------------------------- */

GuiButtonListener* GuiButtonListener::getButtonListenerById( int id ){
  if( s_listeners.size() > id ){
    return s_listeners[id];
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getButtonListenerByMenuButtonId --                                          */
/* --------------------------------------------------------------------------- */

GuiButtonListener* GuiButtonListener::getButtonListenerByMenuButtonId( const int& id ){
  int i(-1);
  while( s_listeners.size() > ++i ){
    if (s_listeners[i]->getButton() &&
        s_listeners[i]->getButton()->getElementIntId() == id) {
      return s_listeners[i];
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getId --                                                                    */
/* --------------------------------------------------------------------------- */

int GuiButtonListener::getId(){
  int id = 0;
  std::vector<GuiButtonListener*>::iterator iter;
  for( iter = s_listeners.begin(); iter != s_listeners.end(); ++iter, ++id ){
    if( *iter == this ){
      return id;
    }
  }
  assert( false );
  return -1;
}

/* --------------------------------------------------------------------------- */
/* manageButton --                                                             */
/* --------------------------------------------------------------------------- */

void GuiButtonListener::manageButton(){
  if( m_button != 0 ){
    m_button->manage();
  }
}

/* --------------------------------------------------------------------------- */
/* unmanageButton --                                                           */
/* --------------------------------------------------------------------------- */

void GuiButtonListener::unmanageButton(){
  if( m_button != 0 ){
    m_button->unmanage();
  }
}

/* --------------------------------------------------------------------------- */
/* allow --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiButtonListener::allow(){
  if( m_button != 0 ){
    m_button->enable();
  }
  m_allow = true;
}

/* --------------------------------------------------------------------------- */
/* disallow --                                                                 */
/* --------------------------------------------------------------------------- */

 void GuiButtonListener::disallow(){
   if( m_button != 0 ){
     m_button->disable();
   }
   m_allow = false;
 }

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void GuiButtonListener::ButtonPressed(){}
void GuiButtonListener::ButtonPressed( GuiEventData *event ){}

/* --------------------------------------------------------------------------- */
/* setButton --                                                                */
/* --------------------------------------------------------------------------- */

void GuiButtonListener::setButton( GuiMenuButton *button ){
  m_button = button->getElement();
}

void GuiButtonListener::setButton( GuiButton *button ){
  m_button = button->getElement();
}

/* --------------------------------------------------------------------------- */
/* getButton --                                                                */
/* --------------------------------------------------------------------------- */

GuiElement *GuiButtonListener::getButton(){
  return m_button;
}
