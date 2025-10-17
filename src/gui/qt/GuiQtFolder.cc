#include "app/AppData.h"

#include <typeinfo>
#include <qtabwidget.h>
#include <QStackedWidget>
#include <QtCore/QSettings>
#include <qlayout.h>
#include <QScrollArea>
#include <QScrollBar>
#include <QTableView>
#include <QLabel>

#include "gui/qt/GuiQtOrientationContainer.h"
#include "gui/qt/GuiQtFolder.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/GuiFolderGroup.h"

#include "job/JobManager.h"
#include "app/DataPoolIntens.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"

#include "gui/qt/QtIconManager.h"
#include "gui/qt/GuiQtFolderUndoElement.h"

#define CONTENT_MARGIN 2//5

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Definition of Class MyQStackedWidget                                        */
/*=============================================================================*/

class MyQStackedWidget : public QStackedWidget {
public:
  MyQStackedWidget( GuiQtFolder* folder, QWidget *w );
  virtual ~MyQStackedWidget();

public:
  QSize getMaxSize() const;

private:
  GuiQtFolder*  m_folder;
};

/*=============================================================================*/
/* Constructor / Destructor of MyQStackedWidget                                */
/*=============================================================================*/

MyQStackedWidget::MyQStackedWidget( GuiQtFolder* folder, QWidget *w )
  : QStackedWidget( w )
  , m_folder( folder ){
}
MyQStackedWidget::~MyQStackedWidget(){
}

/*=============================================================================*/
/* Member Functions of MyQStackedWidget                                        */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getMaxSize --                                                               */
/* --------------------------------------------------------------------------- */

QSize MyQStackedWidget::getMaxSize() const{
  QSize mySize = QStackedWidget::sizeHint();

  // ask folder parents
  GuiElement* pw = m_folder->getParent();
  int orientation = 0;
  if (pw) {
    // look for a folder parent
    while (pw->Type() == GuiElement::type_Container) {
      GuiQtOrientationContainer *con = dynamic_cast<GuiQtOrientationContainer*>(pw->getQtElement());
      if (!con || con->countElements() != 1) {
	orientation |=  con->getOrientation();
	//	  if (orientation == (Qt::Horizontal|Qt::Vertical))
	break;
      }
      pw = pw->getParent();
    }

    // have folder parent?
    if (pw->Type() == GuiElement::type_Folder) {
      QSize s;
      if (dynamic_cast<GuiQtFolder*>(pw)->IsHideButton()) {
	s = dynamic_cast<MyQStackedWidget*>(pw->getQtElement()->myWidget())->getMaxSize();
      }
      else{
	s = pw->getQtElement()->myWidget()->sizeHint();
      }
      if (orientation != Qt::Vertical)   mySize.setHeight( s.height() );
      if (orientation != Qt::Horizontal) mySize.setWidth( s.width() );
    }
  }
  return mySize;
}

/*=============================================================================*/
/* Definition of Class MyQTabWidget                                            */
/*=============================================================================*/

class MyQTabWidget : public  QTabWidget {
public:
  MyQTabWidget(GuiQtFolder *folder, QWidget* par);

public:
  QSize getMaxSize() const;
  void showEvent( QShowEvent *e );
  QTabBar* tabBar() const;
  void setFont();

protected:
  virtual void tabInserted(int index);
  virtual void tabRemoved(int index);

private:
  QSize         m_maxSize;
  bool          m_firstShown;
  GuiQtFolder*  m_folder;
  QSize         m_diff;
};

/*=============================================================================*/
/* Constructor / Destructor of MyQTabWidget                                    */
/*=============================================================================*/

MyQTabWidget::MyQTabWidget(GuiQtFolder *folder, QWidget* par)
  : QTabWidget(par)
  , m_folder(folder)
  , m_maxSize(0,0)
  , m_firstShown(false) {

  // tabs movable?
  if (folder->tabMovable())
    tabBar()->setMovable(true);
  // tabs expanding?
  if (folder->tabStretched())
    tabBar()->setExpanding(true);

    setTabBarAutoHide(!folder->isShowAlwaysButton());  // hide tabbar if less than two pages

  // set font
  setFont();
}

/*=============================================================================*/
/* Member Functions of MyQTabWidget                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getMaxSize --                                                               */
/* --------------------------------------------------------------------------- */

QSize MyQTabWidget::getMaxSize() const{
  GuiElement* pw = m_folder->getParent();
  int orientation = 0;
  QSize mySize = QTabWidget::sizeHint();

  // ask folder parents
  if (pw) {
    // look for a folder parent
    while (pw->Type() == GuiElement::type_Container) {
      GuiQtOrientationContainer *con = dynamic_cast<GuiQtOrientationContainer*>(pw->getQtElement());
      if (con && con->countElements() != 1) {
	orientation |=  con->getOrientation();
	//	  if (orientation == (Qt::Horizontal|Qt::Vertical))
	break;
      }
      pw = pw->getParent();
    }

    // have folder parent?
    if (pw->Type() == GuiElement::type_Folder) {
      QSize s;
      if (dynamic_cast<GuiQtFolder*>(pw)->IsHideButton()) {
	s = dynamic_cast<MyQStackedWidget*>(pw->getQtElement()->myWidget())->getMaxSize();
      }
      else{
	s = pw->getQtElement()->myWidget()->sizeHint();
      }
      // 	if (orientation & Qt::Vertical)
      mySize.setHeight( s.height() - m_diff.height() );
      // 	if (orientation & Qt::Horizontal)
      mySize.setWidth( s.width() );
      return mySize - m_diff;
    }
  }
  return m_maxSize;
}

/* --------------------------------------------------------------------------- */
/* showEvent --                                                                */
/* --------------------------------------------------------------------------- */

void MyQTabWidget::showEvent( QShowEvent *e ){
  int orientation = 0;
  QTabWidget::showEvent(e);

  // evtl. am parent folder ausrichten
  if( !m_firstShown ) {
    //      m_diff = sizeHint() - m_maxSize;
    m_firstShown = true;
    GuiElement* pw = m_folder->getParent();
    if (pw) {
      while (pw->Type() == GuiElement::type_Container) {
	GuiQtOrientationContainer *con = dynamic_cast<GuiQtOrientationContainer*>(pw->getQtElement());
	if (!con || con->countElements() != 1) {
	  break;
	}
	pw = pw->getParent();
      }
      if (0&&pw->Type() == GuiElement::type_Folder) {
	QSize s;
	if (dynamic_cast<GuiQtFolder*>(pw)->IsHideButton()) {
	  s = dynamic_cast<MyQStackedWidget*>(pw->getQtElement()->myWidget())->getMaxSize();
	}
	else{
	  s = dynamic_cast<MyQTabWidget*>(pw->getQtElement()->myWidget())->getMaxSize();
	  setMinimumSize( s );
	}
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* tabInserted --                                                              */
/* --------------------------------------------------------------------------- */

void MyQTabWidget::tabInserted( int index ){
  QTabWidget::tabInserted(index);
  m_maxSize = m_maxSize.expandedTo(  widget(index)->sizeHint() );

  QWidget *w = widget(index);
  if (!m_diff.isValid()) {
    m_diff = sizeHint() - m_maxSize;
    if( w->layout() ){
      m_diff += QSize(2*CONTENT_MARGIN, 2*CONTENT_MARGIN);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* tabRemoved --                                                               */
/* --------------------------------------------------------------------------- */

void MyQTabWidget::tabRemoved( int index ){
  QTabWidget::tabRemoved( index );
}

/* --------------------------------------------------------------------------- */
/* tabBar --                                                                   */
/* --------------------------------------------------------------------------- */

QTabBar*  MyQTabWidget::tabBar() const {
  return  QTabWidget::tabBar();
}

/* --------------------------------------------------------------------------- */
/* setFont --                                                                  */
/* --------------------------------------------------------------------------- */

void MyQTabWidget::setFont() {
  QFont font =  tabBar()->font();
  QFont fontNew =  QtMultiFontString::getQFont( "@folderTab@", font );
  if (font != fontNew) {
    tabBar()->setFont( fontNew );
  }
}

/*=============================================================================*/
/* Constructor / Destructor of GuiQtFolder                                     */
/*=============================================================================*/

GuiQtFolder::GuiQtFolder( GuiElement *parent, const std::string &name )
  : GuiQtElement( parent, name )
  , m_activePage( 0 )
  , m_activeWebPage( 0 )
  , m_disabled_CB( false )
  , m_folder( 0 ){

  // get basic infos from resource
  if( !m_hideButtons )
    readSettings();
}

GuiQtFolder::GuiQtFolder( const GuiQtFolder& fd )
  : GuiQtElement( fd )
  , GuiFolder( fd )
  , m_activePage(  fd.m_activePage )
  , m_activeWebPage(  fd.m_activeWebPage )
  , m_disabled_CB( false )
  , m_folder( 0 ){
  GuiNamedElementList::const_iterator pi = fd.m_elements.begin();
  for( pi = fd.m_elements.begin(); pi != fd.m_elements.end(); ++pi ){
    if( searchNamedElement( m_elements, (*pi).first ) != 0 ){
      // 2014-10-01 mau iopera kunde wünscht gleiche Labels im Tab
      //      continue;
    }

    GuiQtOrientationContainer *oldPage = static_cast<GuiQtOrientationContainer*>((*pi).second);
    if (oldPage && oldPage->cloneable()) {
      std::string pixmap( oldPage->getPixmap() );
      GuiQtOrientationContainer *page = static_cast<GuiQtOrientationContainer*>(oldPage->clone());
      static_cast<GuiElement*>(page)->setParent( this );

      if (pixmap.size())
        page->setPixmap(pixmap);  // used for Tab Widget
      m_elements.push_back( GuiNamedElementList::value_type( (*pi).first, page ) );
    }
  }
}

GuiQtFolder::~GuiQtFolder(){
  delete m_folder;
}

/*=============================================================================*/
/* Member Functions of GuiQtFolder                                             */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

GuiElement* GuiQtFolder::clone() {
  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement())
    m_clonedFolder.push_back( new GuiQtFolder( *this ) );
  else
    return baseElem->clone();
  return m_clonedFolder.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_clonedFolder.begin(), m_clonedFolder.end());
}

/* --------------------------------------------------------------------------- */
/* addFolderPage --                                                            */
/* --------------------------------------------------------------------------- */

GuiOrientationContainer *GuiQtFolder::addFolderPage(const std::string &name,
                                                    const std::string pixmap, bool hidden)
{
  BUG_DEBUG("addFolderPage, name=" << name);

  std::string na = QtMultiFontString::getQString(name).toStdString();
  if( searchNamedElement( m_elements, na ) != 0 ){
    // 2014-10-01 mau iopera kunde wünscht gleiche Labels im Tab
    //    return 0;
  }

  GuiQtOrientationContainer *page = new GuiQtOrientationContainer( orient_Horizontal, this );
  if( pixmap.size() ){
    page->setPixmap( pixmap );  // used for Tab Widget
  }

  if (hidden) {
    page->setHiddenFlag();
    m_elements.push_back( GuiNamedElementList::value_type( na, NULL ) );
  } else {
    m_elements.push_back( GuiNamedElementList::value_type( na, page ) );
  }
  BUG_DEBUG("Page '" << na << "' added");
  return page;
}

/* --------------------------------------------------------------------------- */
/* PageIsActive --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtFolder::PageIsActive( int page ){
  if( AppData::Instance().HeadlessWebMode() ){
    return page == m_activeWebPage;
  }
  if( !m_folder ){
    return page == m_activePage;
  }
  int currentPageIndex = -1;
  if( typeid( *m_folder ) == typeid( MyQTabWidget ) ) {
    currentPageIndex = static_cast<QTabWidget*>(m_folder)->currentIndex();
    QWidget *w = static_cast<QTabWidget*>(m_folder)->currentWidget();
    GuiNamedElementList::iterator iter = m_elements.begin();
    currentPageIndex = 0;
    for (; iter != m_elements.end(); ++iter, ++currentPageIndex) {
      if ((*iter).second && (*iter).second->getQtElement()->myWidget() == w) {
        break;
      }
    }
  }
  else{
    MyQStackedWidget *ws = static_cast<MyQStackedWidget*>(m_folder);
    currentPageIndex = ws->currentIndex();
  }

  // if active return true
  if (currentPageIndex == page)
    return true;

  // has a cloned folder activate this page?
  if ( m_clonedFolder.size() ) {
    std::vector<GuiElement*>::iterator it =  m_clonedFolder.begin();
    for (; it != m_clonedFolder.end(); ++it) {
      if ((*it)->getFolder()->PageIsActive( page ))
        return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* activatePage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::activatePage( int page, GuiFolderGroup::OmitMap omit_map, bool force ){
  BUG_DEBUG("activatePage@" << this << ": page=" << page);
  int activePage = AppData::Instance().HeadlessWebMode() ? m_activeWebPage : m_activePage;
  BUG_DEBUG("activePage=" << activePage);

  if( !force && page == activePage && m_folder){
    BUG_DEBUG("EXIT Page " << page << " already activ");
    return;
  }

  setLastWebUpdated();

  // call cloned folder
  std::vector<GuiElement*>::iterator it =  m_clonedFolder.begin();
  for (; it != m_clonedFolder.end(); ++it){
    BUG_DEBUG("activate clone");
    (*it)->getFolder()->activatePage( page, GuiFolderGroup::omit_Default, force );
  }

  // set active page
  if( omit_map != GuiFolderGroup::omit_Visibility ){
    m_activePage = page;
    if( AppData::Instance().HeadlessWebMode() ){
      m_activeWebPage = page;
    }
  }

  if( !m_folder ) {
    BUG_DEBUG("Folder not created");
    return;
  }

  if(  omit_map != GuiFolderGroup::omit_TTRAIL ){
    GuiQtFolderUndo undo( this, page, activePage );
    if( getTabIndex( page ) < 0 ){
      undo.setAction( GuiQtFolderUndoElement::UNMAP );
    }
  }

  QWidget *w = 0;
  if( typeid( *m_folder ) == typeid( MyQTabWidget ) ){
    QTabWidget *folder = static_cast<QTabWidget*>( m_folder );
    validateTab( page );
    BUG_DEBUG("set current index");
#if ( __GNUC__ < 5 )
    GuiNamedElementList::iterator iter = m_elements.begin();
    int i = 0;
    while(i++ < page) ++iter;
#else
    GuiNamedElementList::iterator iter = std::next(m_elements.begin(), page);
#endif
    if ((*iter).second) {
      QWidget *w = static_cast<QTabWidget*>(m_folder)->currentWidget();
      if( omit_map == GuiFolderGroup::omit_Visibility && !AppData::Instance().HeadlessWebMode() ){
        folder->blockSignals(true);
      }
      folder->setCurrentWidget((*iter).second->getQtElement()->myWidget());
      if( omit_map == GuiFolderGroup::omit_Visibility && !AppData::Instance().HeadlessWebMode() ){
        folder->setCurrentWidget(w);
        folder->blockSignals(false);
      }
    }
    BUG_DEBUG("current page is now " << page);
  }
  else{
    if( omit_map == GuiFolderGroup::omit_Visibility && !AppData::Instance().HeadlessWebMode() )
      // ignore this map command
      return;
    MyQStackedWidget *folder = static_cast<MyQStackedWidget*>( m_folder );
    folder->setCurrentIndex( page );
    BUG_DEBUG("current index is now " << page);

    // scroll back to (0,0)
    // parents may be QSplitter, QScrollView, QScrollView::viewport()
    QWidget* pw = folder->parentWidget();
    while ( pw->objectName() != "QScrollArea" ) {
      if( !pw->parentWidget() )
        break;
      pw = pw->parentWidget();
    }
    if (pw->objectName() == "QScrollArea" ) {
      QScrollArea *sv = dynamic_cast<QScrollArea*>(pw);
      QScrollBar *hsb = sv->horizontalScrollBar();
      if (hsb) hsb->setValue(0);
      QScrollBar *vsb = sv->verticalScrollBar();
      if (vsb) vsb->setValue(0);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* hidePage --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::hidePage( int page ){
  BUG_DEBUG("hidePage@" << this << ": page=" << page);

  BUG_DEBUG("activePage=" << m_activePage);
  GuiQtFolderUndo undo( this, page, m_activePage );
  undo.setAction( GuiQtFolderUndoElement::MAP );

  // call cloned folder
  if( !m_clonedFolder.empty() ){
    BUG_DEBUG("begin of call cloned folders");
    std::vector<GuiElement*>::iterator it =  m_clonedFolder.begin();
    for( ; it != m_clonedFolder.end(); ++it ){
      (*it)->getFolder()->hidePage( page );
    }
    BUG_DEBUG("end of call cloned folders");
  }

  // do own job
  if( !m_folder ){
    BUG_DEBUG("EXIT: no folder available");
    return;
  }


  m_disabled_CB = true;

  if( typeid( *m_folder ) == typeid( MyQTabWidget ) ){
    QTabWidget *folder = static_cast<QTabWidget*>( m_folder );
    int tab_index = getTabIndex( page );
    folder->removeTab( tab_index );
    BUG_DEBUG("EXIT: page " << page << " with index " << tab_index << " removed");
    //    folder->setTabEnabled( page, false ); // eine alternative!!!
  } else {
    QStackedWidget* tabWiget = static_cast<MyQStackedWidget*>(m_folder);
    int cur_idx = tabWiget->currentIndex();
    if (tabWiget && page == cur_idx) {
      tabWiget->setCurrentIndex(page || tabWiget->count() == 1 ? 0 : 1);
    }
  }
  m_disabled_CB = false;
}

/* --------------------------------------------------------------------------- */
/* isHiddenPage --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtFolder::isHiddenPage(int page) {
  if( !m_folder ) return true;
  if( m_hideButtons ){
    MyQStackedWidget *ws = static_cast<MyQStackedWidget*>(m_folder);
    return page == ws->currentIndex();
  } else {
    QTabWidget *folder = static_cast<QTabWidget*>( m_folder );
    int tab_index = getTabIndex( page );
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
    return !folder->isTabVisible( tab_index );
#else
    return false; // old qt version
#endif
  }
}


/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::update( UpdateReason reason ){
  if( !m_folder ){
    return;
  }
  int currentPageIndex = -1;
  if( m_hideButtons ){
    MyQStackedWidget *ws = static_cast<MyQStackedWidget*>(m_folder);
    currentPageIndex = ws->currentIndex();
  } else {
    QWidget *w = static_cast<QTabWidget*>(m_folder)->currentWidget();
    GuiNamedElementList::iterator iter = m_elements.begin();
    currentPageIndex = 0;
    for (; iter != m_elements.end(); ++iter, ++currentPageIndex) {
      if (!(*iter).second) continue;
      if ( (*iter).second->getQtElement()->myWidget() == w) {
        break;
      }
    }
    currentPageIndex = transferIndexFromMoved(currentPageIndex);

     // set font
     static_cast<MyQTabWidget*>( m_folder )->setFont();
  }
#if ( __GNUC__ < 5 )
    GuiNamedElementList::iterator iter = m_elements.begin();
    int i = 0;
    while(i++ < currentPageIndex) ++iter;
#else
  GuiNamedElementList::iterator iter = std::next(m_elements.begin(), currentPageIndex);
#endif
  if ((*iter).second) {
    if ((*iter).second->LastGuiUpdated() == 0)
      // Tab wird nach unterdrücktem update mit reason_Always oder reason_Cycle aktiviert: update(reason_Always)
      (*iter).second->update( GuiElement::reason_Always );
    else {
      // update(reason) mit LastUpdate des Tabs, da im Tab seit damals kein GuiUpdate gemacht wurde
      GuiQtManager::Instance().overrideLastUpdate( (*iter).second->LastGuiUpdated() );
      (*iter).second->update( reason );
      GuiQtManager::Instance().resetOverrideLastUpdate();
    }
    (*iter).second->setLastGuiUpdated( DataPoolIntens::CurrentTransaction() );
  }

  if (reason == GuiElement::reason_Always || reason == GuiElement::reason_Cycle) {
    // bei allen anderen Tabs ein GuiUpdate mit reason_Always erzwingen, wenn er später (irgendwann) aktiviert wird
    GuiNamedElementList::iterator iter2;
    for( iter2 = m_elements.begin(); iter2 != m_elements.end(); ++iter2 ){
      if (!(*iter2).second) continue;
      if (iter != iter2) {
        (*iter2).second->setLastGuiUpdated( 0 );
        ResetLastWebUpdated();  // reason_Always for webtens
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* printSizeInfo --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds) {
  GuiQtElement::printSizeInfo(os, intent, onlyMaxChilds);
  GuiNamedElementList::iterator it, wMaxIt, hMaxIt;
  int w, h, wIdx(0), hIdx(0), wMax(0), hMax(0);

  if (onlyMaxChilds) {
    for(it = m_elements.begin(); it != m_elements.end(); ++it){
      if (!(*it).second) continue;
      (*it).second->getSize(w, h, true);
      QSize s = (*it).second->getQtElement()->myWidget()->sizeHint();
      w = s.width();
      h = s.height();
      if (w > wMax) {
        wMax = w;
        wMaxIt = it;
      }
      if (h > hMax) {
        hMax = h;
        hMaxIt = it;
      }
    }
  }

  int newIntent = intent + 1;
  if (onlyMaxChilds) {
    int i(0);
    while(++i <= intent) os << "  ";
    os << "==> "<< (hMaxIt == wMaxIt ? "MAX_HEIGHT_WIDTH": "MAX_HEIGHT") << " FolderTab: \""<<hMaxIt->first << "\" <==\n";
    hMaxIt->second->printSizeInfo(os, newIntent, onlyMaxChilds);
    if (hMaxIt != wMaxIt) {
      i = 0;
      while(++i <= intent) os << "  ";
      os << "==> MAX_WIDTH FolderTab: \""<<wMaxIt->first << "\" <==\n";
      wMaxIt->second->printSizeInfo(os, newIntent, onlyMaxChilds);
    }
  } else {
    int i(0);
    while(++i <= intent) os << "  ";
    for( it = m_elements.begin(); it != m_elements.end(); ++it ){
      if ((*it).second)
        (*it).second->printSizeInfo(os, newIntent, onlyMaxChilds);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtFolder::hasChanged( TransactionNumber trans, XferDataItem* xfer, bool show ){
  //  BUG_DEBUG("hasChanged");

  GuiNamedElementList::iterator iter;
  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ( (*iter).second && (*iter).second->hasChanged( trans, xfer, show ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setTabOrder --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::setTabOrder(){
//   GuiFolder::setTabOrder();
}


/* --------------------------------------------------------------------------- */
/* unsetTabOrder --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::unsetTabOrder(){
//   GuiFolder::unsetTabOrder();
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::create(){
  BUG_DEBUG("create");

  QWidget *pw = getParent()->getQtElement()->myWidget();
  assert( pw != 0 );
  int ed = 0;

  if( !m_hideButtons ){
    MyQTabWidget *folder = new MyQTabWidget( this, pw );
    m_folder = folder;

    switch( m_button_placement ){
    case align_Top:
      folder->setTabPosition( QTabWidget::North );
      break;
    case align_Bottom:
      folder->setTabPosition( QTabWidget::South );
      break;
    case align_Left:
      folder->setTabPosition( QTabWidget::West );
      break;
    case align_Right:
      folder->setTabPosition( QTabWidget::East );
      break;
    default:
      folder->setTabPosition( QTabWidget::North );
      std::cerr << "Tab position not supported" << std::endl;
      break;
    }

    GuiNamedElementList::iterator iter;
    int i = 0;
    std::vector<int> iList;
    folder->setIconSize(QSize(0,0));
    for( iter = m_elements.begin(); iter != m_elements.end(); ++iter, ++i ){
      if (!iter->second) continue;
      GuiQtOrientationContainer *cwidget =dynamic_cast<GuiQtOrientationContainer*>(iter->second);
      cwidget->create();
      QWidget *w = iter->second->getQtElement()->myWidget();
      ed |= (int) iter->second->getQtElement()->getContainerExpandPolicy();
      QPixmap pm;
      if (cwidget->getPixmap().size())
        QtIconManager::Instance().getPixmap(cwidget->getPixmap(), pm);

      if( w->layout() )
        w->layout()->setContentsMargins(CONTENT_MARGIN,CONTENT_MARGIN,CONTENT_MARGIN,CONTENT_MARGIN);
      QString _tn =  QtMultiFontString::getQString(iter->first );

      // if multiline label create a multiLineLabels as pixmap
      // but do not use it
      // only get height and create a transparent pixmap
      if (_tn.count("\n") >= 1) {
        QFont font =  folder->tabBar()->font();
        bool multiLine;
        QPixmap* pixmap = QtIconManager::Instance().getLabelPixmap(_tn.toStdString(), pm, font, multiLine);
        if (multiLine) {
          if (pm.isNull()) {
            pm = QPixmap(1, pixmap->height());
            pm.fill(Qt::transparent);
            iList.push_back(i);
          } else
            pm = pm.scaled(pm.width(), pixmap->height());
        }
        delete pixmap;
      }

      // resize icon size
      if (!pm.isNull()) {
        QSize s=folder->iconSize();
        s.setHeight( std::max(s.height(), pm.height()) );
        s.setWidth( std::max(s.width(), pm.width()) );
        folder->setIconSize(s);
      }

      // add tab
      // int index=
      folder->addTab( w, pm, _tn );
      // folder->setTabToolTip(index, _tn + "_tp" );
      // folder->setTabWhatsThis(index, _tn + "_wt" );
    }
    // if multiline label exists => delete unused pixmap
    if (iList.size() && folder->iconSize().width() > 1) {
      QIcon nullIcon;
      for(i=0; i <  iList.size();  ++i ){
        folder->setTabIcon( iList[i], nullIcon );
      }
    }
    connect(folder->tabBar(),SIGNAL(tabMoved(int,int)), this, SLOT(tabMoved(int,int)));
  }
  else{
    MyQStackedWidget *folder = new MyQStackedWidget( this, pw );
    m_folder = folder;

    GuiNamedElementList::iterator iter;
    int n(0);
     for( iter = m_elements.begin(); iter != m_elements.end(); ++iter, ++n ){
      if (!iter->second) continue;
      iter->second->create();
      QWidget *w = iter->second->getQtElement()->myWidget();
      ed |= (int) iter->second->getQtElement()->getExpandPolicy();
      if( w->layout() ) {
        w->layout()->setContentsMargins(0,0,0,0);
 	w->layout()->setSpacing( 0 );
      }
      folder->addWidget( w );
    }
  }

  // set resize policy
  m_folder->setSizePolicy( QSizePolicy(  ed & (int)Qt::Horizontal ? (QSizePolicy::MinimumExpanding) : QSizePolicy::Fixed,
					 ed & (int)Qt::Vertical   ? (QSizePolicy::MinimumExpanding) : QSizePolicy::Fixed ) );

  activatePage( m_activePage < m_elements.size() ? m_activePage : 0, GuiFolderGroup::omit_Default, true );

  connect(m_folder,SIGNAL(currentChanged(int)), this, SLOT(currentChangedCB(int)));
  m_folder->setObjectName( QString::fromStdString(getName()) );
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::manage(){
  BUG_DEBUG("manage");

  assert( m_folder != 0 );
  GuiNamedElementList::iterator iter;
  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if (!(*iter).second) continue;
    iter->second->manage();
  }
  myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
  // acticate Page if it was previously set
  if ( m_activePage ) {
    if( typeid( *m_folder ) == typeid( MyQTabWidget ) ){
      QTabWidget *folder = static_cast<QTabWidget*>( m_folder );
      validateTab( m_activePage );
#if ( __GNUC__ < 5 )
      GuiNamedElementList::iterator iter = m_elements.begin();
      int i = 0;
      while(i++ < m_activePage) ++iter;
#else
      GuiNamedElementList::iterator iter = std::next(m_elements.begin(), m_activePage);
#endif
      if ((*iter).second)
        folder->setCurrentWidget((*iter).second->getQtElement()->myWidget());
    }
    else{
      MyQStackedWidget *folder = static_cast<MyQStackedWidget*>( m_folder );
      folder->setCurrentIndex(  m_activePage );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::unmanage(){
  BUG_DEBUG("unmanage");
  assert( m_folder != 0 );
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::enable(){
  BUG_DEBUG("enable");
  GuiElement::enable();

  GuiNamedElementList::iterator iter;
  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ((*iter).second)
      iter->second->enable();
  }
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::disable(){
  BUG_DEBUG("disable");
  GuiElement::disable();

  GuiNamedElementList::iterator iter;
  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if ((*iter).second)
      iter->second->disable();
  }
}

/* --------------------------------------------------------------------------- */
/* currentChangedCB --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::currentChangedCB( int index ){
  BUG_DEBUG("currentChangedCB@" << this << ": index=" << index);
  if (index == -1) return;  // no tabs exists,  nothing to do

  index =  transferIndexFromMoved(index);
 // special case if a tab was hidden, reset index
 if( !m_hideButtons ){
   GuiNamedElementList::iterator iter= m_elements.begin();
   QWidget *w = static_cast<QTabWidget*>(m_folder)->currentWidget();
   index = 0;
   for (; iter != m_elements.end(); ++iter, ++index) {
     if ((*iter).second && (*iter).second->getQtElement()->myWidget() == w)
       break;
   }
   if (index == m_elements.size()) index = 0;
 }

  BUG_DEBUG("activePage= " << m_activePage);

  setLastWebUpdated();

  // call cloned folder
  std::vector<GuiElement*>::iterator it =  m_clonedFolder.begin();
  for (; it != m_clonedFolder.end(); ++it){
    BUG_DEBUG("activate clone");
    (*it)->getFolder()->activatePage( index, GuiFolderGroup::omit_Default, true );
  }

  if( !m_folder->isVisible() ){
    m_activePage  = getTabPage( index );
    BUG_DEBUG("Folder not visible");
    return;
  }

  if( index < 0 ){
    m_activePage  = -1;
    BUG_DEBUG("no tabs left");
    return;
  }

  if( m_disabled_CB ){
    m_activePage  = getTabPage( index );
    BUG_DEBUG("EXIT: Callback disabled. activePage= " << m_activePage);
    return;
  }

  // an diese Stelle ein GuiUpdate machen
  int currentPageIndex  = index;
#if ( __GNUC__ < 5 )
    GuiNamedElementList::iterator iter = m_elements.begin();
    int i = 0;
    while(i++ < currentPageIndex) ++iter;
#else
  GuiNamedElementList::iterator iter = std::next(m_elements.begin(),currentPageIndex);
#endif

  QSize hs = (*iter).second->getQtElement()->myWidget()->sizeHint();
  if ((*iter).second->LastGuiUpdated() == 0)
    // Tab wird nach unterdrücktem update mit reason_Always oder reason_Cycle aktiviert: update(reason_Always)
    (*iter).second->update( GuiElement::reason_Always );
  else {
    // update(reason) mit LastUpdate des Tabs, da im Tab seit damals kein GuiUpdate gemacht wurde
    GuiQtManager::Instance().overrideLastUpdate( (*iter).second->LastGuiUpdated() );
    (*iter).second->update( GuiElement::reason_Process );
    GuiQtManager::Instance().resetOverrideLastUpdate();
  }
  QSize hsOut = (*iter).second->getQtElement()->myWidget()->sizeHint();
  if (hs != hsOut) {
    (*iter).second->getQtElement()->myWidget()->resize(hs);
  }
  (*iter).second->setLastGuiUpdated( DataPoolIntens::CurrentTransaction() );

  int new_page = index;
  GuiQtFolderUndo undo( this, new_page, m_activePage );

  m_activePage = new_page;

  // get tab name for map command
  if (__desLogger__
#if HAVE_LOG4CPLUS
      .isEnabledFor(log4cplus::INFO_LOG_LEVEL)
#endif
    ) {
    std::string name;
    bool ret = GuiFolderGroup::getTabName( this, m_activePage, name );
    if (ret) {
      DES_INFO(compose("  MAP(%1);", name));
      // PYLOG_INFO(compose(PYLOG_MAPFOLDER, name));
    } else {
      DES_INFO(compose("  MAP(%1:%2);", getName(), index));
      // PYLOG_INFO(compose(PYLOG_MAPFOLDER2, getName(), index));
    }
  }

  if( JobManager::Instance().isRunning() ){
    // Falls noch eine Function aktiv ist, sind keine Eingaben möglich. Diese
    // Erfahrung machen nur schnelle Finger.
      // strange error with xgettext here (Non-ASCII string)
      //printMessage( _("a function is aktiv."), GuiElement::msg_Warning );
    BUG_DEBUG("EXIT: Job is running");
    return;
  }

  GuiFolderGroup::activateFolderGroups( this, m_activePage );
}

/* --------------------------------------------------------------------------- */
/* tabMoved --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::tabMoved( int from, int to ) {
  // if not exist, create a movedElement list
  if (m_movedElements.size() == 0) {
    m_movedElements.insert(m_movedElements.end(), m_elements.begin(), m_elements.end());
  }

  // erase "from"-Element
  GuiNamedElementList::iterator iterF = m_movedElements.begin();
  int f = from;
  while (--f > -1) ++iterF;
  m_movedElements.erase(iterF);

  // copy "to"-Element
  GuiNamedElementList::iterator iterT = m_movedElements.begin();
  int t = to;
  while (--t > -1) ++iterT;
  m_movedElements.insert(iterT, std::pair<std::string, GuiElement *>(iterF->first, iterF->second));
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::serializeXML(std::ostream &os, bool recursive ){
  GuiFolder::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiQtFolder::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiFolder::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool GuiQtFolder::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiFolder::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* getVisibleElement --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::getVisibleElement(GuiElementList& res) {
  BUG_DEBUG("getVisibleElement");

  // Testmode
  if( !AppData::Instance().HeadlessWebMode() ){
    m_activeWebPage = m_activePage;
  }

  int i =  m_activeWebPage;
  GuiNamedElementList::iterator iter = m_elements.begin();
  if( m_hideButtons ){
    while (--i>= 0)
      ++iter;
    if ((*iter).second)
      (*iter).second->getVisibleElement( res );
  }
  else{
    int page=0;
    while ( iter != m_elements.end() ){
      if (page == m_activeWebPage) {
        if ((*iter).second)
          (*iter).second->getVisibleElement( res );
      }
      ++iter;
      ++page;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtFolder::getExpandPolicy() {
  if(!tabExpandable())
    return Qt::Orientations();
  int ed = 0;
  GuiNamedElementList::iterator iter;
  Qt::Orientations edTab = Qt::Orientations();
  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if (!iter->second) continue;
    edTab = iter->second->getQtElement()->getExpandPolicy();
    ed |= (int)edTab;
  }
  return (  Qt::Orientations ) ed;
}

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtFolder::getContainerExpandPolicy() {
  if(!tabExpandable())
    return orient_Default;
  int ed = 0;
  GuiNamedElementList::iterator iter;
  GuiElement::Orientation edTab = orient_Default;
  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if (!iter->second) continue;
    edTab = iter->second->getQtElement()->getContainerExpandPolicy();
    ed |= (int) edTab;
  }
  return (GuiElement::Orientation) ed;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtFolder::getDialogExpandPolicy() {
  if(!tabExpandable())
    return GuiElement::orient_Default;
  int ed = orient_Default;
  GuiNamedElementList::iterator iter;
  int edTab =  orient_Default;
  for( iter = m_elements.begin(); iter != m_elements.end(); ++iter ){
    if (!iter->second) continue;
    edTab = iter->second->getQtElement()->getDialogExpandPolicy();
    ed = ed | edTab;
  }
  return getGuiOrientation((Qt::Orientations) ed);
}

/* --------------------------------------------------------------------------- */
/* getTabIndex --                                                              */
/* --------------------------------------------------------------------------- */

int GuiQtFolder::getTabIndex( int page ){
  BUG_DEBUG("getTabIndex, page=" << page);
  // invalid page index, return 0
  if ( page >= m_elements.size() )
    return 0;

  if( typeid( *m_folder ) == typeid( MyQTabWidget ) ){
    QTabWidget *folder = dynamic_cast<QTabWidget*>(m_folder);
#if ( __GNUC__ < 5 )
    GuiNamedElementList::iterator iter = m_elements.begin();
    int i = 0;
    while(i++ < page) ++iter;
#else
    GuiNamedElementList::iterator iter = std::next(m_elements.begin(), page);
#endif
    if (!iter->second)  return 0;
    QWidget *w = iter->second->getQtElement()->myWidget();
    BUG_DEBUG("EXIT: widget w " << w);
    int index = folder->indexOf( w );
    BUG_DEBUG("EXIT: index " << index);
    return index;
  }

  return page;
}

/* --------------------------------------------------------------------------- */
/* getTabPage --                                                               */
/* --------------------------------------------------------------------------- */

int GuiQtFolder::getTabPage( int index ){
  BUG_DEBUG("getTabPage, index=" << index);
  assert( index < m_elements.size() );

  if( typeid( *m_folder ) != typeid( MyQTabWidget ) ){
    return index;
  }

  QTabWidget *folder = dynamic_cast<QTabWidget*>( m_folder );
  int page = 0;
  GuiNamedElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    if (!it->second) continue;
    QWidget *w = it->second->getQtElement()->myWidget();
    if( folder->indexOf( w ) < 0)
      continue;
    if( --index < 0) {
      BUG_DEBUG("page " << page);
      return page;
    }
    page++;
  }

  BUG_DEBUG("EXIT: no page found");
  return -1;
}

/* --------------------------------------------------------------------------- */
/* getTabWidget --                                                             */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtFolder::getTabWidget(int page, std::string& tapeLabel) {
  // invalid page index, set to 0
  if ( page >= m_elements.size() )
    page = 0;
#if ( __GNUC__ < 5 )
  GuiNamedElementList::iterator iter = m_elements.begin();
  int i = 0;
  while(i++ < page) ++iter;
#else
  GuiNamedElementList::iterator iter = std::next(m_elements.begin(), page);
#endif
  if (!iter->second)  return 0;
  QWidget *w = iter->second->getQtElement()->myWidget();
  tapeLabel = iter->first;
  return w;
}

/* --------------------------------------------------------------------------- */
/* validateTab --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::validateTab( int page ){
  BUG_DEBUG("validateTab, page=" << page);

   // add tab if previously removed
  std::string label;
  QTabWidget *folder = static_cast<QTabWidget*>(m_folder);
  QWidget *w = getTabWidget(page, label);
  if (!w) return;
  int index = folder->indexOf( w );
  if( index == -1 ){
    // get visible page index
    int pageVisible(0), i(0);
    for(GuiNamedElementList::iterator iter = m_elements.begin();
        iter != std::next(m_elements.begin(), page); ++iter, ++i ){
      if (getTabIndex(i) != -1)
        ++pageVisible;
    }

    BUG_DEBUG("insert tab with label '" << label << "'");
    m_disabled_CB = true;
    w->show();
    folder->insertTab( pageVisible, w, QString::fromStdString(label) );
    m_disabled_CB = false;
    BUG_DEBUG("inserted");
  }
  else{
    BUG_DEBUG("tab index is now " << index);
    w->show();
  }
}

/* --------------------------------------------------------------------------- */
/* writeSettings --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::writeSettings() {
  if(AppData::Instance().HeadlessWebMode())
    return;
  int currentPageIndex = -1;
  // get active tab
  if(!m_folder){
    currentPageIndex = m_activePage;
  }else
  if( m_hideButtons ){
    MyQStackedWidget *ws = static_cast<MyQStackedWidget*>(m_folder);
    currentPageIndex = ws->currentIndex();
  } else {
    currentPageIndex = static_cast<QTabWidget*>(m_folder)->currentIndex();
    currentPageIndex = transferIndexToMoved(currentPageIndex);  // maybe tabs were moved
  }

  // write settings
  QSettings *settings = GuiQtManager::Settings();
  settings->beginGroup( QString::fromStdString("Folder") );
  QString _tmp = QString::fromStdString(getName());
  if ( currentPageIndex >= 0 )
    settings->setValue(_tmp + ".activeTabIndex", currentPageIndex);
  settings->endGroup();
}

/* --------------------------------------------------------------------------- */
/* readSettings --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::readSettings(){
  BUG_DEBUG("readSettings");
  if( AppData::Instance().HeadlessWebMode() )
    return;

  //  assert(m_folder);
  QSettings *settings = GuiQtManager::Settings();
  settings->beginGroup( QString::fromStdString("Folder") );
  int currentPageIndex = 0;
  QString _tmp = QString::fromStdString(getName());
  int act_page =  settings->value(_tmp + ".activeTabIndex", currentPageIndex ).toInt();
  if (act_page && act_page != m_activePage && act_page < m_elements.size()) {
    activatePage( act_page, GuiFolderGroup::omit_Default, false );
    BUG_DEBUG("Page " << act_page << " activated");
  }
  else if (act_page != m_activePage)  {
    // not created yet, save active page
    m_activePage = act_page;
    BUG_DEBUG("Page " << act_page << " activated (saved)");
  }
  settings->endGroup();
}

/* --------------------------------------------------------------------------- */
/* isFolderClonedBy --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiQtFolder::isFolderClonedBy(GuiFolder* folder) {
  std::vector<GuiElement*>::iterator it = m_clonedFolder.begin();
  for (; it != m_clonedFolder.end(); ++it)
    if ((*it)->getFolder() == folder)
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* transferIndexFromMoved --                                                   */
/* --------------------------------------------------------------------------- */

int GuiQtFolder::transferIndexFromMoved( int index) {
  if( typeid( *m_folder ) == typeid( MyQTabWidget ) ){
    if (m_movedElements.size() == 0) {  // movedElemList is empty => no transfer
      return index;
    }

    // look in elemtList and return this index
#if ( __GNUC__ < 5 )
    GuiNamedElementList::iterator iter = m_elements.begin();
    int ii = 0;
    while(ii++ < index) ++iter;
#else
    GuiNamedElementList::iterator iter= std::next(m_elements.begin(), index);
#endif
    int i=0;
    std::string label = iter->first;
    for( iter = m_elements.begin(); iter != m_elements.end(); ++iter, ++i ){
      if ( iter->first == label) {
	return i;
      }
    }
  }
  return index;
}

/* --------------------------------------------------------------------------- */
/* transferIndexToMoved --                                                     */
/* --------------------------------------------------------------------------- */

int GuiQtFolder::transferIndexToMoved( int index) {
  if( typeid( *m_folder ) == typeid( MyQTabWidget ) ){
    if (m_movedElements.size() == 0) {  // movedElemList is empty => no transfer
      return index;
    }

    // look in movedElemtList and return this index
#if ( __GNUC__ < 5 )
    GuiNamedElementList::iterator iter = m_elements.begin();
    int ii = 0;
    while(ii++ < index) ++iter;
#else
    GuiNamedElementList::iterator iter=  std::next(m_elements.begin(), index);
#endif
    int i=0;
    std::string label = iter->first;
    for( iter = m_movedElements.begin(); iter != m_movedElements.end(); ++iter, ++i ){
      if ( iter->first == label) {
        return i;
      }
    }
  }
  return index;
}

/* --------------------------------------------------------------------------- */
/* getHiddenIndex --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtFolder::getHiddenIndex( int& page ){
  BUG_DEBUG("getHiddenIndex" << this << ": page=" << page);
  page =  transferIndexFromMoved(page);
  BUG_DEBUG("getHiddenIndex transfer_" << this << ": page=" << page);

  // special case if a tab was hidden, reset index
  if( !m_hideButtons ){
    GuiNamedElementList::iterator iter= m_elements.begin();
    int index = 0;
    for (; iter != m_elements.end() && index <= page; ++iter, ++index) {
      if (isHiddenPage(index))
        ++page;
    }
  }
  BUG_DEBUG("getHiddenIndex result: " << this << ": page=" << page);
}
