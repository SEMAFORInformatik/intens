
#include <QLineEdit>
#include <qlistview.h>
#include <qsettings.h>

#include "utils/Debugger.h"
#include "xfer/XferDataItem.h"

#include "app/DataPoolIntens.h"
  #include "datapool/DataElement.h"
  #include "datapool/DataItem.h"

#include "gui/qt/GuiQtNavTextfield.h"
#include "gui/qt/GuiQtNavSetfield.h"
#include "gui/qt/GuiQtNavigator.h"
#include "gui/qt/GuiQtNavStructFolder.h"
#include "gui/qt/GuiQtNavNode.h"
#include "gui/qt/GuiQtListView.h"
#include "gui/qt/GuiQtDiagram.h"
#include "gui/qt/GuiQtDiagramItem.h"

//---------------------------------------------------------------------
// Constructor / Destructor
//---------------------------------------------------------------------
GuiQtNavNode::GuiQtNavNode ( GuiNavElement *parent
			 , GuiNavigator *nav
			 , XferDataItem *item )
  : GuiQtNavElement( nav, parent, item ){

  // NavNodes für die StructurVariablen kreieren
  if( getNavigator()->isCompareMode() ) {
    std::ostringstream os;
    os << item->getName();
    XferDataItemIndex *index= item->getLastIndex();
    if (index && index->getValue() > 0) {
      os << "[" << index->getValue() << "]";
    }

    const DataItem* ditem= dynamic_cast<const DataItem*>(item->Data()->getItem());
    std::string color =  ditem && ditem->isAttributeSet( DATAisEqual ) ?
      "#0000ef" :"#f16040";
    addLabel( os.str(), color );

    if(item->getDataType() == DataDictionary::type_StructVariable) {
      createStructVariableChildren();
      return;
    }
  } else {
    addLabel( item->getName() );
  }

  // ab hier sind keine Wildcards vom NavRootElement mehr möglich
  // ab nun werden ChildNodes erzeugt, welche keine Wildcard
  // haben dürfen
  if (item->getNumOfWildcards()) {
    while (item->getNumOfWildcards()) {
      item->getDataItemIndexWildcard( 1 )->resetWildcard();
    }
    item->setDimensionIndizes();
  }

  // Textfields kreieren
  createTextfields( nav, item );
}

GuiQtNavNode::~GuiQtNavNode(){
  BUG_PARA( BugGui, "GuiQtNavNode::~GuiQtNavNode", this );

  if( getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram) {
    GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
    GuiQtDiagramPixmapItem* item = dynamic_cast<GuiQtDiagramPixmapItem*>(getDiagramItem());
    if (d && item)  {
 //      d->refreshPixmapItem(this, item);
    }
  }

  std::vector<GuiQtNavTextfield *>::iterator iter;
  for( iter = m_textfields.begin(); iter != m_textfields.end(); ++iter )
    delete *iter;
  delete getDataItem();
}

//---------------------------------------------------------------------
// createStructVariableChildren
//---------------------------------------------------------------------
void GuiQtNavNode::createStructVariableChildren( XferDataItem * const item){
  BUG( BugGui, "GuiQtNavNode::createStructVariableChildren" );
  assert( item == 0 );
  DataReference *newRef = 0;
  DataReference::Ptr ref_ptr = DataPool::newDataReference( *( getDataItem()->Data() ) );
  DataReference &dataRef = *ref_ptr;
  XferDataItem *newItem = 0;
  XferDataItemIndex *newIndex = 0;
  DataReference::StructIterator structIter;
  // Schleife über die Structur
  // uns interessieren alle nicht StructurVariable
  // die als einfacher NavNode erstellt werden
  for( structIter = dataRef.begin();structIter != dataRef.end();++structIter ){
    newRef = structIter.NewDataReference();
    if ( newRef != 0 ){
      if( newRef->GetDict()->getDataType() != DataDictionary::type_StructVariable) {
	newItem = new XferDataItem( *( getDataItem() ), newRef );
	newItem->newDataItemIndex();
	newIndex = newItem->getLastIndex();
	newIndex->setLowerbound( 0 );
	newItem->setDimensionIndizes();
	const DataItem* ditem= dynamic_cast<const DataItem*>(newItem->Data()->getItem());

	if (getNavigator()->isOutputCompareMode(GuiNavigator::output_Complete) ||
	    // Ungleich
	    ditem && getNavigator()->isOutputCompareMode(GuiNavigator::output_Inequality) &&
	    ditem && ditem->isAttributeSet( DATAisDifferent ) ||
	    // Gleichheit
	    ditem && getNavigator()->isOutputCompareMode(GuiNavigator::output_Equality) &&
	    ditem->isAttributeSet( DATAisEqual ) )	{
	  addNode(newItem);
	}
	else
	  delete newRef;
      }
      else
	delete newRef;
    }
  }
  createTextfields( getNavigator(), getDataItem() );
}

//---------------------------------------------------------------------
// createChildren
//---------------------------------------------------------------------
void GuiQtNavNode::createChildren( XferDataItem * const item){
  assert( item == 0 );
  UserAttr *userAttr = 0;
  DataReference *newRef = 0;
  DataReference::Ptr ref_ptr = DataPool::newDataReference( *( getDataItem()->Data() ) );
  DataReference &dataRef = *ref_ptr;
  XferDataItem *newItem = 0;
  XferDataItemIndex *newIndex = 0;
  GuiNavElement *newNode;
  std::string newNodeName;

  // iteration
  DataReference::StructIterator structIter;
  for( structIter = dataRef.begin();structIter != dataRef.end();++structIter ){
    newRef = structIter.NewDataReference();
    if ( newRef != 0 ){
      if( newRef->GetDict()->getDataType() == DataDictionary::type_StructVariable &&
	  getNavigator()->isNodeValid( newRef )                                   ){
	newItem = new XferDataItem( *( getDataItem() ), newRef );
	newItem->newDataItemIndex();
	newIndex = newItem->getLastIndex();
	newIndex->setLowerbound( 0 );
	newItem->setDimensionIndizes();
	userAttr = newItem->getUserAttr();
	newNodeName = userAttr->Label( newRef );
	newNode = static_cast<GuiNavElement*>( addNode( newNodeName ));
	newNode->createChildren( newItem );
      }
      else{
	delete newRef;
      }
    }
  }
}

//---------------------------------------------------------------------
// isVisible
//---------------------------------------------------------------------
bool GuiQtNavNode::isVisible() const{
  return true;
}

//---------------------------------------------------------------------
// isFolder
//---------------------------------------------------------------------
bool GuiQtNavNode::isFolder(){
  if( isLastLevel() )
    return false;
  return true;
}

//---------------------------------------------------------------------
// isLastLevel
//---------------------------------------------------------------------
bool GuiQtNavNode::isLastLevel() const{
  int lastLevel = 0;
  if( getLastLevel( lastLevel ) ){
    if( getLevel() >= lastLevel )
      return true;
  }
  else if( getNodes().empty() )
    return true;
  return false;
}

//---------------------------------------------------------------------
// getChildrenCount
//---------------------------------------------------------------------
bool GuiQtNavNode::getChildrenCount( int &actualCount, int &newCount ){
  BUG( BugGui, "GuiQtNavNode::getChildrenCount" );
  newCount = getDataItem()->getNumDimensions();
  actualCount = getNodes().size();
  BUG_MSG( "newCount : " );
  return true;
}

//---------------------------------------------------------------------
// update
//---------------------------------------------------------------------
void GuiQtNavNode::update( GuiElement::UpdateReason ur ){
  BUG_PARA( BugGui, "GuiQtNavNode::update", getLabel()[0].first );
  if( getDataItem()->isDataItemUpdated(GuiManager::Instance().LastGuiUpdate(), true )
      || ur == GuiElement::reason_Cycle
      || ur == GuiElement::reason_Always ){
    BUG_MSG( "DataItem is updated" );
    NavElemVector::const_iterator it;
    const NavElemVector& nodes = getNodes();
    for( it = nodes.begin(); it != nodes.end(); ++it ){
      // it sind alles StructFolders
      int actualCount = 0, newCount = 0;
      // wieviele Elemente dieser Struktur sind vorhanden ?
      (*it)->getChildrenCount( actualCount, newCount );
      if( newCount == 0 && getHideEmptyFolder() ){
        (*it)->newParent();
      }
      if( actualCount < newCount ){
        if( isOpen() )
          getNavigator()->freeze();
        if( actualCount == 1 && getAutoLevel() ){
          // Die Struktur wird wieder angezeigt, also erhalten die Elemente
          // einen neuen parent, nämlich die Struktur
          (*it)->newParent();
        }
        (*it)->addChildren( actualCount );
      }
      else if( actualCount > newCount ){
        if( isOpen() )
          getNavigator()->freeze();
        (*it)->removeChildren( actualCount - newCount );
        if( newCount == 1 && getAutoLevel() ){
          // Die Struktur wird nicht mehr angezeigt, also erhält das verbleibende
          // Element diesen Node als Parent
          (*it)->getQtNavElement()->moveChildren( this );
        }
      }
      (*it)->update( ur );
    }
    //    std::string label;
    //    getStructName( getDataItem(), label );
    //    setLabel( label, 0 );
  }
}

//---------------------------------------------------------------------
// runJobFunction
//---------------------------------------------------------------------
void GuiQtNavNode::runJobFunction( JobElement::CallReason reason, XferDataItem *source ){
  BUG_PARA( BugGui, "GuiQtNavNode::runJobFunction", "Object : " << this );
  JobFunction *func = getJobFunction();
  if( func == 0 ){
    return;
  }

  XferDataItem *input = getDataItem();

  if( !getNavigator()->setRunningMode() ) assert( false );

  JobStarter *trigger = getNavigator()->getNewTrigger( func );
  trigger->setReason( reason );
  if( input != 0 ){
    trigger->setDataItem( input );
  }
  if( source != 0 ){
    trigger->setSource( source );
  }
  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // doEndOfWork() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // löscht sich am Ende selbständig.
  // Da wir kein GuiElement sind, überlassen wir diese Arbeit dem Navigator
}

//---------------------------------------------------------------------
// createTextfields4StructVariable
//---------------------------------------------------------------------
void GuiQtNavNode::createTextfields4StructVariable( GuiNavigator *nav, XferDataItem *item ){
  BUG_PARA( BugGui, "GuiQtNavNode::createTextfields4StructVariable", getLabel()[0].first );

  // label NavTextfield
  std::string s=item->userAttr().Label( item->Data() );
  GuiQtNavTextfield* textfield = new GuiQtNavTextfield( getNavigator()->getElement(), s, getLabel()[0].second );
  m_textfields.push_back( textfield );

  // ein Struct NavNode
  if( item->getDataType() != DataDictionary::type_StructVariable ) {
    DataContainer::SizeList dims;
    item->getAllDataDimensionSize(dims);
    int numDims = item->getAllDataDimensionSize(dims);

    // value columns NavTextfield
#if 1
    //    int numDims = item->getNumDimensions();
    if (numDims>1) {
      // bei mehrdimensionalen item, alle indizes auf dem level erzeugen
      for (int ii=1; ii<numDims; ++ii) {
	item->newDataItemIndex( item->getNumberOfLevels()-1 );
	item->setDimensionIndizes();
      }
    }
#endif
    XferDataItemIndex *index = getIndex( item );
    int maxIndex = index->getDimensionSize( item->Data() );
    for( int i = 0; i <  dims[0]; ++i ){
      XferDataItem* newItem = new XferDataItem( *item );
      index = getIndex( newItem );

#if 1
      if ( numDims > 1) {
	// bei mehrdimensionalen item
	newItem->setIndex( newItem->getNumIndexes()-2, i );
      } else
#endif
	index->setLowerbound( i );

      newItem->setDimensionIndizes();
      std::string val;
      if (!newItem->getValue(val) || val.size() > 20) {
	// do not print, ignore
	delete newItem;
	continue;
      }
      textfield = new GuiQtNavTextfield( getNavigator()->getElement(), newItem );
      m_textfields.push_back( textfield );
    }
  }
}

//---------------------------------------------------------------------
// createTextfields
//---------------------------------------------------------------------
void GuiQtNavNode::createTextfields( GuiNavigator *nav, XferDataItem *item ){
  BUG( BugGui, "GuiQtNavNode::createTextfields");
  if( getNavigator()->isCompareMode() ) {
    // wir müssen die Textfields für die StructurVariablen kreieren
    createTextfields4StructVariable( nav, item );
    return;
  }
  if( !( item->getDataType() == DataDictionary::type_StructVariable) )
    return;
  GuiNavigator::ColsVector &cols = nav->getCols();
  // Diese Zeilen unterstützen den alten Syntax, nachdem der Navigator in der Struktur nach einer
  // Variablen "name" sucht. Es sind keine zusätzlichen Spalten möglich
  // Kann später gelöscht werden.
  if( cols.empty() )
    ;//    assert( false );

  GuiNavigator::ColsVector::iterator colsIter;
  for( colsIter = cols.begin(); colsIter != cols.end(); ++colsIter ){
    DataReference *newRef = 0;
    DataReference::StructIterator structIter = item->Data()->begin();
    while(structIter != item->Data()->end()){
      newRef = structIter.NewDataReference();
      UserAttr *userAttr  = static_cast<UserAttr*>( newRef->getUserAttr() );
      if( userAttr->hasTag( (*colsIter)->getTag() ) )
	break;
      delete newRef;
      newRef = 0;
      ++structIter;
    }
    GuiQtNavTextfield *textfield = 0;
    if( newRef != 0 ){
      XferDataItem *newItem = new XferDataItem( newRef );
      XferDataItemIndex      *index;
      newItem->newDataItemIndex();
      index = newItem->getLastIndex();
      index->setLowerbound( 0 );
      newItem->setDimensionIndizes();
      std::string datasetName = newItem->getUserAttr()->DataSetName();
      if( datasetName.empty() )
        textfield = new GuiQtNavTextfield( getNavigator()->getElement(), newItem );
      else{
	textfield = new GuiQtNavSetfield( getNavigator()->getElement(), newItem );
      }
      textfield->setWidth( (*colsIter)->getWidth() );
      textfield->setPrecision( (*colsIter)->getPrecision() );
      if( (*colsIter)->scale() != 0 )
	textfield->setScalefactor( (*colsIter)->scale()->copy() );
      if( (*colsIter)->thousandSep() )
	textfield->setThousandSep();
    }
    m_textfields.push_back( textfield );
  }
}

//---------------------------------------------------------------------
// refreshLabels
//---------------------------------------------------------------------
bool GuiQtNavNode::refreshLabels(){
  const std::vector<stringPair> oldLabels = getLabel();
  std::vector<GuiQtNavTextfield*>::iterator iter;
  clearLabels();

  // add colums, if needed (CompareMode)
  getNavigator()->addColumnsExtension(m_textfields.size());

  // refresh labels
  int col=0;
  for( iter = m_textfields.begin(); iter != m_textfields.end() &&
	 col < getNavigator()->getCols().size(); ++col, ++iter ){
    std::string label, color;

    // if style is pixmap get unformatted value (or Output-Value of DataSet)
    if (getNavigator()->getCols()[col]->style() == GuiNavigator::columnStyle_Pixmap) {
      if( *iter != 0 )
	(*iter)->getValue( label );
    }
    if (label.empty()) {
      if( *iter != 0 ){
	(*iter)->getFormattedValue( label );
      }
    }
    if( *iter != 0 )
      (*iter)->getColor( color );
    addLabel( label, color );
  }
  return oldLabels != getLabel();
}

//---------------------------------------------------------------------
// move
//---------------------------------------------------------------------
void GuiQtNavNode::move( QTreeWidgetItem *i ){
  BUG_PARA( BugGui, "GuiQtNavNode::move", getLabel()[0].first );
  if( myTreeItem() != 0 && i != 0 ){
    int myIdx = myTreeItem()->parent()->indexOfChild(myTreeItem());
    QTreeWidgetItem* myIt =  myTreeItem()->parent()->takeChild(myIdx);
    if (myIt)
      i->addChild( myIt );
  }
}

//---------------------------------------------------------------------
// addNode
//---------------------------------------------------------------------
GuiNavElement *GuiQtNavNode::addNode( XferDataItem *item ){
  BUG_PARA( BugGui, "GuiQtNavNode::addNode", getLabel()[0].first );
  GuiNavElement *newNode = new GuiQtNavNode( this
					     , getNavigator()
					     , item
					     );
  GuiNavElement::addNode( newNode );
  return newNode;
}

GuiNavElement *GuiQtNavNode::addNode( const std::string &label ){
  BUG_PARA( BugGui, "GuiQtNavNode::addNode", getLabel()[0].first );
  GuiNavElement *newNode = new GuiQtNavStructFolder( getNavigator()
						     , this
						     , label );
  GuiNavElement::addNode( newNode );
  return newNode;
}

//---------------------------------------------------------------------
// refresh
//---------------------------------------------------------------------
void GuiQtNavNode::refresh(){
  refreshLabels();
  BUG_PARA( BugGui, "GuiQtNavNode::refresh", getLabel()[0].first );
  if( /*myTree*/hasItem() != 0 && isVisible() ){
    updateWidget();
  }
  else{

    if( myTreeItem() != 0 ){
      moveChildren( getParent() );
      setTreeItem( 0 );
    }
    if( isVisible() ){
      QTreeWidgetItem *parent = 0;
      if( ( parent = getParentTreeItem() )  != 0 ) {
	setTreeItem( new MyQListViewItem( this, parent, getLastTreeChild( parent ) ) );
      } else{
	QTreeWidget *lv=NULL;
	switch (getNavigator()->getNavigatorType()) {
	  case GuiNavigator::type_IconView:
	    {
	      GuiQtListView *lv = dynamic_cast<GuiQtListView*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
	      if (lv) {
		QPixmap icon;
		getPixmap(icon);
		if (!icon.isNull()) {
		  long idx = lv->model()->addItem(icon, this);
		  setModelItemIndex( idx );
		}
	      }
//	      return;
	    }
	    break;


  	  case GuiNavigator::type_Diagram: {
	    GuiQtDiagram *d = 0;
	    if (getNavigator()->getQtNavigator()->getNavView())
	      d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
	    //	    std::cout << d<<"Node Refresh Diagram nodeName["<<getNodeName()<<"] varname["<<getDataItem()->getFullName(true)<<"]!!!!! isFolder["<<isFolder()<<"]\n"<<std::flush;
	    //	    return;
	    std::string type;
	    getType(type);
	    if (d && !isFolder() && type.size())
	      setDiagramItem( d->addPixmapItem(this) );
	    break;
	  }
	  case GuiNavigator::type_Default:
  	  default:
	    {
	      QTreeWidget *lv = 0;
	      if (getNavigator()->getQtNavigator()->getNavView())
		lv= dynamic_cast<QTreeWidget*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
	      if (lv) {
		// special case all parents invisible => get previous root node
		QTreeWidgetItem* parItem =  lv->topLevelItem(lv->topLevelItemCount()-1);
		setTreeItem( new MyQListViewItem( this, lv, parItem) );
		lv->setDragEnabled( true );
	      }
	      break;
	    }
	}
      }
      QAbstractItemView *iv = 0;
      if (getNavigator()->getQtNavigator()->getNavView())
	iv =  dynamic_cast<QAbstractItemView*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
      if (iv)
        iv->setDragEnabled( true );
      setLabels();
      if (myTreeItem()) {
        if (isOpen())
          dynamic_cast<GuiQtNavigator*>(getNavigator())->setExpanded(myTreeItem() );
        else
          dynamic_cast<GuiQtNavigator*>(getNavigator())->setCollapsed(myTreeItem() );
        myTreeItem()->setExpanded(isOpen());
      }
    }

    if( isFolder() )
      refreshChildren();

    // try to find a icon
    QPixmap icon;
    getPixmap(icon);
    if (myTreeItem() && !icon.isNull()) {
      QTreeWidget *tw = dynamic_cast<QTreeWidget*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
      assert(tw);
      myTreeItem()->setIcon(0, QIcon(icon) );

      // increase navigator icon size for greater icon
      QSize iSize = tw->iconSize();
      if (icon.size().width()  > iSize.width())  iSize.setWidth ( icon.size().width()  );
      if (icon.size().height() > iSize.height()) iSize.setHeight( icon.size().height() );
      tw->setIconSize( iSize );
    }
  }
}

//---------------------------------------------------------------------
// updateWidget
//---------------------------------------------------------------------
void GuiQtNavNode::updateWidget(){
  if( isVisible() ){
    if( getNavigator()->getNavigatorType() == GuiNavigator::type_Default &&
	hasNewParent() ){
      assert( getParentTreeItem() != 0 );
      move( getParentTreeItem() );
      newParent( false );
    }
    if( getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram) {
      GuiQtDiagram *d = dynamic_cast<GuiQtDiagram*>(getNavigator()->getQtNavigator()->getNavView()->getContentWidget());
      GuiQtDiagramPixmapItem* item = dynamic_cast<GuiQtDiagramPixmapItem*>(getDiagramItem());
      if (d)  {
	d->refreshPixmapItem(this, item);
      }
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

//---------------------------------------------------------------------
// startRename
//---------------------------------------------------------------------
bool GuiQtNavNode::startRename( int col ){
  int numCols = getTextfields().size();
  assert( numCols > col );
  return getTextfields()[col]->startRename();
}

//---------------------------------------------------------------------
// okRename
//---------------------------------------------------------------------
void GuiQtNavNode::okRename( int col ){
  int numCols = getTextfields().size();
  assert( numCols > col );
  getTextfields()[col]->finalWork( getTreeItem()->text( col ).toStdString() );
}

//---------------------------------------------------------------------
// setValidator
//---------------------------------------------------------------------
void GuiQtNavNode::setValidator( int col ){
  int numCols = getTextfields().size();
  assert( numCols > col );
  assert(false);
//   QLineEdit *editor = dynamic_cast<QLineEdit*>
//     (getItem()->listView()->viewport()->child("qt_renamebox", 0, false));
//   getTextfields()[col]->setValidator( editor );
}

//---------------------------------------------------------------------
// getColors
//---------------------------------------------------------------------
bool GuiQtNavNode::getColors( int column, QColor &background, QColor &foreground ){
  assert( m_textfields.size() > column );
  if( m_textfields[column] != 0 ){
    int dark_fac;
    m_textfields[column]->getTheColor( background, foreground, dark_fac, true );
    return ( background.isValid() && foreground.isValid() );
  }
  return false;
}
