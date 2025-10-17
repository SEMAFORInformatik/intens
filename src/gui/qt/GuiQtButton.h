
#if !defined(GUI_QT_BUTTON_INCLUDED_H)
#define GUI_QT_BUTTON_INCLUDED_H

#include <qobject.h>

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiButton.h"

class QPushButton;

/** Dieses Objekt wird in der Regel als ActionButton in der GuiButtonbar
    verwendet.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtButton.h,v 1.5 2005/03/08 10:05:39 tar Exp $
 */
class GuiButtonListener;
class GuiEventData;

class GuiQtButton : public GuiQtElement, public GuiButton
{
  Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event = 0 );

  virtual ~GuiQtButton();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Button; }
  virtual void create();
  virtual void manage();
  virtual bool destroy();
  virtual void enable();
  virtual void disable();
  virtual void update( UpdateReason ) {}
  virtual QWidget* myWidget();
  virtual void addTabGroup();
  virtual void removeTabGroup();

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  const std::string &Label() { return m_label; }
  GuiElement * getElement() { return this; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
  private slots:
    void activate();

/*=============================================================================*/
/* private Data                                                              */
/*=============================================================================*/
private:
  QPushButton       *m_buttonwidget;
};

#endif
