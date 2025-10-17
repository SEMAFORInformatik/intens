#include <iostream>
#include <algorithm>

#include <QWidget>
#include <QCursor>
#include <QApplication>
#include <QSettings>
#if QT_VERSION > 0x050600
#include <QScreen>
#else
#include <QDesktopWidget>
#endif

#include "gui/qt/GuiQtDialog.h"
#include "gui/qt/GuiQtManager.h"
#include <gui/qt/GuiQtForm.h>
#include "utils/Debugger.h"

INIT_LOGGER();

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
GuiQtDialog::GuiQtDialog()
  : m_readFromSettings(false)
  , m_readSettingWidth(0)
  , m_readSettingHeight(0)
  , m_qtWindowStates(Qt::WindowNoState)
{
}

/* --------------------------------------------------------------------------- */
/* readFromSettings --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiQtDialog::readFromSettings() {
  return  m_readFromSettings;
}

/* --------------------------------------------------------------------------- */
/* getSettingsSize --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiQtDialog::getSettingsSize(int& width, int& height) {
  bool getSettingsSize(int& width, int& height);
  width = 0;
  height = 0;
  if (m_readSettingWidth > 0 && m_readSettingHeight > 0 ) {
    width  = m_readSettingWidth;
    height = m_readSettingHeight;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* overrideSettingsSize --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQtDialog::overrideSettingsSize(int width, int height) {
  m_readSettingWidth = width;
  m_readSettingHeight = height;
}

/* --------------------------------------------------------------------------- */
/* extendSize --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtDialog::extendSize(int width, int height) {
  BUG_DEBUG("extendSize width: " << width << ", height: " << height);
  if (width || height) {
#if QT_VERSION > 0x050600
    QRect maxDesktop= QGuiApplication::primaryScreen()->availableGeometry();
#else
    QRect maxDesktop= QApplication::desktop()->availableGeometry();
#endif
    QSize hs = getDialogWidget()->sizeHint();
    QSize ms = getDialogWidget()->maximumSize();
    int nw = std::min(maxDesktop.width(), ms.width() + width);
    int nh = std::min(maxDesktop.height(), ms.height() + height);
    getDialogWidget()->setMaximumSize(nw, nh);
    BUG_DEBUG("New Size width: "  << nw << ", height: " << nh );
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* installWaitCursor --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtDialog::installWaitCursor( GuiDialog *installer ) {
  BUG_DEBUG("GuiQtDialog::installWaitCursor: Installer : " << installer );

  if( std::find(m_wait_installer.begin(),
                m_wait_installer.end(),
                installer) != m_wait_installer.end() ){ // allready installed
    return;
  }
  QWidget* dialog = getDialogWidget() ;
  if( dialog == 0 ){
    BUG_DEBUG("No dialog => exit");
    return;
  }

  { // hand made => set WaitCursor for all childs inside dialog
    QList<QWidget*> allWidgets = dialog->findChildren<QWidget*>();
    for (int i=0; i<allWidgets.size(); ++i) {
      QWidget *w = allWidgets.at(i);
      if (w && w->window() == dialog){
        BUG_DEBUG(" - set cursor to WaitCursor");
        w->setCursor(  QCursor( Qt::WaitCursor ) );
      }
    }
  }

  BUG_DEBUG(" - set cursor of dialog to WaitCursor");
  dialog->setCursor(  QCursor( Qt::WaitCursor ) );

  m_wait_installer.push_back( installer );
}

/* --------------------------------------------------------------------------- */
/* removeWaitCursor --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtDialog::removeWaitCursor( GuiDialog *remover ) {
  BUG_DEBUG("GuiQtDialog::removeWaitCursor: Remover : " << remover );

  std::vector<GuiDialog*>::iterator it = std::find(m_wait_installer.begin(),
                                                   m_wait_installer.end(),
                                                   remover);
  if( it == m_wait_installer.end()  ){
    return;
  }
  m_wait_installer.erase( it );
  if ( m_wait_installer.size() )
    return;
  QWidget* dialog = getDialogWidget();
  if( dialog == 0 ){
    return;
  }

  { // hand made => unset WaitCursor for all childs inside dialog
    QList<QWidget*> allWidgets = dialog->findChildren<QWidget*>();
    for (int i=0; i<allWidgets.size(); ++i) {
      QWidget *w = allWidgets.at(i);
      if (w && w->window() == dialog)
	allWidgets.at(i)->unsetCursor();
    }
  }
  dialog->setCursor(  QCursor( Qt::ArrowCursor ) );
}

/* --------------------------------------------------------------------------- */
/* installDialogsWaitCursor --                                                 */
/* --------------------------------------------------------------------------- */

void  GuiQtDialog::installDialogsWaitCursor( GuiDialog* installer ){
  GuiElementList::iterator it;
  GuiElementList flist, mlist;
  GuiElement::findElementType(flist, GuiElement::type_Form);
  GuiElement::findElementType(mlist, GuiElement::type_Main);
  flist.insert(flist.end(), mlist.begin(), mlist.end());
  for( it = flist.begin(); it != flist.end(); ++it ) {
    if ( (*it)->getDialog() == installer ) {
      continue;
    }
    GuiQtElement *e = static_cast<GuiQtElement*>(*it);
    if( e ){
      GuiDialog *d = e->getDialog();
      if( d ){
	d->installWaitCursor( installer );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* removeDialogsWaitCursor --                                                  */
/* --------------------------------------------------------------------------- */

void  GuiQtDialog::removeDialogsWaitCursor( GuiDialog* remover ){
  GuiElementList::iterator it;
  GuiElementList flist, mlist;
  GuiElement::findElementType(flist, GuiElement::type_Form);
  GuiElement::findElementType(mlist, GuiElement::type_Main);
  flist.insert(flist.end(), mlist.begin(), mlist.end());
  for( it = flist.begin(); it != flist.end(); ++it ) {
    GuiQtElement *e = static_cast<GuiQtElement*>(*it);
    if( e ){
      GuiDialog *d = e->getDialog();
      if( d ){
	d->removeWaitCursor( remover );
      }
    }
  }
  GuiQtManager::removeWaitCursorsFromList();
}

/* --------------------------------------------------------------------------- */
/* readSettings --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtDialog::readSettings()
{
  BUG_PARA(BugGui,"GuiQtDialog::readSettings Named: ", getDialogName());
  // SettingDialogLevel
  //    0 => Weder Applikations- noch User-Settings werden verwendet
  //    1 => (Nur) Applikations-Settings werden verwendet
  // >= 2 => (Default) Alle Settings werden geschrieben und nach einem Restart wieder eingelesen

  assert(getDialogWidget());
  QSettings *settings = GuiQtManager::Settings();
  int dlevel = settings->value("Intens/SettingDialogLevel", std::numeric_limits<int>::max()).toInt();
  settings->beginGroup( QString::fromStdString("Dialog") );
  QString _tmp = QString::fromStdString(getDialogName());

  m_readFromSettings = settings->contains(_tmp + ".size");
  if ( dlevel >= 1) {
#if QT_VERSION > 0x050600
    QRect desktopRect = QGuiApplication::primaryScreen()->availableGeometry();
#else
    QRect desktopRect = QApplication::desktop()->availableGeometry();
#endif

    // read size
    getDialogWidget()->adjustSize();
    QSize hsize = getDialogWidget()->sizeHint();
    QSize hsizeDlg(hsize);
    if (GuiQtManager::Instance().hasSettingFileActualVersion()) {
      hsize = settings->value(_tmp + ".size", hsize).toSize();
      if (hsize.width() > desktopRect.width()) hsize.setWidth( desktopRect.width() );
      if (hsize.height() > desktopRect.height()) hsize.setHeight( desktopRect.height() );
	  if (hsize != hsizeDlg) {
		m_readSettingWidth  = hsize.width();
		m_readSettingHeight = hsize.height();
	  }
    }
    if (hsize != getDialogWidget()->size()) {
      BUG_MSG("Set to new Size["<<hsize.width()<<", "<<hsize.height()<<"]");
      getDialogWidget()->resize(hsize);
    }

    // read position
    QPoint pt = settings->value(_tmp + ".pos", getDialogWidget()->pos()).toPoint();
    int screen = settings->value(_tmp + ".screen", 0).toInt();
    if (pt != getDialogWidget()->pos()) {
      BUG_MSG("Set to new Position ["<<pt.x()<<", "<<pt.y()<<"]");
      getDialogWidget()->move(pt);
    }

    // read windowState
    m_qtWindowStates = settings->value(_tmp + ".windowState", m_qtWindowStates).toInt();
  }
  settings->endGroup();
}
