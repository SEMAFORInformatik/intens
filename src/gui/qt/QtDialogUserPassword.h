
#if !defined(QT_DIALOGUSERPASSWORD_H)
#define QT_DIALOGUSERPASSWORD_H

#include <qobject.h>

#include "gui/DialogUserPassword.h"
#include "gui/qt/GuiQtDialog.h"
#include "gui/GuiButtonListener.h"

class GuiButtonbar;
class UserPasswordListener;
class QDialog;
class QLineEdit;
class QLabel;
class QComboBox;

class QtDialogUserPassword : public QObject, public GuiQtDialog, public DialogUserPassword
{

Q_OBJECT

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  static QtDialogUserPassword* Instance(UserPasswordListener *listener,
                                        DialogUnmapListener *unmap);

private:
  QtDialogUserPassword( UserPasswordListener *, DialogUnmapListener *);
  virtual ~QtDialogUserPassword();
  virtual QDialog* getDialogWidget() { return NULL; }
  virtual std::string getDialogName() { return "QtDialogUserPassword"; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void showDialog(const std::string &, const std::string &, std::string errorMessage="");

  virtual bool isShown();
  virtual void updateDialog(const std::string& errorMessage);
  virtual void invalidPassword();
  virtual void setWaitCursorState( bool state ){}
  virtual void resetListener( UserPasswordListener *, DialogUnmapListener *);

public slots:
  virtual void okButtonPressed();
  virtual void cancelButtonPressed();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void setTitle( const std::string &title ) { m_title = title; }
  void setDbConnection( const std::string &conn );
  void setUsername( const std::string &user );
  virtual void create();

protected:
  virtual void manage();
  virtual void unmanage();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  std::string             m_title;
  std::string             m_dbconnect;
  std::vector<std::string> m_dbconnect_list;
  std::string             m_username;
  std::vector<std::string> m_username_list;
  std::string             m_password;
  std::string             m_errorMessage;
  QComboBox*              m_db_list_w;
  QLineEdit*              m_db_w;
  QComboBox*              m_user_list_w;
  QLineEdit*              m_user_w;
  QLineEdit*              m_password_w;
  QLabel*                 m_errorLabel;
  int                     m_field_length;
  PasswordOkListener      m_password_ok;
  PasswordCancelListener  m_password_cancel;
  UserPasswordListener   *m_listener;
  QDialog                *userDialog;
  DialogUnmapListener    *m_unmap;

};

#endif
