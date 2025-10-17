
#if !defined(GUI_LISTENERCONTROLLER_INCLUDED_H)
#define GUI_LISTENERCONTROLLER_INCLUDED_H

#include <assert.h>
#include <list>
#include <string>

class GuiElement;
class GuiListener;
class GuiButtonListener;
class GuiToggleListener;
class GuiButtonbar;

/** Der ListenerController kontrolliert mehrere Button- oder ToggleListeners, welche
    in einer Buttonbar oder in einem Pulldownmenu installiert sind. Objekte, welche
    über mehrere Buttons oder Toggles aktiviert werden wollen, werden einfach
    von GuiListenerController abgeleitet.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiListenerController.h,v 1.12 2003/11/25 14:25:28 hob Exp $
*/
class GuiListenerController
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiListenerController();
  virtual ~GuiListenerController() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Mit allow() wird das Aktivieren aller GuiListener erlaubt. Dies erfolgt
      durch den Aufruf deren Funktion enable();
   */
  void allow();

  /** Mit disallow() wird das Aktivieren aller GuiListener unterbunden. Dies
      erfolgt durch den Aufruf deren Funktion disable();
   */
  void disallow();

  /** Die Funktion setLock() kann zu jedem Zeitpunkt aufgerufen werden.
      Es wird sofort dafür gesorgt, dass alle Buttons und Toggles, welche
      mit einem vom ListenerController kontrollierten Listener verbunden sind, nicht mehr
      aktiviert werden können. Ein Lock kann nicht mehr rückgängig gemacht werden.
      Er gehört zum selektiven Zugriffskonzept der Usergroups.
  */
  void setLock();

  /** Diese Funktion liefert bei jedem Aufruf den Pointer eines neuen ButtonListeners.
      Es können so beliebig viele Listeners für verschiedene Buttons abgerufen werden.
      @return Pointer auf neuen GuiButtonListener.
  */
  GuiButtonListener *getButtonListener();
  /** Diese Funktion liefert bei jedem Aufruf den Pointer eines neuen ButtonListeners.
      Es können so beliebig viele Listeners für verschiedene Buttons abgerufen werden.
      @return Pointer auf neuen GuiButtonListener.
  */
  GuiButtonListener *getNavigatorButtonListener( GuiElement * );

  /** Diese Funktion liefert bei jedem Aufruf den Pointer eines neuen ToggleListeners.
      Es können so beliebig viele Listeners für verschiedene Toggles abgerufen werden.
      @return Pointer auf neuen GuiToggleListener.
  */
  GuiToggleListener *getToggleListener();

  /** Diese Funktion setzt das entsprechende Flag im Controller.
   */
  void setMenuInstalled(){ m_menu_installed = true; }

  /** Diese Funktion teilt mit, ob das entsprechende Flag nit der Funktion
      setMenuInstalled() gesetzt worden ist.
  */
  bool MenuInstalled() { return m_menu_installed; }

  /** Mit dieser Funktion wird diejenige Form bestimmt in welcher ein Button
      in der Buttonbar installiert werden soll.
  */
  void setForm( const std::string &name );

  /** Mit dem Aufruf dieser Funktion wird dafür gesorgt, dass keine Buttons automatisch
      generiert werden.
  */
  void unsetForm();

  /** die Funktion liefert einen Pointer auf die Buttonbar der gewünschten Form, welche
      mit setForm() gesetzt wurde.
      @return Pointer auf GuiButtonbar.
  */
  GuiButtonbar *getButtonbar();

  /** Mit der Funktion sensitive() kann jederzeit nachgefragt werden, ob die
      Buttons der entsprechenden Listeners sensitiv sein sollen.
      @return True: => Buttons dürfen sensitiv sein.
  */
  bool sensitive() { return m_sensitive; }

  void setNoButtonbar() { m_no_buttonbar = true; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Diese Funktion soll einen neuen GuiButtonListener erstellen.
      @return Pointer auf neuen GuiButtonListener.
  */
  virtual GuiButtonListener *createButtonListener() = 0;
  virtual GuiButtonListener *createNavigatorButtonListener( GuiElement *menu ){
    assert( false );
    return 0;
  }

  /** Diese Funktion soll einen neuen GuiToggleListener erstellen.
      @return Pointer auf neuen GuiToggleListener.
  */
  virtual GuiToggleListener *createToggleListener() = 0;

  /** Diese Funktion ruft bei allen vorhandenen GuiToggleListener die Funktion
      setAllToggleStatus() auf und gibt den Parameter state einfach an alle
      Listeners weiter.
      @param state Neuer gewünschter Status der Toggles
  */
  void setAllToggleStatus( bool state );

  /** Diese Funktion wird von allow() aufgerufen.
   */
  virtual void isAllowed() {}

  /** Diese Funktion wird von disallow() aufgerufen.
   */
  virtual void isNotAllowed() {}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::list<GuiListener *>  GuiListenerList;

  GuiListenerList    m_listeners;
  bool               m_locked;
  bool               m_sensitive;
  bool               m_menu_installed;
  GuiButtonbar      *m_buttonbar;
  bool               m_no_buttonbar;

};

#endif
