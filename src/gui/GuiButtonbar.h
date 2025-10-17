
#if !defined(GUI_BUTTONBAR_INCLUDED_H)
#define GUI_BUTTONBAR_INCLUDED_H

#include "gui/GuiElement.h"

class GuiButtonbar
{
 public:
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
  GuiButtonbar();
  ~GuiButtonbar();

  /*=============================================================================*/
  /* public member functions                                                     */
  /*=============================================================================*/
  virtual GuiElement::ElementType Type() { return GuiElement::type_Buttonbar; }
  virtual GuiElement *getElement() = 0;
  virtual void attach( GuiElement *e ) { m_elements.push_back(e); }
  virtual void front( GuiElement *e )  { m_elements.insert(m_elements.begin(), e); }
  void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual void getVisibleElement(GuiElementList& res);

  /*=============================================================================*/
  /* protected Data                                                              */
  /*=============================================================================*/
 protected:
  GuiElementList m_elements;
};

#endif
