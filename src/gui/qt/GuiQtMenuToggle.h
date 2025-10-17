
#if !defined(GUI_QT_MENUTOGGLE_INCLUDED_H)
#define GUI_QT_MENUTOGGLE_INCLUDED_H

#include <string>

#include "gui/GuiMenuToggle.h"
#include "gui/qt/GuiQtElement.h"

class GuiToggleListener;
class GuiEventData;
class QAction;

/** Der GuiMenuToggle ist ein Toggle, welcher in einem PulldownMenu installiert
    werden kann. Er gibt den Input über einen GuiToggleListener weiter. Er kann von
    einem GuiListenerController kontrolliert werden.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtMenuToggle.h,v 1.3 2008/06/20 12:03:45 amg Exp $
 */
class GuiQtMenuToggle : public GuiQtElement, public GuiMenuToggle
{
  Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtMenuToggle( GuiElement *parent
		    , GuiToggleListener *listener
		    , GuiEventData *event = 0 );

  virtual ~GuiQtMenuToggle();
private:
  GuiQtMenuToggle( const GuiQtMenuToggle &menutoggle );

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  /** Diese Funktion liefert den Typ des GuiMenuToggle-Objekts.
      @return type_MenuToggle
   */
  virtual GuiElement::ElementType Type() { return GuiElement::type_MenuToggle; }

  /** Die Funktion create() erstellt das Qt-Widget des Toggles. Sie wird in
      der Regel vom kontrollierenden Parent-Container aufgerufen.
      @param parent Parent-Widget des Buttons
  */
  virtual void create();

  /** Die Funktion manage() realisiert den Toggle auf der Oberfläche. Sie wird in
      der Regel vom kontrollierenden Parent-Container aufgerufen.
   */
  virtual void manage() {}

/*   virtual bool destroy(); */

  /** Die Funktion enable() aktiviert den Toggle wieder nach einem früheren Aufruf
      von disable(). Sie wird in der Regel von einem GuiListenerController
      aufgerufen.
  */
   virtual void enable();

  /** Die Funktion disable() deaktiviert den Button. Er kann anschliessend mit
      ensable() wieder aktiviert werden. Sie wird in der Regel von einem
      GuiListenerController aufgerufen.
  */
  virtual void disable();

  /** Die Funktion update() tut nichts. Sie muss implemetiert sein. Sie
      wird in der Regel vom kontrollierenden Parent-Container aufgerufen.
   */
  virtual void update( UpdateReason ) {}

  /** Diese Funktion liefert das Qt-Widget des Buttons.
      @return Widget des Buttons
  */
  virtual QWidget* myWidget();

  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtMenuToggle( *this ); }

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

  virtual GuiToggleListener* getToggleListener() { return m_listener; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Mit der Funktion setLabel() wird der Text des Labels des Toggles gesetzt.
      @param text Label-Text des Toggles
   */
  virtual bool setLabel( const std::string &text );
  virtual const std::string getLabel() { return m_label; }

  /** Mit der Funktion setDialogLabel() wird der Text des Labels des Toggles gesetzt.
      Dem Label werden drei Punkte angehängt zum Zeigen, das durch das Aktivieren
      des Toggles eine weitere Dialog-Form erscheint.
      @param text Label-Text des Toggles
   */
  void setDialogLabel( const std::string &text );

  /** Mit der Funktion setToggleStatus() kann der Status des Toggles verändert
      werden.
      @param state True: => der Toggle wird gedrückt angezeigt
  */
  virtual void setToggleStatus( bool state );
  /** Mit der Funktion getToggleStatus() kann der Status des Toggles abgefragt
      werden.
  */
  virtual bool getToggleStatus();

  /** Mit der Funktion setAccelerator() kann für das Aktivieren ein Shortcut
      installiert werden.
      @param acc_key Zeichenfolge des Shortcuts
      @param acc_text Text des Shortcuts
  */
  virtual void setAccelerator( const std::string &acc_key, const std::string &acc_text );

  virtual GuiElement * getElement();

/*=============================================================================*/
/* Qt specific methods                                                         */
/*=============================================================================*/
public :
  /** Liefert den hinter dem Menuepunkt liegende QAction zurueck
   */
  QAction*             getMenuAction() const { return m_toggleAct; }
  /** Liefert den hinter dem Menuepunkt liegende GuiEventData zurueck
   */
  GuiEventData*        getEventData() const { return m_event; }
private slots:
  void                 ValueChanged();

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  GuiToggleListener *m_listener;
  std::string        m_label;
  GuiEventData      *m_event;
 private:
  bool               m_toggle_state;
  QAction           *m_toggleAct;
  std::string        m_accelerator_key;
  std::string        m_accelerator_text;
};

#endif
