
#include <algorithm>
#include <iomanip>
#include "utils/Debugger.h"

#include "gui/GuiPopupMenu.h"
#include "gui/GuiNavigator.h"
#include "gui/GuiNavElement.h"
#include "gui/GuiFactory.h"

#include "xfer/Scale.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferConverter.h"

#include "job/JobManager.h"

INIT_LOGGER();

const char* GuiNavigator::MIME_COMPONENT ="text/plain";
int GuiNavigator::Trigger::s_cnt = 0;

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
GuiNavigator::GuiNavigator(GuiNavigator::Type navType)
  : m_widgetWidth( 300 )
  , m_widgetHeight( 300 )
  , m_isDefaultSize( true )
  , m_jobRunningKey( 0 )
  , m_updating( false )
  , m_freezed( false )
  , m_multiSelection( false )
  , m_expandable( false )
  , m_scrollable( false )
  , m_compareMode( false )
  , m_outputCompareMode( output_Inequality )
  , m_navType(navType) {
}

GuiNavigator::~GuiNavigator(){
  RootsVector::iterator rootIter;
  for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter ){
    delete *rootIter;
  }
  ColsVector::iterator colsIter;
  for( colsIter = m_cols.begin(); colsIter != m_cols.end(); ++colsIter)
    delete *colsIter;
}

/* --------------------------------------------------------------------------- */
/* doEndOfWork --                                                              */
/* --------------------------------------------------------------------------- */
void GuiNavigator::doEndOfWork( bool error ){
  // Diese Funktion wird nur benötigt, wenn eine Function an der Variablen
  // angehängt ist (ein sogenannter trigger).
  if( error ){
    getElement()->updateForms( GuiElement::reason_Cancel );
    GuiManager::Instance().showErrorBox( getElement() );
  }
  else{
    getElement()->updateForms( GuiElement::reason_FieldInput );
  }
  unsetRunningMode();
}

/* --------------------------------------------------------------------------- */
/* getNewTrigger --                                                            */
/* --------------------------------------------------------------------------- */
JobStarter *GuiNavigator::getNewTrigger( JobFunction *func ){
  return new Trigger( this, func );
}

/* --------------------------------------------------------------------------- */
/* countTrigger --                                                             */
/* --------------------------------------------------------------------------- */
int GuiNavigator::countTrigger(){
  return Trigger::getCountInstances();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiNavigator::getSize( int &width, int &height ){
  width = m_widgetWidth;
  height = m_widgetHeight;
}


/* --------------------------------------------------------------------------- */
/* setRunningMode --                                                           */
/* --------------------------------------------------------------------------- */
bool GuiNavigator::setRunningMode(){
  m_jobRunningKey = JobManager::Instance().setRunningMode( m_jobRunningKey );
  return m_jobRunningKey != 0;
}

/* --------------------------------------------------------------------------- */
/* unsetRunningMode --                                                         */
/* --------------------------------------------------------------------------- */
void GuiNavigator::unsetRunningMode(){
  if( m_jobRunningKey != 0 ){
    JobManager::Instance().unsetRunningMode( m_jobRunningKey );
    m_jobRunningKey = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* isNodeValid --                                                              */
/* --------------------------------------------------------------------------- */
bool GuiNavigator::isNodeValid( XferDataItem *item ){
  return isNodeValid( item->Data() );
}

/* --------------------------------------------------------------------------- */
/* isNodeValid --                                                              */
/* --------------------------------------------------------------------------- */
bool GuiNavigator::isNodeValid( const DataReference *ref ){
  if( !( ref->GetDict()->getDataType() == DataDictionary::type_StructVariable ) ){
    return false;
  }
  if ( isCompareMode() ){
    // hier wollen wir alles anzeigen, da muss auch kein Tag definiert sein
    return true;
  }

  DataReference *newRef = 0;
  DataReference::StructIterator structIter;
  UserAttr *userAttr = 0;
  for( structIter = ref->begin(); structIter != ref->end(); ++structIter){
    newRef = structIter.NewDataReference();
    userAttr = static_cast<UserAttr*>( newRef->getUserAttr() );
    if( ( m_cols.empty() ? false : userAttr->hasTag( m_cols[0]->getTag() ) ) ||
	( m_cols.empty() && newRef->nodeName() == "name" ) )
      break;
    else{
      delete newRef;
      newRef = 0;
    }
  }
  if( newRef == 0 ){
    return false;
  }
  delete newRef;
  return true;
}

/* --------------------------------------------------------------------------- */
/* updateTree --                                                               */
/* --------------------------------------------------------------------------- */
void GuiNavigator::updateTree() {
  BUG( BugGui, "GuiNavigator::updateTree" );
  // Erst hier werden jetzt die neuen Children( Widgets ) erzeugt
  RootsVector::iterator rootIter = m_roots.begin();
  while ( rootIter != m_roots.end()){
    BUG_MSG( (*rootIter)->getName() );
    BUG_MSG( "updating" );
    (*rootIter)->refresh( (rootIter - m_roots.begin()) );
    ++rootIter;
  }
  repaint();
  m_updating = false;
}

/*=============================================================================*/
/* setSize                                                                     */
/*=============================================================================*/
void GuiNavigator::setSize( const int &width, const int &height ){
  m_isDefaultSize = false;
  m_widgetWidth = width;
  m_widgetHeight = height;
}

/*=============================================================================*/
/* setToolTipTag                                                               */
/*=============================================================================*/
void GuiNavigator::setToolTipTag( const std::string &tag ){
  m_toolTipTag = tag;
}

/*=============================================================================*/
/* addColumn                                                                   */
/*=============================================================================*/
void GuiNavigator::addColumn( const std::string &tag
				 , const std::string &label
				 , const int width
				 , const int precission
				 , Scale *scale
			         , const bool thousand_sep
			         , ColumnStyle colStyle ){
  int w = width;
  if( m_cols.size() == 0 )
    if( width > 0 )
      w *= -1;

  m_cols.push_back( new Column( tag
				, label
				, w == 0 ? 8 : w
				, precission
				, scale
				, thousand_sep
				, colStyle) );
}

/*=============================================================================*/
/* setMultipleSelection                                                        */
/*=============================================================================*/
void GuiNavigator::setMultipleSelection(bool multiSelection) {
  m_multiSelection = multiSelection;
}

/*=============================================================================*/
/* isMultipleSelection                                                         */
/*=============================================================================*/
bool GuiNavigator::isMultipleSelection() {
  return m_multiSelection;
}

/*=============================================================================*/
/* setExpandable                                                        */
/*=============================================================================*/
void GuiNavigator::setExpandable(bool expandable) {
  m_expandable = expandable;
}

/*=============================================================================*/
/* setScrollable                                                        */
/*=============================================================================*/
void GuiNavigator::setScrollable(bool scrollable) {
  m_scrollable = scrollable;
}

/*=============================================================================*/
/* isExpandable                                                         */
/*=============================================================================*/
bool GuiNavigator::isExpandable() {
  return m_expandable;
}

/*=============================================================================*/
/* isScrollable                                                        */
/*=============================================================================*/
bool GuiNavigator::isScrollable() {
  return m_scrollable;
}

/*=============================================================================*/
/* addTransparentFolder                                                        */
/*=============================================================================*/
void GuiNavigator::addTransparentFolder( const std::string& hide_folder ){
  m_transparentFolderNames.push_back( hide_folder );
}
/*=============================================================================*/
/* isHiddenFolder                                                        */
/*=============================================================================*/
bool GuiNavigator::isTransparentFolder(const std::string& folder_name){
  return std::find( m_transparentFolderNames.begin(),
		    m_transparentFolderNames.end(), folder_name ) != m_transparentFolderNames.end();
}

/* --------------------------------------------------------------------------- */
/* createTree --                                                               */
/* --------------------------------------------------------------------------- */
void GuiNavigator::createTree( Root * const root, int rootIdx ){
  root->createChildren();
  freeze();
  root->refresh(rootIdx);
  repaint();
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */

void GuiNavigator::Trigger::backFromJobStarter( JobAction::JobResult result ){
  --s_cnt;
  if (!s_cnt)
    m_navigator->doEndOfWork( result != JobAction::job_Ok );
}


GuiNavigator::Root::~Root(){
  m_node->destroy();
  delete m_node;
  delete m_dataItem;
  delete m_lastLevel;
  delete m_firstStructFolder;
  delete m_openLevels;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiNavigator::Root::acceptIndex( const std::string &name, int inx ){
  BUG(BugGuiFld,"GuiNavigator::Root::acceptIndex");
  assert( getDataItem() != 0 );

  // leider haben wir zwei datarefs
  bool retA = m_dataItem->acceptIndex( name, inx );
  bool retB = getNode() ? getNode()->getDataItem()->acceptIndex( name, inx ) : false;
  if( !retA || !retB ) {
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiNavigator::Root::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiNavigator::Root::setIndex",name << " =[" << inx << "]");
  assert( getDataItem() != 0 );
  assert( inx >= 0 );

  // leider haben wir zwei datarefs
  m_dataItem->setIndex( name, inx );
  getNode() ? getNode()->getDataItem()->setIndex( name, inx ) : false;
  return ;
}

void GuiNavigator::Root::refresh(int rootIdx){
  if( m_node != 0 ) {
    m_node->setRootIndex(rootIdx);
    m_node->refresh();
  }
}

void GuiNavigator::Root::createChildren(){
  if( m_node != 0 )
    m_node->createChildren( new XferDataItem( *m_dataItem) );
}

bool GuiNavigator::Root::isOpen(){
  if( m_node != 0 )
    return m_node->isOpen();
  return false;
}

void GuiNavigator::Root::addChildren( int n ){
  if( m_node != 0 )
    m_node->addChildren( n );
}

void GuiNavigator::Root::removeChildren( int n ){
  if( m_node != 0 )
    m_node->removeChildren( n );
}

void GuiNavigator::Root::update( GuiElement::UpdateReason ur ){
  if( m_node != 0 )
    m_node->update( ur );
}

bool GuiNavigator::Root::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  if( m_node != 0 )
    return m_node->serializeJson(jsonObj, onlyUpdated);
  return false;
}

#if HAVE_PROTOBUF
bool GuiNavigator::Root::serializeProtobuf(in_proto::ElementList* eles, in_proto::NavigatorNode* element, bool onlyUpdated) {
  if( m_node != 0 )
    return m_node->serializeProtobuf(eles, element, onlyUpdated);
  return false;
}
#endif

GuiNavigator::Column::~Column(){
  if( m_scale != 0 ) delete m_scale;
}

const double GuiNavigator::Column::getScale(){
  return m_scale != 0 ? m_scale->getValue() : 1.;
}

std::string GuiNavigator::Column::getFormatedLabel(){
  StringConverter conv( m_width );
  std::ostringstream os;
  conv.write( m_label, os );
  return os.str();
}

/* --------------------------------------------------------------------------- */
/* setRoot --                                                                  */
/* --------------------------------------------------------------------------- */
GuiNavigator::Root *GuiNavigator::setRoot( XferDataItem *const item, const std::string &name ){
  Root *root = 0;
  XferDataItemIndex *index = 0;
  index = item->getDataItemIndexWildcard( 1 );
if (index) {
  assert( index != 0 );
  index->setLowerbound( 0 );
}
  item->setDimensionIndizes();
  std::string newName( name );
  if( newName.empty() ){
    UserAttr *userAttr = item->getUserAttr();
    newName = userAttr->Label( item->Data() );
  }
  root =  new Root( newName, item );
  if( root != 0 )
    m_roots.push_back( root );
  return root;
}

/*=============================================================================*/
/* Root::setFirstVisibleStructFolder                                           */
/*=============================================================================*/
void GuiNavigator::Root::setFirstStructFolder( int firstStructFolder ){
  if( m_autoLevel == false ){
    if( m_firstStructFolder == 0 )
      m_firstStructFolder = new int( firstStructFolder );
    else
      *m_firstStructFolder = firstStructFolder;
  }
}

/*=============================================================================*/
/* Root::setLastLevel                                                          */
/*=============================================================================*/
void GuiNavigator::Root::setLastLevel( int lastLevel ){
  if( m_lastLevel == 0 )
    m_lastLevel = new int( lastLevel );
  else
    *m_lastLevel = lastLevel;
}

/*=============================================================================*/
/* Root::setAutoLevel                                                          */
/*=============================================================================*/
void GuiNavigator::Root::setAutoLevel(){
  delete m_firstStructFolder;
  m_firstStructFolder = 0;
  m_autoLevel = true;
}

/*=============================================================================*/
/* Root::setHideEmptyFolder                                                    */
/*=============================================================================*/
void GuiNavigator::Root::setHideEmptyFolder(){
  m_hideEmptyFolder = true;
}

/*=============================================================================*/
/* Root::setOpenLevels                                                          */
/*=============================================================================*/
void GuiNavigator::Root::setOpenLevels( int openLevels ){
  if( m_openLevels == 0 )
    m_openLevels = new int( openLevels );
  else
    *m_openLevels = openLevels;
}

/*=============================================================================*/
/* Root::isUpdated                                                             */
/*=============================================================================*/
bool GuiNavigator::Root::isUpdated() const{
  if ( m_dataItem->isDataItemUpdated( GuiManager::Instance().LastGuiUpdate(), true ) ){
    return true;
  }
  return false;
}

/*=============================================================================*/
/* Root::getChildrenCount                                                      */
/*=============================================================================*/
bool GuiNavigator::Root::getChildrenCount( int &actualCount, int &newCount ){
  m_node->getChildrenCount( actualCount, newCount );
  XferDataItemIndex *index = m_dataItem->getDataItemIndexWildcard( 1 );
  newCount = index ? index->getDimensionSize( m_dataItem->Data() ) : m_dataItem->getLastSize();
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiNavigator::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:Navigator";
  os << " fullname=\"" << m_roots[0]->getDataItem()->getFullName(false) << "\"";
  os << ">" << std::endl;
  os << "</intens:Navigator>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiNavigator::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  bool updated(false);
  for(RootsVector::iterator rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter){
    if( (*rootIter)->getNode() == 0 ){
      createTree( *rootIter, (rootIter - m_roots.begin()) );
    }
    if((*rootIter)->isUpdated()) {
      updated = true;
    }
  }
  if (onlyUpdated && !updated) {
    return false;
  }
  getElement()->update(GuiElement::reason_Process);  // update navigator tree
  onlyUpdated = false;   // Navigator Root updated, export completly
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["fullname"] = m_roots[0]->getDataItem()->getFullName(true);
  jsonObj["action"] = m_roots[0]->getNode() && m_roots[0]->getNode()->getJobFunction() ?
    m_roots[0]->getNode()->getJobFunction()->Name() : "";
  jsonObj["width"] = m_widgetWidth;
  jsonObj["height"] = m_widgetHeight;
  jsonObj["default_size"] = m_isDefaultSize;
  //  jsonObj["navigator_type"] = StringNavType();
  jsonObj["tooltip_tag"] = m_toolTipTag;
  jsonObj["multi_selection"] = m_multiSelection;
  jsonObj["expandable"] = m_expandable;
  jsonObj["scrollable"] = m_scrollable;
  jsonObj["freezed"] = m_freezed;
  jsonObj["updating"] = m_updating;
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  RootsVector::iterator rootIter;
  for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter ){
    Json::Value jsonElem = Json::Value(Json::objectValue);
    (*rootIter)->serializeJson(jsonElem, false);
    if (m_navType == GuiNavigator::type_Diagram ||
        m_navType == GuiNavigator::type_IconView) {
      // remove unused Nodes and flatten the data struct (only list of nodes)
      Json::Value jsonAryNode = Json::Value(Json::arrayValue);
      getVisibleNodes(jsonElem, jsonAryNode);
      jsonElem.removeMember("node");
      jsonElem["node"] = jsonAryNode;
    }
    jsonAry.append(jsonElem);
  }
  jsonObj["root"] = jsonAry;
  jsonAry = Json::Value(Json::arrayValue);
  ColsVector::iterator colsIter;
  for( colsIter = m_cols.begin(); colsIter != m_cols.end(); ++colsIter) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    jsonElem["tag"] = (*colsIter)->getTag();
    jsonElem["label"] = (*colsIter)->getLabel();
    jsonElem["width"] = (*colsIter)->getWidth();
    jsonElem["precision"] = (*colsIter)->getPrecision();
    jsonElem["thousandSep"] = (*colsIter)->thousandSep();
    jsonElem["scale"] = (*colsIter)->scale() ? (*colsIter)->scale()->getValue() : 1.;
    jsonElem["style"] = (*colsIter)->style();
    jsonAry.append(jsonElem);
  }
  //  if (m_navType == GuiNavigator::type_Default) {
  jsonObj["column"] = jsonAry;
  //  }

  // popMenu
  GuiNavElement *node = dynamic_cast<GuiNavElement*>(m_roots[0]->getNode());
  if (node && node->getPopupMenu()) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    node->getPopupMenu()->serializeJson(jsonElem, onlyUpdated);
    jsonObj["popup_menu"] = jsonElem;
  }

  // Diagram
  if (m_navType == GuiNavigator::type_Diagram) {
    jsonObj["diagram_connection_ratio"]  = getDiagramConnectionRatio();

    std::vector<std::string> vec0, vec1, select_items;
    std::vector<std::string>::iterator it, it1;
    std::vector<ConnectionAttr> attr;
    std::vector<ConnectionAttr>::iterator attrIt;
    getDiagramConnections(vec0, vec1, attr);
    getDiagramSelectItems(select_items);

    // connect
    jsonAry = Json::Value(Json::arrayValue);
    for(it = vec0.begin(), it1 = vec1.begin(); it != vec0.end() && it1 != vec1.end(); ++it, ++it1) {
      Json::Value jsonAry2 = Json::Value(Json::arrayValue);
      jsonAry2.append(*it);
      jsonAry2.append(*it1);
      jsonAry.append(jsonAry2);
    }
    jsonObj["connect"] = jsonAry;
    // connect Attr
    jsonAry = Json::Value(Json::arrayValue);
    for(attrIt = attr.begin(); attrIt != attr.end(); ++attrIt) {
      Json::Value jsonAttr = Json::Value(Json::objectValue);
      jsonAttr["lineWidth"] = (*attrIt).lineWidth;
      jsonAttr["lineStyle"] = (*attrIt).lineStyle;
      jsonAttr["lineColor"] = (*attrIt).lineColor;
      if ((*attrIt).xpos.empty() || (*attrIt).ypos.empty() ||
          (*attrIt).anchor_xpos.empty() || (*attrIt).anchor_ypos.empty())
        continue;
      Json::Value jsonPts = Json::Value(Json::arrayValue);
      std::vector<double>::iterator itx, ity;
      for(itx = (*attrIt).xpos.begin(), ity = (*attrIt).ypos.begin();
          itx != (*attrIt).xpos.end() && ity != (*attrIt).ypos.end();
          ++itx, ++ity) {
        Json::Value jsonPt = Json::Value(Json::arrayValue);
        jsonPt.append(*itx);
        jsonPt.append(*ity);
        jsonPts.append(jsonPt);
      }
      jsonAttr["lineVector"] = jsonPts;
      Json::Value jsonA = Json::Value(Json::arrayValue);
      std::vector<int>::iterator itax, itay;
      for(itax = (*attrIt).anchor_xpos.begin(), itay = (*attrIt).anchor_ypos.begin();
          itax != (*attrIt).anchor_xpos.end() && itay != (*attrIt).anchor_ypos.end();
          ++itax, ++itay) {
        Json::Value jsonPt = Json::Value(Json::arrayValue);
        jsonPt.append(*itax);
        jsonPt.append(*itay);
        jsonA.append(jsonPt);
      }
      jsonAttr["anchor"] = jsonA;
      jsonAry.append(jsonAttr);
    }
    jsonObj["connectAttr"] = jsonAry;

    // selected items
    jsonAry = Json::Value(Json::arrayValue);
    for(it = select_items.begin(); it != select_items.end(); ++it) {
      jsonAry.append(*it);
    }
    jsonObj["selected_item"] = jsonAry;
  }
  // DebugOuput in file
  if (__debugLogger__) {
    static int nNb = 0;
    std::ostringstream osFn;
    osFn << "nav_" << getElement()->getElementId() << std::setfill('0') << std::setw(5) << ++nNb << ".json";
    std::ofstream osf(osFn.str());
    osf << ch_semafor_intens::JsonUtils::value2string(jsonObj, true);
    osf.close();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiNavigator::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  bool updated(false);
  for(RootsVector::iterator rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter){
    if( (*rootIter)->getNode() == 0 ){
      createTree( *rootIter, (rootIter - m_roots.begin()) );
    }
    if((*rootIter)->isUpdated()) {
      updated = true;
    }
  }
  if (onlyUpdated && !updated) {
    return false;
  }
  getElement()->update(GuiElement::reason_Process);  // update navigator tree
  onlyUpdated = false;   // Navigator Root updated, export completly
  auto element = eles->add_navigators();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_full_name(m_roots[0]->getDataItem()->getFullName(true));
  element->set_action(m_roots[0]->getNode() && m_roots[0]->getNode()->getJobFunction() ?
    m_roots[0]->getNode()->getJobFunction()->Name() : "");
  element->set_width(m_widgetWidth);
  element->set_height(m_widgetHeight);
  element->set_expandable(m_expandable);
  element->set_scrollable(m_scrollable);
  RootsVector::iterator rootIter;
  for( rootIter = m_roots.begin(); rootIter != m_roots.end(); ++rootIter ){
    auto rootEle = element->mutable_root();
    (*rootIter)->serializeProtobuf(eles, rootEle, false);
  }

  ColsVector::iterator colsIter;
  for( colsIter = m_cols.begin(); colsIter != m_cols.end(); ++colsIter) {
    auto column = element->add_columns();
    column->set_tag((*colsIter)->getTag());
    column->set_width(std::abs((*colsIter)->getWidth()));
    column->set_scale((*colsIter)->scale() ? (*colsIter)->scale()->getValue() : 1.);
    column->set_style((*colsIter)->style());
    column->set_label((*colsIter)->getLabel());
  }

  // popMenu
  GuiNavElement *node = dynamic_cast<GuiNavElement*>(m_roots[0]->getNode());
  if (node && node->getPopupMenu()) {
    node->getPopupMenu()->serializeProtobuf(eles, element->mutable_popup_menu(), onlyUpdated);
  }

  // Diagram
  if (m_navType == GuiNavigator::type_Diagram) {
    element->set_diagram_connection_ratio(getDiagramConnectionRatio());

    std::vector<std::string> vec0, vec1, select_items;
    std::vector<std::string>::iterator it, it1;
    std::vector<ConnectionAttr> attr;
    std::vector<ConnectionAttr>::iterator attrIt;
    getDiagramConnections(vec0, vec1, attr);
    // debug info
    for (auto a: attr){
      BUG_ERROR("lineWidth: " << a.lineWidth
                << ", lineColor: " << a.lineColor
                << ", lineStyle: " << a.lineStyle
                << ", xposSize: " << a.xpos.size()
                << ", xpos: " << a.xpos[0] << ", " << a.ypos[1]
                << ", ypos: " << a.ypos[0] << ", " << a.ypos[1]
                << ", xposAnchorSize:: " << a.anchor_xpos.size()
                << ", yposAnchorSize: " << a.anchor_ypos.size()
                << ", connectType: " << a.connectType);
    }
    getDiagramSelectItems(select_items);

    // connect
    for(it = vec0.begin(), it1 = vec1.begin(); it != vec0.end() && it1 != vec1.end(); ++it, ++it1) {
      auto connection = element->add_connections();
      connection->set_a(*it);
      connection->set_b(*it1);
      BUG_DEBUG("connection: " << *it << " => " << *it1);
    }
    // connect Attr
    for(attrIt = attr.begin(); attrIt != attr.end(); ++attrIt) {
      auto attr = element->add_connection_attrs();
      attr->set_line_width((*attrIt).lineWidth);
      attr->set_line_style((*attrIt).lineStyle);
      attr->set_line_color((*attrIt).lineColor);
      if ((*attrIt).xpos.empty() || (*attrIt).ypos.empty())
        continue;
      std::vector<double>::iterator itx, ity;
      for(itx = (*attrIt).xpos.begin(), ity = (*attrIt).ypos.begin();
          itx != (*attrIt).xpos.end() && ity != (*attrIt).ypos.end();
          ++itx, ++ity) {
        auto pos = attr->add_line_vectors();
        pos->set_x(*itx);
        pos->set_y(*ity);
      }
    }

    // selected items
    for(it = select_items.begin(); it != select_items.end(); ++it) {
      element->add_select_items(*it);
    }
  }
  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* StringNavType() --                                                          */
/* --------------------------------------------------------------------------- */
const std::string GuiNavigator::StringNavType() {
  switch (m_navType) {
  case type_Default:
    return "Navigator";
  case type_IconView:
    return "IconView";
  case GuiNavigator::type_Diagram:
    return "Diagram";
  }
  std::ostringstream os;
  os << Type();
  return os.str();
}

/* --------------------------------------------------------------------------- */
/* getVisibleNodes() --                                                        */
/* --------------------------------------------------------------------------- */
void GuiNavigator::getVisibleNodes(Json::Value& jsonElem, Json::Value& jsonResult) {
  if (jsonElem.isMember("node") && jsonElem["node"].isArray()) {
    Json::Value::iterator it = jsonElem["node"].begin();
    for (; it != jsonElem["node"].end(); ++it) {
      if ((*it).isMember("icon") && (*it)["isVisible"].asBool()) {
        jsonResult.append(*it);
      }
      getVisibleNodes((*it), jsonResult);
    }
  }
}
