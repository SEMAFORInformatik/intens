
#include <string>

#include <QTreeWidget>

#include "utils/Debugger.h"

#include "gui/qt/GuiQtNavigator.h"
#include "xfer/XferDataItem.h"
#include "gui/qt/GuiQtNavNode.h"
#include "gui/qt/GuiQtNavRoot.h"

INIT_LOGGER();

//---------------------------------------------------------------------------==
// Constructor / Destructor
//---------------------------------------------------------------------------==
GuiQtNavRoot::GuiQtNavRoot( GuiNavigator::Root *root, GuiNavigator *nav )
  : GuiQtNavElement( nav ){
  BUG_PARA( BugGui, "GuiQtNavRoot::GuiQtNavRoot", root->getName() );
  m_lastLevel = root->getLastLevel();
  m_firstStructFolder = root->getFirstStructFolder();
  m_autoLevel = root->autoLevel();
  m_hideEmptyFolder = root->hideEmptyFolder();
  m_rootIndex = -1;
  m_openLevels = root->getOpenLevels();
  addLabel( root->getName() );
}

GuiQtNavRoot::GuiQtNavRoot( GuiNavigator *nav
			, const std::string &label
			, int *lastLevel
			, int *firstStructFolder
			, bool autoLevel
			, int *openLevels
         		, bool hideEmptyFolder)
  : GuiQtNavElement( nav )
  , m_lastLevel( lastLevel )
  , m_firstStructFolder( firstStructFolder )
  , m_autoLevel( autoLevel )
  , m_hideEmptyFolder( hideEmptyFolder )
  , m_rootIndex( -1  )
  , m_openLevels( openLevels ){
  BUG_PARA( BugGui, "GuiQtNavRoot::GuiQtNavRoot", label );
  clearLabels();
}

GuiQtNavRoot::~GuiQtNavRoot(){
  BUG( BugGui, "GuiQtNavRoot::~GuiQtNavRoot" );
  //!!! Das dataItem darf nicht gelöscht werden !!! Es gehört uns nicht !!!}
}
/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQtNavRoot::acceptIndex( const std::string &name, int inx ){
  BUG(BugGui,"GuiQtNavRoot::acceptIndex");
  assert( getDataItem() != 0 );
  if( !getDataItem()->acceptIndex( name, inx ) ){
    BUG_EXIT("nav root item '"<<getDataItem()->getFullName(true)<<"' does not accept index.");
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtNavRoot::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtDataField::setIndex",name << " =[" << inx << "]");
  assert( getDataItem() != 0 );
  assert( inx >= 0 );

  getDataItem()->setIndex( name, inx );
}

//---------------------------------------------------------------------------
// getIndex
//---------------------------------------------------------------------------
XferDataItemIndex *GuiQtNavRoot::getIndex( XferDataItem *item ){
  return item->getDataItemIndexWildcard( 1 );
}

/*=============================================================================*/
/* newParent                                                                   */
/*=============================================================================*/
void GuiQtNavRoot::newParent(){
  BUG( BugGui, "GuiQtNavElement::newParent" );
  NavElemVector::const_iterator nodesIter = getNodes().begin();
  assert( getNodes().size() < 2 );
  if( getNodes().size() ){
    getNodes()[0]->newParent( true );
    getNavigator()->getQtNavigator()->getNavView()->takeTopLevelItem( getNodes()[0]->getQtNavElement()->myTreeItemIndex() );
    //    (*nodesIter)->getQtNavElement()->setItem( 0 );
    ++nodesIter;
  }
}

//------------------------------------------------------------
// moveChildren
//------------------------------------------------------------
void GuiQtNavRoot::moveChildren( QTreeWidgetItem *i ){
  assert( getNodes().size() == 1 );
  QTreeWidgetItem *child = getNodes()[0]->getQtNavElement()->myTreeItem();
  if( child ){
    myTreeItem()->takeChild( getNodes()[0]->getQtNavElement()->myTreeItemIndex() );
    getNavigator()->getQtNavigator()->getNavView()->addTopLevelItem ( child );
  }
}

//---------------------------------------------------------------------------
// getFirstStructFolder
//---------------------------------------------------------------------------
bool GuiQtNavRoot::getFirstStructFolder( int &firstStructFolder ) const{
  if( m_firstStructFolder != 0 ){
    firstStructFolder = *m_firstStructFolder;
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
// getLastLevel
//---------------------------------------------------------------------------
bool GuiQtNavRoot::getLastLevel( int &lastLevel ) const{
  if( m_lastLevel != 0 ){
    lastLevel = *m_lastLevel;
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
// getAutoLevel
//---------------------------------------------------------------------------
bool GuiQtNavRoot::getAutoLevel() const{
  BUG_PARA( BugGui, "GuiQtNavRoot::getAutoLevel", m_autoLevel );
  return m_autoLevel;
}

//---------------------------------------------------------------------------
// getHideEmptyFolder
//---------------------------------------------------------------------------
bool GuiQtNavRoot::getHideEmptyFolder() const{
  BUG_PARA( BugGui, "GuiQtNavRoot::getHideEmptyFolder", m_hideEmptyFolder );
  return m_hideEmptyFolder;
}

//---------------------------------------------------------------------------
// setRootIndex
//---------------------------------------------------------------------------
void GuiQtNavRoot::setRootIndex(int rootIdx) {
  BUG_PARA( BugGui, "GuiQtNavRoot::setRootIndex", rootIdx );
  m_rootIndex = rootIdx;
}

//---------------------------------------------------------------------------
// getOpenLevels
//---------------------------------------------------------------------------
bool GuiQtNavRoot::getOpenLevels( int &openLevels ) const{
  if( m_openLevels != 0 ){
    openLevels = *m_openLevels;
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
// createChildren
//---------------------------------------------------------------------------
void GuiQtNavRoot::createChildren( XferDataItem * const item ){
  if( item == 0 )
    return;
  setDataItem( item );
  GuiQtNavElement::createChildren( item );
}

//---------------------------------------------------------------------------
// getChildrenCount
//---------------------------------------------------------------------------
bool GuiQtNavRoot::getChildrenCount( int &actualCount, int &newCount ){
  actualCount = getNodes().size();
  return false;
}

//---------------------------------------------------------------------------
// addNode
//---------------------------------------------------------------------------
GuiNavElement *GuiQtNavRoot::addNode( XferDataItem *item ){
  BUG_DEBUG("GuiQtNavRoot::addNode :" << item->getFullName(true));
  GuiNavElement *newNode = new GuiQtNavNode( this
					     , getNavigator()
					     , item
					     );
  GuiNavElement::addNode( newNode );
  return newNode;
}


QTreeWidgetItem* lastItem(QTreeWidget *tree, int rootIdx)
{
  // rootIdx == 0 => return 0
  if (rootIdx > -1 && rootIdx < tree->topLevelItemCount())
    return tree->topLevelItem(rootIdx-1);
  else
    return tree->topLevelItem(tree->topLevelItemCount()-1);
//  assert (lastItem != 0);
//   QTreeWidgetItem* item = tree.children(); //firstChild();
//     if (item) {
//         while (item->nextSibling() || item->firstChild()) {
//             if (item->nextSibling())
//                 item = item->nextSibling();
//             else
//                 item = item->firstChild();
//         }
//     }
//     return item;
}

//---------------------------------------------------------------------------
// refresh
//---------------------------------------------------------------------------
void GuiQtNavRoot::refresh(){
  BUG_PARA( BugGui, "GuiQtNavRoot::refresh", getLabel()[0].first );

  assert( getLabel().size() == 1 );
  if( myTreeItem() == 0 && isVisible() ){
    QTreeWidget *listview = dynamic_cast<QTreeWidget*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    if (!listview) {
      return;
    }
    QTreeWidgetItem *item = new MyQListViewItem( this, listview, lastItem(listview, m_rootIndex));
    if (item) {
      item->setText( 0, QString::fromStdString(getLabel()[0].first) );

      // set foreground color
      if (getLabel()[0].second.size()) {
        QBrush brush(QColor(getLabel()[0].second.c_str()));
        item->setForeground( 0, brush );
      }
      setTreeItem( item );
    }
  }
  else if(myTreeItem() != 0 && !isVisible()){
    NavElemVector::const_iterator nodesIter = getNodes().begin();
    while( nodesIter != getNodes().end()){
      QTreeWidgetItem *child = (*nodesIter)->getQtNavElement()->myTreeItem();
      if( child ){
	myTreeItem()->takeChild( (*nodesIter)->getQtNavElement()->myTreeItemIndex() ); //child );
	getNavigator()->getQtNavigator()->getNavView()->insertTopLevelItem( m_rootIndex, child );
	//	getNavigator()->getQtNavigator()->getNavView()->addTopLevelItem( child );
      }
      ++nodesIter;
    }
    setTreeItem( 0 );
  }
  if( myTreeItem() && isVisible() ) {
    QTreeWidget *treeWidget = dynamic_cast<QTreeWidget*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    if (treeWidget) {
      // get top level treeItem and (re)add it
      int idx = treeWidget->indexOfTopLevelItem(myTreeItem());
      QTreeWidgetItem *child = treeWidget->takeTopLevelItem(idx);
      treeWidget->addTopLevelItem(child);
    }
    myTreeItem()->setExpanded(isOpen());
  }
  updateWidget();
}

//----------------------------------------------------------------------------
// isVisible
//----------------------------------------------------------------------------
bool GuiQtNavRoot::isVisible() const{
  BUG( BugGui, "GuiQtNavRoot::isVisible" );
  if( getDataItem() != 0 ){
    DataDictionary *dict = getDataItem() -> getDict();
    if( dict != 0 ){
      if( !dict->getFolderFlag() ){
	return false;
      }
    }
  }
  return GuiNavElement::isVisible();
}
