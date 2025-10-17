
#if !defined(GUI_BUTTON_LISTENER_INCLUDED_H)
#define GUI_BUTTON_LISTENER_INCLUDED_H

#include <vector>

#include "gui/GuiListener.h"

class GuiElement;
class GuiMenuButton;
class GuiButton;
class GuiEventData;
class JobAction;

/** Der GuiButtonListener ist die Verbindung zwischen einem Button und einem Objekt,
    welches durch einen Button aktiviert werden soll. Ein GuiButtonListener kann
    nur mit einem Button verbunden werden.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiButtonListener.h,v 1.11 2005/10/28 09:51:43 ked Exp $
*/
class GuiButtonListener : public GuiListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiButtonListener();

  virtual ~GuiButtonListener(){
  }

/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  /** Diese Funktion liefert den Typ des Listeners.
      @return ListenerType
   */
  virtual ListenerType getListenerType() { return type_Button; }

  /** Mit manageButton() wird die Funktion manage() des Buttons aufgerufen.
   */
  virtual void manageButton();

  /** Mit unmanageButton() wird die Funktion unmanage() des Buttons aufgerufen.
   */
  virtual void unmanageButton();

  /** Mit allow() wird das Aktivieren des Buttons erlaubt.
   */
  virtual void allow();

  /** Mit disallow() wird das Aktivieren des Buttons unterbunden.
   */
  virtual void disallow();

  /** Mit isAllowed() wird das allowed flag abgefragt.
   */
  bool  isAllowed() { return m_allow; }

  /** Mit dem Aufruf dieser Funktion teilt der Button mit, dass er gedrückt wurde.
  */
  virtual void ButtonPressed();
  virtual void ButtonPressed( GuiEventData *event );

  /** Der Button liefert einen Pointer auf sich selber um die Kommunikation
      in beide Richtungen zu ermöglichen.
      @param Pointer auf den Button.
  */
  virtual void setButton( GuiMenuButton *button );
  virtual void setButton( GuiButton *button );

  /** Unter Umstaenden kann der Parent neu gesetzt werden
      (Nur im Navigator noetig, da dort das Menu in verschiedenen
       Navigatoren kommen kann)
  */
  virtual void setParent( GuiElement *par ) {}

  GuiElement *getButton();

  virtual JobAction *getAction() = 0;

  static GuiButtonListener* getButtonListenerById( int id );
  static GuiButtonListener* getButtonListenerByMenuButtonId( const int& id );
  int getId();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  GuiElement    *m_button;
  bool           m_allow;
private:
  static std::vector<GuiButtonListener*> s_listeners;
};

#endif
