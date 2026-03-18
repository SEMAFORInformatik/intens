#if !defined(GUI_QT_VOID_INCLUDED_H)
#define GUI_QT_VOID_INCLUDED_H

#include "gui/GuiVoid.h"
#include "gui/qt/GuiQtElement.h"

class GuiQtVoid : public GuiQtElement, public GuiVoid
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtVoid(GuiElement* parent);
  virtual ~GuiQtVoid(){}

protected:
  GuiQtVoid(const GuiQtVoid &v);

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Void; }
  virtual void create();
  virtual void manage() {}

  virtual bool destroy() { return true; }
  virtual void getSize(int &x, int &y, bool hint=true) {GuiVoid::getSize(x, y);}
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();
  virtual bool cloneable() { return true; }
  virtual bool isShown() { return false; }
  virtual GuiElement *clone() { return new GuiQtVoid(*this); }

  virtual void serializeXML(std::ostream &os, bool recursive = false) { GuiVoid::serializeXML(os, recursive); }
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false) { return GuiVoid::serializeJson(jsonObj, onlyUpdated); }
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) { return GuiVoid::serializeProtobuf(eles, onlyUpdated); }
#endif
  GuiElement *getElement() override { return this; }
/*=============================================================================*/
/* private member functions of GuiQtVoid                                       */
/*=============================================================================*/
private:
  bool recalcSize();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QWidget* m_widget;
};

#endif
