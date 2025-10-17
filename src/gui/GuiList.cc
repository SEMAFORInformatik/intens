
#include "gui/GuiList.h"
#include "gui/GuiTable.h"
#include "gui/GuiFactory.h"
#include "gui/GuiPopupMenu.h"
#include "gui/qt/QtMultiFontString.h"
#include "utils/HTMLConverter.h"
#include "utils/StringUtils.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "xfer/XferParameterComplex.h"
#include "xfer/XferParameterString.h"
#include "app/DataSet.h"

INIT_LOGGER();

GuiList::GuiList()
  : m_function( 0 )
  , m_titleAlign( GuiElement::align_Default )
  , m_hasOptionalColumns( false )
  , m_modifiableLabels( false )
  , m_withIndex( false )
  , m_resizeable( false )
  , m_multipleSelection( false )
  , m_countDecimalPlace( -1 )
  , m_toolTipColumn( -1 )
  , m_colorColumn( -1 )
  , m_tableSize( 0 )
  , m_sort_enabled( true )
  , m_horzHeaderLines( 1 )
{
  m_cols.push_back( new Column( 0, "Index", 0, GuiElement::align_Default, 0, 0, 0, false, false ) );
}

GuiList::GuiList(const GuiList& list)
  : m_function(list.m_function )
  , m_title( list.m_title )
  , m_titleAlign( list.m_titleAlign )
  , m_hasOptionalColumns( list.m_hasOptionalColumns )
  , m_modifiableLabels( list.m_modifiableLabels )
  , m_withIndex( list.m_withIndex )
  , m_resizeable( list.m_resizeable )
  , m_multipleSelection( list.m_multipleSelection )
  , m_countDecimalPlace( list.m_countDecimalPlace )
  , m_toolTipColumn( list.m_toolTipColumn )
  , m_colorColumn( list.m_colorColumn )
  , m_tableSize( list.m_tableSize )
  , m_sort_enabled( list.m_sort_enabled )
  , m_horzHeaderLines( list.m_horzHeaderLines )
{
  int i=0;
  for(ColsVector::const_iterator iter = list.m_cols.begin(); iter != list.m_cols.end(); ++iter, ++i ) {
    m_cols.push_back( new Column( *(*iter) ) );
  }
}

GuiList::~GuiList() {
}

/* --------------------------------------------------------------------------- */
/*  getMaxRows                                                                 */
/* --------------------------------------------------------------------------- */
int GuiList::getMaxRows(){
  std::vector<Column*>::iterator iter;

  int rows = 0;
  for( iter = m_cols.begin(); iter != m_cols.end(); ++iter ){
    int r = (*iter)->getRowsCount();
    rows = r > rows ? r : rows;
  }
  return rows;
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiList::setTitle( const std::string &title, GuiElement::Alignment align ) {
  m_title = title;
  m_titleAlign = align;
}

/* --------------------------------------------------------------------------- */
/* addColumn --                                                                */
/* --------------------------------------------------------------------------- */
void GuiList::addColumn( const std::string &label
		 , XferDataItem *item
		 , GuiElement::Alignment align
		 , int length
		 , int precision
		 , Scale *scale
		 , bool thousand_sep
		 , bool optional) {
  XferDataItemIndex *index = 0;
  index = item->getDataItemIndexWildcard( 1 );
  if( index == 0 )
    index = item->getLastIndex();
  assert( index != 0 );
  index->setLowerbound( 0 );
  item->setDimensionIndizes();
  int col = m_cols.size();
  Column *column = new Column( col, label, item, align,
			       length == 0 ? 8 : length ,
			       precision, scale, thousand_sep, optional );
  if (optional) m_hasOptionalColumns = optional;
  m_cols.push_back( column );

  // max countDecimalPlace
  if (precision > m_countDecimalPlace)
    m_countDecimalPlace = precision;
}

/* --------------------------------------------------------------------------- */
/* addColumn --                                                                */
/* --------------------------------------------------------------------------- */
void GuiList::addColumn( XferDataItem *dataitemLabel
		 , XferDataItem *item
		 , GuiElement::Alignment align
		 , int length
		 , int precision
		 , Scale *scale
		 , bool thousand_sep
		 , bool optional) {
  //
  XferDataItemIndex *index = 0;
  index = item->getDataItemIndexWildcard( 1 );
  if( index == 0 )
    index = item->getLastIndex();
  assert( index != 0 );
  index->setLowerbound( 0 );
  item->setDimensionIndizes();
  int col = m_cols.size();
  Column *column = new Column( col, dataitemLabel, item, align,
			       length == 0 ? 8 : length ,
			       precision, scale, thousand_sep, optional );
  m_cols.push_back( column );

  if (optional) m_hasOptionalColumns = optional;
  m_modifiableLabels = true;
}

/* --------------------------------------------------------------------------- */
/* addColumnToolTip --                                                         */
/* --------------------------------------------------------------------------- */
void GuiList::addColumnToolTip(XferDataItem *item){
  XferDataItemIndex *index = 0;
  index = item->getDataItemIndexWildcard( 1 );
  if( index == 0 )
    index = item->getLastIndex();
  assert( index != 0 );
  index->setLowerbound( 0 );
  item->setDimensionIndizes();
  m_toolTipColumn = m_cols.size();
  Column *column = new Column( m_toolTipColumn, "ToolTip", item, GuiElement::align_Default, 0, 0, 0, false, false);
  m_cols.push_back( column );
}

/* --------------------------------------------------------------------------- */
/* addColumnColor --                                                           */
/* --------------------------------------------------------------------------- */
void GuiList::addColumnColor(XferDataItem *item){
  XferDataItemIndex *index = 0;
  index = item->getDataItemIndexWildcard( 1 );
  if( index == 0 )
    index = item->getLastIndex();
  assert( index != 0 );
  index->setLowerbound( 0 );
  item->setDimensionIndizes();
  m_colorColumn = m_cols.size();
  Column *column = new Column( m_colorColumn, "Color", item, GuiElement::align_Default, 0, 0, 0, false, false);
  m_cols.push_back( column );
}

/* --------------------------------------------------------------------------- */
/* setTableSize --                                                             */
/* --------------------------------------------------------------------------- */
void GuiList::setTableSize( int size ) {
  m_tableSize = size;
}

/* --------------------------------------------------------------------------- */
/* setEnableSort --                                                            */
/* --------------------------------------------------------------------------- */
void GuiList::setEnableSort( bool ok ) {
  m_sort_enabled = ok;
}

/* --------------------------------------------------------------------------- */
/* setFunction --                                                              */
/* --------------------------------------------------------------------------- */
void GuiList::setFunction( JobFunction *func ) {
  assert( func != 0 );
  m_function = func;
  m_function->setUsed();
}

/* --------------------------------------------------------------------------- */
/* withIndex --                                                                */
/* --------------------------------------------------------------------------- */
void GuiList::withIndex( int width ) {
  m_withIndex = true;
  int w = std::abs(width);
  col( 0 )->setWidth( w );
  if( w < 5 )
    col( 0 )->setLabel( "Id" );
}

/* --------------------------------------------------------------------------- */
/* col --                                                                      */
/* --------------------------------------------------------------------------- */
GuiList::Column *GuiList::col( int i )const{
#if ( __GNUC__ < 3 )
  if( m_cols.size()>i && i >=0 )
    return m_cols[i];
  throw std::out_of_range( "out of range in m_cols" );
#else
  if (i < m_cols.size())
    return m_cols.at( i );
  return NULL;
#endif
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */
bool GuiList::isDataItemUpdated( TransactionNumber trans ) {
  for( std::vector<Column*>::iterator  iter = m_cols.begin();
       iter != m_cols.end(); ++iter ){
    if ( (*iter)->isDataItemUpdated(trans) )
      return true;
  }
  return false;

}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiList::serializeXML(std::ostream &os, bool recursive){
  std::string s = getElement()->getName();
  HTMLConverter::convert2HTML( s );

  XferDataItem *xfer = col(1)->getParam()->DataItem();
  assert( xfer != 0 );
  XferDataItemIndex *index = xfer->getDataItemIndexWildcard( 1 );
  assert( index != 0 );
  int level = index->Level();
  std::string listItem = col(1)->getParam()->getFullName();
  std::string::size_type x = listItem.find('.');
  for( int i = 0; i != level; ++i ){
    x = listItem.find('.', ++x );
  }

  if( x != std::string::npos ){
    listItem.erase( x, listItem.size()-1 );
  }
  x = listItem.rfind('[');

  if( x != std::string::npos ){
    listItem.erase( x, listItem.size()-1 );
  }

  os << "<intens:List";
  os << " name=\"" << s << "\"";
  os << " base=\"" << listItem << "\"";

  std::string fn = listItem;

  std::string::size_type pos = 0;
  while( ( pos = fn.find( '[', pos ) ) != std::string::npos ){
    fn.insert( pos, "_list" );
    pos += 6;
  }

  pos = 0;
  while( ( pos = fn.find( '.', pos ) ) != std::string::npos ){
    if( fn[pos-1] != ']' ){
      fn.insert( pos, "[0]" );
      pos += 8;
    }
    else{
      ++pos;
    }
  }

  os << " value=\"" << fn << "\"";
  if( m_function != 0 )
    os << " action=\"" << m_function->Name() << "\"";
  os << ">" << std::endl;

  ColsVector::iterator iter = m_cols.begin();
  ++iter;
  int n = 1;
  std::string item;
  for(; iter != m_cols.end(); ++iter, ++n){
#if HAVE_HEADLESS
    s = (*iter)->getLabel();
#else
    s = QtMultiFontString::getQString( (*iter)->getLabel() ).toStdString();
#endif
    HTMLConverter::convert2HTML( s );
    item = col(n)->getParam()->getFullName();
    item.erase( 0, listItem.size() );
    x = item.find('.');
    if( x != std::string::npos ){
      item.erase( 0, ++x );
    }
    os << "<intens:Column label=\"" << s;
    fn = item;

    std::string::size_type pos = 0;

    while( ( pos = fn.find( '.', pos ) ) != std::string::npos ){
      if( fn[pos-1] != ']' ){
	fn.insert( pos, "[0]" );
	pos += 8;
      }
      else{
	++pos;
      }
    }

    os << "\" value=\"" << fn << "\"";
    os << "/>" << std::endl;
  }
  os << "</intens:List>"<< std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiList::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  bool updated = isDataItemUpdated(GuiManager::Instance().LastWebUpdate());
  if (onlyUpdated && !updated) {
    return false;
  }
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["title"] = getTitle();
  jsonObj["title_alignment"] = GuiElement::StringAlignmentType(m_titleAlign);
  jsonObj["expand"] = isResizeable() ? getElement()->StringExpandType() : "none"; // override
  jsonObj["rowCount"] = tableSize();
  jsonObj["updated"] = updated;
  std::vector<int> selectedIdxs;
  getSelectedRows(selectedIdxs);
  jsonObj["selected"] = Json::Value(Json::arrayValue);
  if (selectedIdxs.size()) {
    std::vector<int>::iterator it;
    for (it = selectedIdxs.begin(); it != selectedIdxs.end(); ++it) {
      jsonObj["selected"].append(*it);
    }
  }
  jsonObj["multi_selection"] = isMultipleSelection();
  if( m_function != 0 )
    jsonObj["action"] = m_function->Name();

  int rows = getMaxRows();
  // row colors
  jsonObj["bgcolor"] = Json::Value(Json::arrayValue);
  jsonObj["bgcolor"].resize(rows);
  if (getColorColumn() != -1) {
    jsonObj["fgcolor"] = Json::Value(Json::arrayValue);
    jsonObj["fgcolor"].resize(rows);
  }
  for (int r = 0; r < rows; ++r) {
    std::string foreground("#000000");
    std::string background("#ffffff");
    if (getColorColumn() != -1) {
      // Column Color
      columnCellColor(getColorColumn(), r, foreground, background);
      jsonObj["fgcolor"][r] = foreground;
    } else {
      // Alternate Color
      GuiTable::alternateColor(background, r);
    }
    jsonObj["bgcolor"][r] = background;
  }

  ColsVector::iterator iter=m_cols.begin();
  ++iter;
  int n = 1;
  jsonObj["element"] = Json::Value(Json::arrayValue);
  if (m_cols.size() > 1)
    jsonObj["element"].resize(m_cols.size() - 1);

  for(; iter != m_cols.end(); ++iter, ++n){
    jsonObj["element"][n-1] = Json::Value(Json::objectValue);
    // values
    jsonObj["element"][n-1]["values"] = Json::Value(Json::arrayValue);
    jsonObj["element"][n-1]["values"].resize(rows);
    if (getColorColumn() == -1){
      jsonObj["element"][n-1]["fgcolor"] = Json::Value(Json::arrayValue);
      jsonObj["element"][n-1]["fgcolor"].resize(rows);
      jsonObj["element"][n-1]["bgcolor"] = Json::Value(Json::arrayValue);
      jsonObj["element"][n-1]["bgcolor"].resize(rows);
    }
    for (int i = 0; i < rows; ++i) {
      std::string foreground("#000000");
      std::string background("#ffffff");
      std::string s = col(n)->cellFormattedValue(i);
      trim(s);
      jsonObj["element"][n-1]["values"][i] = s;
      if (getColorColumn() == -1){
        if (columnCellColor(n, i, foreground, background)){
          jsonObj["element"][n-1]["fgcolor"][i] = foreground;
          jsonObj["element"][n-1]["bgcolor"][i] = background;
        }
      }
    }
    jsonObj["element"][n-1]["label"] = (*iter)->getLabel();
    getElement()->writeDataTypeJsonProperties(jsonObj["element"][n-1], col(n)->getParam()->DataItem(), col(n)->getParam());
    jsonObj["element"][n-1]["alignment"] = GuiElement::StringAlignmentType((*iter)->getAlignment());
    jsonObj["element"][n-1]["optional"] = col(n)->isOptional();
    jsonObj["element"][n-1]["width"] = col(n)->getWidth();

    XferDataItemIndex *index = col(n)->getParam()->DataItem()->getDataItemIndexWildcard( 1 );
    if( index == 0 )
      index = col(n)->getParam()->DataItem()->getLastIndex();
    col(n)->getParam()->DataItem()->setIndex( index, -1 );
    jsonObj["element"][n-1]["fullname"] = col(n)->getParam()->getFullName();
  }

  // popup base menu
  GuiPopupMenu* pm = getElement()->getPopupMenu();
  if (pm) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    pm->serializeJson(jsonElem, onlyUpdated);
    jsonObj["popup_base_menu"] = jsonElem;
  }

  return true;
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiList::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  bool updated = isDataItemUpdated(GuiManager::Instance().LastWebUpdate());
  if (onlyUpdated && !updated) {
    return false;
  }
  auto element = eles->add_lists();
  element->set_allocated_base(getElement()->writeProtobufProperties());
  element->set_title(getTitle());
  element->set_title_alignment(GuiElement::ProtoAlignmentType(m_titleAlign));
  element->mutable_base()->set_expand(isResizeable() ?
                                        getElement()->ProtoOrientationType() :
                                        in_proto::GuiElement::Orientation::GuiElement_Orientation_None);

  element->set_row_count(tableSize());
  std::vector<int> selectedIdxs;
  getSelectedRows(selectedIdxs);
  element->mutable_selected()->Assign(selectedIdxs.begin(), selectedIdxs.end());

  element->set_multi_selection(isMultipleSelection());
  if( m_function != 0 )
    element->set_action(m_function->Name());

  int rows = getMaxRows();
  for (int r = 0; r < rows; ++r) {
    std::string foreground("#000000");
    std::string background("#ffffff");
    if (getColorColumn() != -1) {
      // Column Color
      columnCellColor(getColorColumn(), r, foreground, background);

    } else {
      // Alternate Color
      GuiTable::alternateColor(background, r);
    }
    element->add_fgcolor(foreground);
    element->add_bgcolor(background);
  }

  ColsVector::iterator iter=m_cols.begin();
  ++iter;
  int n = 1;

  for(; iter != m_cols.end(); ++iter, ++n){
    auto entry = element->add_entries();
    // values
    for (int i = 0; i < rows; ++i) {
      std::string s = col(n)->cellFormattedValue(i);
      trim(s);
      entry->add_values(s);
      if (getColorColumn() == -1){
        std::string foreground("#000000");
        std::string background("#ffffff");
        if (columnCellColor(n, i, foreground, background)){
          entry->add_fgcolors(foreground);
          entry->add_bgcolors(background);
        } else {
          entry->add_fgcolors("");
          entry->add_bgcolors("");
        }
      }
    }
    entry->set_label((*iter)->getLabel());
    entry->set_allocated_value(getElement()->writeDataTypeProtobufProperties(col(n)->getParam()->DataItem(), col(n)->getParam()));
    entry->set_alignment(GuiElement::ProtoAlignmentType((*iter)->getAlignment()));
    entry->set_optional(col(n)->isOptional());
    entry->set_width(col(n)->getWidth());
    XferDataItemIndex *index = col(n)->getParam()->DataItem()->getDataItemIndexWildcard( 1 );
    if( index == 0 )
      index = col(n)->getParam()->DataItem()->getLastIndex();
    col(n)->getParam()->DataItem()->setIndex( index, -1 );
    entry->set_full_name(col(n)->getParam()->getFullName());
  }

  // popup base menu
  GuiPopupMenu* pm = getElement()->getPopupMenu();
  if (pm) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    pm->serializeProtobuf(eles, element->mutable_popup_base_menu(), onlyUpdated);
  }

  return true;
}
#endif

/*=============================================================================*/
/* private class Column                                                        */
/*=============================================================================*/
GuiList::Column::Column( int column
	    ,const std::string &label
	    , XferDataItem *dataItem
	    , const GuiElement::Alignment &align
	    , const int length
	    , const int precision
	    , Scale *scale
	    , const bool thousand_sep
	    , bool optional )
  : m_alignment( align )
  , m_label( label )
  , m_xfer_label( 0 )
  , m_column( column )
  , m_isInverted( length < 0 )
  , m_optional( optional )
  , m_param(0) {
  m_width = std::abs( length );
  if( dataItem != 0 ){
    installDataItem( dataItem );
    assert( m_param != 0 );
    if ( dataItem->getDataType() != DataDictionary::type_String ) {
      m_param->setLength( length );
    }
    m_param->setPrecision( precision );
    m_param->setScalefactor( scale );
    if( thousand_sep )
      m_param->setThousandSep();
  }
  else
    m_param = 0;
}

GuiList::Column::Column( int column
	    , XferDataItem *dataItemLabel
	    , XferDataItem *dataItem
	    , const GuiElement::Alignment &align
	    , const int length
	    , const int precision
	    , Scale *scale
	    , const bool thousand_sep
	   , bool optional)
  : m_alignment( align )
  , m_xfer_label( dataItemLabel )
  , m_column( column )
  , m_isInverted( length < 0 )
  , m_optional( optional )
  , m_param(0) {
  m_width = std::abs( length );
  if( dataItem != 0 ){
    installDataItem( dataItem );
    assert( m_param != 0 );
    if ( dataItem->getDataType() != DataDictionary::type_String ) {
      m_param->setLength( length );
    }
    m_param->setPrecision( precision );
    m_param->setScalefactor( scale );
    if( thousand_sep )
      m_param->setThousandSep();
  }
  else
    m_param = 0;
}

GuiList::Column::Column( const Column& col )
  : m_alignment( col.m_alignment )
  , m_label ( col.m_label  )
  , m_xfer_label( 0 )
  , m_column( col.m_column )
  , m_param ( col.m_param ? col.m_param->clone() : 0)
  , m_width ( col.m_width  )
  , m_isInverted ( col.m_isInverted  )
  , m_optional ( col.m_optional  )
{
  if (col.m_xfer_label)
    m_xfer_label=new XferDataItem( *(col.m_xfer_label) );
}


GuiList::Column::~Column() {
  delete m_param;
  delete m_xfer_label;
}

//-------------------------------------------------------------------
// getLabel --
//-------------------------------------------------------------------

const std::string& GuiList::Column::getLabel(){
  if( m_xfer_label ){
    if( !m_xfer_label->getValue(m_label) ){
      m_label="";
    }
  }
  else if( !m_label.empty() ){
  }
  else{
    m_label=m_xfer_label->getLabel();
  }
  return m_label;
  }

//-------------------------------------------------------------------
// getTootip --
//-------------------------------------------------------------------

const std::string GuiList::Column::getTooltip(){
  std::string label("");
  if( m_param ){
    label = m_param->DataItem()->userAttr().Helptext();
  }
  return label;
}

/* --------------------------------------------------------------------------- */
/* installDataItem --                                                          */
/* --------------------------------------------------------------------------- */
bool GuiList::Column::installDataItem( XferDataItem *dataitem ){
  assert( dataitem != 0 );
  assert( m_param == 0 );
  DataReference *data = dataitem->Data();
  assert( data != 0 );

  switch( data->getDataType() ){
  case DataDictionary::type_Integer:
    m_param = new XferParameterInteger( 0, 0, 0 );
    break;
  case DataDictionary::type_Real:
    m_param = new XferParameterReal( 0, 0, -1 , 0, false );
    break;

  case DataDictionary::type_Complex:
    switch( dataitem->getParamType() ){
    case XferDataItem::param_Default:
      m_param = new XferParameterComplex( 0, 0, -1, 0, false );
      break;
    case XferDataItem::param_ComplexReal:
    case XferDataItem::param_ComplexImag:
    case XferDataItem::param_ComplexAbs:
    case XferDataItem::param_ComplexArg:
      m_param = new XferParameterReal( 0, 0, -1 , 0, false );
      break;
    default:
      assert( false );
    }
    break;
  case DataDictionary::type_String:
    m_param = new XferParameterString( 0, 0, '\0' );
    break;

  default:
    return false;
  }
  assert( m_param != 0 );
  m_param->setDataItem( dataitem );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getDataType --                                                              */
/* --------------------------------------------------------------------------- */
std::string GuiList::Column::getDataType(){
  DataDictionary::DataType dataType = DataDictionary::type_None;
  if( m_param == 0 )
    return "";
  dataType = m_param->DataItem()->Data()->getDataType();
  switch(dataType){
  case DataDictionary::type_Integer:
    return "integer";
  case DataDictionary::type_Real:
    return "double";
  case DataDictionary::type_Complex:
    return "complex";
  case DataDictionary::type_String:
    return "string";
  case DataDictionary::type_CharData:
    return "cdata";
  default:
    // ignore 'type_None', 'type_StructDefinition', 'type_StructVariable'
    ;
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* getDataType --                                                              */
/* --------------------------------------------------------------------------- */
bool GuiList::Column::getDataType( DataDictionary::DataType &dataType ){
  dataType = DataDictionary::type_None;
  if( m_param == 0 )
    return "";
  dataType = m_param->DataItem()->Data()->getDataType();
  return true;
}

/* --------------------------------------------------------------------------- */
/*  -- getRowsCount                                                            */
/* --------------------------------------------------------------------------- */
const int GuiList::Column::getRowsCount(){
  if( m_param == 0 ) {
    return 0;
  }
  XferDataItem *dataItem = m_param->DataItem();
  XferDataItemIndex *index = dataItem->getDataItemIndexWildcard( 1 );
  dataItem->setIndex( index, 0 );
  return dataItem->isValid() ? index->getDimensionSize( dataItem->Data() ) : 0;
}

/* --------------------------------------------------------------------------- */
/*  -- cellFormattedValue                                                      */
/* --------------------------------------------------------------------------- */
std::string GuiList::Column::cellFormattedValue( int idx ){
   std::string value;
   if( m_param != 0 ){
     XferDataItem *dataItem = m_param->DataItem();
     XferDataItemIndex *index = dataItem->getDataItemIndexWildcard( 1 );
     dataItem->setIndex( index, idx );
     m_param->getFormattedValue( value );

     if (AppData::Instance().HeadlessWebMode() &&
         dataItem->getUserAttr()->IsToggle()) {
       QString qvalue = QString::fromStdString(value).trimmed();
        bool ret(false);
        int num;
        if (value.size() == 0)
          return "";
          num = qvalue.toInt(&ret);
        if (ret && num) {
         return "✖";  //✔☑◼
       }
       return ""; //☐
     }

     // falls dataset
     std::string str;
     if(dataItem->getDataSetInputValue(str, value)) {
       return QString::fromStdString(str).trimmed().toStdString();
     }
     return value;
   }
   return value;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiList::Column::setIndex( const std::string &name, int inx ){
  BUG_PARA(BugGui,"GuiQtList::Column::setIndex",name << " =[" << inx << "]");
  if( m_param != 0 ){
    assert( inx >= 0 );
    m_param->DataItem()->setIndex( name, inx );
  }
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiList::Column::acceptIndex( const std::string &name, int inx ){
  BUG(BugGuiFld,"GuiQtList::Column::acceptIndex");
  if( m_param == 0 ){
    return true;
  }
  return m_param->DataItem()->acceptIndex( name, inx );
}


/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */
bool GuiList::Column::isDataItemUpdated( TransactionNumber trans ) {
  if (m_xfer_label && m_xfer_label->isDataItemUpdated(trans))
    return true;
  if( m_param != 0 )
    //    if ( m_param->DataItem())
    return m_param->DataItem()->isDataItemUpdated(trans);
  return false;
}

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string GuiList::variantMethod(const std::string& method,
                                     const Json::Value& jsonArgs,
                                   JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);

  // MenuButtonAction
  if (lower(method) == "menubuttonaction") {
    if (jsonArgs.isMember("id") && jsonArgs["id"].isInt()) {
      GuiButtonListener* btnLsnr = GuiButtonListener::getButtonListenerByMenuButtonId(jsonArgs["id"].asInt());
      if (btnLsnr) {
        if (jsonArgs.isMember("selected") && jsonArgs["selected"].isArray()) {
          pushArgSelected(jsonArgs["selected"]);
        }
        btnLsnr->ButtonPressed();
        jsonElem["status"] = "OK";
      } else {
        jsonElem["message"] = "Wrong Parameter, id=" + jsonArgs["id"].asString();
        jsonElem["status"] = "Error";
      }
    }
  }

  // GetSelectList
  if (method == "GetSelectList") {
    Json::Value jsonAry = Json::Value(Json::arrayValue);
    std::vector<int> idxs;
    getSelectedRows(idxs);
    std::vector<int>::iterator it = idxs.begin();
    for(; it != idxs.end(); ++it)
      jsonAry.append( *it );

    jsonElem["selected_rows"] = jsonAry;
    jsonElem["status"] = "OK";
  } else

  // SelectList
  if (method == "SetSelectList") {
    Json::Value::Members members = jsonArgs.getMemberNames();
    BUG_DEBUG("SelectList Args: " << ch_semafor_intens::JsonUtils::value2string(jsonArgs)
              << ", isArray: " << jsonArgs["id"].isArray());
    if (jsonArgs.isMember("id") && jsonArgs["id"].isArray()) {
      std::vector<int> intVector;
      for (int i=0; i< jsonArgs["id"].size(); ++i) {
        int idx = jsonArgs["id"].get(i, 0).asInt();
        intVector.push_back(idx);
      }
      selectRows(intVector);
      jsonElem["status"] = "OK";
    } else {
      jsonElem["message"] = "Wrong Parameter";
      jsonElem["status"] = "Error";
    }
  }

  if (!jsonElem.isNull()) {
    BUG_DEBUG("variantMethod Method[" << method << "], Args["
              << ch_semafor_intens::JsonUtils::value2string(jsonArgs) << "]  Return: "
              << ch_semafor_intens::JsonUtils::value2string(jsonElem));
    return ch_semafor_intens::JsonUtils::value2string(jsonElem);
  }
  return getElement()->defaultVariantMethod(method, jsonArgs, eng);
}

/* --------------------------------------------------------------------------- */
/* pushArgSelected --                                                          */
/* --------------------------------------------------------------------------- */
void GuiList::pushArgSelected(const Json::Value& jsonAry) {
  BUG_DEBUG("Push Selected: " << ch_semafor_intens::JsonUtils::value2string(jsonAry));
  ch_semafor_intens::JsonUtils::value2vectorInt(jsonAry,  m_argSelected);
}

/* --------------------------------------------------------------------------- */
/* popArgSelected --                                                          */
/* --------------------------------------------------------------------------- */
bool GuiList::popArgSelected(int& x0, int& y0, int& x1, int& y1) {
  x0 = -1, y0 = -1, x1 = -1, y1 = -1;
  BUG_DEBUG("POP Selected: " << m_argSelected.size());
  if (m_argSelected.size()) {
    if (m_argSelected.size() > 0) x0 = m_argSelected[0];
    if (m_argSelected.size() > 1) y0 = m_argSelected[1];
    if (m_argSelected.size() > 2) x1 = m_argSelected[2];
    if (m_argSelected.size() > 3) y1 = m_argSelected[3];
    m_argSelected.clear();
    return true;
  }
  return false;
}
