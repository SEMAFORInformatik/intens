
#include "utils/Debugger.h"
#include "gui/GuiDialog.h"
#include "gui/GuiForm.h"
#include "gui/GuiFactory.h"
#include "app/AppData.h"

INIT_LOGGER();

GuiDialog::GuiDialogStack   GuiDialog::s_dialog_stack;

/* --------------------------------------------------------------------------- */
/* setApplicationModal --                                                      */
/* --------------------------------------------------------------------------- */

void GuiDialog::setApplicationModal(){
  BUG_DEBUG("setApplicationModal");
  m_application_modal = true;
}

/* --------------------------------------------------------------------------- */
/* resetApplicationModal --                                                    */
/* --------------------------------------------------------------------------- */

void GuiDialog::resetApplicationModal(){
  BUG_INFO("resetApplicationModal");
  m_application_modal = false;
}

/* --------------------------------------------------------------------------- */
/* setDialogTitle --                                                           */
/* --------------------------------------------------------------------------- */

void GuiDialog::setDialogTitle( const std::string &text ){
  m_title = text;
}

/* --------------------------------------------------------------------------- */
/* getDialogTitle --                                                           */
/* --------------------------------------------------------------------------- */

const std::string& GuiDialog::getDialogTitle() {
  return m_title;
}

/* --------------------------------------------------------------------------- */
/* removeParentWaitCursor --                                                   */
/* --------------------------------------------------------------------------- */

void GuiDialog::removeParentWaitCursor( GuiDialog *installer ){
  if( m_parent_dialog != 0 ){
    // Wenn kein Parent vorhanden ist, passiert einfach nichts.
    m_parent_dialog->removeWaitCursor( installer );
  }
}

/* --------------------------------------------------------------------------- */
/* installParentWaitCursor --                                                  */
/* --------------------------------------------------------------------------- */

void GuiDialog::installParentWaitCursor( GuiDialog *installer ){
  if( m_parent_dialog != 0 ){
    // Wenn kein Parent vorhanden ist, passiert einfach nichts.
    m_parent_dialog->installWaitCursor( installer );
  }
}

/* --------------------------------------------------------------------------- */
/* setParentDialog --                                                          */
/* --------------------------------------------------------------------------- */

void GuiDialog::setParentDialog( GuiDialog *dialog ){
  m_parent_dialog = dialog;
}

/* --------------------------------------------------------------------------- */
/* pushParentDialog --                                                         */
/* --------------------------------------------------------------------------- */

void GuiDialog::pushParentDialog( GuiDialog *dialog ){
  BUG_PARA(BugGui,"GuiDialog::pushParentDialog"
	   ,"GuiDialog '" << dialog->getDialogTitle() << "' (" << dialog << ")" );

  s_dialog_stack.push( dialog );
}

/* --------------------------------------------------------------------------- */
/* popParentDialog --                                                          */
/* --------------------------------------------------------------------------- */

void GuiDialog::popParentDialog( GuiDialog *dialog ){
  BUG_PARA(BugGui,"GuiDialog::popParentDialog"
	   ,"GuiDialog '" << dialog->getDialogTitle() << "' (" << dialog << ")" );

  assert( !s_dialog_stack.empty() );

  GuiDialog *d = s_dialog_stack.top();
  s_dialog_stack.pop();
  if( d ){
    BUG_MSG("d '" << d->getDialogTitle() << "'" );
  }
  assert( d == dialog );
}

/* --------------------------------------------------------------------------- */
/* getParentDialog --                                                          */
/* --------------------------------------------------------------------------- */

GuiDialog *GuiDialog::getParentDialog( GuiElement *el ){
  BUG_PARA(BugGui,"GuiDialog::getParentDialog","GuiElement=" << el);
  GuiDialog *dialog = 0;
  if( s_dialog_stack.empty() ){
    BUG_MSG("stack is empty");
    if( el == 0 ){
      GuiForm *form = GuiElement::getMainForm();
      dialog = form->getDialog();
      BUG_MSG("get main form: " << dialog);
    }
    else{
      dialog = el->getDialog();
      BUG_MSG("get my form: " << dialog);
    }
  }
  else{
    dialog = s_dialog_stack.top();
    BUG_MSG("get from stack: " << dialog);
  }
  if( dialog ){
    BUG_EXIT("dialog '" << dialog->getDialogTitle() << "'" );
  }
  return dialog;
}

/* --------------------------------------------------------------------------- */
/* removeWaitCursor --                                                         */
/* --------------------------------------------------------------------------- */

void GuiDialog::removeWaitCursor( GuiDialog *installer ){
  BUG_DEBUG("removeWaitCursor");

  if( m_wait_installer != installer ){
    BUG_DEBUG("isnt installer of Wait Cursor (" << m_wait_installer << ")");

    if( m_wait_installer != 0 ){
      return;
    }
  }
  setWaitCursorState( false );
  if( parentDialog() != 0 && !isApplicationModal() ){
    BUG_DEBUG("remove WaitCursor");
    parentDialog()->removeWaitCursor( installer );
  }
  m_wait_installer = 0;
}

/* --------------------------------------------------------------------------- */
/* installWaitCursor --                                                        */
/* --------------------------------------------------------------------------- */

void GuiDialog::installWaitCursor( GuiDialog *installer ){
  assert( installer != 0 );
  BUG_DEBUG("installWaitCursor");

  if( m_wait_installer != 0 ){
    BUG_DEBUG("Wait Cursor already installed (" << m_wait_installer << ")");
    return;
  }
  setWaitCursorState( true );
  if( parentDialog() != 0 && !isApplicationModal() ){
    BUG_DEBUG("install WaitCursor");
    parentDialog()->installWaitCursor( installer );
  }
  m_wait_installer = installer;
}
