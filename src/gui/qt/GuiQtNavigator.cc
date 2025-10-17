
#include <QTreeWidget>
#include <QMimeData>
#include <QHeaderView>
#include <QScrollBar>
#include <algorithm>    // std::find

#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "xfer/XferDataItem.h"

#include "gui/GuiFactory.h"
#include "gui/GuiSeparator.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtNavRoot.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/GuiQtListView.h"
#include "gui/qt/GuiQtDiagram.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtDialogCompare.h"

static const char* folder_closed_xpm[]={
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

static const char* folder_open_xpm[]={
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"};

static const char * file_xpm []={
    "16 16 7 1",
    "# c #000000",
    "b c #ffffff",
    "e c #000000",
    "d c #404000",
    "c c #c0c000",
    "a c #ffffc0",
    ". c None",
    "................",
    ".........#......",
    "......#.#a##....",
    ".....#b#bbba##..",
    "....#b#bbbabbb#.",
    "...#b#bba##bb#..",
    "..#b#abb#bb##...",
    ".#a#aab#bbbab##.",
    "#a#aaa#bcbbbbbb#",
    "#ccdc#bcbbcbbb#.",
    ".##c#bcbbcabb#..",
    "...#acbacbbbe...",
    "..#aaaacaba#....",
    "...##aaaaa#.....",
    ".....##aa#......",
    ".......##......."};

// ---------------------------------------------------------------------------
// Constructor --
// ---------------------------------------------------------------------------
GuiQtNavigator::GuiQtNavigator( const std::string & name, GuiNavigator::Type navType )
  : GuiQtElement( 0, name ), GuiNavigator(navType)
  , m_compareModeMenu( this )
  , m_headerMenu( 0 )
  , m_navView( 0 ) {
  m_folderClosed = new QPixmap( folder_closed_xpm );
  m_folderOpen = new QPixmap( folder_open_xpm );
  m_file = new QPixmap( file_xpm );
}

// ---------------------------------------------------------------------------
// Destructor --
// ---------------------------------------------------------------------------
GuiQtNavigator::~GuiQtNavigator(){
  delete m_file;
  m_file = 0;

  delete m_folderOpen;
  m_folderOpen = 0;

  delete m_folderClosed;
  m_folderClosed = 0;

  delete m_navView;
  m_navView = 0;
}


/* --------------------------------------------------------------------------- */
/* doEndOfWork --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtNavigator::doEndOfWork( bool error ){
  GuiNavigator::doEndOfWork( error ); // call base method incl. !!! GuiUpdate

  // after function call and gui Update maybe a popup menu call is requested
  if (!m_menuRequestedPosition.isNull()) {
    menuRequested(m_menuRequestedPosition);
    m_menuRequestedPosition = QPoint();
  }
}

// ---------------------------------------------------------------------------
// getCurrentNode --
// ---------------------------------------------------------------------------
GuiNavElement *GuiQtNavigator::getCurrentNode(){
  QTreeWidgetItem *item = m_navView->currentItem();
  if (item) {
    GuiQtNavElement *node = static_cast<MyQListViewItem*>(item)->parent();
    return node;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// repaint --
// ---------------------------------------------------------------------------
void GuiQtNavigator::repaint(){
}

// ---------------------------------------------------------------------------
// addAction --
// ---------------------------------------------------------------------------
void GuiQtNavigator::addActions(){
}

// ---------------------------------------------------------------------------
// Type --
// ---------------------------------------------------------------------------
GuiElement::ElementType GuiQtNavigator::Type() {
  switch(getNavigatorType()) {
  case type_Diagram:
    return type_NavDiagram;
  case type_IconView:
    return type_NavIconView;
  case type_Default:
  default:
    return type_Navigator;
  }
}

// ---------------------------------------------------------------------------
// create --
// ---------------------------------------------------------------------------
void GuiQtNavigator::create(){
  QWidget *parent = getParent()->getQtElement()->myWidget();
  //  assert( parent != 0 );

  m_navView = new NavigatorView( this, parent );
  m_navView->setAcceptDrops(true);
  m_navView->setRootIsDecorated( true );
  m_navView->setDropIndicatorShown(true);
//   m_navView->setDefaultRenameAction( QTreeWidget::Accept );
  m_navView->setSelectionMode( isMultipleSelection() ? QTreeWidget::ExtendedSelection : QTreeWidget::SingleSelection );

  // ich setze die gewünschte Groesse
  // UND lasse expand property vertikal zu
  int w=0, h=0;
  if (!isDefaultSize()) {
    getSize(w,h);
  }
  // set resize policy
  // vertical expanding, horizontal expanding only for diagram type
  QSizePolicy::Policy defExpandPolicyHorz = isExpandable() ? QSizePolicy::MinimumExpanding :
    QSizePolicy::Fixed;
  m_navView->setSizePolicy( QSizePolicy((getNavigatorType()== type_Diagram) ?
					(QSizePolicy::MinimumExpanding) : (defExpandPolicyHorz),
					QSizePolicy::MinimumExpanding)  );

  // set font
  QFont font = m_navView->font();
  m_navView->setFont( QtMultiFontString::getQFont( "@list@", font ) );
  m_navView->setContextMenuPolicy ( Qt::CustomContextMenu ); // sonst wird kein Signal ausgeloest!!!
  if (getNavigatorType() == type_Default) {
    QTreeWidget *tw = dynamic_cast<QTreeWidget*>(getNavView()->getContentWidget());

    connect(tw,SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(setExpanded(QTreeWidgetItem*)));
    connect(tw,SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(setCollapsed(QTreeWidgetItem*)));
    if ( isCompareMode() ) {
      connect(tw,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(activated(QTreeWidgetItem*,int)));
    } else {
      connect(tw,SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(activated(QTreeWidgetItem*,int)));
    }
    connect(tw, SIGNAL(itemSelectionChanged()), this, SLOT(selected()));
    connect(tw,SIGNAL(customContextMenuRequested(const QPoint&) ),
	    this, SLOT(menuRequested(const QPoint&) ) );
    connect(m_navView->header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(headerMenuRequested()));
  }
//   connect(m_navView,SIGNAL(contextContextMenuRequested( QTreeWidgetItem *, const QPoint& , int ) ),
// 	  this, SLOT(menuRequested( QTreeWidgetItem *, const QPoint &, int ) ) );
  ColsIterator iter;
  int w_cols = 0;
  if( m_cols.empty() ){
    std::cout << "GuiQtNavigator should never happen ==> empty Navigator\n";
    //    assert( false );
  }
  m_navView->setColumnCount(m_cols.size());
  QTreeWidgetItem* headerItem = m_navView->headerItem();
  if (headerItem) {
    int i = 0;
    for(iter = m_cols.begin(); iter != m_cols.end(); ++i, ++iter ){
      headerItem->setText(i, QString::fromStdString((*iter)->getLabel()));
    }
  }
  if( getPopupMenu() )
    installPopupMenu();

  // set Column Width (default size and stretch)
  int i = 0;
  int charlen = m_navView->fontMetrics().horizontalAdvance( 'w' ) + 0; // Korrektur + 2
  double maxwidth = 0;
  std::vector<int> cws;
  for(; i < m_cols.size(); ++i ){
    int cw = charlen * (1+std::abs(m_cols[i]->getWidth()));
    maxwidth += cw;
    cws.push_back(cw);
  }
  maxwidth+=m_cols.size()*8; // Korrektur der Breite
  double fac = w!=0 ? 1/*w/maxwidth*/ : 1;
  std::vector<int>::iterator it;
  for(i = 0, it = cws.begin(); it != cws.end(); ++it, ++i ) {
    if (m_navView->header()) {
      m_navView->header()->resizeSection( i,  (int) floor((*it)*fac+0.5) );
      // if label is to short => set tooltip
      if (std::abs(m_cols[i]->getWidth()*fac) < m_navView->headerItem()->text(i).size())
	m_navView->headerItem()->setToolTip(i, m_navView->headerItem()->text(i));
    }
  }
  // Setzen der Breite
  if (isDefaultSize())
    m_navView->setMinimumWidth( maxwidth+200 );
  else {
    m_navView->setMinimumSize( w, h );
    if (getNavigatorType() == type_Default && !isExpandable())
      m_navView->setMaximumWidth( w );
  }
  setDebugTooltip();

  // set sort column option
  if (getNavigatorType() == type_Default) {
    QTreeWidget *tw = dynamic_cast<QTreeWidget*>(getNavView()->getContentWidget());

    // looking for a column with sort option
    int sortCol = 0;
    for (; sortCol < getNavigator()->getCols().size(); ++sortCol) {
      if (getNavigator()->getCols()[sortCol]->style() == GuiNavigator::columnStyle_Sort)
	break;
    }
    if (sortCol < getNavigator()->getCols().size()) { // found a column with sort option
      tw->setSortingEnabled ( true );
      tw->sortByColumn(sortCol, Qt::AscendingOrder);
    }
  }
}

//=============================================================================//
// installPopupMenu                                                            //
//=============================================================================//
void GuiQtNavigator::installPopupMenu(){
  getPopupMenu()->resetMenuPost();
  getPopupMenu()->getElement()->getQtElement()->create();
}


// ---------------------------------------------------------------------------
// manage --
// ---------------------------------------------------------------------------
void GuiQtNavigator::manage(){
  if( m_navView )
    myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

// ---------------------------------------------------------------------------
// getSize --
// ---------------------------------------------------------------------------
void GuiQtNavigator::getSize( int &width, int &height ){
  GuiNavigator::getSize(width, height);
}

// ---------------------------------------------------------------------------
// getTreeItemKey --
// (generate a unique key of the tree item)
// ---------------------------------------------------------------------------
std::string GuiQtNavigator::getTreeItemKey(QTreeWidgetItem *item) {
  // get list of parentItems labels
  std::ostringstream path;
  QTreeWidgetItem *itm = item;
  // type and label of Node
  path << item->data(0, Qt::UserRole).toString().toStdString() << ";" << itm->text(0).toStdString() << ";";
  // is parent, get child id
  if (itm->parent())
    path << itm->parent()->indexOfChild(itm) << ":";
  //  get parent nodes
  while ((itm = itm->parent())) {
    path << item->data(0, Qt::UserRole).toString().toStdString() << ";";
    if (itm->parent())
      path << itm->parent()->indexOfChild(itm) << ";";
    path << itm->text(0).toStdString();
  }
  return path.str();
}

// ---------------------------------------------------------------------------
// update --
// ---------------------------------------------------------------------------
void GuiQtNavigator::update( UpdateReason ur){
  BUG( BugGui, "GuiQtNavigator::update" );
  if( ur == GuiElement::reason_Cancel || m_updating ){
    BUG_MSG( "reason_FieldInput || m_updating" );
    return;
  }

  QWidget *rootWidget = 0;
  bool bSorted = false;
  RootsVector::iterator rootIter;
  std::map<QString, std::string> seletectItems;
  int oldHorzSBPos = 0,  oldVertSBPos = 0;

  // falls das selectItem verloren geht, merken wir uns schon mal die Position
  bool isBlocked(false);
  if (getNavigatorType() == type_Default) {
    QTreeWidget *tw = dynamic_cast<QTreeWidget*>(getNavView()->getContentWidget());
    if ( tw->isSortingEnabled() ) {
      bSorted = true;
      tw->setSortingEnabled(false); // performance reason
    }
    oldVertSBPos = tw->verticalScrollBar()->sliderPosition();
    oldHorzSBPos = tw->horizontalScrollBar()->sliderPosition();

    // von selektierten items merken wir uns einen generierten key
    QList<QTreeWidgetItem *> selItems = tw->selectedItems();
    for (QList<QTreeWidgetItem *>::iterator it = selItems.begin(); it != selItems.end(); ++it) {
      if (*it) {
	seletectItems[ (*it)->text(0) ] = getTreeItemKey((*it));
      }
    }

    // block signals
    isBlocked = tw->signalsBlocked();
    tw->blockSignals(true);
  }

  // save old selected items
  if (getNavigatorType() == type_Diagram && getNavView()) {
    dynamic_cast<GuiQtDiagram*>(getNavView()->getContentWidget())->saveItemSelection();
  }

  bool updated = false;
  //
  // Default navigator type
  // Weil im QTreeWidget die Reihenfolge der TopLevelItems
  // trotz entsprechender Funktionen beliebig ist,
  // müssen wir auch bei kleinen Updates immer einen kompletten Update machen
  if (getNavigatorType() == type_Default) {
    for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter){
      if((*rootIter)->isUpdated())
	updated = true;
    }
  }
  for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter){
    BUG_MSG( (*rootIter)->getName() );
    if( (updated && getNavigatorType() == type_Default)
        || (*rootIter)->isUpdated()
        || ur == GuiElement::reason_Cycle
        || ur == GuiElement::reason_Always ){
      BUG_MSG( (*rootIter)->getName() << " isUpdated" );
      updated = true;
      ResetLastWebUpdated();  // reason_Always for webtens


      // clear all
      if( (*rootIter)->getNode()!= 0 )
        (*rootIter)->getNode()->deleteNodes();

      // type_IconView => clear view icons
      if ( getNavigatorType() == type_IconView ) {
        GuiQtListView *lv = dynamic_cast<GuiQtListView*>(getNavView()->getContentWidget());
        lv->removeAll();
      }

      if( (*rootIter)->getNode()!= 0 ){
        m_updating = true;
        rootWidget = m_navView;
        int newCount = 0, actualCount = 0;
        (*rootIter)->getChildrenCount( actualCount, newCount );
        if( actualCount < newCount ){
          BUG_MSG( "ChildrenCount has changed" );
          if( (*rootIter)->isOpen() )
            freeze();
          if( actualCount == 1 && (*rootIter)->autoLevel() ){
            // Die Struktur wird wieder angezeigt, also erhalten die Elemente
            // einen neuen parent, nämlich die Struktur
            (*rootIter)->getNode()->newParent();
          }
          (*rootIter)->addChildren( actualCount );
        }
        else if( actualCount > newCount ){
          if((*rootIter)->isOpen() )
            freeze();
          (*rootIter)->removeChildren( actualCount - newCount );
          if( newCount == 1 && (*rootIter)->autoLevel() ){
            // Die Struktur wird nicht mehr angezeigt, also erhält das verbleibende
            // Element diesen Node als Parent
            (*rootIter)->getNode()->getQtNavElement()->moveChildren();
            (*rootIter)->getNode()->getQtNavElement()->setTreeItem( 0 );
          }
        }
        (*rootIter)->update( ur );
      }
      else{
        createTree( *rootIter, (rootIter - m_roots.begin()) );
      }
    }
  }
  if (updated) {
    updateTree();
  }

  // update Connections
  if (getNavigatorType() == type_Diagram) {
    if (getNavView()) {
      GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavView()->getContentWidget());
      d->refreshConnections();
      d->initShortcuts();
      d->restoreItemSelection();
    }
  }

  // falls das selectItem verloren geht, aus alter Position neu setzen
  if (getNavigatorType() == type_Default) {
    QTreeWidget *tw = dynamic_cast<QTreeWidget*>(getNavView()->getContentWidget());
    if (bSorted) {
      tw->setSortingEnabled(true); // performance reason
    }
    std::map<QString, std::string>::iterator sit = seletectItems.begin();
    for (; sit != seletectItems.end(); ++sit) {
      QList<QTreeWidgetItem *> selItems = tw->findItems(sit->first, Qt::MatchExactly|Qt::MatchRecursive);
      if (selItems.size()) {
	int idx = 0;
	while (idx < selItems.size()) {
	  if (getTreeItemKey(selItems[idx]) == sit->second)
	    break;
	  ++idx;
	}
	if (idx < selItems.size()) {
	  // reselection
	  tw->setCurrentItem( selItems[idx] , 0, QItemSelectionModel::Select);
	} else {
	  // clear selection
	  //	  tw->clearSelection ();
	}
      }
    }
    // reset slider positions
    tw->verticalScrollBar()->setSliderPosition(oldVertSBPos);
    tw->horizontalScrollBar()->setSliderPosition(oldHorzSBPos);

    // block signals zuruecksetzen
    tw->blockSignals(isBlocked);

    // reset collapsed folder
    std::vector<QString>::iterator it = m_collapseItems.begin();
    for ( ; it != m_collapseItems.end(); ++it) {
      QList<QTreeWidgetItem *> items = tw->findItems((*it), Qt::MatchExactly|Qt::MatchRecursive);
      for (int i = 0; i < items.size(); ++i)
	tw->collapseItem( items[i] );
    }
  }
  // reset last DragMoveSelect item (only used in TreeWidget)
  if (m_navView) m_navView->resetLastDragMoveSelectItem();
}

/* --------------------------------------------------------------------------- */
/* hasChanged --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtNavigator::hasChanged(TransactionNumber trans, XferDataItem* xfer, bool show) {
#if 0
  BUG( BugGui, "GuiQtNavigator::hasChanged" );
  RootsVector::iterator rootIter;
  for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter) {
    if ( !xfer || (xfer && xfer->isAncestorOf( (*rootIter)->getDataItem() )) )
      if ((*rootIter)->hasChanged( trans, xfer, show )) {
	//      if( (*rootIter)->isUpdated() ) {
	BUG_EXIT("root item has Changed");
	return true;
      }
  }
#endif
  return false;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtNavigator::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtNavigator::setIndex",name << " =[" << inx << "]");

  RootsVector::iterator rootIter;
  for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter){
    (*rootIter)->setIndex( name, inx );
  }

  if(myWidget()) // if created!!!
    update( reason_Always );

}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtNavigator::acceptIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtNavigator::acceptIndex",name << " =[" << inx << "]");

  // if not created => return true
  if(myWidget() == NULL)
    return true;

  RootsVector::iterator rootIter;
  for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter){
    if( !(*rootIter)->acceptIndex( name, inx ) ){
      return false;
    }
  }
  return true;
}

// ---------------------------------------------------------------------------
// myWidget --
// ---------------------------------------------------------------------------
QWidget *GuiQtNavigator::myWidget(){
  return m_navView;
}

// ---------------------------------------------------------------------------
// grabShortcutAction --
// ---------------------------------------------------------------------------
void GuiQtNavigator::grabShortcutAction(std::string& accelerator_key, QAction * act) {
  BUG_PARA(BugGui,"GuiQtNavigator::grabShortcutAction","shortcut" << accelerator_key << "]");
  // insert
  int id = m_navView->grabShortcut(QString::fromStdString(accelerator_key), Qt::WidgetWithChildrenShortcut);
  m_grabActions[id] = act;
  connect(act, SIGNAL(destroyed(QObject*)),
	  this, SLOT(slot_destroyedAction(QObject*)));
}

// ---------------------------------------------------------------------------
// callGrabShortcutAction --
// ---------------------------------------------------------------------------
void GuiQtNavigator::callGrabShortcutAction(const QKeySequence& key_seq) {
  BUG_PARA(BugGui,"GuiQtNavigator::callGrabShortcutAction", "shortcut" << key_seq.toString().toStdString() << "]");
  std::map<int, QAction*>::iterator it = m_grabActions.begin();
  for (; it != m_grabActions.end(); it++) {
    if ((*it).second->shortcut() == key_seq) {
      (*it).second->trigger();
      return; // work done
    }
  }
}


// ---------------------------------------------------------------------------
// createTree --
// ---------------------------------------------------------------------------
void GuiQtNavigator::createTree( Root * const root, int rootIdx ){
  root->setNode( new GuiQtNavRoot( root, this ) );
  GuiNavigator::createTree( root, rootIdx );
}


void GuiQtNavigator::setExpanded( QTreeWidgetItem *item ){
  if( item ) {
    // auskommentiert amg 2013-09-10
    // if( static_cast<MyQListViewItem*>(item)->isFolder() )
    //   item->setIcon( 0, QIcon(*m_folderOpen) );
    // else
    //   item->setIcon( 0, QIcon(*m_file) );
    // remove from collapse list
    if ( !item->treeWidget()->signalsBlocked() ) {
      std::vector<QString>::iterator it = std::find(m_collapseItems.begin(), m_collapseItems.end(),
						    item->text(0));
      if (it != m_collapseItems.end())
	m_collapseItems.erase(it);
    }
  }
}

void GuiQtNavigator::setCollapsed( QTreeWidgetItem *item ){
  if( item ) {
    // auskommentiert amg 2013-09-10
    // if( static_cast<MyQListViewItem*>(item)->isFolder() )
    //   item->setIcon( 0, QIcon(*m_folderClosed) );
    // else
    //   item->setIcon( 0, QIcon(*m_file) );
    // add to collapse list
    if (item->text(0).size()) {
      std::vector<QString>::iterator it = std::find(m_collapseItems.begin(), m_collapseItems.end(),
						    item->text(0));
      if (it == m_collapseItems.end())
	m_collapseItems.push_back(item->text(0));
    }
  }
}

void GuiQtNavigator::activated( QTreeWidgetItem *item, int col ){
  if(item) {
    if ( isCompareMode() ) {
      std::vector<XferDataItem*> xferNodes;
      GuiQtNavElement *node = static_cast<MyQListViewItem*>(item)->parent();

      // Liste aller xfers
      QList<QTreeWidgetItem *> selItems =  m_navView->selectedItems();
      // multiple selection
      if (selItems.size() > 0 ) {

	for (QList<QTreeWidgetItem *>::iterator it = selItems.begin();
	     it !=  selItems.end(); ++it) {
	  if( (*it) == 0 ) continue;

	  // folder interessieren uns nicht!!
	  MyQListViewItem *viewItem = dynamic_cast<MyQListViewItem*>( (*it) );
	  if( viewItem == 0 || viewItem->isFolder() ) continue;

	  // uns interessiert nur das XferDataItem
	  GuiQtNavElement *node = viewItem->parent();
	  xferNodes.push_back( node->getDataItem() );
	}
      }

      // QtDialogCompare
      QtDialogCompare& dialog = QtDialogCompare::Instance();
      std::vector<std::string> labels;
      dialog.setHeaders( m_roots[0]->getDataItem()->getUserAttr()->getTagList() );
      dialog.build(xferNodes, labels);
      dialog.exec();
    } else
      static_cast<MyQListViewItem*>(item)->activated();
  }
}

void GuiQtNavigator::selected(){
  // wier blockieren weitere Signals, da es sonst evtl. zu viele Events sind
  bool isBlocked(isBlocked = getNavView()->signalsBlocked());
  getNavView()->blockSignals(true);
  QTreeWidgetItem *item = m_navView->currentItem();
  if( m_navView->selectedItems().size() ){
    if(item)
      static_cast<MyQListViewItem*>(item)->selected();
  }
  // block signals zuruecksetzen
  getNavView()->blockSignals(isBlocked);
}

void GuiQtNavigator::slot_destroyedAction(QObject * obj){
  std::map<int, QAction*>::iterator it = m_grabActions.begin();
  for (; it != m_grabActions.end(); it++) {
    if ((*it).second == obj) {
      m_grabActions.erase( it );
      break;
    }
  }
}

bool GuiQtNavigator::menuRequested( const QPoint & pos ){
  // is a function already running?
  if (countTrigger()) {
    // save position for a later call
    m_menuRequestedPosition = pos;
    return false;
  }
  if( m_navView->selectedItems().size() > 1){
    return false;
  }
  QTreeWidgetItem *item = m_navView->currentItem();
  if (item) {
    GuiQtNavElement *node = static_cast<MyQListViewItem*>(item)->parent();
    if( node != 0 ) {
      if (node->popup(pos))
	return true;
    }
  }
  if( getPopupMenu() != 0 ) {
    getPopupMenu()->popup();
    return true;
  }
  return false;
}

bool GuiQtNavigator::dropMenuRequested( QTreeWidgetItem *item, const QPoint & pos, std::string& data ){
  if (item) {
    GuiQtNavElement *node = static_cast<MyQListViewItem*>(item)->parent();
    if( node != 0 ) {
      if (node->popupDrop(pos, data))
	return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getHeaderMenu --                                                            */
/* --------------------------------------------------------------------------- */

GuiPopupMenu* GuiQtNavigator::getHeaderMenu() {
  if (!m_headerMenu) {
    GuiMenuToggle *toggle = 0;

    m_headerMenu = dynamic_cast<GuiQtPopupMenu*>(GuiFactory::Instance()->createPopupMenu( this ));
    m_headerMenu->setTitle( _("Compare Mode Menu") );
    m_headerMenu->resetMenuPost();
    m_headerMenu->attach( GuiFactory::Instance()->createSeparator(m_headerMenu)->getElement() );

    CompareModeMenu::MyEvent *event = new CompareModeMenu::MyEvent( output_Equality );
    toggle = GuiFactory::Instance()->createMenuToggle( m_headerMenu, &m_compareModeMenu, event );
    toggle->setLabel( _("&Equality") );
    m_headerMenu->attach( toggle->getElement() );
    m_compareModeMenu.m_equal = toggle;

    event = new CompareModeMenu::MyEvent( output_Inequality );
    toggle = GuiFactory::Instance()->createMenuToggle( m_headerMenu, &m_compareModeMenu, event );
    toggle->setLabel( _("&Inequality") );
    m_headerMenu->attach( toggle->getElement() );
    m_compareModeMenu.m_inequal = toggle;

    event = new CompareModeMenu::MyEvent( output_Complete );
    toggle = GuiFactory::Instance()->createMenuToggle( m_headerMenu, &m_compareModeMenu, event );
    toggle->setLabel( _("&Complete") );
    m_headerMenu->attach( toggle->getElement() );
    m_compareModeMenu.m_all = toggle;

    m_headerMenu->getElement()->create();
    m_compareModeMenu.setMenuButtons( getOutputCompareMode() );
  }

  return m_headerMenu;
}

bool GuiQtNavigator::headerMenuRequested( const QPoint & pos ){
  if ( !isCompareMode() )
    return false;
  GuiPopupMenu *menu = getHeaderMenu();
  if (menu)
    menu->popup();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getVisibleDataPoolValues --                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtNavigator::getVisibleDataPoolValues( GuiValueList& vmap ) {
  XferDataItem           *newItem = 0;
  XferDataItem *xfer =  m_roots[0]->getDataItem();

  XferDataItemIndex      *index = 0;

  XferDataItemIndex *xferIndex = xfer->getLastIndex();

  createWebTree( xfer, true, vmap );
}

/* --------------------------------------------------------------------------- */
/* createWebTree --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtNavigator::createWebTree( XferDataItem *xfer, bool root, GuiValueList& vmap ){
  if( !root ){
    xfer->newDataItemIndex();
  }
  XferDataItemIndex *xferIndex = xfer->getLastIndex();
  if( !root ){
    xferIndex->setLowerbound( 0 );
    xfer->setDimensionIndizes();
  }

  int maxIndex = xferIndex->getDimensionSize( xfer->Data() );

  std::string s, value;
  for( int i = 0; i < maxIndex; ++i ){
    xferIndex->setLowerbound( i );
    xfer->setDimensionIndizes();

    DataReference::StructIterator structIter = xfer->Data()->begin();
    while(structIter != xfer->Data()->end()){
      DataReference *newRef = structIter.NewDataReference();
      if( newRef->nodeName() == "name" ){
	newRef->GetValue( s );
	std::string::size_type posA, posE;
	posA = s.find_first_not_of(" ");
	if (posA == std::string::npos)  posA = 0;
	posE = s.find_last_not_of(" ");
	if (posE == std::string::npos)  posE = s.length();
	value = s.substr(posA, posE+1)+"@false";
	vmap.insert(  GuiValueList::value_type(newRef->fullName(true), value ));
	//	std::cout << newRef->fullName(true) << " = " << value << std::endl;
      }
      ++structIter;
    }
    structIter = xfer->Data()->begin();
    while(structIter != xfer->Data()->end()){
      DataReference *newRef = structIter.NewDataReference();
      if( newRef->getDataType() == DataDictionary:: type_StructVariable ){
	XferDataItem *newXfer = new XferDataItem( *xfer, newRef );
	createWebTree( newXfer, false, vmap );
	delete newXfer;
      }
      ++structIter;
    }
  }
}

/* -------------------------------------------------------------------------- */
/* writeFile --                                                               */
/* -------------------------------------------------------------------------- */

void GuiQtNavigator::writeFile( const std::string &filename, bool bPrinter ){
  BUG_PARA( BugGui, "FileStream::writeFile","File '" << filename << "'" );

  // only support diagram, others are using default in GuiQtElement
  if (getNavigatorType() == type_Diagram) {

    QWidget* w = myWidget();
    if (!w) {
      create();
      update(reason_Always);
      // GuiFactory::Instance()->showDialogInformation(this, compose(_("GuiElement '&s' not yet created!"), filename),
      // 						  _("Please create it first."));
      // endFileStream( JobAction::job_Aborted );
      // return;
    }

    if (getNavView()) {
      GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavView()->getContentWidget());
      d->copy(filename);
    }
    endFileStream( JobAction::job_Ok );
  } else GuiQtElement::writeFile(0, filename, bPrinter);  // background transparent
}

/* --------------------------------------------------------------------------- */
/*  CompareModeMenu::ToggleStatusChanged --                                    */
/* --------------------------------------------------------------------------- */
void GuiQtNavigator::CompareModeMenu::ToggleStatusChanged( GuiEventData *event ){
  if( event == 0 )
    return;
  MyEvent *myEvent = static_cast<MyEvent *>(event);
  eOutputCompareMode style;
  if( myEvent->m_modeType == output_Equality )
    style = output_Equality;
  if( myEvent->m_modeType == output_Inequality )
    style = output_Inequality;
  if( myEvent->m_modeType == output_Complete )
    style = output_Complete;
  m_nav->setOutputCompareMode( style );
  m_nav->getElement()->update(GuiElement::reason_Always);
  setMenuButtons(style);
}

/* --------------------------------------------------------------------------- */
/* CompareModeMenu::setMenuButtons --                                          */
/* --------------------------------------------------------------------------- */
void GuiQtNavigator::CompareModeMenu::setMenuButtons( GuiNavigator::eOutputCompareMode style ) {
  switch( style ) {
  case GuiNavigator::output_Equality:
    m_equal->setToggleStatus( true );
    m_inequal->setToggleStatus( false );
    m_all->setToggleStatus( false );
    break;
  case GuiNavigator::output_Inequality:
    m_equal->setToggleStatus( false );
    m_inequal->setToggleStatus( true );
    m_all->setToggleStatus( false );
    break;
  case GuiNavigator::output_Complete:
    m_equal->setToggleStatus( false );
    m_inequal->setToggleStatus( false );
    m_all->setToggleStatus( true );
    break;
  default:
    break;
  }
}

/* -------------------------------------------------------------------------- */
/* getDiagramConnections --                                                   */
/* -------------------------------------------------------------------------- */
void GuiQtNavigator::getDiagramConnections(std::vector<std::string>& vecA,
                                           std::vector<std::string>& vecB,
                                           std::vector<ConnectionAttr>& attr) {
  GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavView()->getContentWidget());
  d->getConnections(vecA, vecB, attr);
}

/* -------------------------------------------------------------------------- */
/* getDiagramSelectItems --                                                   */
/* -------------------------------------------------------------------------- */
void GuiQtNavigator::getDiagramSelectItems(std::vector<std::string>& select_items) {
  GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavView()->getContentWidget());
  d->getItemSelection(select_items);

}

/* -------------------------------------------------------------------------- */
/* getDiagramConnectionRatio --                                                   */
/* -------------------------------------------------------------------------- */
double GuiQtNavigator::getDiagramConnectionRatio() {
  GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavView()->getContentWidget());
  return d->getDiagramConnectionRatio();

}

/* -------------------------------------------------------------------------- */
/* addColumnsExtension --                                                     */
/* -------------------------------------------------------------------------- */

void GuiQtNavigator::addColumnsExtension(int c) {
  if ( !isCompareMode() )
    return;
  // compareMode
  UserAttr* attr = m_roots[0]->getDataItem()->getUserAttr();
  QTreeWidgetItem* headerItem = getNavView()->headerItem();
  if (getNavigator()->getCols().size() >= c ) {
    for (int i=0; i < attr->getTagList().size(); ++i) {
      std::ostringstream os;
      if ( i < attr->getTagList().size()) {
	os << attr->getTagList()[ i ];
      }
      headerItem->setText(i+1, QString::fromStdString(os.str()));
      getNavigator()->addColumn( os.str(), os.str(), 10, -1,0, false);
    }
  } else
    while (getNavigator()->getCols().size() < c && headerItem->columnCount() < c) {
      std::ostringstream os;
      // compareMode
      if ( headerItem->columnCount()-1 < attr->getTagList().size()) {
	os << attr->getTagList()[ headerItem->columnCount()-1 ];
      } else
	os << "Value: " << headerItem->columnCount();
      headerItem->setText(headerItem->columnCount(), QString::fromStdString(os.str()));
      getNavigator()->addColumn( os.str(), os.str(), 10, -1,0, false);
    }
  m_navView->header()-> resizeSections(  QHeaderView::ResizeToContents );
}
