
#if !defined(DIALOGUSERPASSWORD_H)
#define DIALOGUSERPASSWORD_H

#include "gui/GuiButtonListener.h"

class GuiButtonbar;
class UserPasswordListener;
class DialogUnmapListener;

class DialogUserPassword
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  static DialogUserPassword *Instance() { return s_instance; }

protected:
  DialogUserPassword();
  virtual ~DialogUserPassword();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool isShown() = 0;
  virtual void updateDialog(const std::string& errorMessage) = 0;
  virtual void invalidPassword() = 0;
  virtual void unmanage() = 0;
  virtual void okButtonPressed() = 0;
  virtual void cancelButtonPressed() = 0;
  virtual void resetListener( UserPasswordListener *, DialogUnmapListener *) = 0;

/*=============================================================================*/
/* private classes                                                             */
/*=============================================================================*/
protected:
  class PasswordOkListener : public GuiButtonListener
  {
  public:
    PasswordOkListener( DialogUserPassword *d );
    virtual ~PasswordOkListener();
  public:
    virtual void ButtonPressed();
    JobAction* getAction(){ return 0; }
  private:
    DialogUserPassword *m_dialog;
  };

  class PasswordCancelListener : public GuiButtonListener
  {
  public:
    PasswordCancelListener( DialogUserPassword *d );
    virtual ~PasswordCancelListener();
    virtual void ButtonPressed();
    JobAction* getAction(){ return 0; }
  private:
    DialogUserPassword *m_dialog;
  };
 protected:
  static DialogUserPassword *s_instance;
};


class UserPasswordListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  UserPasswordListener(){}
  virtual ~UserPasswordListener(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool okButtonPressed( const std::string &, const std::string &, const std::string & ) = 0;
  virtual void cancelButtonPressed() = 0;
  void invalidPassword() { m_dialog->invalidPassword(); }
  void unmanageDialog() { if(m_dialog) m_dialog->unmanage();  }
  void setDialog( DialogUserPassword *d ) { m_dialog = d; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  DialogUserPassword *m_dialog;
};

#endif
