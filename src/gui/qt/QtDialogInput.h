
#if !defined(QT_DIALOG_INPUT_H)
#define QT_DIALOG_INPUT_H

#include "gui/ConfirmationListener.h"
#include "gui/GuiElement.h"

class QtDialogInput
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtDialogInput( ConfirmationListener *listener );
  virtual ~QtDialogInput() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static std::string getText(GuiElement *parent,
			     const std::string &caption, const std::string &message,
			     const std::string &label, ConfirmationListener *, bool& ok);

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

};

#endif
