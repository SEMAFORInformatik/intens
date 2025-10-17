
#if !defined(GUI_TOGGLE_LISTENER_INCLUDED_H)
#define GUI_TOGGLE_LISTENER_INCLUDED_H

#include "gui/GuiListener.h"
#include "gui/GuiElement.h"

class GuiEventData;

/** Der GuiToggleListener ist die Verbindung zwischen einem Toggle und einem Objekt,
    welches durch ein Toggle aktiviert werden soll. Ein GuiToggleListener kann
    nur mit einem Button verbunden werden.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiToggleListener.h,v 1.6 2003/10/07 09:27:44 hob Exp $
*/
class GuiToggleListener : public GuiListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiToggleListener()
    : m_toggle(0), m_allow(true) {
  }
  virtual ~GuiToggleListener(){
  }

/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  /** Diese Funktion liefert den Typ des Listeners.
      @return ListenerType
   */
  virtual ListenerType getListenerType() { return type_Toggle; }

  /** Mit allow() wird das Aktivieren des Toggles erlaubt.
   */
  virtual void allow() { if( m_toggle != 0 ) m_toggle->enable(); m_allow = true; }

  /** Mit disallow() wird das Aktivieren des Toggles unterbunden.
   */
  virtual void disallow() { if( m_toggle != 0 ) m_toggle->disable(); m_allow = false; }

  /** Mit dem Aufruf dieser Funktion teilt das Toggle bei einer Aenderung des
      Status den neuen Status mit.
      @param state Neuer Status des Toggles
  */
  /** Mit isAllowed() wird das allowed flag abgefragt.
   */
  bool  isAllowed() { return m_allow; }
  virtual void ToggleStatusChanged( bool state ){};
  virtual void ToggleStatusChanged( GuiEventData *event ){};

  /** Das Toggle liefert einen Pointer auf sich selber um die Kommunikation
      in beide Richtungen zu ermöglichen.
      @param Pointer auf das Toggle.
  */
  void setToggle( GuiElement *toggle ) { m_toggle = toggle; }

  /** Diese Funktion teilt dem Toggle den neuen gewünschten Status mit.
      @param state Neuer Status für das Toggle.
  */
  virtual void setToggleStatus( bool state ){
    if( m_toggle != 0 ) m_toggle->setToggleStatus( state );
  }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiElement   *m_toggle;
  bool          m_allow;
};

#endif
