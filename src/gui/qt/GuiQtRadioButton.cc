
#include <QCheckBox>

#include "utils/Debugger.h"
#include "gui/qt/GuiQtRadioButton.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQtRadioButton::GuiQtRadioButton( GuiElement *parent )
  : GuiRadioButton( parent ), GuiQtToggle( parent )
{
#if defined(GUI_STANDALONE_TEST)
  m_helptext = "GuiQtRadioButton";
#endif
}

GuiQtRadioButton::GuiQtRadioButton( const GuiQtRadioButton &toggle )
  : GuiRadioButton( toggle ), GuiQtToggle( toggle )
{
#if defined(GUI_STANDALONE_TEST)
  m_helptext = "GuiQtRadioButton";
#endif
}

GuiQtRadioButton::~GuiQtRadioButton(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtRadioButton::setInputValue(){
  BUG(BugGui,"GuiQtRadioButton::setInputValue");
  double value = 0.0;
  if( getToggle()->isChecked() ){
    value = 1.0;
    if( getRadioBox() != 0 ){
      getRadioBox()->reset( this );
    }
  }
  m_param->DataItem()->setValue( value );
  return true;
}

/* --------------------------------------------------------------------------- */
/* resetValue --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtRadioButton::resetValue(){
  BUG(BugGui,"GuiQtRadioButton::resetValue");
  m_param->DataItem()->setValue( 0.0 );

  // block Signals , we are already inside a GuiUpdate
  bool bret=myWidget()->signalsBlocked();
  myWidget()->blockSignals(true);
  getToggle()->setChecked(false);
  myWidget()->blockSignals(bret);
}

/* --------------------------------------------------------------------------- */
/* CloneForFieldgroupTable --                                                  */
/* --------------------------------------------------------------------------- */

GuiQtDataField *GuiQtRadioButton::CloneForFieldgroupTable(){
  GuiQtRadioButton *txt = new GuiQtRadioButton( *this );
  return txt;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtRadioButton::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdate){
  return GuiQtDataField::serializeProtobuf(eles->add_data_fields(), onlyUpdate);
}
#endif
