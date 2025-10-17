
#include <qlistview.h>

#include "utils/Debugger.h"
#include "xfer/XferDataItem.h"

#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/GuiQtNavNode.h"
#include "gui/qt/GuiQtListView.h"
#include "gui/qt/GuiQtNavStructFolder.h"

//----------------------------------------------------------------------------
// Constructor / Destructor
//----------------------------------------------------------------------------
GuiQtNavStructFolder::GuiQtNavStructFolder ( GuiNavigator *nav
					     , GuiNavElement *parent
					     , const std::string &label )
  : GuiQtNavElement( nav, parent ){
  addLabel( label );
}

GuiQtNavStructFolder::~GuiQtNavStructFolder(){
  delete getDataItem();
}

//----------------------------------------------------------------------------
// refresh
//----------------------------------------------------------------------------
void GuiQtNavStructFolder::refresh(){
  BUG_PARA( BugGui, "GuiQtNavStructFolder::refresh", getLabel()[0].first );
  assert( getLabel().size() == 1 );

  // disable drag
  if( myTreeItem() && myTreeItem()->flags()|Qt::ItemIsDragEnabled)
    myTreeItem()->setFlags( myTreeItem()->flags()^Qt::ItemIsDragEnabled );

  if( myTreeItem() != 0 && isVisible() ){
    updateWidget();
  }
  else{
    if( myTreeItem() != 0 ){
      moveChildren( getParent() );
      setTreeItem( 0 );
    }
    if( isVisible() ){
	switch (getNavigator()->getNavigatorType()) {
	  case GuiNavigator::type_IconView:
	    break;
	  case GuiNavigator::type_Diagram:
	    //    std::cout << "Struct Refresh Diagram   nodeName["<<getNodeName()<<"] varname["<<getDataItem()->getFullName(true)<<"] !!!!!!\n"<<std::flush;
	    //	    return;
	    break;
	  case GuiNavigator::type_Default:
  	  default:
	    {
	      QTreeWidgetItem *parent = getParentTreeItem();
	      assert( parent != 0 );
	      setTreeItem( new MyQListViewItem( this, parent, getLastTreeChild( parent ) ) );
	      myTreeItem()->setText( 0, QString::fromStdString(getLabel()[0].first) );

              // set foreground color
        if (getLabel()[0].second.size()) {
	        QBrush brush(QColor(getLabel()[0].second.c_str()));
	        myTreeItem()->setForeground( 0, brush );
        }
	      if (isOpen())
          dynamic_cast<GuiQtNavigator*>(getNavigator())->setExpanded(myTreeItem() );
	      else
          dynamic_cast<GuiQtNavigator*>(getNavigator())->setCollapsed(myTreeItem() );
	      myTreeItem()->setExpanded(isOpen());
	    }
	}
    }
    refreshChildren();
  }
  // disable drag
  if( myTreeItem() && myTreeItem()->flags()|Qt::ItemIsDragEnabled)
    myTreeItem()->setFlags( myTreeItem()->flags()^Qt::ItemIsDragEnabled );

}

//----------------------------------------------------------------------------
// createChildren
//----------------------------------------------------------------------------
void GuiQtNavStructFolder::createChildren( XferDataItem * const item ){
  BUG( BugGui, "GuiNavStructFolder::~GuiNavStructFolder" );
  if( item == 0 )
    return;
  setDataItem( item );
  GuiNavElement::createChildren( item );
}

//----------------------------------------------------------------------------
// addNode
//----------------------------------------------------------------------------
GuiNavElement *GuiQtNavStructFolder::addNode( XferDataItem *item ){
  BUG_PARA( BugGui, "GuiQtNavStructFolder::addNode", getLabel()[0].first );
  GuiNavElement *newNode = new GuiQtNavNode( this
					     , getNavigator()
					     , item
					     );
  GuiNavElement::addNode( newNode );
  return newNode;
}

//----------------------------------------------------------------------------
// getChildrenCount
//----------------------------------------------------------------------------
bool GuiQtNavStructFolder::getChildrenCount( int &actualCount, int &newCount ){
  BUG_PARA( BugGui, "GuiQtNavStructFolder::getChildrenCount", getLabel()[0].first );
  newCount = 0;
  actualCount = getNodes().size();
  XferDataItem *newItem = 0;
  XferDataItemIndex *index = getIndex( getDataItem() );
  int maxIndex = index->getDimensionSize( getDataItem()->Data() );
  for( int i = 0; i < maxIndex; ++i ){
    newItem = new XferDataItem( *getDataItem() );
    index = getIndex( newItem );
    index->setLowerbound( i );
    newItem->setDimensionIndizes();
    if( getNavigator()->isNodeValid( newItem ) )
      ++newCount;
    delete newItem;
  }
  BUG_MSG( "actual : " << actualCount << "  new : " << newCount );
  return true;
}

//----------------------------------------------------------------------------
// move
//----------------------------------------------------------------------------
void GuiQtNavStructFolder::move( QTreeWidgetItem *i ){
  assert( false );
//   BUG_PARA( BugGui, "GuiQtNavStructFolder::move", getLabel()[0].first );
//   if( myItem() != 0 && i != 0 ){
//     Q3ListViewItem *parent = myItem()->parent();
//     assert( parent != 0 );
//     parent->takeItem( myItem() );
//     i->insertItem( myItem() );
//   }
}

//---------------------------------------------------------------------
// updateWidget
//---------------------------------------------------------------------
void GuiQtNavStructFolder::updateWidget(){
  BUG_PARA( BugGui, "GuiQtNavStructFolder::updateWidget", getLabel()[0].first );
  if( isVisible() ){
    if( hasNewParent() ){
      assert( getParentTreeItem() != 0 );
      move( getParentTreeItem() );
      newParent( false );
    }

    setLabels();

    if( isFolder() )
      refreshChildren();
  }
  else if( getNodes().size() != 0 ){
    getLastNode()->getQtNavElement()->setTreeItem( myTreeItem() );
    getLastNode()->refresh();
  }
}

//----------------------------------------------------------------------------
// isVisible
//----------------------------------------------------------------------------
bool GuiQtNavStructFolder::isVisible() const{
  BUG( BugGui, "GuiQtNavStructFolder::isVisible" );
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
