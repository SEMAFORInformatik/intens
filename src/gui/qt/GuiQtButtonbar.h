
#if !defined(GUI_QT_BUTTONBAR_INCLUDED_H)
#define GUI_QT_BUTTONBAR_INCLUDED_H

#include <vector>
#include "gui/GuiButtonbar.h"
#include "gui/qt/GuiQtElement.h"

class QWidget;
class QMotifWidget;

class GuiQtButtonbar : public GuiQtElement, public GuiButtonbar
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtButtonbar( GuiElement *parent );
  virtual ~GuiQtButtonbar();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Buttonbar; }
  virtual void create();
  virtual void manage();
  virtual bool destroy();
  virtual void setTabOrder();
  virtual void unsetTabOrder();
  virtual void serializeXML(std::ostream &os, bool recursive = false) {
    GuiButtonbar::serializeXML(os, recursive);
  }
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false) {
    return GuiButtonbar::serializeJson(jsonObj, onlyUpdated);
  }
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) {
    return GuiButtonbar::serializeProtobuf(eles, onlyUpdated);
  }
#endif
  virtual void getVisibleElement(GuiElementList& res) {
    GuiButtonbar::getVisibleElement(res);
  }
  virtual GuiElement::Orientation getDialogExpandPolicy();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setMaxButtonsPerLine( int );
  virtual GuiElement *getElement(){ return this; }
  virtual QWidget* myWidget();
  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason r);
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  virtual GuiButtonbar *getButtonbar() { return this; }
  virtual GuiQtButtonbar *getQtButtonbar() { return this; }

  virtual void attach( GuiElement *e ) { GuiButtonbar::attach(e); }
  virtual void front( GuiElement *e )  { GuiButtonbar::front(e); }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  int getButtonsPerLine( int & );
  void createButtonLine( int, GuiElementList::iterator &, int &, int & );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int            m_maxbuttons;
  QWidget       *m_buttonbar;
};

#endif
