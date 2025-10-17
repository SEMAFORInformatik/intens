
#if !defined(GUI_QTRADIOBUTTON_INCLUDED_H)
#define GUI_QTRADIOBUTTON_INCLUDED_H

#include "gui/GuiRadioButton.h"
#include "gui/qt/GuiQtToggle.h"

class GuiRadioBox;

class GuiQtRadioButton : public GuiRadioButton, public GuiQtToggle
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtRadioButton( GuiElement *parent );
  GuiQtRadioButton( const GuiQtRadioButton &toggle );

  virtual ~GuiQtRadioButton();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  /** Beschreibung siehe GuiElement.
   */
  virtual GuiElement::ElementType Type() { return GuiElement::type_RadioButton; }

  /** Der als Parameter übergebene RadioButton wird derselben Radiobox zugeordnet.
   */
  virtual GuiQtDataField *CloneForFieldgroupTable();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtRadioButton( *this ); }
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdate = false);
#endif

  virtual void resetValue();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue();
};



#endif
