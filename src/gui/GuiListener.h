
#if !defined(GUI_BASE_LISTENER_INCLUDED_H)
#define GUI_BASE_LISTENER_INCLUDED_H

#include "gui/GuiListenerController.h"

/** The GuiListener is the base class for all GuiListenerController
    managed Listeners
*/
class GuiListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiListener()
    : m_controller( 0 ){
  }
  virtual ~GuiListener() {}

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum ListenerType
  { type_Toggle
  , type_Button
  };

/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  /** Diese Funktion liefert den Typ des Listeners.
      @return ListenerType
   */
  virtual ListenerType getListenerType() = 0;

  /** Mit allow() wird das Aktivieren aller GuiListener erlaubt.
   */
  virtual void allow() = 0;

  /** Mit disallow() wird das Aktivieren aller GuiListener unterbunden.
   */
  virtual void disallow() = 0;

  /** Ein Listener kann jederzeit darüber Auskunft geben, ob der entsprechende
      Button sensitiv sein soll oder nicht, sofern er mit einem GuiListenerController
      verbunden ist ( siehe setController() ).
      @return True: => Button darf sensitiv sein.
  */
  bool sensitive() { if( m_controller ) return m_controller->sensitive(); return true; }

  /** Ein GuiListenerController verbindet sich mit allen GuiListener-Objekten, welche
      seiner Kontrolle unterstehen.
      @param controller Pointer auf das kontrollierende GuiListenerController-Objekt
  */
  void setController( GuiListenerController *c ) { m_controller = c; }

  /** Diese Funktion teilt einem Toggle den neuen gewünschten Status mit.
      @param state Neuer Status für ein Toggle.
  */
  virtual void setToggleStatus( bool state ) {}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiListenerController *m_controller;

};

#endif
