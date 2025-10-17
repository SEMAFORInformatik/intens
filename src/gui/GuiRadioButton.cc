
#include "utils/Debugger.h"
#include "gui/GuiRadioButton.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiRadioButton::~GuiRadioButton(){
  if( m_radiobox != 0 ){
    m_radiobox->remove( this );
    m_radiobox = 0;
  }
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* connectRadioBox --                                                          */
/* --------------------------------------------------------------------------- */

void GuiRadioButton::connectRadioBox( GuiRadioButton *button ){
  if( button == 0 ) return;

  if( m_radiobox == 0 ){
    m_radiobox = button->getRadioBox();
    m_radiobox->attach( this );
  }
  else{
    if( !button->hasRadioBox() ){
      button->setRadioBox( m_radiobox );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getRadioBox --                                                              */
/* --------------------------------------------------------------------------- */

GuiRadioButton::GuiRadioBox *GuiRadioButton::getRadioBox(){
  if( m_radiobox == 0 ){
    m_radiobox = new GuiRadioBox( this );
    m_radiobox->attach( this );
  }
  return m_radiobox;
}

/* --------------------------------------------------------------------------- */
/* setRadioBox --                                                              */
/* --------------------------------------------------------------------------- */

void GuiRadioButton::setRadioBox( GuiRadioButton::GuiRadioBox *box ){
  assert( box != 0 );
  assert( m_radiobox == 0 );
  m_radiobox = box;
  m_radiobox->attach( this );
}

/* --------------------------------------------------------------------------- */
/* attach --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiRadioButton::GuiRadioBox::attach( GuiRadioButton *button ){
  BUG(BugGui,"GuiRadioButton::GuiRadioBox::attach");
  assert( button != 0 );
  GuiRadioButtonList::iterator i;
  for ( i = m_buttonlist.begin(); i != m_buttonlist.end(); ++i ){
    if( (*i) == button ) return;
  }
  m_buttonlist.push_back( button );
}

/* --------------------------------------------------------------------------- */
/* reset --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiRadioButton::GuiRadioBox::reset( GuiRadioButton *button ){
  BUG(BugGui,"GuiRadioButton::GuiRadioBox::reset");
  assert( button != 0 );
  GuiRadioButtonList::iterator i;
  for ( i = m_buttonlist.begin(); i != m_buttonlist.end(); ++i ){
    if( (*i) != 0 && (*i) != button ){
      (*i)->resetValue();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* remove --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiRadioButton::GuiRadioBox::remove( GuiRadioButton *button ){
BUG(BugGui,"GuiRadioButton::GuiRadioBox::remove");
  assert( button != 0 );
  GuiRadioButtonList::iterator i;
  for ( i = m_buttonlist.begin(); i != m_buttonlist.end(); ++i ){
    if( (*i) == button ){
      m_buttonlist.erase( i );
    }
  }
  if( m_buttonlist.empty() ){
    delete this;
  }
}
