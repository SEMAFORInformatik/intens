#include <QApplication>
#include <QBoxLayout>
#if QT_VERSION > 0x050600
#include <QScreen>
#else
#include <QDesktopWidget>
#endif
#include <QScrollBar>

#include "gui/qt/QtInputDialog.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/gettext.h"


/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
QtInputDialog::QtInputDialog( const QString& caption
			    , const QString& textMessage
			    , const QString& textLabel
			    , QWidget *parent )
  : QInputDialog( parent )
  ,  m_msg(textMessage.toStdString()), m_label(textLabel), m_textEdit(0) {
  setWindowTitle(caption);
  // set default message label
  if (textLabel.size())
    setLabelText(textMessage + "\n\n" + textLabel);
  else
    setLabelText(textMessage);
  setInputMode( QInputDialog::TextInput );
  init();
}


/* --------------------------------------------------------------------------- */
/* getParentWidget --                                                          */
/* --------------------------------------------------------------------------- */
QWidget* QtInputDialog::getParentWidget(GuiElement* element) {
  QWidget *parent = qApp->activeWindow() ?
    qApp->activeWindow() : GuiQtManager::Instance().Toplevel();
  if (element){
    GuiQtElement *qe = element->getQtElement();
    parent = element->getQtElement() ? qe->myWidget() : parent;
  }
  return parent;
}

/* --------------------------------------------------------------------------- */
/* init --                                                                     */
/* --------------------------------------------------------------------------- */
void QtInputDialog::init(){

  setOkButtonText( _("OK") );
  setCancelButtonText( _("Cancel") );
  QFont font = QInputDialog::font();
  setFont( QtMultiFontString::getQFont( "@messagebox@", font ) );

  QBoxLayout*   boxLayout = dynamic_cast<QBoxLayout* >(layout());
  if (m_msg.size() > 1000 ||
      (isMultiFont() && boxLayout) ) {

    m_textEdit = new QTextEdit( );//QString::fromStdString(message) );
    m_textEdit->setFocusPolicy(Qt::NoFocus);
    boxLayout->insertWidget( 0, m_textEdit, 111 );

    setMultiFontMsg();
    m_textEdit->document()->adjustSize();
    m_textEdit->adjustSize();
#if QT_VERSION > 0x050600
    QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#else
    QRect drect = QApplication::desktop()->availableGeometry();
#endif
    int wdiff = m_textEdit->verticalScrollBar()->isVisible() ? m_textEdit->verticalScrollBar()->width() : 16;
    int wsize = std::max( ((int)m_textEdit->document()->size().width()), m_textEdit->viewport()->size().width());
    wsize += wdiff ;
    int delta=50;
    if ((wsize+delta) > drect.width())
      wsize = drect.width()-delta;
    int hsize =  std::min( ((int)m_textEdit->document()->size().height()), m_textEdit->viewport()->size().height());
    hsize +=  wdiff ;
    if ((hsize+delta) > drect.height())
      hsize = drect.height()- delta;

    // m_textEdit->setMinimumWidth(wsize);
    m_textEdit->setMinimumHeight(hsize);
    setSizePolicy( QSizePolicy(  QSizePolicy::Expanding,  QSizePolicy::Expanding )  );
  }
}

/* --------------------------------------------------------------------------- */
/* isMultiFont --                                                              */
/* --------------------------------------------------------------------------- */
bool QtInputDialog::isMultiFont(){
  if (m_msg.size())
    if( m_msg.find_first_of('@') != std::string::npos)
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* setMultiFontMsg --                                                             */
/* --------------------------------------------------------------------------- */
bool QtInputDialog::setMultiFontMsg(){
  QFont font = m_textEdit->font();
  int last_font_size = font.pointSize();
  std::string str;
  std::stringstream ostr;

  std::string::size_type posA=0, posE=0, pos, lpos;
  bool openFontTag( false );
  bool openBoldTag( false );
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
    int fsize = (int) floor(0.5*(newFont.pointSize()-last_font_size));
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
    ostr << str.substr(lpos, pos-lpos) << "<br/>";
    ++pos;
    lpos = pos;
  }
  if (lpos<(str.size()-1))
    ostr << str.substr(lpos);
  if (openBoldTag)
    ostr << "</b>";
  if (openFontTag)
    ostr << "</font>";
  setLabelText( m_label );
  m_textEdit->setHtml(QString::fromStdString(ostr.str()));
  return true;
}

/* --------------------------------------------------------------------------- */
/* exec --                                                             */
/* --------------------------------------------------------------------------- */
int QtInputDialog::exec() {
  // QInputDialog::exec looses activeWindow
  QWidget *activeWindow = GuiQtManager::Instance().getActiveWindow();

  int ret = QInputDialog::exec();

  // restore activeWindow
  if ( activeWindow )
    activeWindow->activateWindow();

  return ret;
}
