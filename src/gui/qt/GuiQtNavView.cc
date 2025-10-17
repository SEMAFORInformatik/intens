
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <QLabel>
#include <QShortcutEvent>
#include <algorithm>
#include <QDrag>

#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/GuiQtNavRoot.h"
#include "gui/qt/GuiQtListView.h"
#include "gui/qt/GuiQtDiagram.h"
#include "gui/qt/GuiQtDiagramScene.h"
#include "gui/qt/GuiQtManager.h"

#include "app/DataPoolIntens.h"
#include "xfer/XferDataItem.h"
#include "utils/JsonUtils.h"

// ---------------------------------------------------------------------------
// NavigatorView class  --
// ---------------------------------------------------------------------------

GuiQtNavigator::NavigatorView::NavigatorView( GuiQtNavigator *nav, QWidget *parent )
  : QStackedWidget( parent )
  , m_treeWidget(0)
  , m_listView(0)
  , m_diagramView(0)
  , m_diagramScene(0)
  , m_lastDragMoveSelectItem(0)
  , m_parent( nav ){
  switch (m_parent->getNavigatorType()) {
    case GuiNavigator::type_IconView:
      m_listView = new GuiQtListView(m_parent);
      addWidget(m_listView);
      break;
    case GuiNavigator::type_Diagram:
      int w, h;
      m_parent->getSize(w, h);
      m_diagramScene = new GuiQtDiagramScene(QRectF(0,0, w, h));
      m_diagramView = new GuiQtDiagram(nav, m_parent->m_roots, m_diagramScene, this);
      addWidget(m_diagramView);
      break;
    case GuiNavigator::type_Default:
      m_treeWidget = new MyQTreeWidget(this);
      addWidget(m_treeWidget);
      break;
    default:
      break;
  }
}


void GuiQtNavigator::NavigatorView::startDrag(Qt::DropActions supportedActions) {
  BUG( BugGui, "GuiQtNavigator::NavigatorView::startDrag");
  QList<QTreeWidgetItem *> selItems = selectedItems();

  QDrag *drag = new QDrag(this);
  std::vector<std::string> varnameList;
  std::vector<bool> isFolderList;
  for (QList<QTreeWidgetItem *>::iterator it = selItems.begin(); it != selItems.end(); ++it) {
    if( (*it) == 0 ) continue;

    XferDataItem *data = static_cast<MyQListViewItem*>(*it)->parent()->getDataItem();
    if( data == 0 ) continue;

    varnameList.push_back( data->getFullName(true) );
    isFolderList.push_back( static_cast<MyQListViewItem*>(*it)->isFolder() );

    // !!! from first item, we create a pixmap
    //    if (it != selItems.begin()) continue;
    if (drag->pixmap().isNull()) {
	QPixmap pixmap ((*it)->icon(0).pixmap(32/*40,4*/));
	if (pixmap.isNull()) {
	  pixmap.fill();

	  // create a label pixmap
	  QLabel lbl;
	  QPalette palette = lbl.palette();
	  palette.setColor( lbl.backgroundRole(), GuiQtManager::Instance().warnBackgroundColor() );
	  palette.setColor( lbl.foregroundRole(), GuiQtManager::Instance().warnForegroundColor() );
	  lbl.setPalette( palette );
	  lbl.setText( QString::fromStdString( dynamic_cast<GuiQtNavElement*>(m_parent->getCurrentNode())->getNodeName()) );
	  lbl.setText( (*it)->text(0) );
	  lbl.setAutoFillBackground(true);
	  pixmap = QPixmap(lbl.sizeHint().width(), lbl.sizeHint().height());
	  lbl.render(&pixmap);
	}

	if (m_parent->getCurrentNode()) {
	  dynamic_cast<GuiQtNavElement*>(m_parent->getCurrentNode())->getPixmap(pixmap);
#if 0
	  // auskommentiert, da unter centos7 und einer X-Verbindung mit einem
	  // nomachine-Client war der Drag/Drop in den Diagrammer nicht möglich
	  // (X11 and DropObject under Mouse causes problems ...)
	  drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
#endif
	  drag->setPixmap(pixmap);
	}
    }
  }

  Json::Value dragJson = ch_semafor_intens::JsonUtils::createDragJsonObject( m_parent->getName(), varnameList, isFolderList );
  QMimeData *mimeData = new QMimeData;
  mimeData->setText( QString::fromStdString( ch_semafor_intens::JsonUtils::value2string(dragJson, true) ) );
  BUG_MSG( "dragTextJson: " <<  ch_semafor_intens::JsonUtils::value2string(dragJson, true) );
  drag->setMimeData(mimeData);
#if  QT_VERSION < 0x040300
  drag->start(Qt::CopyAction);
#else
  drag->exec(Qt::CopyAction);
#endif
}

bool GuiQtNavigator::NavigatorView::event ( QEvent * event ) {
  // special case: Shortcut inside popupmenu
  // we do some selfmade work
  if (event->type() == QEvent::Shortcut) {
    QShortcutEvent *scEvent = dynamic_cast<QShortcutEvent*>(event);
    if (scEvent)
      m_parent->callGrabShortcutAction(scEvent->key());
  }
  return QStackedWidget::event(event);
}

void GuiQtNavigator::NavigatorView::initShortcuts() {
  switch (m_parent->getNavigatorType()) {
    case GuiNavigator::type_IconView:
      break;
    case GuiNavigator::type_Diagram:
      if (m_diagramView)
	m_diagramView->initShortcuts();
      break;
    case GuiNavigator::type_Default:
      break;
    default:
      break;
  }
}

void GuiQtNavigator::NavigatorView::keyPressEvent( QKeyEvent * event ) {
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    m_parent->activated(currentItem(), 0);
  if (m_treeWidget)
    m_treeWidget->MyQTreeWidget::keyPressEvent(event);
}

void GuiQtNavigator::NavigatorView::contextMenuEvent ( QContextMenuEvent * event ) {
      m_parent->menuRequested(event->pos());
}

void GuiQtNavigator::NavigatorView::dragEnterEvent(QDragEnterEvent* event){
  if (event->mimeData()->text().length()) {
    event->acceptProposedAction();
  }
}

void GuiQtNavigator::NavigatorView::dragLeaveEvent(QDragLeaveEvent* event){
  dragMoveSelectItem(0); // unhighlight item
}

void  GuiQtNavigator::NavigatorView::dragMoveSelectItem(QTreeWidgetItem* item) {
  static QBrush lastBrush;
  QBrush brush;
  // highlight new item
  if (item &&  item != m_lastDragMoveSelectItem) {
    brush = item->foreground(0);
    QBrush newBrush(GuiQtManager::Instance().highlightColor());
    item->setForeground(0, newBrush);
  }

  // unhighlight last item
  if (m_lastDragMoveSelectItem && item != m_lastDragMoveSelectItem) {
    try {
      m_lastDragMoveSelectItem->setForeground(0, lastBrush);
    } catch (...) {
      // nop
    }
  }

  // save last item
  m_lastDragMoveSelectItem = item;
  lastBrush=brush;
}

void GuiQtNavigator::NavigatorView::getItemClassName(std::vector<std::string>& itemClassNames, const QMimeData* mimeData) {
  itemClassNames.clear();

  // get ClassName of drag object
  Json::Value dragObject;
  try {
    dragObject = ch_semafor_intens::JsonUtils::parseJson( mimeData->text().toStdString() );
  } catch (ch_semafor_intens::JsonException e) {
    std::cerr << "error message '"<< e.what() <<"' string ('"<< mimeData->text().toStdString() <<"')\n";
    return;
  }

  std::string& key = ch_semafor_intens::JsonUtils::DRAG_OBJECT_VARNAME;
  if (dragObject[ key ].isArray()) {
    for( Json::ValueIterator it = dragObject[key].begin(); it != dragObject[key].end() ; it++ ) {
      DataReference *ref= DataPoolIntens::getDataReference( (*it).asString() );
      if (!ref) continue;
      itemClassNames.push_back( static_cast<UserAttr*>( ref->getUserAttr() )->Classname() );
      delete ref;
    }
    return;
  }
}

void GuiQtNavigator::NavigatorView::dragMoveEvent(QDragMoveEvent* event){
  bool strict = 0; // TODO
  if (!m_treeWidget) {
    return;
  }
  // get ClassName of drag object
  std::vector<std::string> itemClassNames;
  getItemClassName(itemClassNames,  event->mimeData() );

#if QT_VERSION >= 0x060000
  QPoint pos(event->position().toPoint());
#else
  QPoint pos(event->pos());
#endif
  MyQListViewItem *item = dynamic_cast<MyQListViewItem*>(itemAt(pos, true ));
  // if empty (dragMove over empty area), use root (topLevel) item
  // !!! Condition: Equal ClassName

  if (!item) {
    // find a root item with equal ClassName
    for (int i=0; i < m_treeWidget->topLevelItemCount(); ++i) {
      item = dynamic_cast<MyQListViewItem*>(m_treeWidget->topLevelItem(i));
      if (!strict && m_treeWidget->topLevelItemCount() == 1) break;
      if (std::find(itemClassNames.begin(), itemClassNames.end(),
		    item->parent()->getClassName()) != itemClassNames.end())  {
	break;  // found & break
      }
      item = 0;  // not found
    }
  } else {
      if (strict && std::find(itemClassNames.begin(), itemClassNames.end(),
		    item->parent()->getClassName()) == itemClassNames.end())  {
	item = 0;  // not found
      }
  }
  // is drag object text is json object?
  try {
    Json::Value dragObject = ch_semafor_intens::JsonUtils::parseJson( event->mimeData()->text().toStdString() );
  } catch (ch_semafor_intens::JsonException e) {
    item = 0;
  }


  // has item af function or a drop menu?
  if (item) {
    if (item->parent()) {
      GuiQtPopupMenu * menu = item->parent()->getPopupDropMenu();
      if (item->parent()->getJobFunction() || menu != 0 ) {
	dragMoveSelectItem(item); // highlight item
	event->acceptProposedAction();
	return;
      }
    }
  }
  dragMoveSelectItem(0); // unhighlight item
  event->ignore();
}

void GuiQtNavigator::NavigatorView::dropEvent(QDropEvent* event){
  if (!m_treeWidget) {
    return;
  }
#if QT_VERSION >= 0x060000
  QPoint pos(event->position().toPoint());
#else
  QPoint pos(event->pos());
#endif
  MyQListViewItem *item = dynamic_cast<MyQListViewItem*>(itemAt(pos, true ));
  // if empty (dragMove over empty area), use root (topLevel) item
  // !!! Condition: Equal ClassName
  if (!item) {
    // get ClassName of drag object
    std::vector<std::string> itemClassNames;
    getItemClassName( itemClassNames, event->mimeData() );

    // find a root item with equal ClassName
    for (int i=0; i < m_treeWidget->topLevelItemCount(); ++i) {
      item = dynamic_cast<MyQListViewItem*>(m_treeWidget->topLevelItem(i));
      if (std::find(itemClassNames.begin(), itemClassNames.end(),
		    item->parent()->getClassName()) != itemClassNames.end())  {
	break;  // found & break
      }
      item = 0;  // not found
    }
  }

  // process drop Event
  if( item != 0 ){
    QString data = event->mimeData()->text();
    std::string dataStr = data.toStdString();
    dragMoveSelectItem(0); // unhighlight item
    // dropMenu exists ?
#if QT_VERSION >= 0x060000
  QPoint pos(event->position().toPoint());
#else
  QPoint pos(event->pos());
#endif
    if (m_parent->dropMenuRequested(item, pos, dataStr ))
      return;
    if ( data.length() )
      static_cast<MyQListViewItem*>(item)->drop( data );
  }
}

void GuiQtNavigator::NavigatorView::resizeEvent ( QResizeEvent * event ) {
  if (m_diagramView) {
    // size of diagram scene must be set
    QMargins margins(m_diagramView->contentsMargins());
    int left(margins.left());
    int top(margins.top());
    int right(margins.right());
    int bottom(margins.bottom());
    m_diagramScene->setSceneRect ( 0,0, event->size().width()-left-right,
				   event->size().height()-top-bottom );
  }
  QStackedWidget::resizeEvent(event);
}

QSize GuiQtNavigator::NavigatorView::sizeHint() const {
  int w, h;
  m_parent->getSize(w, h);
  return QSize(w, h);
}

    // addition interface methods
QWidget* GuiQtNavigator::NavigatorView::getContentWidget() {
  if (m_treeWidget)
    return m_treeWidget;
  if (m_listView)
    return m_listView;
  if (m_diagramView)
     return m_diagramView;
  return NULL;
}

void GuiQtNavigator::NavigatorView::clearSelection() {
  if (m_treeWidget)
    m_treeWidget->clearSelection();
  if (m_listView)
    m_listView->clearSelection();
  if (m_diagramView && m_diagramView->scene())
	m_diagramView->scene()->clearSelection();
}

void GuiQtNavigator::NavigatorView::setItemSelection(const std::string& id, bool bSelected) {
  if (m_diagramView)
    m_diagramView->setItemSelection(id, bSelected);
}

void GuiQtNavigator::NavigatorView::getItemsSelected(std::vector<std::string>& varnameList) {
  varnameList.clear();
  if (m_treeWidget) {
    QList<QTreeWidgetItem *> selItems = m_treeWidget->selectedItems();
    for (QList<QTreeWidgetItem *>::iterator it = selItems.begin(); it != selItems.end(); ++it) {
      // folder interessieren uns nicht!!
      if( (*it) == 0 || static_cast<MyQListViewItem*>(*it)->isFolder()) continue;

      XferDataItem *data = static_cast<MyQListViewItem*>(*it)->parent()->getDataItem();
      if( data == 0 ) continue;

      varnameList.push_back( data->getFullName(true) );
    }
  }
}

QTreeWidgetItem* GuiQtNavigator::NavigatorView::takeTopLevelItem(int index) {
  if (m_treeWidget)
    return m_treeWidget->takeTopLevelItem(index);
  assert(false);
  return NULL;
}

void GuiQtNavigator::NavigatorView::addTopLevelItem(QTreeWidgetItem* tl) {
  if (m_treeWidget)
    m_treeWidget->addTopLevelItem(tl);
  assert(false);
}

void GuiQtNavigator::NavigatorView::insertTopLevelItem(int rootIdx, QTreeWidgetItem* tl) {
  if (m_treeWidget)
    m_treeWidget->insertTopLevelItem(rootIdx, tl);
  assert(false);
}

void GuiQtNavigator::NavigatorView::setRootIsDecorated ( bool show ) {
  if (m_treeWidget)
    m_treeWidget->setRootIsDecorated(show);
}


void GuiQtNavigator::NavigatorView::setSortingEnabled( bool show ) {
  if (m_treeWidget)
    m_treeWidget->setSortingEnabled(show);
}


void GuiQtNavigator::NavigatorView::setDropIndicatorShown( bool show ) {
  if (m_treeWidget)
    m_treeWidget->setDropIndicatorShown(show);
}

void GuiQtNavigator::NavigatorView::setSelectionMode( QAbstractItemView::SelectionMode mode ) {
  if (m_treeWidget)
    m_treeWidget->setSelectionMode(mode);
}

void GuiQtNavigator::NavigatorView::setColumnCount(int c) {
  if (m_treeWidget)
    m_treeWidget->setColumnCount(c);
}

QTreeWidgetItem* GuiQtNavigator::NavigatorView::headerItem() const {
  if (m_treeWidget)
    return m_treeWidget->headerItem();
  return NULL;
}

QTreeWidgetItem* GuiQtNavigator::NavigatorView::currentItem() const {
  if (m_treeWidget)
    return m_treeWidget->currentItem();
  return NULL;
}

QTreeWidgetItem* GuiQtNavigator::NavigatorView::itemAt(const QPoint& p, bool checkHeader) const {
  QPoint _p(p);
  if (m_treeWidget) {
    if (checkHeader && m_treeWidget->header()->isVisible())
      _p.setY(p.y()-m_treeWidget->header()->size().height());
    return m_treeWidget->itemAt(_p);
  }
  return NULL;
}

QList<QTreeWidgetItem *> GuiQtNavigator::NavigatorView::selectedItems() const {
  if (m_treeWidget)
    return m_treeWidget->selectedItems();
  assert(false);
  return QList<QTreeWidgetItem *>();
}

QHeaderView* GuiQtNavigator::NavigatorView::header () const {
  if (m_treeWidget)
    return m_treeWidget->header();
  return NULL;
}

void GuiQtNavigator::NavigatorView::setMinimumSize(int w, int h) {
  if (m_listView) {
    // set itemview size
    int lw = 6 * QStackedWidget::lineWidth();
    int _w = std::max(w, m_listView->gridSize().width() + 2 * m_listView->spacing() + lw);
    int _h = std::max(h, m_listView->gridSize().height()+ 2 * m_listView->spacing() + lw);
    QStackedWidget::setMinimumSize(_w,_h);
    if (w < h)
      QStackedWidget::setMaximumWidth(_w);
    else
      QStackedWidget::setMaximumHeight(_h);
  } else
    QStackedWidget::setMinimumSize(w,h);
}

void GuiQtNavigator::NavigatorView::setSizePolicy ( QSizePolicy pol) {
  QWidget::setSizePolicy(pol);
  if (m_treeWidget)
    m_treeWidget->setSizePolicy(pol);
  if (m_listView)
    m_listView->setSizePolicy(pol);
  if (m_diagramView)
    m_diagramView->setSizePolicy(pol);

}
