
#if !defined(GUI_RADIOBUTTON_INCLUDED_H)
#define GUI_RADIOBUTTON_INCLUDED_H

#include "gui/GuiElement.h"

class GuiRadioButton
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiRadioButton( GuiElement *parent )
    : m_radiobox( 0 ){
#if defined(GUI_STANDALONE_TEST)
    m_helptext = "GuiRadioButton";
#endif
  }
  GuiRadioButton( const GuiRadioButton &toggle )
    : m_radiobox( 0 ){
#if defined(GUI_STANDALONE_TEST)
    m_helptext = "GuiRadioButton";
#endif
  }

  virtual ~GuiRadioButton();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  /** Beschreibung siehe GuiElement.
   */
  virtual GuiElement::ElementType Type() { return GuiElement::type_RadioButton; }

  /** Der als Parameter übergebene RadioButton wird derselben Radiobox zugeordnet.
   */
  void connectRadioBox( GuiRadioButton *button );

  virtual bool cloneableForFieldgroupTable() { return false; }

  virtual void resetValue() = 0;

/*=============================================================================*/
/* protected class                                                             */
/*=============================================================================*/
class GuiRadioBox
{
public:
  GuiRadioBox( GuiRadioButton *button ){
    attach( button );
  }
  ~GuiRadioBox(){}

public:
  void attach( GuiRadioButton *button );
  void reset( GuiRadioButton *button );
  void remove(GuiRadioButton *button );

private:
  typedef std::vector<GuiRadioButton *> GuiRadioButtonList;

  GuiRadioButtonList  m_buttonlist;

};

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue() = 0;
  GuiRadioBox *getRadioBox();
  void setRadioBox( GuiRadioBox *box );
  bool hasRadioBox() { return m_radiobox != 0; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiRadioBox   *m_radiobox;

};



#endif
