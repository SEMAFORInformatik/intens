
#include <sstream>
#include "utils/Debugger.h"
#include "xfer/XferDataItemIndex.h"
// only needed to prevent undeclared compile errors with QT
#ifdef HAVE_QT
typedef short Position;
#endif
// FIX IT!
#include "gui/GuiManager.h"
#include "gui/GuiIndex.h"
#include "xml/XMLDocumentHandler.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XferDataItemIndex::XferDataItemIndex( int level, bool isGuiIndex )
  : m_level( level )
  , m_indexnum( 0 )
  , m_lowerbound( 0 )
  , m_upperbound( 0 )
  , m_indexval( 0 )
  , m_wildcard( false )
  , m_isGuiIndex( isGuiIndex ){
}

XferDataItemIndex::XferDataItemIndex( XferDataItemIndex &index )
  : m_indexname( index.m_indexname )
  , m_fg_name( index.m_fg_name )
  , m_level( index.m_level )
  , m_indexnum( index.m_indexnum )
  , m_lowerbound( index.m_lowerbound )
  , m_upperbound( index.m_upperbound )
  , m_indexval( index.m_indexval )
  , m_wildcard( index.m_wildcard )
  , m_isGuiIndex( index.m_isGuiIndex ){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setIndexName --                                                             */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setIndexName( const std::string &name, const std::string &fg_name ){
  m_indexname = name;
  m_fg_name = fg_name;
}

/* --------------------------------------------------------------------------- */
/* setIndexNum --                                                              */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setIndexNum( int num ){
  m_indexnum = num;
}

/* --------------------------------------------------------------------------- */
 /* setLowerbound --                                                           */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setLowerbound( int low ){
  m_lowerbound = low;
}

/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setUpperbound( int up ){
  m_upperbound = up;
}

/* --------------------------------------------------------------------------- */
/* setWildcard --                                                              */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setWildcard(){
  m_wildcard = true;
}

/* --------------------------------------------------------------------------- */
/* resetWildcard --                                                            */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::resetWildcard(){
  m_wildcard = false;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool XferDataItemIndex::acceptIndex( const std::string &name, int inx ){
  if( name != m_indexname ){
    return true; /* nicht interessant */
  }
  if( inx < 0 ){
    return false; /* falsch */
  }
  if( m_upperbound > m_lowerbound ){
    if( m_lowerbound + inx > m_upperbound ){
      return false;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItemIndex::setIndex( DataReference *dataref
				, const std::string &name
				, int inx )
{
  BUG_PARA( BugXfer, "setIndex", "name=" << name << ", inx=" << inx );

  if( name != m_indexname ){
    BUG_EXIT( "doesnt match" );
    return false;
  }
  BUG_MSG("Level=" << m_level << ", Num=" << m_indexnum << ", Inx=" << inx);
  m_indexval = inx;
  dataref->SetDimensionIndexOfLevel( m_lowerbound + inx, m_level, m_indexnum );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setIndex( DataReference *dataref, int inx ){
  BUG_PARA(BugXfer,"setIndex"
          ,"Level=" << m_level << ", Num=" << m_indexnum << ", Inx=" << inx);

  m_indexval = inx;
  dataref->SetDimensionIndexOfLevel( m_lowerbound + inx, m_level, m_indexnum );
}

/* --------------------------------------------------------------------------- */
/* IndexName --                                                                */
/* --------------------------------------------------------------------------- */

const std::string &XferDataItemIndex::IndexName(){
  BUG( BugXfer, "IndexName" );
  BUG_EXIT( "return " << m_indexname );
  return m_indexname;
}

/* --------------------------------------------------------------------------- */
/* IndexOrFgName --                                                            */
/* --------------------------------------------------------------------------- */

const std::string &XferDataItemIndex::IndexOrFgName(){
  if( m_fg_name.empty() ){
    return m_indexname;
  }
  return m_fg_name;
}

/* --------------------------------------------------------------------------- */
/* isGuiIndex --                                                               */
/* --------------------------------------------------------------------------- */

bool XferDataItemIndex::isGuiIndex() const{
  return m_isGuiIndex;
}

/* --------------------------------------------------------------------------- */
/* Level --                                                                    */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::Level(){
  return m_level;
}

/* --------------------------------------------------------------------------- */
/* setLevel --                                                                 */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setLevel(int l){
  m_level=l;
}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::getDimensionSize( DataReference *dataref, bool incl_attr ) const{
  return getDimensionSize( *dataref, incl_attr );
}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::getDimensionSize( const DataReference &dataref, bool incl_attr ) const{
  BUG_PARA(BugXfer,"getDimensionSize",dataref.fullName( true ));

  DataReference::Ptr ref_ptr = DataPool::newDataReference( dataref );
  DataReference &ref = *ref_ptr;

  int level = ref.GetNumberOfLevels() -1;
  assert( level >= m_level );
  for(;level > m_level; level-- ){
    ref.ConvertToParentReference();
  }
  while( ref.hasWildcardsNotLastLevel() ){
    ref.ConvertToParentReference();
  }

  BUG_MSG( "Parent: " << ref.fullName( true ) );
  DataContainer::SizeList dimsize;
  int sz = ref.GetAllDataDimensionSize( dimsize, incl_attr );

  if( sz < 1 ){
    return 0;
  }

  int numinx = ref.GetNumberOfDimensionIndizes();
  assert( m_indexnum < numinx );

  int diff = numinx - sz;
  if( m_indexnum < diff ){
    return dimsize[0] == 0 ? 0 : 1;
  }

  int inx = m_indexnum - diff;
  assert( inx < sz );
  return dimsize[inx];
}

/* --------------------------------------------------------------------------- */
/* IndexNum --                                                                 */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::IndexNum(){
  return m_indexnum;
}

/* --------------------------------------------------------------------------- */
/* InitialIndex --                                                             */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::InitialIndex(){
  if( m_wildcard ){
    return m_lowerbound;
  }
  return m_lowerbound + m_indexval;
}

/* --------------------------------------------------------------------------- */
/* Lowerbound --                                                               */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::Lowerbound(){
  return m_lowerbound;
}

/* --------------------------------------------------------------------------- */
/* Upperbound --                                                               */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::Upperbound(){
  return m_upperbound;
}

/* --------------------------------------------------------------------------- */
/* isWildcard --                                                               */
/* --------------------------------------------------------------------------- */

bool XferDataItemIndex::isWildcard(){
  return m_wildcard;
}

/* --------------------------------------------------------------------------- */
/* isIndexed --                                                                */
/* --------------------------------------------------------------------------- */

bool XferDataItemIndex::isIndexed(){
  BUG( BugXfer, "isIndexed" );
  bool what = !m_indexname.empty() && !m_isGuiIndex;
  BUG_EXIT( "return " << what );
  return what;
}

/* --------------------------------------------------------------------------- */
/* erase --                                                                    */
/* invalidates all items                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::erase( DataReference *dref ){
  if( dref == 0 ){
    return;
  }

  BUG( BugXfer,"erase" );

  int saved_inx = dref->GetDimensionIndexOfLevel( m_level, m_indexnum );
  dref->SetDimensionIndexOfLevel( DataReference::WildcardIndex, m_level, m_indexnum );
  dref->clearDataItem();
  dref->SetDimensionIndexOfLevel( saved_inx, m_level, m_indexnum );
}

/* --------------------------------------------------------------------------- */
/* exchangeDataPointers --                                                     */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::exchangeDataPointers( DataReference * &ref1, DataReference * &ref2 ){
  DataReference *ref0 = ref1;
  ref1 = ref2;
  ref2 = ref0;
}

/* --------------------------------------------------------------------------- */
/* insertData --                                                               */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::insertData( const DataReference &ref, int inx ){
  BUG_PARA(BugXfer,"insertData","Index = " << inx);

  DataReference::Ptr ref_from = DataPool::newDataReference( ref );
  DataReference::Ptr ref_to   = DataPool::newDataReference( ref );

  int dimsize    = getDimensionSize( ref_from.get_ptr() );
  int startinx   = m_lowerbound + inx;
  int upperbound = dimsize + 1; // Einfach zu gross

  if( m_upperbound > 0 ){
    dimsize    = m_upperbound+1 < dimsize ? m_upperbound+1 : dimsize;
    upperbound = m_upperbound;
    if( startinx > m_upperbound ) return;
  }
  else{
    if( startinx >= dimsize ) return;
  }

  ref_to->SetDimensionIndexOfLevel( dimsize, m_level, m_indexnum );

  for( int i = dimsize -1; i >= startinx; i-- ){
    BUG_MSG("upperbound = " << upperbound << ", index = " << i);
    ref_from->SetDimensionIndexOfLevel( i, m_level, m_indexnum );
    if( i < upperbound ){
      ref_to->AssignDataElement( *ref_from );
    }
    ref_from.exchange( ref_to );
  }

  ref_to->SetDimensionIndexOfLevel( startinx, m_level, m_indexnum );
  ref_to->SetItemInvalid_PreInx();
}

/* --------------------------------------------------------------------------- */
/* deleteData --                                                               */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::deleteData( const DataReference &ref, int inx ){
  BUG_PARA(BugXfer,"deleteData","Index = " << inx);

  DataReference::Ptr ref_from = DataPool::newDataReference( ref );
  DataReference::Ptr ref_to   = DataPool::newDataReference( ref );

  int dimsize    = getDimensionSize( *ref_from );
  int startinx   = m_lowerbound + inx;
  int upperbound = dimsize + 1; // Einfach zu gross

  if( m_upperbound > 0 ){
    dimsize    = m_upperbound+1 < dimsize ? m_upperbound+1 : dimsize;
    upperbound = m_upperbound;
    if( startinx > m_upperbound ) return;
  }
  else{
    if( startinx >= dimsize ) return;
  }

  ref_to->SetDimensionIndexOfLevel( startinx, m_level, m_indexnum );

  for( int i = startinx; i < dimsize; i++ ){
    if( i < upperbound ){
      ref_from->SetDimensionIndexOfLevel( i+1, m_level, m_indexnum );
      ref_to->AssignDataElement( *ref_from );
    }
    else{
      ref_to->SetItemInvalid_PreInx();
    }
    ref_from.exchange( ref_to );
  }
}

/* --------------------------------------------------------------------------- */
/* duplicateData --                                                            */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::duplicateData( const DataReference &ref, int inx ){
  BUG_PARA(BugXfer,"duplicateData","Index = " << inx);

  DataReference::Ptr ref_from = DataPool::newDataReference( ref );
  DataReference::Ptr ref_to   = DataPool::newDataReference( ref );

  int dimsize    = getDimensionSize( *ref_from );
  int startinx   = m_lowerbound + inx;
  int upperbound = dimsize + 1; // Einfach zu gross

  if( m_upperbound > 0 ){
    dimsize    = m_upperbound+1 < dimsize ? m_upperbound+1 : dimsize;
    upperbound = m_upperbound;
    if( startinx > m_upperbound ) return;
  }
  else{
    if( startinx >= dimsize ) return;
  }
  ref_to->SetDimensionIndexOfLevel( dimsize, m_level, m_indexnum );

  for( int i = dimsize -1; i >= startinx; i-- ){
    ref_from->SetDimensionIndexOfLevel( i, m_level, m_indexnum );
    if( i < upperbound ){
      ref_to->AssignDataElement( *ref_from );
    }
    ref_from.exchange( ref_to );
  }
}

/* --------------------------------------------------------------------------- */
/* clearData --                                                                */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::clearData( const DataReference &ref, int inx ){
  BUG_PARA(BugXfer,"clearData","Index = " << inx);

  DataReference::Ptr ref_to = DataPool::newDataReference( ref );

  int startinx = m_lowerbound + inx;
  if( m_upperbound > 0 ){
    if( startinx > m_upperbound ) return;
  }
  ref_to->SetDimensionIndexOfLevel( startinx, m_level, m_indexnum );
  ref_to->SetItemInvalid_PreInx();
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

int XferDataItemIndex::getValue(){
  return m_lowerbound + m_indexval;
}

/* --------------------------------------------------------------------------- */
/* packData --                                                                 */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::packData( const DataReference &ref, int inx ){
  BUG_PARA(BugXfer,"packData","Index = " << inx);

  DataReference::Ptr ref_from = DataPool::newDataReference( ref );
  DataReference::Ptr ref_to   = DataPool::newDataReference( ref );

  int dimsize  = getDimensionSize( *ref_from );
  int startinx = m_lowerbound;

  if( m_upperbound > 0 ){
    dimsize    = m_upperbound+1 < dimsize ? m_upperbound+1 : dimsize;
  }
  if( startinx >= dimsize ) return;

  // Wir such den ersten ungültigen Wert
  int i;
  int i_empty = dimsize +1;
  for( i = startinx; i < dimsize; i++ ){
    ref_to->SetDimensionIndexOfLevel( i, m_level, m_indexnum );
    if( !ref_to->GetItemValid_PreInx() ){
      i_empty = i;
      i = dimsize;
    }
  }
  if( i_empty >= dimsize ) return;

  // Die Werte werden zusammengeschoben.
  for( i = i_empty +1; i < dimsize; i++ ){
    ref_from->SetDimensionIndexOfLevel( i, m_level, m_indexnum );
    if( ref_from->GetItemValid_PreInx() ){
      ref_to->AssignDataElement( *ref_from );
      ref_to->SetDimensionIndexOfLevel( ++i_empty, m_level, m_indexnum );
    }
  }

  // Die verbleibenden Werte werden gelöscht
  for( i = i_empty; i < dimsize; i++ ){
    ref_to->SetDimensionIndexOfLevel( i, m_level, m_indexnum );
    ref_to->SetItemInvalid_PreInx();
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::print( std::ostream & o ) const {
  o <<  "XferDataItemIndex: " << this << m_indexname << " level= " << m_level;
  o << " IndexNum= " << m_indexnum << " lower=" <<  m_lowerbound;
  o << " upper=" <<  m_upperbound << " val=" <<     m_indexval << " wildcard=" << m_wildcard << std::endl;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::marshal( std::ostream &os ){
  os << "<indexes"
     << " level=\""    << m_level      << "\"";
  if( ! m_indexname.empty() )
    os << " name=\""   << m_indexname  << "\"";
  if( m_indexnum != 0 )
    os << " num=\""    << m_indexnum   << "\"";
  if( m_lowerbound != 0 || m_indexname.empty() )
    os << " lbound=\"" << m_lowerbound << "\"";
  if( m_upperbound != 0 )
    os << " ubound=\"" << m_upperbound << "\"";
  if( m_indexval != 0 )
    os << " val=\""    << m_indexval   << "\"";
  if( m_wildcard )
    os << " wildcard=\"true\"";
  if( m_isGuiIndex )
    os << " isGuiIndex=\"true\"";
  os << ">";
  os << "</indexes>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable * XferDataItemIndex::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "indexes" ){
    //--name-----------------------------------
    std::string sName=attributeList["name"];
    if( sName.empty() )
      m_indexname = "";
    else {
      m_indexname = sName;
    }
    //--name--GUIobject------------------------
    std::string::size_type pos = m_indexname.find( '#', 0 );	//[0]
    if( pos == std::string::npos && m_indexname.size()!=0 ){
      GuiIndex::addIndexContainer( m_indexname, GuiElement::findElement(m_indexname)->getGuiIndex() );
    }
    //--level----------------------------------
    // m_level is set by constructor( int )
    //--num------------------------------------
    std::string sNum=attributeList["num"];
    if( sNum.empty() )
      m_indexnum = 0;
    else {
      int iNum;
      std::istringstream is( sNum );
      is >> iNum;
      m_indexnum = iNum;
    }
    //--lowerBound-----------------------------
    std::string sLbound=attributeList["lbound"];
    if( sLbound.empty() )
      m_lowerbound = 0;
    else {
      int iLbound;
      std::istringstream is( sLbound );
      is >> iLbound;
      m_lowerbound = iLbound;
    }
    //--upperBound-----------------------------
    std::string sUbound=attributeList["ubound"];
    if( sUbound.empty() )
      m_upperbound = 0;
    else {
      int iUbound;
      std::istringstream is( sUbound );
      is >> iUbound;
      m_upperbound = iUbound;
    }
    //--val------------------------------------
    std::string sVal=attributeList["val"];
    if( sVal.empty() )
      m_indexval = 0;
    else {
      int iVal;
      std::istringstream is( sVal );
      is >> iVal;
      m_indexval = iVal;
    }
    //--wildcard-------------------------------
    std::string s=attributeList["wildcard"];
    m_wildcard = (!s.empty());
    //--wildcard-------------------------------
    std::string sGui=attributeList["isGuiIndex"];
    m_isGuiIndex = (!sGui.empty());
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void XferDataItemIndex::setText( const std::string &text ){
}
