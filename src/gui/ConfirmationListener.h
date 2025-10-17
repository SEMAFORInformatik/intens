
#if !defined(CONFIRMATION_LISTENER_H)
#define CONFIRMATION_LISTENER_H

class ConfirmationListener
{
/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed() = 0;
  virtual void confirmNoButtonPressed() = 0;
  virtual void confirmCancelButtonPressed() {}
};

#endif
