
#include <QApplication>
#include <QGridLayout>
#include <QScreen>
#include <QRegularExpression>
#if QT_VERSION < 0x060400
#include <QDesktopWidget>
#endif
#include <QScrollBar>
#include <QPushButton>
#include <QLabel>
#include <QCloseEvent>

#include "gui/qt/QtMessageBoxExt.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/gettext.h"


QString QtMessageBoxExt::getButtonLabel(GuiElement::ButtonType buttonType,
                                        const char *default_label,
                                        const std::map<GuiElement::ButtonType, std::string>& buttonText) {
  std::map<GuiElement::ButtonType, std::string>::const_iterator it = buttonText.find(buttonType);
  if(it != buttonText.end())
    return QString::fromStdString(it->second);
  return QString::fromStdString(default_label);
}

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
QtMessageBoxExt::QtMessageBoxExt( const QString& caption
			    , const QString& text
			    , const char* dialog_icon
			    , QMessageBox::StandardButtons buttons
			    , const std::map<GuiElement::ButtonType, std::string>& buttonText
			    , QWidget *parent )
  : QDialog(parent)
  ,  m_msg(text.toStdString()), m_textEdit(0) {

  // 2015-02-13 QtBug wrap funktioniert im pre-Tag leider nicht
  if (text.contains("<pre>", Qt::CaseInsensitive)) {
    QString qstr(text);
    qstr.replace("<pre>", "<pre style=\"white-space:normal;\">", Qt::CaseInsensitive);
    m_msg = qstr.toStdString();
  }

  //
  setWindowTitle( caption );
  setWindowFlags( windowFlags()^Qt::WindowContextHelpButtonHint);

  QFont font = QDialog::font();
  setFont( QtMultiFontString::getQFont( "@messagebox@", font ) );

  QGridLayout *grLayout = new QGridLayout;
  setLayout(grLayout);

  // textEdit
  m_textEdit = new QTextEdit( );
  m_textEdit->setFocusPolicy(Qt::NoFocus);
  grLayout->addWidget( m_textEdit, 0, 1, 1, -1);
  setMultiFontMsg();
  m_textEdit->document()->adjustSize();

  // ok button
  grLayout->setColumnStretch(1,10);
  int btnCnt = 2;
  if (buttons & QMessageBox::Ok) {
    QPushButton* okButton = new QPushButton(_("&OK"));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-ok") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_OkButtonPressed()));
  }

  // yes button
  if (buttons & QMessageBox::Yes) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_Yes,
                                                           _("&Yes"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-ok") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_YesButtonPressed()));
  }

  // save button
  if (buttons & QMessageBox::Save) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_Save,
                                                           _("&Save"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-ok") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_SaveButtonPressed()));
  }

  // no button
  if (buttons & QMessageBox::No) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_No,
                                                           _("&No"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-cancel") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_NoButtonPressed()));
  }

  // discard button
  if (buttons & QMessageBox::Discard) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_Discard,
                                                           _("&Discard"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-cancel") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_DiscardButtonPressed()));
  }

  // apply button
  if (buttons & QMessageBox::Apply) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_Apply,
                                                           _("&Apply"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-ok") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_ApplyButtonPressed()));
  }

  // open button
  if (buttons & QMessageBox::Open) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_Open,
                                                           _("&Open"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-ok") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_OpenButtonPressed()));
  }

  // cancel button
  if (buttons & QMessageBox::Cancel) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_Cancel,
                                                           _("&Cancel"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-cancel") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_CancelButtonPressed()));
  }

  // abort button
  if (buttons & QMessageBox::Abort) {
    QPushButton* okButton = new QPushButton(getButtonLabel(GuiElement::button_Abort,
                                                           _("&Abort"), buttonText));
    okButton->setCheckable(true);
    okButton->setAutoDefault(false);
    okButton->setIcon( QIcon::fromTheme("dialog-cancel") );
    grLayout->addWidget( okButton, 1, btnCnt++, Qt::AlignRight);
    connect(okButton, SIGNAL(pressed()), this, SLOT(slot_AbortButtonPressed()));
  }

  // dialog icon
  if (dialog_icon) {
     QLabel* lbl = new QLabel();
     lbl->setPixmap( QIcon::fromTheme(dialog_icon).pixmap(50,50) );
     grLayout->addWidget( lbl, 0, 0, Qt::AlignTop);
  }

  // size properties
#if QT_VERSION > 0x050600
  QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#else
  QRect drect = QApplication::desktop()->availableGeometry();
#endif
  int wdiff = m_textEdit->verticalScrollBar()->isVisible() ? m_textEdit->verticalScrollBar()->width() : 16;
  int wsize = m_textEdit->document()->size().width();
  wsize += wdiff ;
  int delta=100;  // for frame, buttonbar, buttonbar
  if ((wsize+delta) > drect.width())
    wsize = drect.width()-delta;
  int hsize =  std::min( ((int)m_textEdit->document()->size().height()), m_textEdit->viewport()->size().height());
  hsize +=  wdiff ;
  if ((hsize+delta) > drect.height())
    hsize = drect.height()- delta;

  m_textEdit->setMinimumWidth(wsize);
  m_textEdit->setMinimumHeight(hsize);

#if QT_VERSION <= 0x060000
  // stylesheet
  QPalette pal=  QApplication::palette();
  m_textEdit->setStyleSheet(QString::fromStdString("QTextEdit { background-color: "+
						   pal.color(QPalette::Window).name().toStdString() +
						   "; color: "+
						   pal.color(QPalette::WindowText).name().toStdString()+" }"));
#endif
}


/* --------------------------------------------------------------------------- */
/* getParentWidget --                                                          */
/* --------------------------------------------------------------------------- */
QWidget* QtMessageBoxExt::getParentWidget(GuiElement* element) {
  QWidget *parent = qApp->activeWindow() ?
    qApp->activeWindow() : GuiQtManager::Instance().Toplevel();
  if (element){
    GuiQtElement *qe = element->getQtElement();
    parent = element->getQtElement() ? qe->myWidget() : parent;
  }
  return parent;
}

/* --------------------------------------------------------------------------- */
/* hasBrTag --                                                                 */
/* --------------------------------------------------------------------------- */
bool QtMessageBoxExt::hasBrTag(){
  if (m_msg.size())
    if( QString::fromStdString(m_msg).contains(QRegularExpression("<br[^>]*>")))
      return true;
    if( QString::fromStdString(m_msg).contains(QRegularExpression("<td[^>]*>")))
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* setMultiFontMsg --                                                             */
/* --------------------------------------------------------------------------- */
bool QtMessageBoxExt::setMultiFontMsg(){
  BUG(BugGui,"QtMessageBox::setMultiFontMsg");
  QFont font = m_textEdit->font();
  int last_font_size = font.pointSize();
  std::string str;
  std::stringstream ostr;

  std::string::size_type posA=0, posE=0, pos, lpos;
  bool openFontTag( false );
  bool openBoldTag( false );
  bool hasBr( this->hasBrTag() );
  BUG_MSG("hasBrTag["<<hasBr<<"]");

  while( (posA = m_msg.find_first_of('@', posE)) != std::string::npos) {

    str = m_msg.substr(posE, posA-posE);
    pos=0, lpos=0;
    while ( (pos = str.find_first_of('\n', pos)) != std::string::npos) {
      ostr << str.substr(lpos, pos-lpos) << "<br/>";
      ++pos;
      lpos = pos;
    }
    if (lpos<(str.size()-1))
      ostr << str.substr(lpos);

    if ((posE = m_msg.find_first_of('@', posA+1)) == std::string::npos) {
      m_textEdit->setHtml(QString::fromStdString(ostr.str()));
      return false; // Ende oder Falsch
    }

    std::string fs=m_msg.substr(posA, posE-posA+1);
    QFont newFont = QtMultiFontString::getQFont( fs, font );
    m_textEdit->setCurrentFont( QtMultiFontString::getQFont( fs, font ) );
    if (openBoldTag)
      ostr << "</b>";
    if (openBoldTag)
      ostr << "</font>";
    else openFontTag= true;
    int fsize = floor(0.5*(newFont.pointSize()-last_font_size));
    char pc = fsize>=0 ? '+' : '-';
    ostr << "<font face=\""<<newFont.family().toStdString()<<"\" size=\""<<pc<<abs(fsize)<<"\">";
    //    last_font_size = newFont.pointSize();
    if (newFont.bold()) {
      ostr << "<b>";
      openBoldTag = true;
    }
    else openBoldTag = false;
    ++posE;
  }

  // verarbeitung des Restes (nach letzten bzw. auch keinem font tag)
  str=m_msg.substr(posE);
  pos=0, lpos=0;
  while ( (pos = str.find_first_of('\n', pos)) != std::string::npos) {
    ostr << str.substr(lpos, pos-lpos);
    if (!hasBr)
      ostr << "<br/>";
    ++pos;
    lpos = pos;
  }
  if (lpos<(str.size()-1))
    ostr << str.substr(lpos);
  if (openBoldTag)
    ostr << "</b>";
  if (openFontTag)
    ostr << "</font>";
  //  setText("");
  m_textEdit->setHtml(QString::fromStdString(ostr.str()));
  return true;
}

/* --------------------------------------------------------------------------- */
/* exec --                                                                     */
/* --------------------------------------------------------------------------- */
int QtMessageBoxExt::exec() {
  // QMessageBox::exec looses activeWindow
  QWidget *activeWindow = GuiQtManager::Instance().getActiveWindow();

  int ret = QDialog::exec();

  // restore activeWindow
  if ( activeWindow )
    activeWindow->activateWindow();

  return ret;
}

/* --------------------------------------------------------------------------- */
/* slots --                                                                    */
/* --------------------------------------------------------------------------- */

void QtMessageBoxExt::slot_OkButtonPressed() {
  done(QMessageBox::Ok);
}

void QtMessageBoxExt::slot_YesButtonPressed() {
  done(QMessageBox::Yes);
}

void QtMessageBoxExt::slot_SaveButtonPressed() {
  done(QMessageBox::Save);
}

void QtMessageBoxExt::slot_NoButtonPressed() {
  done(QMessageBox::No);
}

void QtMessageBoxExt::slot_DiscardButtonPressed() {
  done(QMessageBox::Discard);
}

void QtMessageBoxExt::slot_ApplyButtonPressed() {
  done(QMessageBox::Apply);
}

void QtMessageBoxExt::slot_OpenButtonPressed() {
  done(QMessageBox::Open);
}

void QtMessageBoxExt::slot_CancelButtonPressed() {
  done(QMessageBox::Cancel);
}

void QtMessageBoxExt::slot_AbortButtonPressed() {
  done(QMessageBox::Abort);
}

void QtMessageBoxExt::closeEvent ( QCloseEvent * e )  {
  // close disabled
  if (e)
    e->ignore();
  QMessageBox::information( this, "Close dialog",
			    _("Unable to close dialog from title bar.") );
}

void QtMessageBoxExt::reject()  {
  closeEvent(NULL);
}
