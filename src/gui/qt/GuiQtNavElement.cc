
#include <typeinfo>
#include <limits>
#include <qsettings.h>
#include <qlistview.h>
#include <QGraphicsView>

#include "datapool/DataDictionary.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataItem.h"
#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/GuiQtNavigatorMenu.h"
#include "gui/qt/GuiQtNavElement.h"
#include "gui/qt/GuiQtNavNode.h"
#include "gui/qt/GuiQtListView.h"
#include "gui/qt/GuiQtDiagram.h"
#include "gui/qt/GuiQtDiagramItem.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/GuiFactory.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiSeparator.h"
#include "utils/StringUtils.h"
#include "utils/gettext.h"
#include "utils/JsonUtils.h"

INIT_LOGGER();

GuiQtNavElement::PixmapMap GuiQtNavElement::s_pixmap_map;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtNavElement::GuiQtNavElement ( GuiNavigator *nav,
                                   GuiNavElement *parent,
                                   XferDataItem *item )
  : GuiNavElement( nav, parent, item )
  , m_treeItem( 0 )
  , m_diagramItem(0)
  , m_modelItemIndex(-1)
  , m_menu(0) {
}

GuiQtNavElement::~GuiQtNavElement(){
  delete m_treeItem;
  if (m_diagramItem) {
    GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    if( d ){
      d->scene()->removeItem(m_diagramItem);
    }
  }
  delete  m_diagramItem;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* moveChildren --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtNavElement::moveChildren( GuiNavElement *i ){
  NavElemVector::const_iterator nodesIter = getNodes().begin();
  while( nodesIter != getNodes().end()){
    (*nodesIter)->getQtNavElement()->move( dynamic_cast<GuiQtNavElement*>(i)->myTreeItem() );
    ++nodesIter;
  }
}

/* --------------------------------------------------------------------------- */
/* setLabels --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtNavElement::setLabels(){
  int col = 0;
  std::vector<std::pair<std::string,std::string> >::const_iterator iter;
  GuiQtListView *lv = 0;
  std::string tip;

  getToolTip(getDataItem(), tip);
  if (getNavigator()->getQtNavigator()->getNavView())
    lv = dynamic_cast<GuiQtListView*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
  for( iter = getLabel().begin(); iter != getLabel().end(); ++iter, ++col ){
    if (myTreeItem()) {
      // pixmap column style
      if (col < getNavigator()->getCols().size()){
        if (getNavigator()->getCols()[col]->style() == GuiNavigator::columnStyle_Pixmap) {
          QPixmap icon;
          if( QtIconManager::Instance().getPixmap( (*iter).first, icon ) ){
            myTreeItem()->setData(col, Qt::DecorationRole, icon);
            myTreeItem()->setToolTip( col, QString::fromStdString((*iter).first) );
            continue; // done
          }
        }
      }

      // set Type as UserData
      std::string type;
      getType(type);
      myTreeItem()->setData(col, Qt::UserRole, QString::fromStdString(type));

      // set label and tooltip
      BUG_DEBUG("setLabel: Text = '" << (*iter).first << "'");
      myTreeItem()->setText( col, QString::fromStdString((*iter).first) );
      myTreeItem()->setToolTip( col, QString::fromStdString(tip) );

      // set foreground color
      if (getLabel()[0].second.size()) {
        QBrush brush(QColor(getLabel()[0].second.c_str()));
        myTreeItem()->setForeground( 0, brush );
      }
    }
    else {
      if (lv) {
        lv->model()->setToolTipData(getModelItemIndex(), tip);
      }
      else {
        GuiQtDiagramPixmapItem* item = dynamic_cast<GuiQtDiagramPixmapItem*>(getDiagramItem());
        if (item) {
          item->setToolTip(QString::fromStdString(getLabel()[0].first));
          std::string tip;
          getToolTip(getDataItem(), tip);
          item->setToolTip(QString::fromStdString(tip));
        }
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* setTreeItem --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQtNavElement::setTreeItem( QTreeWidgetItem *item ){
  if( m_treeItem != 0 )
    delete m_treeItem;
  m_treeItem = item;
}

/* --------------------------------------------------------------------------- */
/* setDiagramItem --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtNavElement::setDiagramItem( QGraphicsItem *item ){
  //delete  m_diagramItem;
  if (m_diagramItem) {
    GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    if( d ){
      d->scene()->removeItem(m_diagramItem);
    }
  }

  m_diagramItem = dynamic_cast<GuiQtDiagramPixmapItem*>(item);
  if (!GuiQtDiagram::isCompositePixmap(getDataItem()->getFullName(true)) &&
      !GuiNavElement::isVisible() && m_diagramItem) {
    GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    if (d) {
      d->scene()->removeItem(m_diagramItem);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getDiagramItem --                                                           */
/* --------------------------------------------------------------------------- */

QGraphicsItem *GuiQtNavElement::getDiagramItem(){
  return m_diagramItem;
}

/* --------------------------------------------------------------------------- */
/* setModelItem --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtNavElement::setModelItemIndex(long index){
  m_modelItemIndex = index;
}

/* --------------------------------------------------------------------------- */
/* hasItem --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtNavElement::hasItem(){
  if (m_treeItem || m_diagramItem)
    return true;
  return (m_modelItemIndex > -1);
}

/* --------------------------------------------------------------------------- */
/* updateWidget --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQtNavElement::updateWidget(){
  refreshChildren();
}

/* --------------------------------------------------------------------------- */
/* getParentTreeItem --                                                        */
/* --------------------------------------------------------------------------- */

QTreeWidgetItem *GuiQtNavElement::getParentTreeItem() const{
  if( getParent() == 0 )
    return 0;
  return getParent()->getQtNavElement()->getTreeItem();
}

/* --------------------------------------------------------------------------- */
/* getLastTreeChild --                                                         */
/* --------------------------------------------------------------------------- */

QTreeWidgetItem *GuiQtNavElement::getLastTreeChild( QTreeWidgetItem *item ) const{
  if( !item )
    return 0;
  if (item->childCount())
    return item->child(item->childCount()-1);
  return 0;
}

/* --------------------------------------------------------------------------- */
/* addPopupMenu --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiQtNavElement::addPopupMenu( std::map<std::string
                                  , GuiMenuButtonListener*>& addMenu )
{
  if( getDataItem() == 0 )
    return false;
  DataDictionary *dict = getDataItem() -> getDict();
  if( dict == 0 )
    return false;
  dict = dict->getStructure();
  if( dict == 0 )
    return false;
  UserAttr *attr = static_cast<UserAttr*>( dict -> GetAttr() );
  if( attr == 0 || attr->getPopupMenu() == 0)
    return false;
  m_menu = dynamic_cast<GuiQtNavigatorMenu*>(attr->getPopupMenu()->getElement()->clone()->getQtElement());

  // add additional entries
  if (addMenu.size()) {

    // if menu not exists => create empty navigator menu
    if( m_menu == 0 ) {
      m_menu = new GuiQtNavigatorMenu( getNavigator()->getElement() );
      attr->setPopupMenu(m_menu);
    }
    else{
      m_menu->front( GuiFactory::Instance()->createSeparator(m_menu)->getElement() );
    }

    std::map<std::string, GuiMenuButtonListener*>::iterator it = addMenu.begin();
    for (; it != addMenu.end(); ++it) {
      GuiMenuButton* button =
        GuiFactory::Instance()->createMenuButton( m_menu, (it)->second, 0, true );
      QString lbl = QString::fromStdString( (it)->first );
      if (lbl.contains("@")) {
	QStringList lbls = lbl.split("@");
	button->setLabel( lbls[0].toStdString() );
	if (lbls.size() == 3){
	  button->setAccelerator( lbls[1].toStdString(), lbls[2].toStdString() );
        }
      } else{
	button->setLabel( (it)->first );
      }
      m_menu->front( button->getElement() );
    }
  }
  return m_menu ? true : false;
}

/* --------------------------------------------------------------------------- */
/* popup --                                                                    */
/* --------------------------------------------------------------------------- */

bool GuiQtNavElement::popup(const QPoint& pos, unsigned int idx, bool show) const{
  GuiQtNavigatorMenu *menu = m_menu;
  if( menu == 0 )
    menu = static_cast<GuiQtNavigatorMenu*>(getPopupMenu(idx));
  if( menu == 0 )
    return false;

  // temporary set last index invalid and popup menu
  int lastId = std::numeric_limits<int>::min();
  if ( typeid(*this) != typeid(GuiQtNavNode) ){
    XferDataItemIndex* lastIndex = getDataItem()->getLastIndex();
    if (lastIndex) {
      lastId =  lastIndex->getValue();
      getDataItem()->setIndex(getDataItem()->getNumIndexes()-1, -1);
    }
  }

  menu->create( getNavigator(), getDataItem() );
  if (show) {
    GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    QPoint p = d ? d->getGridPoint(pos) : pos;
    menu->popup( p.x(), p.y());
  }
  else menu->myWidget()->setHidden(true);
  // reset last index
  if ( lastId != std::numeric_limits<int>::min() ){
    getDataItem()->setIndex(getDataItem()->getNumIndexes()-1, lastId);
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getPopupMenu --                                                             */
/* --------------------------------------------------------------------------- */

GuiQtPopupMenu* GuiQtNavElement::getPopupMenu(unsigned int idx) const {
  return dynamic_cast<GuiQtPopupMenu*>( GuiNavElement::getPopupMenu(idx) );
}

/* --------------------------------------------------------------------------- */
/* getPopupDropMenu --                                                         */
/* --------------------------------------------------------------------------- */

GuiQtPopupMenu* GuiQtNavElement::getPopupDropMenu() const {
  return dynamic_cast<GuiQtPopupMenu*>( GuiNavElement::getPopupDropMenu() );
}

/* --------------------------------------------------------------------------- */
/* popupDrop --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiQtNavElement::popupDrop(const QPoint& pos, std::string& data, bool show) const{
  GuiQtNavigatorMenu * menu = static_cast<GuiQtNavigatorMenu*>(getPopupDropMenu());
  if( menu == 0 )
    return false;

  menu->create( getNavigator(), getDataItem() );
  if (show) {
    GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    QPoint p = d ? d->getGridPoint(pos) : pos;

    std::vector<XferDataItem*> sourceList;
    Json::Value dragObject = ch_semafor_intens::JsonUtils::parseJson( data );

    std::string& key = ch_semafor_intens::JsonUtils::DRAG_OBJECT_VARNAME;
    if (dragObject[key].isArray()) {
      for(unsigned int index=0; index<dragObject[key].size();++index) {
	DataReference *ref= DataPoolIntens::getDataReference( dragObject[key][index].asString() );
	if (!ref) continue;
	XferDataItem* source = new XferDataItem( ref );

	// only single source object
	if (dragObject[key].size() == 1) {
	  if( ref != 0 ) {
	    source = new XferDataItem( ref );
	    menu->popup( p.x(), p.y(), source);
	    delete source;
	  }
	  return true;
	}
	// source object list
	sourceList.push_back(source);
      }
      menu->popup( p.x(), p.y(), sourceList);
      std::vector<XferDataItem*>::iterator it = sourceList.begin();
      for (;it != sourceList.end(); ++it) {
	delete (*it);
      }
    }
  }
  else menu->myWidget()->setHidden(true);

  return true;
}

/* --------------------------------------------------------------------------- */
/* getTreeItem --                                                              */
/* --------------------------------------------------------------------------- */

QTreeWidgetItem *GuiQtNavElement::getTreeItem(){
  QTreeWidgetItem* item = myTreeItem();
  if( item != 0 )
    return item;
  return getParentTreeItem();
}

/* --------------------------------------------------------------------------- */
/* isSelected --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiQtNavElement::isSelected(){
  if( myTreeItem() ){
    return myTreeItem()->isSelected();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getPixmap --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtNavElement::getPixmap(QPixmap& icon) {
  IconManager::ICON_TYPE icon_type;
  std::string fn = getIconPixmapFileName(icon_type);
  if (icon_type != IconManager::NONE) {
    QtIconManager::Instance().getPixmap(fn, icon_type, icon);
  }
}

/*=============================================================================*/
/* Constructor / Destructor of MyQListViewItem                                 */
/*=============================================================================*/

MyQListViewItem::MyQListViewItem( GuiQtNavElement *parent,
                                  QTreeWidgetItem *item,
                                  QTreeWidgetItem *after )
  : QTreeWidgetItem( item, after )
  , m_parent( parent ){
}

MyQListViewItem::MyQListViewItem( GuiQtNavElement *parent,
                                  QTreeWidget *view,
                                  QTreeWidgetItem *after )
  : QTreeWidgetItem( view, after )
  , m_parent( parent ){
}

MyQListViewItem::~MyQListViewItem(){
}

/*=============================================================================*/
/* member functions of MyQListViewItem                                         */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* drop --                                                                     */
/* --------------------------------------------------------------------------- */

void MyQListViewItem::drop( const QString &data ){
  m_parent->transferProc( data.toStdString() );
}

/* --------------------------------------------------------------------------- */
/* startRename --                                                              */
/* --------------------------------------------------------------------------- */

void MyQListViewItem::startRename( int col ){
  if( !m_parent->startRename( col ) )
    return;
  assert( false );
//   QTreeWidgetItem::startRename(col);
  m_parent->setValidator( col );
}

/* --------------------------------------------------------------------------- */
/* okRename --                                                                 */
/* --------------------------------------------------------------------------- */

void MyQListViewItem::okRename( int col ){
  assert( false );
//   QTreeWidgetItem::okRename( col );
//   m_parent->okRename( col );
}

/* --------------------------------------------------------------------------- */
/* activated --                                                                */
/* --------------------------------------------------------------------------- */

void MyQListViewItem::activated(){
  m_parent->runJobFunction( JobElement::cll_Activate );
}

/* --------------------------------------------------------------------------- */
/* selected --                                                                 */
/* --------------------------------------------------------------------------- */

void MyQListViewItem::selected(){
  m_parent->runJobFunction( JobElement::cll_Select );
}
