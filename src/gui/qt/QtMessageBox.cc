
#include <QApplication>
#include <QGridLayout>
#include <QScreen>
#include <QRegularExpression>
#if QT_VERSION < 0x060000
#include <QDesktopWidget>
#endif
#include <QScrollBar>

#include "gui/qt/QtMessageBox.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/gettext.h"


/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
QtMessageBox::QtMessageBox( const QString& caption
			    , const QString& text
			    , Icon icon
			    , QMessageBox::StandardButtons buttons
			    , QWidget *parent )
  // Neue Syntax, beachte setzen des default button fehlt noch
  // : QMessageBox( icon, caption, text,
  // 		 (QMessageBox::StandardButtons)(button0 | button1 | button2), parent )
  : QMessageBox( icon, caption, text,
		 buttons, parent )
  ,  m_msg(text.toStdString()), m_textEdit(0) {
  init();
}


/* --------------------------------------------------------------------------- */
/* getParentWidget --                                                          */
/* --------------------------------------------------------------------------- */
QWidget* QtMessageBox::getParentWidget(GuiElement* element) {
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
void QtMessageBox::init(){

  QFont font = QMessageBox::font();
  setFont( QtMultiFontString::getQFont( "@messagebox@", font ) );

  QGridLayout* grLayout = dynamic_cast<QGridLayout* >(layout());
  if (m_msg.size() > 1000 ||
      (isMultiFont() && grLayout) ) {

    m_textEdit = new QTextEdit( );//QString::fromStdString(message) );
    m_textEdit->setFocusPolicy(Qt::NoFocus);
    grLayout->addWidget( m_textEdit, 0, 1 );

    setMultiFontMsg();
    m_textEdit->document()->adjustSize();
    m_textEdit->adjustSize();
#if QT_VERSION > 0x050600
    QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#else
    QRect drect = QApplication::desktop()->availableGeometry();
#endif
    int wdiff = m_textEdit->verticalScrollBar()->isVisible() ? m_textEdit->verticalScrollBar()->width() : 16;
    int wsize = m_textEdit->document()->size().width();
    wsize += wdiff ;
    int delta=100;
    if ((wsize+delta) > drect.width())
      wsize = drect.width()-delta;
    int hsize =  std::min( ((int)m_textEdit->document()->size().height()), m_textEdit->viewport()->size().height());
    hsize +=  wdiff ;
    if ((hsize+delta) > drect.height())
      hsize = drect.height()- delta;

    m_textEdit->setMinimumWidth(wsize > 200 ? wsize-200 : wsize);  // irgendwie sind zirka 200 zu viel
    m_textEdit->setMinimumHeight(hsize);

    m_textEdit->setSizePolicy( QSizePolicy(  QSizePolicy::Expanding,  QSizePolicy::Expanding )  );
    //    m_textEdit->setHtml(QString::fromStdString(m_msg));
    // nur mit stylesheet ist der background zu setzen
    QPalette pal=  QApplication::palette();
    m_textEdit->setStyleSheet(QString::fromStdString("QTextEdit { background-color: "+pal.color(QPalette::Window).name().toStdString()+
    "; color: "+pal.color(QPalette::WindowText).name().toStdString()+" }"));
  }
  else
    setText(QtMultiFontString::getQString(m_msg));
}

/* --------------------------------------------------------------------------- */
/* isMultiFont --                                                              */
/* --------------------------------------------------------------------------- */
bool QtMessageBox::isMultiFont(){
  if (m_msg.size())
    if( m_msg.find_first_of('@') != std::string::npos)
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasBrTag --                                                                 */
/* --------------------------------------------------------------------------- */
bool QtMessageBox::hasBrTag(){
  if (m_msg.size()) {
    if( QString::fromStdString(m_msg).contains(QRegularExpression("<br[^>,^\\s]*>")))
      return true;
    if( QString::fromStdString(m_msg).contains(QRegularExpression("<BR[^>,^\\s]*>")))
      return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setMultiFontMsg --                                                             */
/* --------------------------------------------------------------------------- */
bool QtMessageBox::setMultiFontMsg(){
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
  setText("");
  m_textEdit->setHtml(QString::fromStdString(ostr.str()));
  return true;
}

/* --------------------------------------------------------------------------- */
/* exec --                                                             */
/* --------------------------------------------------------------------------- */
int QtMessageBox::exec() {
  // QMessageBox::exec looses activeWindow
  QWidget *activeWindow = qApp->activeWindow();

  int ret = QMessageBox::exec();

  // restore activeWindow
  if ( activeWindow )
    activeWindow->activateWindow();

  return ret;
}
