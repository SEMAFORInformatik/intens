
#include <assert.h>

#include "utils/Debugger.h"

#include "app/DataPoolIntens.h"
#include "job/JobStarter.h"
#include "xfer/XferDataItemIndex.h"
#include "xfer/XferDataItem.h"
#include "gui/GuiNavigator.h"
#include "gui/GuiNavElement.h"
#include "gui/IconManager.h"
#ifdef HAVE_QT
#include <QSettings>
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtNavElement.h"
#include "gui/qt/GuiQtDiagram.h"
#endif
#include "gui/GuiPopupMenu.h"
#include "utils/JsonUtils.h"
#include "utils/StringUtils.h"

#define MAX_NODES 500

INIT_LOGGER();

GuiNavElement::PixmapNameMap GuiNavElement::s_pixmap_name_map;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiNavElement::GuiNavElement ( GuiNavigator *nav
			       , GuiNavElement *parent
			       , XferDataItem *item )
  : m_navigator( nav )
  , m_parent( parent )
  , m_newParentFlag( false )
  , m_dataItem( item ){
  if( item ){
    //    std::cout << item->getName() << std::endl;
  }
}

GuiNavElement::~GuiNavElement(){
  BUG_PARA( BugGui, "GuiNavElement::~GuiNavElement", this );
}

/*=============================================================================*/
/* update                                                                      */
/*=============================================================================*/
void GuiNavElement::update( GuiElement::UpdateReason ur ){
  BUG_PARA( BugGui, "GuiNavElement::update", m_labels[0].first );
  NavElemVector::iterator it;
  for( it = m_nodes.begin(); it != m_nodes.end(); ++it ){
    (*it)->updateNode( ur );
  }
}

/*=============================================================================*/
/* getFirstStructFolder                                                        */
/*=============================================================================*/
bool GuiNavElement::getFirstStructFolder( int &firstStructFolder ) const{
  assert( m_parent != 0 );
  return m_parent->getFirstStructFolder( firstStructFolder );
}

/*=============================================================================*/
/* getLastLevel                                                                */
/*=============================================================================*/
bool GuiNavElement::getLastLevel( int &lastLevel ) const{
  assert( m_parent != 0 );
  return m_parent->getLastLevel( lastLevel );
}

bool GuiNavElement::getAutoLevel() const{
  assert( m_parent != 0 );
  return m_parent->getAutoLevel();
}

bool GuiNavElement::getHideEmptyFolder() const{
  assert( m_parent != 0 );
  return m_parent->getHideEmptyFolder();
}

/*=============================================================================*/
/* getOpenLevels                                                               */
/*=============================================================================*/
bool GuiNavElement::getOpenLevels( int &openLevels ) const{
  assert( m_parent != 0 );
  return m_parent->getOpenLevels( openLevels );
}

/*=============================================================================*/
/* refreshChildren                                                             */
/*=============================================================================*/
void GuiNavElement::refreshChildren(){
  BUG( BugGui, "GuiNavElement::refreshChildren" );
  NavElemVector::iterator nodesIter = m_nodes.begin();
  while( nodesIter != m_nodes.end()){
    (*nodesIter)->refresh();
    ++nodesIter;
  }
}

/*=============================================================================*/
/* createChildren                                                              */
/*=============================================================================*/
void GuiNavElement::createChildren( XferDataItem * const item ){
  BUG( BugGui, "GuiNavElement::createChildren" );
  if( item == 0 )
    return;
  XferDataItem           *newItem = 0;
  XferDataItemIndex      *index = 0;
  GuiNavElement       *newNode = 0;
  XferDataItemIndex *xferIndex = getIndex( item );

  //  assert(xferIndex);
  int maxIndex = xferIndex ? xferIndex->getDimensionSize( item->Data() ) : (item->getLastSize() ? 1 : 0);

  if ( maxIndex > MAX_NODES ) maxIndex = MAX_NODES;
  for( int i = 0; i < maxIndex; ++i ){
    newItem = new XferDataItem( *item );
    index = this->getIndex( newItem );
    if (index)
      index->setLowerbound( i );
    newItem->setDimensionIndizes();
    if( !( m_navigator->isNodeValid( newItem ) ) ){
      delete newItem;
      continue;
    }
    newNode = addNode( newItem );
    assert( newNode != 0 );
    newNode->createChildren();
  }
}

/*=============================================================================*/
/* addChildren                                                                 */
/*=============================================================================*/
void GuiNavElement::addChildren( int actualCount ){
  BUG( BugGui, "GuiNavElement::addChildren" );
  assert( m_dataItem != 0 );
  XferDataItem           *newItem = 0;
  XferDataItemIndex      *index = 0;
  GuiNavElement       *newNode = 0;

  XferDataItemIndex *xferIndex = getIndex( m_dataItem );
  int maxIndex = xferIndex ?  xferIndex->getDimensionSize( m_dataItem->Data() ) : m_dataItem->getLastSize();

  NavElemVector::iterator iter;
  for( iter = m_nodes.begin();iter != m_nodes.end();++iter){
    if( (*iter)->isSelected() ){
      getNavigator()->clearSelection();
    }
  }
  for( int i = actualCount; i < maxIndex; ++i ){
    newItem = new XferDataItem( *m_dataItem );
    index = this->getIndex( newItem );
    if (index)
      index->setLowerbound( i );
    newItem->setDimensionIndizes();
    if( !( m_navigator->isNodeValid( newItem ) ) ){
      delete newItem;
      continue;
    }
    newNode = addNode( newItem );
    newNode->createChildren();

    // no wildcard (and Index) => BREAK
    if (!xferIndex)
      break;
  }
}

/*=============================================================================*/
/* removeChildren                                                              */
/*=============================================================================*/
void GuiNavElement::removeChildren( int count ){
  BUG_PARA( BugGui, "GuiNavElement::removeChildren", count );

  NavElemVector::iterator iter;
  for( iter = m_nodes.begin();iter != m_nodes.end();++iter){
    if( (*iter)->isSelected() ){
      getNavigator()->clearSelection();
    }
  }

  for( int i = 0; i < count; ++i ){
    delete m_nodes.back();
    m_nodes.pop_back();
  }
}

/*=============================================================================*/
/* getStructName                                                               */
/*=============================================================================*/
void GuiNavElement::getStructName( XferDataItem *item, std::string &name ){
  name = "";
  if( item->getDataType() == DataDictionary::type_StructVariable){
    bool doit = true;
    DataReference *newRef = 0;
    GuiNavigator::ColsVector &cols = m_navigator->getCols();
    DataReference::StructIterator structIter = item->Data()->begin();
    while( structIter != item->Data()->end() && doit ){
      newRef = structIter.NewDataReference();
      UserAttr *userAttr  = static_cast<UserAttr*>( newRef->getUserAttr() );
      if( ( cols.empty() ? false : userAttr->hasTag( cols[0]->getTag() ) ) ||
          ( cols.empty() && newRef->nodeName() == "name" )                 ){
	doit = false;
      }
      else{
	delete newRef;
	newRef = 0;
	++structIter;
      }
    }
    if( newRef != 0 )
      newRef->GetValue( name, 0 );
    delete newRef;
  }
}

/*=============================================================================*/
/* getToolTip                                                                  */
/*=============================================================================*/
void GuiNavElement::getToolTip( XferDataItem *item, std::string &tooltip ){
  tooltip = "";
  if( item->getDataType() == DataDictionary::type_StructVariable){
    bool doit = true;
    DataReference *newRef = 0;
    DataReference::StructIterator structIter = item->Data()->begin();
    while( structIter != item->Data()->end() && doit ){
      newRef = structIter.NewDataReference();
      UserAttr *userAttr  = static_cast<UserAttr*>( newRef->getUserAttr() );
      if( ( m_navigator->getToolTipTag().empty() ? false : userAttr->hasTag( m_navigator->getToolTipTag() ) ) )
	doit = false;
      else{
	delete newRef;
	newRef = 0;
	++structIter;
      }
    }
    if ( newRef != 0 && m_navigator->isCompareMode() ) {
      tooltip = newRef->fullName(true);
    }
    else{
      if( newRef != 0 && !newRef->hasWildcards() ){
	newRef->GetValue( tooltip, 0 );
      }
    }
    delete newRef;
  }
}

/*=============================================================================*/
/* getIndex                                                                    */
/*=============================================================================*/
XferDataItemIndex *GuiNavElement::getIndex( XferDataItem *item ){
  return item->getLastIndex();
}

/*=============================================================================*/
/* isVisible                                                                   */
/*=============================================================================*/
bool GuiNavElement::isVisible() const{
  BUG( BugGui, "GuiNavElement::isVisible" );
  int firstLevel = 0;
  if( getHideEmptyFolder() ) {
    if (!m_nodes.size())
      return false;
  }
  if( getAutoLevel() ){
    if( m_nodes.size() == 1 )
      return false;
    if ( getNavigator()->isTransparentFolder( getNodeName() ) )
      return false;
  }
  else if( getFirstStructFolder( firstLevel ) )
    if( getLevel() < firstLevel )
      return false;
  return true;
}

/*=============================================================================*/
/* getNodeName                                                                 */
/*=============================================================================*/
const std::string GuiNavElement::getNodeName() const{
  if( m_dataItem == 0 )
    return m_labels[0].first;
  return m_dataItem->getName();
}

/*=============================================================================*/
/* getClassName                                                                */
/*=============================================================================*/
const std::string GuiNavElement::getClassName() const{
  UserAttr *userAttr = m_dataItem->getUserAttr();
  return userAttr ? userAttr->Classname() : "";
}

/*=============================================================================*/
/* getType                                                                     */
/*=============================================================================*/
bool GuiNavElement::getType(std::string& type ) const{
  bool ret(false);
  DataReference *refType= DataPoolIntens::getDataReference(getDataItem()->Data(), "type");
  XferDataItem* sourceType = 0;
  // try to get type (".type") icon
  if( refType != 0 )
    sourceType = new XferDataItem( refType );
  if (sourceType) {
    ret=sourceType->getValue(type);
    delete sourceType;
  }
  return ret;
}

/*=============================================================================*/
/* isOpen                                                                      */
/*=============================================================================*/
bool GuiNavElement::isOpen(){
  int openLevels = 0;
  if( !(getOpenLevels( openLevels )) )
    return true;
  if( openLevels > ( getLevel() -1 ) )
    return true;
  return false;
}

/*=============================================================================*/
/* getLevel                                                                    */
/*=============================================================================*/
int GuiNavElement::getLevel() const{
  assert( m_dataItem != 0 );
  return m_dataItem->Data()->GetNumberOfLevels();
}

/*=============================================================================*/
/* getRootLevel                                                                */
/*=============================================================================*/
int GuiNavElement::getRootLevel() const{
  if( m_parent != 0 )
    return m_parent->getRootLevel();
  return m_dataItem->Data()->GetNumberOfLevels();
}

/*=============================================================================*/
/* getJobFunction                                                              */
/*=============================================================================*/
JobFunction *GuiNavElement::getJobFunction() const {
  if( m_dataItem == 0 )
    return 0;
  UserAttr *userAttr = m_dataItem->getUserAttr();
  assert( userAttr != 0 );
  return userAttr->getFunction();
}

/*=============================================================================*/
/* getPopupMenu                                                                */
/*=============================================================================*/
GuiPopupMenu *GuiNavElement::getPopupMenu(unsigned int idx) const {
  if( getDataItem() == 0 )
    return 0;
  DataDictionary *dict = getDataItem() -> getDict();
  if( dict == 0 )
    return 0;
  dict = dict->getStructure();
  if( dict == 0 )
    return 0;
  UserAttr *attr = static_cast<UserAttr*>( dict->GetAttr() );
  if( attr == 0 )
    return 0;
  return attr->getPopupMenu(idx);
}

/*=============================================================================*/
/* getPopupDropMenu                                                            */
/*=============================================================================*/
GuiPopupMenu *GuiNavElement::getPopupDropMenu() const {
  if( getDataItem() == 0 )
    return 0;
  DataDictionary *dict = getDataItem() -> getDict();
  if( dict == 0 )
    return 0;
  dict = dict->getStructure();
  if( dict == 0 )
    return 0;
  UserAttr *attr = static_cast<UserAttr*>( dict -> GetAttr() );
  if( attr == 0 )
    return 0;
  return attr->getPopupDropMenu();
}

/*=============================================================================*/
/* transferProc                                                                */
/*=============================================================================*/
void GuiNavElement::transferProc( const std::string &data ){
  XferDataItem *source = 0;
  std::vector<XferDataItem*> sourceList;
  Json::Value dragObject;
  try {
    dragObject = ch_semafor_intens::JsonUtils::parseJson( data );
  } catch (...) {
    std::cerr << "invalid json string '"<< data <<"'\n";
    return;
  }

  std::string& key = ch_semafor_intens::JsonUtils::DRAG_OBJECT_VARNAME;
  if (dragObject[key].isArray()) {
    for(unsigned int index=0; index<dragObject[key].size();++index) {
      DataReference *ref= DataPoolIntens::getDataReference( dragObject[key][index].asString() );
      if (!ref) continue;
      source = new XferDataItem( ref );
      // only single source object =>  call function with single source
      if (dragObject[key].size() == 1) {
	runJobFunction( JobElement::cll_Drop, source );
	delete source;
	return;
      }
      // source object list
      sourceList.push_back(source);
    }
  }

  // call function with sourceList
  if (sourceList.size())
    runJobFunction( JobElement::cll_Drop, sourceList );

  // delete
  std::vector<XferDataItem*>::iterator it = sourceList.begin();
  for (;it != sourceList.end(); ++it) {
    delete (*it);
  }

}

/*=============================================================================*/
/* runJobFunction                                                              */
/*=============================================================================*/
void GuiNavElement::runJobFunction( JobElement::CallReason reason, XferDataItem *source, int xpos, int ypos ){
  BUG( BugGui, "GuiNavElement::runJobFunction" );
  // !! GuiNavNode hat seine eigene runJobFunction !!
  JobFunction *func = getJobFunction();
  if( func == 0 ){
    return;
  }

  if( !m_navigator->setRunningMode() ) assert( false );

  JobStarter *trigger = m_navigator->getNewTrigger( func );
  trigger->setReason( reason );
  trigger->setDiagramXPos( xpos );
  trigger->setDiagramYPos( ypos );

  if( source != 0 ){
    trigger->setSource( source );
  }
  trigger->setDataItem( m_dataItem );

  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // doEndOfWork() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // löscht sich am Ende selbständig.
  // Da wir kein GuiElement sind, überlassen wir diese Arbeit dem Navigator
}

/*=============================================================================*/
/* runJobFunction                                                              */
/*=============================================================================*/
void GuiNavElement::runJobFunction( JobElement::CallReason reason,
				    const std::vector<XferDataItem*>& sourceList, int xpos, int ypos ) {
  BUG( BugGui, "GuiNavElement::runJobFunction" );
  // !! GuiNavNode hat seine eigene runJobFunction !!
  JobFunction *func = getJobFunction();
  if( func == 0 ){
    return;
  }

  if( !m_navigator->setRunningMode() ) assert( false );

  JobStarter *trigger = m_navigator->getNewTrigger( func );
  trigger->setReason( reason );
  trigger->setDiagramXPos( xpos );
  trigger->setDiagramYPos( ypos );

  if( sourceList.size() ){
    trigger->setSource( sourceList );
  }
  trigger->setDataItem( m_dataItem );

  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // doEndOfWork() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // löscht sich am Ende selbständig.
  // Da wir kein GuiElement sind, überlassen wir diese Arbeit dem Navigator
}

/*=============================================================================*/
/* runJobFunction                                                              */
/*=============================================================================*/
void GuiNavElement::runJobFunction( JobElement::CallReason reason,
				    XferDataItem *source,
				    XferDataItem *sourceEnd){
  BUG( BugGui, "GuiNavElement::runJobFunction" );
  // !! GuiNavNode hat seine eigene runJobFunction !!
  JobFunction *func = getJobFunction();
  if( func == 0 ){
    return;
  }

  if( !m_navigator->setRunningMode() ) assert( false );

  JobStarter *trigger = m_navigator->getNewTrigger( func );
  trigger->setReason( reason );
  if( source != 0 ){
    trigger->setSource( source );
  }
  if( sourceEnd != 0 ){
    trigger->setSource2( sourceEnd );
  }
  trigger->setDataItem( m_dataItem );

  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // doEndOfWork() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // löscht sich am Ende selbständig.
  // Da wir kein GuiElement sind, überlassen wir diese Arbeit dem Navigator
}

/*=============================================================================*/
/* newParent                                                                   */
/*=============================================================================*/
void GuiNavElement::newParent(){
  BUG( BugGui, "GuiNavElement::newParent" );
  NavElemVector::iterator nodesIter = m_nodes.begin();
  while( nodesIter != m_nodes.end()){
    (*nodesIter)->newParent( true );
    ++nodesIter;
  }
}

/*=============================================================================*/
/* deleteNodes                                                                 */
/*=============================================================================*/
void GuiNavElement::deleteNodes(){
  NavElemVector::iterator nodesIter = m_nodes.begin();
  while( nodesIter != m_nodes.end()){
    (*nodesIter)->deleteNodes();
    delete *nodesIter;
    ++nodesIter;
  }
  m_nodes.clear();
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiNavElement::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  // data
  bool updated = m_dataItem->isUpdated(GuiManager::Instance().LastWebUpdate(), true);
  if (onlyUpdated && !updated) {
    return false;
  }
  update(GuiElement::reason_Process);  // update navigator tree

  Json::Value jsonElemPopupMenu = Json::Value(Json::objectValue);
  Json::Value jsonElemPopupDropMenu = Json::Value(Json::objectValue);

  // menus
  if(isFolder()||isVisible()) {
    // popMenu
    int menuId;
    getDiagramMenuId(menuId);
    if (getPopupMenu(menuId)) {
      getPopupMenu(menuId)->serializeJson(jsonElemPopupMenu, onlyUpdated);
    }
    // popMenu
    if (getPopupDropMenu()) {
      getPopupDropMenu()->serializeJson(jsonElemPopupDropMenu, onlyUpdated);
    }
  }

  // base properties
  std::string elemType;
  std::string tip;
  jsonObj["name"] = getNodeName();
  // wieviele Elemente dieser Struktur sind vorhanden ?
  int actualCount = 0, newCount = 0;
  getChildrenCount( actualCount, newCount );
  jsonObj["actualCount"] = actualCount;
  jsonObj["newCount"] = newCount;
  jsonObj["isVisible"] = isVisible();
  jsonObj["isFolder"] = isFolder();
  getToolTip(getDataItem(), tip);
  jsonObj["tooltip"] = tip;
  std::string fn(m_dataItem->getFullName(true));
  while(fn.find("*") != std::string::npos) {
    fn.replace(fn.find("*"), sizeof("*")-1, "0");
  }
  jsonObj["fullName"] = fn;

  // Menus
  if(isVisible()) {
    // popMenu, popDropMenu
    jsonObj["popup_menu"] = jsonElemPopupMenu;
    jsonObj["popup_drop_menu"] = jsonElemPopupDropMenu;
  }

  // child nodes
  Json::Value jsonAryChilds = Json::Value(Json::arrayValue);
  if(isFolder()) {
    NavElemVector::iterator it;
    for( it = m_nodes.begin(); it != m_nodes.end(); ++it ){
      Json::Value jsonElem = Json::Value(Json::objectValue);
      if ((*it)->serializeJson(jsonElem, onlyUpdated))
        jsonAryChilds.append(jsonElem);
    }
    jsonObj["node"] = jsonAryChilds;
    jsonObj["isFolder"] = isFolder() && jsonAryChilds.size() > 0;
  }
  // return if folder and type diagram or iconView
  if (!isVisible() ||
      (getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram && isFolder()) ||
      (getNavigator()->getNavigatorType() == GuiNavigator::type_IconView && isFolder())) {
    return jsonAryChilds.size() > 0;
  }

  // Icon
  std::string content;
  if (getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram) {
    QPixmap pixmap;
    std::string varname = m_dataItem->getFullName(true);
    if ( GuiQtDiagram::isCompositePixmap(varname) ) {
      GuiQtDiagram::getCompositePixmap(pixmap, varname, 1.0);
    } else
      GuiQtDiagram::getPixmap(pixmap, varname, true);
    if (IconManager::Instance().getDataUrlContent(pixmap, content)) {
      jsonObj["icon"] = content;
    } else {
      BUG_DEBUG("icon '"<<getNodeName()<<"' not found.");
    }
  } else {
    // other navigator types
    IconManager::ICON_TYPE icon_type;
    std::string fn = getIconPixmapFileName(icon_type);
    if (IconManager::Instance().getDataUrlFileContent(icon_type, fn, content, false)) {
      jsonObj["icon"] = content;
    } else {
      BUG_DEBUG("icon '"<<getNodeName()<<"' not found.");
    }
  }

  // Others
  jsonObj["name"] = getNodeName();
  jsonObj["classname"] = getClassName();
  jsonObj["editable"] = false;
  jsonObj["updated"] = updated;
  getType(elemType);
  jsonObj["element_type"] = elemType;
  jsonObj["level"] = getLevel();
  JobFunction *func = getJobFunction();
  if( func != 0 ){
    jsonObj["action"] =  func->Name();
  }

  // Diagram
  if (getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram) {
    int xpos, ypos;
    std::string node_name;
    if (getDiagramNodeName(node_name)) {
      jsonObj["node_name"] = node_name;
    }
    if (getDiagramPosition(xpos, ypos) && !isFolder()) {
      Json::Value jsonAry = Json::Value(Json::arrayValue);
      jsonAry.append(xpos);
      jsonAry.append(ypos);
      jsonObj["position"] = jsonAry;
      getToolTip(getDataItem(), tip);
      jsonObj["tooltip"] = tip;
    } else {
      jsonObj["isVisible"] = false;
      return isFolder() ? jsonAryChilds.size() > 0 : false;
    }
    return (xpos != std::numeric_limits<int>::min() &&
            ypos != std::numeric_limits<int>::min());
  } else if (getNavigator()->getNavigatorType() == GuiNavigator::type_Default) {

    // labels
    Json::Value jsonAry = Json::Value(Json::arrayValue);
    std::vector<std::pair<std::string,std::string> >::const_iterator iter;
    int cnt=0;
    for(iter = getLabel().begin(); iter != getLabel().end(); ++iter, ++cnt){
      if (getNavigator()->getCols()[cnt]->style() == GuiNavigator::columnStyle_Pixmap) {
        IconManager::ICON_TYPE icon_type;
        std::string icon, file, content;
        if (IconManager::Instance().getDataUrlFileContent(icon_type, (*iter).first, content)) {
          jsonAry.append(content); // image found, export
          continue;
        }
      }
      jsonAry.append((*iter).first);
    }
    jsonObj["label"] = jsonAry;
    if (getLabel()[0].second.size()) {
      jsonObj["fgcolor"] = getLabel()[0].second;
    }
  }
  return true;
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiNavElement::serializeProtobuf(in_proto::ElementList* eles, in_proto::NavigatorNode* element, bool onlyUpdated){
  // data
  bool updated = m_dataItem->isUpdated(GuiManager::Instance().LastWebUpdate(), true);
  if (onlyUpdated && !updated) {
    return false;
  }
  update(GuiElement::reason_Process);  // update navigator tree

  // base properties
  std::string elemType;
  std::string tip;
  element->set_name(getNodeName());
  // wieviele Elemente dieser Struktur sind vorhanden ?
  int actualCount = 0, newCount = 0;
  getChildrenCount( actualCount, newCount );
  element->set_new_count(newCount);
  element->set_visible(isVisible());
  element->set_folder(isFolder());
  getToolTip(getDataItem(), tip);
  element->set_tooltip(tip);
  std::string fn(m_dataItem->getFullName(true));
  while(fn.find("*") != std::string::npos) {
    fn.replace(fn.find("*"), sizeof("*")-1, "0");
  }
  element->set_full_name(fn);

  // Menus
  if(isVisible()) {
    int menuId;
    getDiagramMenuId(menuId);
    if (getPopupMenu(menuId)) {
      getPopupMenu(menuId)->serializeProtobuf(eles, element->mutable_popup_menu(), onlyUpdated);
    }
  }

  // child nodes
  if(isFolder()) {
    NavElemVector::iterator it;
    for( it = m_nodes.begin(); it != m_nodes.end(); ++it ){
      auto child = element->add_nodes();
      Json::Value jsonElem = Json::Value(Json::objectValue);
      (*it)->serializeProtobuf(eles, child, onlyUpdated);
    }
    element->set_folder(isFolder() && element->nodes_size() > 0);
  }
  // return if folder and type diagram or iconView
  if (!isVisible() ||
      (getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram && isFolder()) ||
      (getNavigator()->getNavigatorType() == GuiNavigator::type_IconView && isFolder())) {
    return element->nodes_size() > 0;
  }

  // Icon
  std::string content;
  if (getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram) {
    QPixmap pixmap;
    std::string varname = m_dataItem->getFullName(true);
    if ( GuiQtDiagram::isCompositePixmap(varname) ) {
      GuiQtDiagram::getCompositePixmap(pixmap, varname, 1.0);
    } else
      GuiQtDiagram::getPixmap(pixmap, varname, true);
    if (IconManager::Instance().getDataUrlContent(pixmap, content)) {
      element->set_icon(content);
    } else {
      BUG_DEBUG("icon '"<<getNodeName()<<"' not found.");
    }
  } else {
    // other navigator types
    IconManager::ICON_TYPE icon_type;
    std::string fn = getIconPixmapFileName(icon_type);
    if (IconManager::Instance().getDataUrlFileContent(icon_type, fn, content, false)) {
      element->set_icon(content);
    } else {
      BUG_DEBUG("icon '"<<getNodeName()<<"' not found.");
    }
  }

  // Others
  element->set_name(getNodeName());
  JobFunction *func = getJobFunction();
  if( func != 0 ){
    element->set_action(func->Name());
  }

  // Diagram
  if (getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram) {
    int xpos, ypos;
    std::string node_name;
    if (getDiagramNodeName(node_name)) {
      element->set_node_name(node_name);
    }
    if (getDiagramPosition(xpos, ypos) && !isFolder()) {
      auto pos = element->mutable_position();
      pos->set_x(xpos);
      pos->set_y(ypos);
      getToolTip(getDataItem(), tip);
      element->set_tooltip(tip);
    } else {
      element->set_visible(false);
      return isFolder() ? element->nodes_size() > 0 : false;
    }
    return (xpos != std::numeric_limits<int>::min() &&
            ypos != std::numeric_limits<int>::min());
  } else if (getNavigator()->getNavigatorType() == GuiNavigator::type_Default) {

    // labels
    std::vector<std::pair<std::string,std::string> >::const_iterator iter;
    int cnt=0;
    for(iter = getLabel().begin(); iter != getLabel().end(); ++iter, ++cnt){
      if (getNavigator()->getCols()[cnt]->style() == GuiNavigator::columnStyle_Pixmap) {
        IconManager::ICON_TYPE icon_type;
        std::string icon, file, content;
        if (IconManager::Instance().getDataUrlFileContent(icon_type, (*iter).first, content)) {
          element->add_labels(content); // image found, export
          continue;
        }
      }
      element->add_labels((*iter).first);
    }
    if (getLabel()[0].second.size()) {
      element->set_fgcolor(getLabel()[0].second);
    }
  }
  return true;
}
#endif

/*=============================================================================*/
/* addNodes                                                                    */
/*=============================================================================*/
void GuiNavElement::addNode( GuiNavElement *node ){
  m_nodes.push_back( node );
}

/*=============================================================================*/
/* addLabel                                                                    */
/*=============================================================================*/
void GuiNavElement::addLabel( const std::string &label, const std::string &tag ){
  m_labels.push_back( std::make_pair( label.c_str(), tag.c_str() ) );
}

/*=============================================================================*/
/* addLabel                                                                    */
/*=============================================================================*/
void GuiNavElement::addLabel( const std::string &label ){
  m_labels.push_back( std::make_pair( label.c_str(), (const char*)"" ) );
}

/*=============================================================================*/
/* setLabel                                                                    */
/*=============================================================================*/
bool GuiNavElement::setLabel( const std::string &label, int index ){
  if( index < 0 || index >= m_labels.size() )
    return false;
  m_labels[index].first = label;
  return true;
}

/*=============================================================================*/
/* setLabel                                                                    */
/*=============================================================================*/
std::string GuiNavElement::getIconPixmapFileName(IconManager::ICON_TYPE& icon_type){
  std::string icon;
  icon_type = IconManager::NONE;

  // try to find a icon
  std::vector<std::string> searchVec;
  std::string section, type;
  switch (m_navigator->getNavigatorType()) {
  case GuiNavigator::type_IconView:
    section = "IconView";
    if (getType(type))
      searchVec.push_back( type );
    break;
  case GuiNavigator::type_Diagram:
    section = "Diagram";
    if (getType(type))
      searchVec.push_back( type );
    break;
  case GuiNavigator::type_Default:
  default:
    section = "Navigator";
    break;
  }
  searchVec.push_back( getNodeName() );
  if (getNavigator()->getNavigatorType() == GuiNavigator::type_Diagram)
    searchVec.push_back( "default" );

  std::string name, key;
  for (int i=0; i < searchVec.size(); ++i) {
    name = searchVec[i];
    // search in pixmap cache
    key = section + "/" + name;
    PixmapNameMap::iterator it = s_pixmap_name_map.find(key);
    if (it != s_pixmap_name_map.end()) {
      icon = it->second.icon_filename; // found
      icon_type = it->second.icon_type; // found
      BUG_DEBUG("cached FOUND, key: " << key << ", icon: " << icon << ", icon_type: " << icon_type);
      return icon; // TODO icon type fehlt noch
    }
    std::string str = section+"/"+name+".iconPixmap";
    QString pm = GuiQtManager::Settings()->value
      (QString::fromStdString(str), QString::fromStdString(name)).toString();
    icon = IconManager::Instance().locateFile(icon_type, pm.toStdString());
    if(icon.empty()) {
      icon = IconManager::Instance().locateFile(icon_type, lower(name));
      if(icon.empty())
        icon = IconManager::Instance().locateFile(icon_type, lower(name+"-small"));
    }
    // gefunden?
    if (!icon.empty()) {
      s_pixmap_name_map[key] = PixmapData(icon, icon_type);
      BUG_DEBUG("FOUND, key: " << key <<", icon: "<< icon);
      return icon;
    }
  }
  // not found
  key = section + "/" + getNodeName();
  s_pixmap_name_map[key] = PixmapData(icon, icon_type);
  BUG_DEBUG("not found, key: " << key);
  return icon;
}

/*=============================================================================*/
/* getDiagramNodeName                                                          */
/*=============================================================================*/
bool GuiNavElement::getDiagramNodeName(std::string& node_name ) const{
  bool ret(false);
  node_name.clear();
  DataReference *ref = DataPoolIntens::getDataReference(getDataItem()->Data(), "node_name");
  XferDataItem* xfer = 0;
  // try to get (".node_name")
  if( ref != 0 )
    xfer = new XferDataItem( ref );
  if (xfer) {
    ret=xfer->getValue(node_name);
    delete xfer;
  }
  return ret;
}

/*=============================================================================*/
/* getDiagramMenuId                                                            */
/*=============================================================================*/
bool GuiNavElement::getDiagramMenuId(int& menuId ) const{
  bool ret(false);
  menuId = 0;
  DataReference *ref = DataPoolIntens::getDataReference(getDataItem()->Data(), "menuId");
  XferDataItem* xfer = 0;
  // try to get (".menuId")
  if( ref != 0 )
    xfer = new XferDataItem( ref );
  if (xfer) {
    ret=xfer->getValue(menuId);
    delete xfer;
  }
  return ret;
}

/*=============================================================================*/
/* getDiagramPosition                                                          */
/*=============================================================================*/
bool GuiNavElement::getDiagramPosition(int& xpos, int& ypos) {
  // get icon coordinate
  xpos = std::numeric_limits<int>::min();
  ypos = std::numeric_limits<int>::min();
  DataReference *ref= DataPoolIntens::getDataReference(getDataItem()->Data(), "xpos");
  if (ref) {
    XferDataItem *posXfer = new XferDataItem( ref );
    int p;
    if (!(posXfer && posXfer->isValid() && posXfer->getValue(p)))
      return false;
    xpos = p;

    ref= DataPoolIntens::getDataReference(getDataItem()->Data(), "ypos");
    if (ref) {
      XferDataItem *posXfer = new XferDataItem( ref );
      if (!(posXfer && posXfer->isValid() && posXfer->getValue(p)))
        return false;
      ypos = p;
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------
// updateNode
//---------------------------------------------------------------------
void GuiNavElement::updateNode( GuiElement::UpdateReason ur ){
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
          (*it)->getQtNavElement()->moveChildren(this);
        }
      }
      (*it)->update( ur );
    }
  }
}
