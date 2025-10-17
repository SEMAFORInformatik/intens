
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/GuiForm.h"
#include "gui/GuiListenerController.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiToggleListener.h"
#include "gui/GuiButtonbar.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiListenerController::GuiListenerController()
  : m_locked( false )
  , m_sensitive( true )
  , m_menu_installed( false )
  , m_buttonbar( 0 )
  , m_no_buttonbar( false ) {
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setLock --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiListenerController::setLock(){
  disallow();
  m_locked = true;
}

/* --------------------------------------------------------------------------- */
/* allow --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiListenerController::allow(){
  if( m_locked ) return;

  m_sensitive = true;
  GuiListenerList::iterator i;
  for( i = m_listeners.begin(); i != m_listeners.end(); ++i ){
    (*i)->allow();
  }
  // Falls die abgeleitete Klasse davon etwas wissen will, kann sie diese
  // Funktion implementieren.
  isAllowed();
}

/* --------------------------------------------------------------------------- */
/* disallow --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiListenerController::disallow(){
  m_sensitive = false;
  GuiListenerList::iterator i;
  for( i = m_listeners.begin(); i != m_listeners.end(); ++i ){
    (*i)->disallow();
  }
  // Falls die abgeleitete Klasse davon etwas wissen will, kann sie diese
  // Funktion implementieren.
  isNotAllowed();
}

/* --------------------------------------------------------------------------- */
/* getButtonListener --                                                        */
/* --------------------------------------------------------------------------- */

GuiButtonListener *GuiListenerController::getButtonListener(){
  if( m_locked ) return 0;

  GuiButtonListener *ls = createButtonListener();
  if( ls != 0 ){
    m_listeners.push_back( ls );
  }
  ls->setController( this );
  return ls;
}

/* --------------------------------------------------------------------------- */
/* getNavigatorButtonListener --                                               */
/* --------------------------------------------------------------------------- */

GuiButtonListener *GuiListenerController::getNavigatorButtonListener( GuiElement *menu ){
  if( m_locked ) return 0;

  GuiButtonListener *ls = createNavigatorButtonListener( menu );
  if( ls != 0 ){
    m_listeners.push_back( ls );
  }
  ls->setController( this );
  return ls;
}

/* --------------------------------------------------------------------------- */
/* getToggleListener --                                                        */
/* --------------------------------------------------------------------------- */

GuiToggleListener *GuiListenerController::getToggleListener(){
  if( m_locked ) return 0;

  GuiToggleListener *ls = createToggleListener();
  if( ls != 0 ){
    m_listeners.push_back( ls );
  }
  ls->setController( this );
  return ls;
}

/* --------------------------------------------------------------------------- */
/* setAllToggleStatus --                                                       */
/* --------------------------------------------------------------------------- */

void GuiListenerController::setAllToggleStatus( bool state ){
  GuiListenerList::iterator i;
  for( i = m_listeners.begin(); i != m_listeners.end(); ++i ){
    (*i)->setToggleStatus( state );
  }
}

/* --------------------------------------------------------------------------- */
/* setForm --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiListenerController::setForm( const std::string &name ){
  GuiForm *form = GuiElement::getForm( name );
  if( form != 0 ){
    m_buttonbar = form->getButtonbar();
    m_no_buttonbar = false;
  }
}

/* --------------------------------------------------------------------------- */
/* unsetForm --                                                                */
/* --------------------------------------------------------------------------- */

void GuiListenerController::unsetForm(){
  m_no_buttonbar   = true;
  m_menu_installed = true;
}

/* --------------------------------------------------------------------------- */
/* getButtonbar --                                                             */
/* --------------------------------------------------------------------------- */

GuiButtonbar *GuiListenerController::getButtonbar(){
  if( m_no_buttonbar ) return 0;

  if( m_buttonbar == 0 ){
    m_buttonbar = GuiManager::Instance().getMainButtonbar();
  }
  return m_buttonbar;
}
