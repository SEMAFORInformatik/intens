
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qsettings.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <QKeyEvent>
#include <QGridLayout>
#include <QComboBox>

#include "utils/utils.h"
#include "utils/StringUtils.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtButton.h"
#include "gui/qt/QtDialogUserPassword.h"
#include "gui/qt/QtDialogInformation.h"
#include "gui/qt/QtMultiFontString.h"
#include "app/App.h"
#include "gui/GuiFactory.h"

INIT_LOGGER();

class MyDialog : public QDialog {
public:
  MyDialog(QtDialogUserPassword* parent)
    : QDialog(GuiQtManager::Instance().Toplevel(), Qt::Dialog)
    , m_parent( parent )
{
}

  ~MyDialog(){}
protected:
  virtual void closeEvent( QCloseEvent* e){
    m_parent->cancelButtonPressed();
  }

  // to catch esc key event
  void reject() {
    m_parent->cancelButtonPressed();
  }
private:
  QtDialogUserPassword* m_parent;
};

class MyDUPQLineEdit : public QLineEdit {
public:
  MyDUPQLineEdit( QWidget* parent )
    : QLineEdit( parent ) {}
private:
  virtual void keyPressEvent ( QKeyEvent *e ) {
    switch (e->key()) {
      case Qt::Key_Enter:
      case Qt::Key_Return:
	focusNextChild();
	e->accept();
	return;
    }
    QLineEdit::keyPressEvent( e );
  }
};

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

QtDialogUserPassword* QtDialogUserPassword::Instance(UserPasswordListener *listener
                                                     , DialogUnmapListener *unmap){
  if(listener) {
    if(s_instance == 0)
      s_instance = new QtDialogUserPassword(listener, unmap);
    else
      s_instance->resetListener(listener, unmap);
  }
  return dynamic_cast<QtDialogUserPassword*>(s_instance);
}

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

QtDialogUserPassword::QtDialogUserPassword(UserPasswordListener *listener
                                           , DialogUnmapListener *unmap)
  : m_field_length( 0 )
    , m_password_ok( this )
    , m_password_cancel( this )
    , m_listener( listener )
    , m_unmap( unmap )
    , m_errorMessage("")
    , m_dbLabel(0)
    , m_userLabel(0)
    , m_pwLabel(0)
    , m_db_list_w(0)
    , m_db_w(0)
    , m_user_list_w(0)
    , m_user_w(0)
    , m_password_w(0)
    , m_errorLabel(0)
    , userDialog(0)
    , m_timerId(-1)
{
  listener->setDialog( this );
}

QtDialogUserPassword::~QtDialogUserPassword(){
  if (userDialog)
    delete userDialog;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
/* --------------------------------------------------------------------------- */
/* isShown --                                                                  */
/* --------------------------------------------------------------------------- */
bool QtDialogUserPassword::isShown() {
  return userDialog ? userDialog->isVisible() : false;
}

/* --------------------------------------------------------------------------- */
/* showDialog --                                                               */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::showDialog( const std::string &connect,
                                       const std::string &username,
                                       std::string errorMessage ){

  if( !connect.empty() ){
    setDbConnection( connect );
  }
  if( !username.empty() ){
    setUsername( username );
  }
  updateDialog(errorMessage);
  if (isShown()) return;

  setParentDialog( GuiQtManager::Instance().getParentDialog( 0 ) );
  create();
  userDialog->setMaximumSize(userDialog->sizeHint());
  manage();
  m_errorLabel->setVisible(m_errorMessage.size() > 0 ? true : false);

  // NetworkAuth
  auto appData = AppData::Instance();
  if (!appData.OAuth().empty()) {
    BUG_INFO("DialogUserPassword::showDialog NetworkAuth: " << appData.OAuth());

    // hide widgets
    hideDataWidgets();
    updateDialog(_("Sign in to your account "));
    App::Instance().runOAuthClient(m_listener);
  }
  // DashboardURL
  if (!appData.DashboardURL().empty()) {
    BUG_INFO("DialogUserPassword::showDialog DashboardURL: " << appData.DashboardURL());

    // hide widgets
    hideDataWidgets();
    updateDialog(_("Sign in to your account "));
    App::Instance().runDashboardClient(m_listener);
    m_timerId = startTimer(1000);
  }

  GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( false );
  GuiManager::Instance().attachEventLoopListener( loopcontrol );
  userDialog->exec();
  loopcontrol->goAway();
}

/* --------------------------------------------------------------------------- */
/* updateDialog --                                                             */
/* --------------------------------------------------------------------------- */
void QtDialogUserPassword::updateDialog(const std::string& errorMessage){
  m_errorMessage = errorMessage;
  trim(m_errorMessage);
  if (m_errorLabel) {
    m_errorLabel->setText(QString::fromStdString(m_errorMessage));
    m_errorLabel->setVisible(m_errorMessage.size() > 0 ? true : false);
  }
  if (m_password_w) {
    m_password_w->setText("");
  }
}

/* --------------------------------------------------------------------------- */
/* invalidPassword --                                                          */
/* --------------------------------------------------------------------------- */
void QtDialogUserPassword::invalidPassword(){
  std::string message( compose(_("Cannot connect to database '%1' as user '%2'\n"),m_dbconnect,m_username) );

  QtDialogInformation::showDialog( 0, "Database connection", message, 0 );
}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
void QtDialogUserPassword::resetListener( UserPasswordListener *listener, DialogUnmapListener *unmap) {
  m_listener = listener;
  m_listener->setDialog( this );
  m_unmap = unmap;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::create(){
  userDialog = new MyDialog(this);
  std::string title = AppData::Instance().Title() + "  " + _("Login");
  userDialog->setWindowTitle( QString::fromStdString(title) );

  // error label
  m_errorLabel = new QLabel(QString::fromStdString(m_errorMessage), userDialog);
  QFont font = m_errorLabel->font();
  font = QtMultiFontString::getQFont( "@label@", font );
  m_errorLabel->setFont( font );
  m_errorLabel->setStyleSheet("QLabel { font-weight: bold; color : red; }");


  if (AppData::Instance().OAuth().empty()) {
    // The strings should be taken from a language resource file
    m_dbLabel = new QLabel(_("Database"), userDialog);
    m_userLabel = new QLabel(_("Username"), userDialog);
    m_pwLabel = new QLabel(_("Password"), userDialog);

    // set font
    m_dbLabel->setFont( font );
    m_userLabel->setFont( font );
    m_pwLabel->setFont( font );

    if ( m_dbconnect_list.empty() ) {
      m_db_w = new MyDUPQLineEdit(userDialog);
    } else {
      m_db_list_w = new QComboBox(userDialog);
    }
    if ( m_username_list.empty() ) {
      m_user_w = new MyDUPQLineEdit(userDialog);
    } else {
      m_user_list_w = new QComboBox(userDialog);
    }
    m_password_w = new MyDUPQLineEdit(userDialog);
    m_password_w -> setEchoMode(QLineEdit::Password);

    // get font
    font = m_password_w->font();
    font = QtMultiFontString::getQFont( "@text@", font );

    // set font
    if ( m_dbconnect_list.empty() ) {
      m_db_w->setFont( font );
    } else {
      m_db_list_w->setFont( font );
    }
    if ( m_username_list.empty() ) {
      m_user_w->setFont( font );
    } else {
      m_user_list_w->setFont( font );
    }
    m_password_w->setFont( font );
  }

  QPushButton *okButton = new QPushButton(_("OK"), userDialog);
  QFont font_btn = okButton->font();
  font_btn = QtMultiFontString::getQFont( "@button@", font_btn );
  okButton->setFont( font_btn );
  okButton->setAutoDefault( true );
  okButton->setDefault( true );
  QObject::connect( okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));

  ///  QPushButton *cancelButton = new QPushButton(_("CancelXXXX"), userDialog);
  QPushButton *cancelButton = 0;
  if (AppData::Instance().OAuth().empty()) {
    cancelButton = new QPushButton(_("Cancel"), userDialog);
    cancelButton->setFont( font_btn );
    cancelButton->setAutoDefault( true );
    cancelButton->setDefault( true );
    QObject::connect( cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));

    int len = 0;
    if ( m_dbconnect_list.empty() ) {
      m_db_w->setText( QString::fromStdString(m_dbconnect) );
      len = std::max(QFontMetrics(font).horizontalAdvance( QString::fromStdString(m_dbconnect) ), len);
    } else if (m_db_list_w) {
      // add urls to combobox
      for ( std::vector<std::string>::iterator it = m_dbconnect_list.begin();
            it != m_dbconnect_list.end(); ++it ) {
        // should this entry be selected?
        bool select = false;
        if ( (*it)[0] == '*' ) {
          (*it).erase(0,1);
          select = true;
        }
        m_db_list_w->addItem( QString::fromStdString(*it) );
        if ( select ) {
          m_db_list_w->setCurrentIndex(it-m_dbconnect_list.begin());
        }
        len = std::max(QFontMetrics(font).horizontalAdvance( QString::fromStdString(*it) ), len);
      }
    }

    if ( m_username_list.empty() ) {
      m_user_w->setText( QString::fromStdString(m_username) );
      len = std::max(QFontMetrics(font).horizontalAdvance( QString::fromStdString(m_username) ), len);
    } else {
      // add usernames to combobox
      for ( std::vector<std::string>::iterator it = m_username_list.begin();
            it != m_username_list.end(); ++it ) {
        // should this entry be selected?
        bool select = false;
        if ( (*it)[0] == '*' ) {
          (*it).erase(0,1);
          select = true;
        }
        m_user_list_w->addItem( QString::fromStdString(*it) );
        if ( select ) {
          m_user_list_w->setCurrentIndex(it-m_username_list.begin());
        }
        len = std::max(QFontMetrics(font).horizontalAdvance( QString::fromStdString(*it) ), len);
      }
    }

    len = std::max(len+10, 180);
    if ( m_dbconnect_list.empty() ) {
      m_db_w->setMinimumWidth(len);
    } else if (m_db_list_w) {
      m_db_list_w->setMinimumWidth(len);
    }
    if ( m_username_list.empty() ) {
      m_user_w->setMinimumWidth(len);
    } else {
      m_user_list_w->setMinimumWidth(len);
    }
    m_password_w->setMinimumWidth(len);
  }

  // Layout gui components
  QGridLayout* layout = new QGridLayout();
  layout->setSpacing( 10 );
  layout->setContentsMargins(10,10,10,10);

  unsigned int row = 0;
  // error
  if ( m_errorLabel != 0 ) {
    layout->addWidget(m_errorLabel, row++, 0, 1, -1);
  }

  // direct rest service
  if (AppData::Instance().OAuth().empty()) {
    layout->addWidget(m_dbLabel, row, 0);
    if ( m_dbconnect_list.empty() ) {
      layout->addWidget(m_db_w, row++, 1);
    } else {
      layout->addWidget(m_db_list_w, row++, 1);
    }

    // user
    layout->addWidget(m_userLabel, row, 0);
    if ( m_username_list.empty() ) {
      layout->addWidget(m_user_w, row++, 1);
    } else {
      layout->addWidget(m_user_list_w, row++, 1);
    }

    // password
    layout->addWidget(m_pwLabel, row, 0);
    layout->addWidget(m_password_w, row++, 1);

  }
  // buttons
  layout->addWidget(okButton, row, 0);
  if (cancelButton)
    layout->addWidget(cancelButton, row++, 1);

  userDialog->setLayout( layout );

  userDialog -> setModal( true );
}

/* --------------------------------------------------------------------------- */
/* hideDataWidgets --                                                          */
/* --------------------------------------------------------------------------- */
void QtDialogUserPassword::hideDataWidgets(){
  // hide widgets
  if (m_dbLabel) m_dbLabel->hide();
  if (m_userLabel) m_userLabel->hide();
  if (m_pwLabel) m_pwLabel->hide();
  if (m_db_list_w) m_db_list_w->hide();
  if (m_db_w) m_db_w->hide();
  if (m_user_list_w) m_user_list_w->hide();
  if (m_user_w) m_user_w->hide();
  if (m_password_w) m_password_w->hide();
}

/* --------------------------------------------------------------------------- */
/* okButtonPressed --                                                          */
/* --------------------------------------------------------------------------- */

bool QtDialogUserPassword::okButtonPressed(){
  m_password_ok.disallow();
  assert( m_listener != 0 );

  if ( m_dbconnect_list.empty() ) {
    m_dbconnect = m_db_w -> text().toStdString();
  } else {
    m_dbconnect = m_db_list_w->currentText().toStdString();
  }
  if ( m_username_list.empty() ) {
    m_username = m_user_w -> text().toStdString();
  } else {
    m_username = m_user_list_w->currentText().toStdString();
  }
  m_password = m_password_w -> text().toStdString();

  if( m_password.empty() ){
    m_password = "<none>";
  }

  if (m_listener -> okButtonPressed( m_dbconnect, m_username, m_password )){
    unmanage();
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* cancelButtonPressed --                                                      */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::cancelButtonPressed(){
  m_listener -> cancelButtonPressed();
  unmanage();
  if (m_timerId){
    killTimer( m_timerId );
    m_timerId = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::manage(){
  if (userDialog)
    userDialog->show();

  if ( m_dbconnect_list.empty() && m_db_w &&
       m_db_w->text().isEmpty() )
    m_db_w->setFocus();
  else
    if (m_username_list.empty() &&
        m_user_w &&
        m_user_w->text().isEmpty() )
      m_user_w->setFocus();
    else if (m_password_w)
      m_password_w->setFocus();
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::unmanage(){
  if (userDialog) {
    userDialog->hide();
  }
  if( m_unmap != 0 ){
    m_unmap->DialogIsUnmapped();
  }
}

/* --------------------------------------------------------------------------- */
/* setDbConnection --                                                          */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::setDbConnection( const std::string &conn ) {
  m_dbconnect_list.clear();
  m_dbconnect.clear();

  if ( conn.find(";") != std::string::npos ) {
    // split conn
    char delim = ';';
    std::stringstream ss(conn);
    std::string url;
    while (std::getline(ss, url, delim)) {
      m_dbconnect_list.push_back( url );
    }
    if ( !m_dbconnect_list.empty() ) {
      m_dbconnect = m_dbconnect_list[0];
    }
  } else {
    m_dbconnect = conn;
  }

  if (m_dbconnect_list.empty() && m_db_w) {
    m_db_w->setText(QString::fromStdString(conn));
  }
}

/* --------------------------------------------------------------------------- */
/* setUsername --                                                              */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::setUsername( const std::string &user ) {
  m_username_list.clear();
  m_username.clear();

  char delim = ';';
  if ( user.find(delim) != std::string::npos ) {
    // split user
    std::stringstream ss(user);
    std::string username;
    while (std::getline(ss, username, delim)) {
      m_username_list.push_back( username );
    }
    if ( !m_username_list.empty() ) {
      m_username = m_username_list[0];
    }
  } else {
    m_username = user;
  }
  if (m_username_list.empty() && m_user_w) {
    m_user_w->setText(QString::fromStdString(user));
  }
}

/* --------------------------------------------------------------------------- */
/* timerEvent --                                                               */
/* --------------------------------------------------------------------------- */

void QtDialogUserPassword::timerEvent(QTimerEvent *event){
  if( m_timerId == event->timerId() ){
    if (okButtonPressed()){
      killTimer( m_timerId );
      m_timerId = 0;
    }
    return;
  }
}
