
#if !defined(GUI_QT_MENUBUTTON_INCLUDED_H)
#define GUI_QT_MENUBUTTON_INCLUDED_H

#include "gui/GuiMenuButton.h"
#include "gui/qt/GuiQtElement.h"

class GuiButtonListener;
class GuiEventData;
class QAction;

/** Der GuiMenuButton ist ein Pushbutton, welcher in einem PulldownMenu installiert
    werden kann. Er gibt den Input über einen GuiButtonListener weiter. Er kann von
    einem GuiListenerController kontrolliert werden.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtMenuButton.h,v 1.3 2006/03/08 13:54:06 amg Exp $
 */
class GuiQtMenuButton : public GuiQtElement, public GuiMenuButton
{
  Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtMenuButton( GuiElement *parent, GuiButtonListener *listener,
                   GuiEventData *event = 0, bool hide_disabled=false, std::string name="" );

  virtual ~GuiQtMenuButton();
private:
  GuiQtMenuButton( const GuiQtMenuButton &menubtn );
/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
/*   virtual bool destroy(); */
  /** Diese Funktion liefert den Typ des GuiMenuButton-Objekts.
      @return type_MenuButton
   */
  virtual GuiElement::ElementType Type() { return GuiElement::type_MenuButton; }

  /** Die Funktion create() erstellt das Qt-Widget des Buttons. Sie wird in
      der Regel vom kontrollierenden Parent-Container aufgerufen.
      @param parent Parent-Widget des Buttons
  */
  virtual void create();

  /** Die Funktion manage() realisiert den Button auf der Oberfläche. Sie wird in
      der Regel vom kontrollierenden Parent-Container aufgerufen.
   */
  virtual void manage();
  virtual void unmanage();
  virtual void unmap();

  /** Die Funktion enable() aktiviert den Button wieder nach einem früheren Aufruf
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
  virtual GuiElement *clone() { return new GuiQtMenuButton( *this ); }
  virtual void serializeXML( std::ostream &os, bool recursive = false );
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return GuiMenuButton::variantMethod(method, jsonArgs, eng);
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setLabel( const std::string &text );
  virtual const std::string getLabel() { return m_label; }

  /** Mit der Funktion setDialogLabel() wird der Text des Labels des Buttons gesetzt.
      Dem Label werden drei Punkte angehängt zum Zeigen, das durch das Aktivieren
      des Buttons eine weitere Dialog-Form erscheint.
      @param text Label-Text des Buttons
   */
  virtual void setDialogLabel( const std::string &text );

  /** Mit der Funktion setAccelerator() kann für das Aktivieren ein Shortcut
      installiert werden.
      @param acc_key Zeichenfolge des Shortcuts
      @param acc_text Text des Shortcuts
  */
  virtual void setAccelerator( const std::string &acc_key, const std::string &acc_text );

  virtual GuiButtonListener* getButtonListener() { return m_listener; }

  virtual GuiElement * getElement();
  virtual GuiMenuButton* getMenuButton() { return this; }

/*=============================================================================*/
/* Qt specific methods                                                         */
/*=============================================================================*/
private slots:
  void                 Activate();

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  GuiButtonListener *m_listener;
  std::string        m_label;
  std::string        m_accelerator_key;
  std::string        m_accelerator_text;
  GuiEventData      *m_event;
  bool               m_hideDisabled;
private:
  QAction           *m_buttonAct;

};

#endif
