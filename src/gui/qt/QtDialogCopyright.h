
#if !defined(QT_DIALOG_COPYRIGHT_H)
#define QT_DIALOG_COPYRIGHT_H

#include "gui/DialogCopyright.h"
#include "gui/GuiButtonListener.h"

class QtDialogCopyright : public DialogCopyright, public GuiButtonListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtDialogCopyright();
  virtual ~QtDialogCopyright();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  JobAction* getAction(){ return 0; }
  virtual void ButtonPressed();
  virtual void manage();
  virtual void unmanage();
  virtual GuiButtonListener * getElement() { return this; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void create();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};


#endif
