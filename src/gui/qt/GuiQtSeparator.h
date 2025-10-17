
#if !defined(GUI_QT_SEPARATOR_INCLUDED_H)
#define GUI_QT_SEPARATOR_INCLUDED_H

#include "gui/GuiSeparator.h"
#include "gui/qt/GuiQtElement.h"

class QWidget;

class GuiQtSeparator : public GuiSeparator,  public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtSeparator( GuiElement *parent );
  GuiQtSeparator( const GuiQtSeparator &separator );
  virtual ~GuiQtSeparator();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Separator; }
  virtual void create();
  virtual void manage() {}
  virtual bool destroy();
  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason ) {}
  virtual QWidget* myWidget();
  virtual void setOrientation( GuiElement::Orientation );
  virtual GuiElement::Orientation getOrientation() { return m_orientation; }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtSeparator( *this ); }
  virtual GuiElement * getElement() { return this; }
  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QWidget*                 m_sepwidget;
  GuiElement::Orientation  m_orientation;
  bool                     m_showMenu;
};

#endif
