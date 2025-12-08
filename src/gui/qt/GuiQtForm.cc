
#include <qmainwindow.h>
#include <qlayout.h>
#include <QSplitter>
#include <QSettings>
#include <qdialog.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <QIcon>
#include <QScrollArea>
#include <QScrollBar>
#include <QLineEdit>
#include <QCloseEvent>
#if QT_VERSION < 0x060000
#include <QDesktopWidget>
#endif
#include <qapplication.h>
#include <qmessagebox.h>

/* System headers */
#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "app/AppData.h"
#include "job/JobManager.h"
#include "job/JobElement.h"
#include "job/JobController.h"

#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtForm.h"
#include "gui/GuiButton.h"
#include "gui/GuiMenubar.h"
#include "gui/GuiVoid.h"
#include "gui/qt/GuiQtButtonbar.h"
#include "gui/qt/GuiQtCycleButton.h"
#include "gui/qt/GuiQtMessageLine.h"
#include "gui/qt/QtIconManager.h"

INIT_LOGGER();

/*=============================================================================*/
/* public class Definitions GuiQtForm::QtDialog                                */
/*=============================================================================*/
GuiQtForm::QtDialog::QtDialog(GuiQtForm *form, QWidget * parent)
  : QDialog(parent, Qt::Window)
#if QT_VERSION < 0x060000
  , m_screenNumber(-1)
#else
  , m_screenNumber()
#endif
  , m_timerId(0)
  , m_form(form) {
}

#if defined Q_OS_WIN || defined Q_OS_CYGWIN
// for REPLACE
void GuiQtForm::QtDialog::setHintSize( int w, int h ) {
  m_hintSize_resize.setWidth ( w );
  m_hintSize_resize.setHeight( h );
}

void GuiQtForm::QtDialog::resizeEvent ( QResizeEvent * event ) {
  // trick for Windows
  if ( m_hintSize_resize.isValid() ) {
    QDialog::setMaximumSize(m_hintSize_resize);
    BUG_DEBUG( "Set Maximum Size to Hint Size: " << m_hintSize_resize.width() <<
               ", " << m_hintSize_resize.height() );
  }
  QDialog::resizeEvent(event);
  if ( m_hintSize_resize.isValid() )
    QDialog::setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    m_hintSize_resize = QSize();
  }
#endif

bool GuiQtForm::QtDialog::event( QEvent *event ){
  GuiManager &gm = GuiManager::Instance();
  // check WindowActivate
  if ( event->type() == QEvent::WindowActivate ) {
    m_form->runJobFunction( JobElement::cll_Activate );
  }
  // event loop
  gm.checkEventLoopListener();
  if( gm.grabButton() ){
    switch( event -> type() ){
      case QEvent::KeyPress:
      case QEvent::KeyRelease:
      case QEvent::MouseButtonPress:
      case QEvent::MouseButtonRelease:
        return false;
        break;
      default:
        break;
    }
  }
  return QDialog::event( event );
}

void GuiQtForm::QtDialog::moveEvent ( QMoveEvent * event ) {
  setMaximumSize();
  QDialog::moveEvent(event);
}

void GuiQtForm::QtDialog::showEvent ( QShowEvent * event ) {
  // if modal set focus to first QLineEdit (GuiTextfield)
  if (isModal()) {
    QList<QLineEdit *> all = findChildren<QLineEdit *>("GuiTextfield");
    for (int i=0; i < all.size(); ++i) {
      if (all.at(i)->isReadOnly() == false) {
        all.at(i)->setFocus();
        break;
      }
    }
  }

  // call reason-activate function
  m_form->runJobFunction( JobElement::cll_Activate );

  // Beim ersten Mal (manage) wird der Dialog zum Parent ausgerichtet
  // nur wenn die Position nicht im iniFile eingelesen wird
  if ( !m_form->m_firstShown &&
       m_form->readFromSettings() == false &&
       m_form->getParent() ) {
    QPoint center = frameGeometry().center();
    QPoint dlgRectCenter = rect().center();
    QPoint parentRectCenter = m_form->getParent()->getQtElement()->myWidget()->rect().center();
    QPoint centerPos = QPoint(parentRectCenter - dlgRectCenter);
    QPoint dlgGlobalPos = m_form->getParent()->getQtElement()->myWidget()->mapToGlobal(centerPos);
    BUG_DEBUG("CenterGlo: "<< center.x() << ", " << center.y());
    BUG_DEBUG("DlgRectCenter: "<< dlgRectCenter.x() << ", " << dlgRectCenter.y());
    BUG_DEBUG("parentCenter!!: "<< centerPos.x() << ", " << centerPos.y());
    BUG_DEBUG("newGlobalPos: "<< dlgGlobalPos.x() << ", " << dlgGlobalPos.y());
    //  BUG_DEBUG("isVisible: "<< isVisible);
    BUG_DEBUG("ParentRectCenter: "<< parentRectCenter.x() << ", " << parentRectCenter.y());
    move(dlgGlobalPos);
  }

  // save hint sizes
  if (m_form->m_expandPolicy == expand_Default) {
    m_hintSize = QWidget::sizeHint();
  }
  startTimerSetMaximumSize();
  event->accept();
}

void GuiQtForm::QtDialog::timerEvent ( QTimerEvent * event ) {
  killTimer(event->timerId());
  m_timerId = 0;
  setMaximumSize(true);
  m_form->m_firstShown = true;
}

void GuiQtForm::QtDialog::startTimerSetMaximumSize() {
  if (m_timerId == 0) {
    m_timerId = startTimer(200);
    BUG_INFO("START TIMER");
  }
  else BUG_INFO("already STARTed TIMER");
}

void GuiQtForm::QtDialog::setMaximumSize(bool force) {
#if QT_VERSION < 0x060000
  int screenNumber = QApplication::desktop()->screenNumber(this);
#else
  QString screenNumber = QGuiApplication::primaryScreen()->serialNumber();
#endif
  if (!force && m_screenNumber == screenNumber) {
    return;
  }
  m_screenNumber = screenNumber;

  QSize hsAlt = m_hintSize.expandedTo(QWidget::sizeHint());
  int w, h;
  m_form->getSize(w,h);
  QSize hs(w, h);
#if QT_VERSION < 0x060000
  QRect maxDesktop = QApplication::desktop()->availableGeometry(m_form->myWidget());
#else
  QRect maxDesktop = QGuiApplication::primaryScreen()->availableGeometry();
#endif
  GuiElement::Orientation dialog_exand_policy =  m_form->getDialogExpandPolicy();
  BUG_DEBUG("QtDialog::setMaximumSize Name: "<< m_form->getName());

  // nicht kleiner als von den settings gelesen
  int set_w, set_h;
  if (m_form->m_expandPolicy != expand_AtMapTime && m_form->getSettingsSize(set_w, set_h)) {
    w = std::max(w, set_w);
    h = std::max(h, set_h);
    hs = QSize(w,h);
    BUG_DEBUG("Hs New from Settings Dialog[" << hs.width()<< ", " << hs.height() << "]");
  } else {
    BUG_DEBUG("Hs NOT from Settings Dialog[" << hs.width()<< ", " << hs.height() << "]");
  }

  // verkleinern der Höhe
  if (hs.height() > maxDesktop.height()) {
    hs.setHeight( maxDesktop.height() );
    // only move once
    //    if (!m_form->m_firstShown)
    move(x(),maxDesktop.y());
    BUG_INFO("Desktop height to small => set maximum dialog height to desktop height");
    setMaximumHeight(maxDesktop.height());
  }
  // verkleinern der Breite
  if (hs.width() > maxDesktop.width()) {
    hs.setWidth( maxDesktop.width() );
      // only move once
    //    if (!m_form->m_firstShown)
      move(maxDesktop.x(),y());
    BUG_INFO("Desktop width to small => set maximum dialog width to desktop width");
    setMaximumWidth(maxDesktop.width());
  }

  // je nach dialog expandPolicy wird evtl. die maximale Höhe oder Breite gesetzt
  BUG_DEBUG( "DialogExpandPolicy: " << dialog_exand_policy <<
             ", desExpandPolicy" << m_form->m_expandPolicy );
  if (!(dialog_exand_policy & orient_Horizontal)) {
    BUG_INFO("Set Maximum Dialog Width: " << hs.width());
    setMaximumWidth(hs.width());
  }
  else {
    setMaximumWidth(QWIDGETSIZE_MAX);
  }

  if (!(dialog_exand_policy & orient_Vertical)) {
    BUG_INFO("Set Maximum Dialog Height: " << hs.height());
    setMaximumHeight(hs.height());
  }
  else {
    setMaximumHeight(QWIDGETSIZE_MAX);
  }
  // resize
  BUG_INFO(" IF ["<<(m_form->m_expandPolicy == expand_AtMapTime)<<"] || ["
		   <<(!m_form->m_firstShown)<<"]["<<(!m_form->readFromSettings())<<"]");
  if ( m_form->m_expandPolicy == expand_AtMapTime ||
       (!m_form->m_firstShown && !m_form->readFromSettings()) ) {
    BUG_DEBUG("ResizeBisher : "<< hsAlt.width() << ", " << hsAlt.height());
    // nicht grösser als der Desktop
    if (h > maxDesktop.height()) h = maxDesktop.height();
    if (w > maxDesktop.width()) w = maxDesktop.width();
    BUG_INFO("Resize : "<< w << ", " << h);
    if (!(dialog_exand_policy & orient_Horizontal))
      setMaximumWidth(w);
    if (!(dialog_exand_policy & orient_Vertical))
      setMaximumHeight(h);
    resize(w, h);
  }
  BUG_DEBUG("firstShown : ("<<m_form->getDialogName()<<") "<< m_form->m_firstShown);
  BUG_DEBUG("Out MaximumSize[" << maximumWidth() << ", " << maximumHeight() << "]");
}

// to catch esc key event
void GuiQtForm::QtDialog::reject() {
  if( m_form->isStandardForm() ){
    if (QMessageBox::question( this,
                               "Close dialog",
                               QString::fromStdString(compose(_("Do you really want to close dialog '%1'?"),
                                                              m_form->getDialogName())),
                               QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
      m_form->unmanage();
    }
  }
}

void GuiQtForm::QtDialog::closeEvent ( QCloseEvent * e )  {
  assert( m_form != 0 );
  BUG_INFO("overrideSettingsSize(" << size().width() << ", " << size().height() << ")");
  m_form->overrideSettingsSize(size().width(), size().height());
  // close disabled
  e->ignore();
  if( m_form->isStandardForm() ){
    m_form->unmanage();
  }
  else{
    QMessageBox::information( this, "Close dialog",
			      _("Unable to close dialog from title bar.") );
  }
}

class FormScrollView : public QScrollArea {
 public:
   FormScrollView(QWidget* par) : QScrollArea(), m_parentWidget(par) {
     setObjectName ( "FormScrollView" );
     setFrameStyle(QFrame::NoFrame);
  }

   // sizeHint ueberschreiben
   virtual QSize sizeHint() const {
     BUG(BugGui,"FormScrollView::sizeHint");
	 QSize hs = QScrollArea::sizeHint();
     QSize ret;
     QSize parsize = m_parentWidget->size();
     QSize wsize = widget() ? widget()->sizeHint() : parsize;
#if QT_VERSION < 0x060000
     QRect desktopRect = QApplication::desktop()->availableGeometry(this);
#else
     QRect desktopRect = QGuiApplication::primaryScreen()->availableGeometry();
#endif
     QMargins margins(contentsMargins());
     int left(margins.left());
     int top(margins.top());
     int right(margins.right());
     int bottom(margins.bottom());
     int wadd=0, hadd=0;
     ret.setWidth( wsize.width() + left + right );
     ret.setHeight( wsize.height() + top + bottom );
     if (parsize.width() >= desktopRect.width() )  {
       hadd = verticalScrollBar()->sizeHint().width();
       ret.setHeight( hadd + ret.height() );
     }
     if (parsize.height() >= desktopRect.height() ) {
       wadd = horizontalScrollBar()->sizeHint().height();
       ret.setWidth( wadd + ret.width() );
     }
     BUG_DEBUG( "FormScrollView sizeHint [" <<
                hs.width() << ", " << hs.height() << "]");
     BUG_DEBUG( "FormScrollView sizeHint [" <<
                ret.width() << ", " << ret.height() <<
                "] resizeable[" << widgetResizable() << "]");
     return ret;
   }

 private:
   QWidget * m_parentWidget;
 };

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtForm::GuiQtForm( GuiElement *parent, bool useSettings, const std::string &name)
  : GuiQtElement( parent, name )
  , m_dialog( 0 )
  , m_layoutOuter( 0 )
  , m_layoutInner( 0 )
  , m_widgetInner( 0 )
  , m_buttons_per_line( 0 )
  , m_standard_form( true )
  , m_helplistener( 0 )
  , m_jobRunningKey( 0 )
  , m_jobFunction( 0 )
  // , m_jobChangeIndicatorFunction( JobManager::Instance().getFunction( "CHANGE_INDICATOR" ) )
  , m_firstShown(false)
  , m_width( 0 )
  , m_height( 0 )
  , m_scrollView( 0 )
  , m_expandPolicy(expand_Default)
  , m_elements( this, GuiElement::type_Form ) {
  assert( parent != (GuiElement*) this );
  registerIndex();
  GuiForm::useSettings(useSettings);
}
GuiQtForm::~GuiQtForm() {
  unregisterIndex();
}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* setDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtForm::setDialogExpandPolicy(DialogExpandPolicy policy) {
  m_expandPolicy = policy;
}

/* --------------------------------------------------------------------------- */
/* getDialogName --                                                            */
/* --------------------------------------------------------------------------- */

std::string GuiQtForm::getDialogName() {
  if (getName().size()){
    return getName();
  }
  return getDialogTitle();
}

/* --------------------------------------------------------------------------- */
/* Type --                                                                     */
/* --------------------------------------------------------------------------- */

GuiElement::ElementType GuiQtForm::Type(){
  return GuiForm::Type();
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtForm::myWidget() {
  if ( m_widgetInner ){
    return m_widgetInner;
  }
  return m_dialog;
}

/* --------------------------------------------------------------------------- */
/* isCreated --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtForm::isCreated( )const{
  return m_dialog != 0;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

#include <qdatetime.h>
void GuiQtForm::create(){
  BUG_DEBUG( "create(), Elements = " << m_elements.size() <<
             ", Title = " << getDialogTitle());
  assert( m_dialog == 0 );

  int func = 0;

  if(isMain()) {
    BUG_DEBUG("Main Form");
    m_dialog = GuiQtManager::Instance().Toplevel();
    assert( m_dialog != 0 );
    // Die Main-Form wird vom GuiManager aufgestartet und nicht mit manage().
    setShown( true );
  }
  else {
    BUG_DEBUG("Form '" << getDialogTitle() << "'  modal["<< isApplicationModal()<<"]");
    QWidget *parent = NULL;

    m_dialog = new QtDialog(this, parent);
    static_cast<QDialog*>(m_dialog)->setModal( isApplicationModal() );
    m_dialog->setWindowTitle( QString::fromStdString(getDialogTitle()) );
    m_dialog->setObjectName( QString::fromStdString(getName()) );
  }

  // alle haben nun eine Scrollbar
  setScrollbar( GuiElement::scrollbar_AS_NEEDED );

  // Cycle Button und Close Button werden erst ganz am Schluss installiert.
  if( m_standard_form ){
    GuiQtButtonbar *bar = getQtButtonbar();
    if( bar != 0 ){
      if( hasCyclebutton() ){
        bar->GuiButtonbar::front( (GuiFactory::Instance()->createCycleButton( bar->getElement() ))->getElement() );
      }
      if( !isMain() && hasCloseButton() ){
	installCloseButton( bar );
      }
    }
  }
  if( m_helplistener != 0 ){
    GuiQtButtonbar *bar = getQtButtonbar();
    if( bar != 0 ){
      GuiButton *button = GuiFactory::Instance() -> createButton( bar, m_helplistener );
      button->setLabel( _("Help") );
    }
  }
  m_layoutOuter = new QVBoxLayout();
  //  m_layout->setResizeMode(QLayout::Auto); //[Auto,FreeRsesize,Minimum,Fixed]
  m_layoutOuter->setContentsMargins(3,3,3,3);

  // alle meine Childs creieren, managen und anordnen
  int w,h;
  int ed =  0;
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ) {
    // zuerst eine ScrollView kreieren
    if ( !m_scrollView && m_elements.withScrollbars()) {
      m_scrollView = new FormScrollView( m_dialog );

      m_scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      m_scrollView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      // to be able resize widget
      m_scrollView->setWidgetResizable(true);

      m_widgetInner = new QWidget();
      m_layoutInner = new QVBoxLayout();
      m_layoutInner->setContentsMargins(3,3,3,3);
    }

    // Ab einem Container wird zusaetzlich mit dem Splitter angeordnet
    if ( !m_widgetInner &&
	 ((*it)->Type() == type_Container || (*it)->Type() == type_Folder ||
	  (*it)->Type() == type_List      || (*it)->Type() == type_Plugin ||
	  (*it)->Type() == type_Fieldgroup ) ) {
      m_widgetInner = new QSplitter( Qt::Vertical );
      m_layoutOuter->addWidget( m_widgetInner );
    }
    // Ab der MessageLine keine Anordnung mit einem InnerWidget(Splitter)
    if ((*it)->Type() == type_Message || (*it)->Type() == type_Buttonbar) {
      if (m_widgetInner) {
	if ( m_layoutInner )
	  m_widgetInner->setLayout( m_layoutInner );
	if (m_scrollView) {
          m_widgetInner->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
	  m_scrollView->setWidget( m_widgetInner );
	  m_layoutOuter->addWidget( m_scrollView );
	}
        if (ed == 0) {
          m_widgetInner->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed ) );
        }
      }
      m_widgetInner = 0;
    }

    if( (*it)->Type() == type_Buttonbar ) {
      static_cast<GuiQtButtonbar *>( (*it) )->setMaxButtonsPerLine( m_buttons_per_line );
    }
    (*it)->create();
    if (m_widgetInner && !m_scrollView) {
      int fac = (*it)->getQtElement()->getDialogExpandPolicy() & (GuiElement::Orientation) Qt::Vertical ? 1: 0;
      QSplitter* s = dynamic_cast<QSplitter*>(m_widgetInner);
      s->addWidget( (*it)->getQtElement()->myWidget());
      if (fac) {
	s->setStretchFactor( s->count()-1, fac);
      }
    }
    else {
      if (m_widgetInner) {
        if ((*it)->getQtElement()) {
          m_layoutInner->addWidget( (*it)->getQtElement()->myWidget() );
        } else if ((*it)->Type() == type_Void) {
          GuiVoid *v = static_cast<GuiVoid*>( (*it) );
          int x,y;
          v->getSize(x,y);
          m_layoutInner->addSpacing(y);
        }
      }
      else {
        if ((*it)->getQtElement()->myWidget()) {
          m_layoutOuter->addWidget( (*it)->getQtElement()->myWidget() );
        }
      }
    }
    // (*it)->manage();
    if ( !m_widgetInner && (*it)->getQtElement()->myWidget()) {
      // get expand policy of childs
      int exand_policy =  (*it)->getQtElement()->getDialogExpandPolicy();
      if ( (*it)->Type() != type_Header &&
           (*it)->Type() != type_Buttonbar &&
           (*it)->Type() != type_Message ) {
        ed |= (int) exand_policy;
      }
    }
    else {
      // childs to splitter
      if ((*it)->getQtElement()) {
        ed |= (int) (*it)->getQtElement()->getDialogExpandPolicy();
      }
    }
  }

  m_dialog->setLayout( m_layoutOuter );
  setIconPixmap();

  ///  m_expandPolicy = ed;

  readSettings();

#if defined Q_OS_WIN || defined Q_OS_CYGWIN
  // in Verbindung mit der REPLACE-Funktion gibt es hier Probleme
  // Der Maximize-Button bleibt immer deaktiviert.
#else
  if (true){
    // remove maximized button on dialog if expandpolicy is not both directions
    if (getDialogExpandPolicy() != (Qt::Horizontal|Qt::Vertical)) {
      Qt::WindowFlags flags = m_dialog->windowFlags();
      flags &= ~Qt::WindowMaximizeButtonHint;
      m_dialog->setWindowFlags(flags);
    }
  }
#endif

  setDebugTooltip();
}

/* --------------------------------------------------------------------------- */
/* setSizeProperty --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtForm::setSizeProperty() {
  return;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtForm::update( UpdateReason reason ){
  BUG_PARA(BugGui,"GuiQtForm::update: ",getDialogTitle());
  if( m_dialog != 0 && isShown() ){
    BUG_MSG("update");

    m_elements.update( reason );
    BUG_EXIT("done");
  }

  // call form func to handle change indicator calls
  // if( m_dialog && !m_dialog->signalsBlocked() && m_jobChangeIndicatorFunction != 0 ){
  //   m_dialog->blockSignals( true );
  //   runJobChangeIndicatorFunction( JobElement::cll_Input, true );
  // }
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtForm::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
  BUG(BugGui,"GuiQtForm::hasChanged");
  bool ret = m_elements.hasChanged( trans, xfer, show );
  // sollen noch die anderen Forms geprueft werden
  if (!ret && isMain()) {
    return GuiQtManager::Instance().hasChanged( trans, xfer, show );
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtForm::getSize( int &w, int &h ){
  w = h = 0;
  int add = 15;  // Zur Sicherheit
  m_elements.getSize(w,h);
  if (!myWidget()) {
    return;
  }
  BUG_DEBUG( "--- begin getSize() ---" );
  BUG_DEBUG( "FrameSize: " << myWidget()->frameSize().width() << ", " << myWidget()->frameSize().height() );
  BUG_DEBUG( "FrameGeom: Pos: " << myWidget()->frameGeometry().x() << ", " << myWidget()->frameGeometry().y() <<
             ", Size: " << myWidget()->frameGeometry().width() << ", " << myWidget()->frameGeometry().height() );
  BUG_DEBUG( "     Geom: Pos: " << myWidget()->geometry().x() << ", " << myWidget()->geometry().y() <<
             ", Size: " << myWidget()->geometry().width() << ", " << myWidget()->geometry().height() );
  BUG_DEBUG( "GuiQtForm::getSize w: " << w << ", h:" << h );
  if (m_scrollView) {
    add += 2*m_scrollView->verticalScrollBar()->sizeHint().width();
  }
  w += add; // Inner LayoutMargin
  h += add; // Inner LayoutMargin
  BUG_DEBUG("--- end getSize() --- w: " << w << ", h:" << h);
}

/* --------------------------------------------------------------------------- */
/* printSizeInfo --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtForm::printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds) {
  GuiQtElement::printSizeInfo(os, intent, onlyMaxChilds);

  GuiElementList::iterator it;
  int newIntent = intent + 1;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    (*it)->printSizeInfo(os, newIntent, onlyMaxChilds);
  }
}

/* --------------------------------------------------------------------------- */
/* resize --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtForm::resize(){
  BUG_DEBUG( "resize()" );
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtForm::enable(){
  m_elements.enable();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtForm::disable(){
  m_elements.disable();
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtForm::manage(){
  BUG_DEBUG("--- begin manage(" << getDialogName() <<") ---");
  if (AppData::Instance().HeadlessWebMode()) {
    acquireFormOrderNumber();
  }
  if(isMain()) {
    m_elements.manage();
    BUG_DEBUG("--- end manage() MAIN ---");
    return;
  }

  if( !sensitive() ){
    // Solange das DISALLOW aktiv ist, läuft hier nichts.
    BUG_DEBUG("--- end manage() DISALLOW ---");
    return;
  }
  if( s_managedForms.size() > 0 ){
    if ( s_managedForms.back() != this ){
      s_managedForms.push_back( this );
    }else{
      s_managedForms.push_back( this );
    }
  }
  if( m_jobFunction != 0 ){
    runJobFunction( JobElement::cll_Open );
  }
  else{
    _manageForm();
  }

  BUG_DEBUG("--- end manage() ---");
}

/* --------------------------------------------------------------------------- */
/* _manageForm --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtForm::_manageForm(){
  BUG_DEBUG( "_manageForm(" << getDialogName() << ")" );
  if( m_dialog == 0 ){
    create();
    setShown( false );

    // keine Settings gelesen
    // => positionierung an cursor position
    if (!isMain() && readFromSettings() == false) {
      if (QApplication::activeWindow()) {
	m_dialog->move( QApplication::activeWindow()->cursor().pos() );
      }
    }
    if(!getStylesheet().empty()){
      m_dialog->setStyleSheet(QString::fromStdString(getStylesheet()));
    }
  }
  else{
    clearMessage();
  }

  // bereits sichtbar, raise() und nochmals ein show()
  if ( !m_dialog->isHidden() && isShown()) {
    m_dialog->raise();
    if (m_dialog->isMaximized ()) {
      m_dialog->showMaximized();
      BUG_DEBUG(" visbible -> showMaximized");
    }
    else {
      BUG_DEBUG(" visbible -> showNormal");
      m_dialog->showNormal();
    }
    return;
  }

  // nicht sichtbar
  if( !isShown() ){
    setShown( true );
    if( m_dialog->isHidden() || isApplicationModal()){
      BUG_DEBUG(" hidden -> show");
      m_dialog->show();
      m_elements.manage();
    }
    if( isApplicationModal() ) {
      installDialogsWaitCursor( getDialog() );
    }
    if( ! isIconic() ){
      if (m_dialog->isMaximized() || getQtWindowStates() & Qt::WindowMaximized) {
        BUG_DEBUG(" hidden -> showMaximized");
	m_dialog->showMaximized();
      }
      else {
	if (getQtWindowStates() == Qt::WindowMinimized) {
	  BUG_DEBUG(" hidden -> showMinimized");
	  m_dialog->showMinimized();
	}
        else {
	  BUG_DEBUG(" hidden -> showNormal");
	  m_dialog->showNormal();
	}
      }
      update( reason_Always );
      m_dialog->raise();
    }
    else {
      update( reason_Always );
      BUG_DEBUG(" Minimized -> showNormal");
      m_dialog->showNormal();
    }
  }
  else {
    if( !isApplicationModal() ) {
      BUG_DEBUG(" shown -> show");
      m_dialog->show();
    }
  }
  setAllToggleStatus( true );

  if(!isMain()){
    setSizeProperty();
  }
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtForm::unmanage(){
  if (AppData::Instance().HeadlessWebMode()) {
    releaseFormOrderNumber();
  }
  if( m_dialog == 0 ) {
    return;
  }
  if(isMain()) {
    return;
  }
  if (s_managedForms.size()) {
    assert( s_managedForms.back() == this );
    s_managedForms.pop_back();
  }
  if( m_jobFunction != 0 ){
    runJobFunction( JobElement::cll_Close );
  }
  else {
    unmanageForm();
  }
  writeSettings();
  setQtWindowStates( getDialogWidget()->windowState() ); // save windowState
}

/* --------------------------------------------------------------------------- */
/* unmanageForm --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtForm::unmanageForm(){
  if( isApplicationModal() ) {
    m_dialog->hide();
    removeDialogsWaitCursor( getDialog() );
    // removeParentWaitCursor( this );
  }
  else{
    if( isIconic() ){
      m_dialog->showMinimized();
    }
    m_dialog->hide();
  }
  setShown( false );
  setAllToggleStatus( false );

  // aufruf der unmanage methode der Childs
  m_elements.unmanage();
}

/* --------------------------------------------------------------------------- */
/* runJobFunction --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtForm::runJobFunction( JobElement::CallReason reason ){
  BUG( BugJobStart, "runJobFunction" );
  if( m_jobFunction == 0 ){
    return;
  }
  setRunningMode();
  JobStarter *trigger = new Trigger( this, m_jobFunction );
  trigger->setName( "GuiForm-Trigger-" + m_jobFunction->Name() );
  trigger->setReason( reason );
  trigger->startJob();
}

/* --------------------------------------------------------------------------- */
/* doEndOfWork --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtForm::doEndOfWork( bool error, JobElement::CallReason reason ){
  if( error ){
    updateForms( reason_Cancel );
    GuiQtManager::Instance().showErrorBox( this );
  }
  else {
    if( reason == JobElement::cll_Input ) {
      updateForms( reason_Process );
    }
    else{
      updateForms( reason_FieldInput );

      if( reason == JobElement::cll_Close ){
	unmanageForm();
      }
      else
      if( reason == JobElement::cll_Open ){
	_manageForm();
      }
      else{
	if( reason != JobElement::cll_Activate )
	  assert( false );
      }
    }
  }
  unsetRunningMode();
}

/* --------------------------------------------------------------------------- */
/* setRunningMode --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiQtForm::setRunningMode(){
  m_jobRunningKey = JobManager::Instance().setRunningMode( m_jobRunningKey );
  return m_jobRunningKey != 0;
}

/* --------------------------------------------------------------------------- */
/* unsetRunningMode --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtForm::unsetRunningMode(){
  if( m_jobRunningKey != 0 ){
    JobManager::Instance().unsetRunningMode( m_jobRunningKey );
    m_jobRunningKey = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtForm::destroy(){
  BUG(BugGui,"GuiQtForm::destroy");
  if(isMain()){
    BUG_EXIT("Main Form not destroyed");
    return false;
  }
  if( !m_elements.destroy() ){
    BUG_EXIT("Container not destroyed");
    return false;
  }

  m_dialog->deleteLater();
  m_widgetInner = 0;
  m_layoutOuter   = 0;
  m_layoutInner   = 0;
  m_dialog   = 0;
  setShown( false );
  return true;
}

/* --------------------------------------------------------------------------- */
/* replace --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtForm::replace( GuiElement *old_el, GuiElement *new_el ){
  BUG(BugGui,"GuiQtForm::replace");
  assert( old_el != 0 );
  assert( new_el != 0 );

  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it) == old_el ){ // Es hat ihn !
      BUG_MSG("found");
      (*it) = new_el;
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setHelpkey --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtForm::setHelpkey( HelpManager::HelpKey *helpkey ){
  m_helplistener = new GuiFormHelpListener( helpkey );
}

/* --------------------------------------------------------------------------- */
/* attachMenu --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtForm::attachMenu( GuiElement *menu ){
  assert(!isMain());
  if( menu->installed() ) return; /* hängt bereits irgendwo */

  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it)->Type() == type_Menubar ){
      menu->setParent( (*it) );
      (*it)->attach( menu );
      return;
    }
  }
  GuiMenubar *menubar = GuiFactory::Instance()->createMenubar( this );
  front( menubar->getElement() );

  menu->setParent( menubar->getElement() );
  menubar->getElement()->attach( menu );
}

/* --------------------------------------------------------------------------- */
/* getButtonbar --                                                             */
/* --------------------------------------------------------------------------- */

GuiButtonbar *GuiQtForm::getButtonbar(){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it)->Type() == type_Buttonbar ){
      GuiButtonbar *bar = (*it)->getButtonbar();
      assert( bar != 0 );
      return bar;
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getQtButtonbar --                                                           */
/* --------------------------------------------------------------------------- */

GuiQtButtonbar *GuiQtForm::getQtButtonbar(){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it)->Type() == type_Buttonbar ){
      GuiQtElement *el = (*it)->getQtElement();
      assert( el != 0 );
      GuiQtButtonbar *bar = el->getQtButtonbar();
      assert( bar != 0 );
      return bar;
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* createButtonbar --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQtForm::createButtonbar(){
  attach( new GuiQtMessageLine( this ) );
  GuiQtButtonbar *bar = new GuiQtButtonbar( this );
  attach( bar );
}

/* --------------------------------------------------------------------------- */
/* createButtonListener --                                                     */
/* --------------------------------------------------------------------------- */

GuiButtonListener *GuiQtForm::createButtonListener(){
  return new GuiFormButtonListener( this );
}

/* --------------------------------------------------------------------------- */
/* createToggleListener --                                                     */
/* --------------------------------------------------------------------------- */

GuiToggleListener *GuiQtForm::createToggleListener(){
  return new GuiFormToggleListener( this );
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtForm::ButtonPressed(){
  manage();
}

/* --------------------------------------------------------------------------- */
/* ToggleStatusChanged --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQtForm::ToggleStatusChanged( bool pressed ){
  if( pressed ){
    manage();
  }
  else{
    unmanage();
  }
}

/* --------------------------------------------------------------------------- */
/* printMessage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtForm::printMessage( const std::string &msg, MessageType type, time_t delay ){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it)->Type() == type_Message ){
      (*it)->printMessage( msg, type, delay );
      return;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* clearMessage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtForm::clearMessage(){
  GuiElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if( (*it)->Type() == type_Message ){
      (*it)->clearMessage();
      return;
    }
  }
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* installCloseButton --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtForm::installCloseButton( GuiQtButtonbar *bar ){
  GuiButton *button = GuiFactory::Instance()->createButton ( bar,
                                                             new GuiFormCloseListener( this ) );
  button->setLabel( _("Close") );
}

/* --------------------------------------------------------------------------- */
/* setIconPixmap --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtForm::setIconPixmap() {
  QPixmap logo;
  if( GuiQtManager::Settings() &&
      QtIconManager::Instance().getPixmap( GuiQtManager::Settings()->value
 					   ( "Intens/ApplicationIcon",
                                             "intens" ).toString().toStdString(),
                                           logo ) ){
    m_dialog->setWindowIcon( QIcon(logo) );
  }
  else if( QtIconManager::Instance().getPixmap( "intens", logo ) ){
    m_dialog->setWindowIcon( logo );
  }
  //  m_dialog->setWindowIcon(QPixmap("./images/qt4-logo.png"));
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtForm::Trigger::backFromJobStarter( JobAction::JobResult result ){
  m_form->doEndOfWork( result != JobAction::job_Ok, getReason() );
  if (getReason() == JobElement::cll_Input) {
    m_form->m_dialog->blockSignals(false);
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtForm::serializeXML( std::ostream &os, bool recursive ){
  GuiForm::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtForm::serializeJson( Json::Value& jsonObj, bool onlyUpdated ){
   GuiForm::serializeJson(jsonObj, onlyUpdated);
   jsonObj["maximized"] = (myWidget() && myWidget()->isMaximized());
   return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtForm::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated ){
   GuiForm::serializeProtobuf(eles, onlyUpdated);
   return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleElement --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtForm::getVisibleElement( GuiElementList& res ) {
  for( GuiElementList::iterator iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ( isVisibleElementType( (*iter)->Type() ) ) {
      res.push_back(*iter);
      if ((*iter)->Type() != type_Fieldgroup) {
        continue;
      }
    }
    (*iter)->getVisibleElement( res );
  }
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtForm::getDialogExpandPolicy() {
  int ed = 0;

  for( GuiElementList::iterator iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ((*iter)->Type() != type_Message){
      if ((*iter)->getQtElement()) {
        ed |= (int) (*iter)->getQtElement()->getDialogExpandPolicy();
      }
    }
  }
  return (GuiElement::Orientation) ed;
}

/* --------------------------------------------------------------------------- */
/* setMaximumSize --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtForm::setMaximumSize(bool force) {
  if (!isMain() && m_dialog) {
    dynamic_cast<QtDialog*>(m_dialog)->setMaximumSize(force);
  }
}

/* --------------------------------------------------------------------------- */
/* startTimerSetMaximumSize --                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtForm::startTimerSetMaximumSize() {
  if (!isMain() && m_dialog) {
    dynamic_cast<QtDialog*>(m_dialog)->startTimerSetMaximumSize();
  }
}

/* --------------------------------------------------------------------------- */
/* isIconic --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtForm::isIconic() {
  if (myWidget()) {
    return myWidget()->isMinimized();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* writeSettings  --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtForm::writeSettings(){
  if (!useSettings()) return; // no settings
  if (!getDialogWidget()) return; // not created yet
  assert(getDialogWidget());
  QSettings *settings = GuiQtManager::Settings();

  // SettingPlot2DLevel
  //    0 => Weder Applikations- noch User-Settings werden verwendet
  //    1 => (Nur) Applikations-Settings werden verwendet
  // >= 2 => (Default) Alle Settings werden geschrieben und nach einem Restart wieder eingelesen
  int dlevel = settings->value("Intens/SettingDialogLevel", std::numeric_limits<int>::max()).toInt();
  if (dlevel < 2)
    return;

  if (settings) {
    settings->beginGroup( QString::fromStdString("Dialog") );
    QString _tmp = QString::fromStdString(getDialogName());

    bool isVisible = getDialogWidget()->isVisible();
    int windowStates = getDialogWidget()->isVisible() ? (int) getDialogWidget()->windowState() : getQtWindowStates();
    // minimized is ignored (not written to the settings)
    // minimized forms are treated as not visible
    if (windowStates & Qt::WindowMinimized) {
      // clear minimized flag
      windowStates &= ~Qt::WindowMinimized;
      isVisible = false;
    }

#if QT_VERSION < 0x060000
    QDesktopWidget *widget = QApplication::desktop();
#endif
    BUG_INFO(_tmp.toStdString() << " size (settings) read: "
             << settings->value(_tmp + ".size").toString().toStdString());
    if (windowStates == Qt::WindowNoState) {
      settings->setValue(_tmp + ".size", getDialogWidget()->size());
      settings->setValue(_tmp + ".pos", getDialogWidget()->pos());
    }
    BUG_INFO(_tmp.toStdString() << " size (settings) written: "
             << settings->value(_tmp + ".size").toString().toStdString());
    settings->setValue(_tmp + ".windowState", windowStates);

    // write visible flag (only if not ignored and not modal)
    QString value = settings->value(_tmp + ".visible", "").toString();
    if ( value !="ignore" && !isApplicationModal() ) {
      settings->setValue(_tmp + ".visible", isVisible);
    }
    settings->endGroup();
  }
}

/* ------------------------------------------------------------- */
/* acceptIndex --                                                */
/* ------------------------------------------------------------- */

bool GuiQtForm::acceptIndex( const std::string &name, int inx ) {
  return true;
}

/* ------------------------------------------------------------- */
/* setIndex  --                                                  */
/* ------------------------------------------------------------- */

void GuiQtForm::setIndex( const std::string &name, int inx ){
  if (m_dialog) {
    // sollte nur aufrgerufen werden, wenn er von ui interaktiv ausgelöst wurde
    // es wird er besser auskommentiert
    // runJobFunction( JobElement::cll_Input );
  }
}
