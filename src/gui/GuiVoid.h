
#if !defined(GUI_VOID_INCLUDED_H)
#define GUI_VOID_INCLUDED_H

#include "gui/GuiElement.h"

class GuiVoid : public GuiElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiVoid(GuiElement* parent);
  virtual ~GuiVoid(){}

protected:
  GuiVoid(const GuiVoid &v);

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Void; }
  virtual void create();
  virtual void manage() {}

  virtual bool destroy() { return true; }
  virtual void getSize(int &x, int &y, bool hint=true) {x=m_width;y=m_height;}
  virtual void update( UpdateReason );
  virtual void* myWidget();
  virtual bool cloneable() { return true; }
  virtual bool isShown() { return false; }
  virtual GuiElement *clone() { return new GuiVoid(*this); }

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual GuiMotifElement *getMotifElement() { return 0; }
  virtual GuiQtElement *getQtElement()       { return 0; }
/*=============================================================================*/
/* public member functions of GuiVoid                                          */
/*=============================================================================*/
public:
  void setWidth( int w ) { m_width = w; }
  void setHeight( int h ) { m_height = h; }
  void setDisplayPercentWidth( int w );
  void setDisplayPercentHeight( int h );
/*=============================================================================*/
/* private member functions of GuiVoid                                         */
/*=============================================================================*/
private:
  bool recalcSize();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int   m_width;
  int   m_height;
  int   m_displayPercentWidth;
  int   m_displayPercentHeight;
  void* m_widget;
};

#endif
