
#include <numbers>
#include <sstream>
#include <limits.h>

#include "utils/Debugger.h"
#include "utils/NumLim.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataPoolDefinition.h"
#include "app/DataPoolIntens.h"
#include "app/ItemAttr.h"
#include "app/UserAttr.h"
#include "app/DataAlterSetDbModified.h"
#include "app/DataSet.h"
#include "xml/XMLDocumentHandler.h"
#include "gui/GuiIndex.h"

#include <algorithm>

INIT_LOGGER();

#define SAY(X) // std::cout << "SAY: " << X << std::endl;

/* Predicate class for container operations */
class LevelCompare {
public:
  LevelCompare( int l ): m_level(l){}
  bool operator()( XferDataItemIndex *i ){ return m_level==i->Level(); }
private:
  int m_level;
};

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XferDataItem::XferDataItem( const XferDataItem &item )
  : m_dataref( 0 )
  , m_paramtype( item.m_paramtype ){
  BUG_DEBUG( "Copy Constructor XferDataItem" );
  m_dataref = DataPool::newDataReference( *item.m_dataref );

  DataItemIndexList::const_iterator ix;
  for( ix = item.m_indexlist.begin(); ix != item.m_indexlist.end(); ++ix ){
    m_indexlist.push_back( new XferDataItemIndex( *(*ix) ) );
  }
}

XferDataItem::XferDataItem( const XferDataItem &item, DataReference *const ref )
  : m_dataref( ref )
  , m_paramtype( item.m_paramtype ){
  BUG_DEBUG("Copy Constructor XferDataItem (dref)" );
  assert( ref != 0 );

  DataItemIndexList::const_iterator ix;
  for( ix = item.m_indexlist.begin(); ix != item.m_indexlist.end(); ++ix ){
    m_indexlist.push_back( new XferDataItemIndex( *(*ix) ) );
  }
}

XferDataItem::~XferDataItem(){
  for( DataItemIndexList::iterator it = m_indexlist.begin(); it != m_indexlist.end(); ++it ){
    delete (*it);
  }
  m_indexlist.clear();

  if( m_dataref != 0 ){
    delete m_dataref;
  }
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void XferDataItem::marshal( std::ostream &os ){
  os << "<XferDataItem";
//   os << " name=\""    << getName()  << "\"";
  os << " fullName=\""    << getFullName( false ) << "\"";
  if( m_paramtype != 0 )
    os << " parType=\"" << m_paramtype          << "\"";
  os << ">\n";
  DataItemIndexList::iterator ix;
  for( ix = m_indexlist.begin(); ix != m_indexlist.end(); ++ix ){
    (*ix) -> marshal( os );
  }
  os << "</XferDataItem>\n";
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable * XferDataItem::unmarshal( const std::string &element,
					const XMLAttributeMap &attributeList ){
  if( element == "XferDataItem" ){
    std::string sName=attributeList["fullName"];
    if( !sName.empty() ){
      m_dataref = DataPoolIntens::getDataReference( sName );
    }
    std::string sType=attributeList["parType"];
    if( sType.empty() ){
      m_paramtype = (ParameterType) 0;
    }
    else {
      int iType;
      std::istringstream is( sType );
      is >> iType;
      m_paramtype = (ParameterType) iType;
    }
    return this;
  }
  else if( element == "/XferDataItem" ){
    setDimensionIndizes();
    return this;
  }

  else if( element == "indexes" ){
    std::string sLevel=attributeList["level"];
    if( !sLevel.empty() ){
      int iLevel;
      std::istringstream is( sLevel );
      is >> iLevel;
      Serializable *ix = newDataItemIndex( iLevel );
      ix -> unmarshal( element, attributeList );
      return this;
    }
    else {
      Serializable *ix = newDataItemIndex();
      ix -> unmarshal( element, attributeList );
      return this;
    }
  }
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void XferDataItem::setText( const std::string &text ){
}

/*=============================================================================*/
/* member Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getIndexPosition --                                                         */
/* --------------------------------------------------------------------------- */

int  XferDataItem::getIndexPosition( XferDataItemIndex *index ){
  DataItemIndexList::iterator ii;
  int i=0;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++i, ++ii ){
    if( *ii == index ){
      return i;
    }
  }
  assert( false ); // Irgend ein Schlauer fragt hier nach einem Index der nicht zu diesem Item gehört...
  return -1;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool XferDataItem::acceptIndex( const std::string &name, int inx ){
  BUG_PARA( BugXfer, "acceptIndex", "name=" << name << ", inx=" << inx );

  DataItemIndexList::iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( !(*ii)->acceptIndex( name, inx ) ){
      return false;
    }
  }
  BUG_EXIT( "accepted" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::setIndex( const std::string &name, int inx ){
  BUG_PARA( BugXfer, "setIndex", "name=" << name << ", inx=" << inx );

  bool changed = false;
  DataItemIndexList::iterator ii;
  int i=0;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( (*ii)->setIndex( m_dataref, name, inx ) ){
      BUG_MSG( "index found and set" );
      changed = true;
    }
  }
  return changed;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void XferDataItem::setIndex( int pos, int inx ){
  BUG_PARA( BugXfer, "setIndex", "pos=" << pos << ", inx=" << inx );

  assert( pos >= 0 && pos < m_indexlist.size() );
  m_indexlist[pos]->setIndex( m_dataref, inx );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void XferDataItem::setIndex( XferDataItemIndex *index, int inx ){
  BUG_PARA( BugXfer, "setIndex", "index" << index->IndexName() << ", inx=" << inx );
  assert( index != 0 );
  index->setIndex( m_dataref, inx );
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */

int XferDataItem::getIndex( int level, int inx ){
  if( level < 0 ){
    level = m_dataref->GetNumberOfLevels() -1;
  }

  int indizes = m_dataref->GetNumberOfDimensionIndizes( level );
  if( indizes <= inx ) return 0;
  inx = indizes - inx -1;
  return m_dataref->GetDimensionIndexOfLevel( level, inx );
}

/* --------------------------------------------------------------------------- */
/* getNumberOfLevels --                                                        */
/* --------------------------------------------------------------------------- */

int XferDataItem::getNumberOfLevels(){
  return m_dataref->GetNumberOfLevels();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

int XferDataItem::getSize( int pos )const{
  if( pos >= 0 && pos < m_indexlist.size() ){
    return m_indexlist[pos]->getDimensionSize( m_dataref );
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getNumIndexes --                                                            */
/* --------------------------------------------------------------------------- */

int XferDataItem::getNumIndexes() const {
  return m_indexlist.size();
}

/* --------------------------------------------------------------------------- */
/* getNumNamedIndexes --                                                       */
/* --------------------------------------------------------------------------- */

int XferDataItem::getNumNamedIndexes() const {
  BUG( BugXfer, "getNumNamedIndexes" );
  int n=0;
  for( int i=0; i<m_indexlist.size(); ++i ){
    if( !m_indexlist[ i ] -> IndexName().empty() )
      ++n;
  }
  BUG_EXIT( "return " << n );
  return n;
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::getIndex(int i) const {
  return m_indexlist.size()>i?m_indexlist[i]:0;
}

/* --------------------------------------------------------------------------- */
/* getFirstIndexNum --                                                         */
/* --------------------------------------------------------------------------- */

int XferDataItem::getFirstIndexNum( int level ) const {
  DataItemIndexList::const_iterator ii;
  int num = 0;
  for( ii=m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( (*ii)->Level() == level ){
      return num;
    }
    num++;
  }
  return -1;
}

/* --------------------------------------------------------------------------- */
/* getMaxIndex --                                                              */
/* --------------------------------------------------------------------------- */

int XferDataItem::getMaxIndex( int pos )const{
  if( pos >= 0 && pos < m_indexlist.size() ){
    return m_indexlist[pos]->getUpperbound();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getLastIndex --                                                             */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::getLastIndex() const{
  return m_indexlist.empty() ? 0 : m_indexlist.back();
}

/* --------------------------------------------------------------------------- */
/* newDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::newDataItemIndex( int level, bool isGuiIndex ){
  int num   = 0;
  DataItemIndexList::reverse_iterator ii = m_indexlist.rbegin();
  if( ii != m_indexlist.rend() ){
    if( level == (*ii)->Level() ){
      num = (*ii)->IndexNum() + 1;
    }
  }
  //  cerr << "XferDataItem::newDataItemIndex " << getFullName( false ) << " level= " << level << " num= " << num << std::endl;
  XferDataItemIndex *index = new XferDataItemIndex( level, isGuiIndex );
  index->setIndexNum( num );
  m_indexlist.push_back( index );
  return index;
}

/* --------------------------------------------------------------------------- */
/* newDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::newDataItemIndex(){
  int level = -1;;
  int num   = -1;
  DataItemIndexList::reverse_iterator ii = m_indexlist.rbegin();
  if( ii != m_indexlist.rend() ){
    level = (*ii)->Level();
    num   = (*ii)->IndexNum();
  }

  int lvl = m_dataref->GetNumberOfLevels() -1;
  assert( lvl >= 0 );
  if( lvl == level ){
    num++;
  }
  else{
    level = lvl;
    num   = 0;
  }
  // this is not a gui index, eh?
  XferDataItemIndex *index = new XferDataItemIndex( level, false );
  index->setIndexNum( num );
  m_indexlist.push_back( index );
  return index;
}

/* --------------------------------------------------------------------------- */
/* eraseDataItemIndex --                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItem::eraseDataItemIndex(){
  DataItemIndexList::iterator indexiter;
  for( indexiter=m_indexlist.begin(); indexiter!=m_indexlist.end(); ++indexiter ){
    (*indexiter) -> setIndex( m_dataref, 0 );
    delete (*indexiter);
  }
  m_dataref -> ClearDimensionIndizes(); // FIX IT: we should clear them all!
  m_indexlist.clear();
}

/* --------------------------------------------------------------------------- */
/* eraseDataItemIndex --                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItem::eraseDataItemIndex( int level ){
  DataItemIndexList::iterator indexiter;
  while( (indexiter=std::find_if( m_indexlist.begin(), m_indexlist.end()
			     , LevelCompare( level ) ))!=m_indexlist.end() ){
    (*indexiter) -> setIndex( m_dataref, 0 );
    delete (*indexiter);
    m_indexlist.erase( indexiter );
  }
  if( m_dataref->GetNumberOfLevels() -1 == level ){
    m_dataref -> ClearDimensionIndizes(); // FIX IT: we should clear level indexes!
  }
}

/* --------------------------------------------------------------------------- */
/* eraseLastDataItemIndex --                                                   */
/* --------------------------------------------------------------------------- */

void XferDataItem::eraseLastDataItemIndex(){
  assert ( !m_indexlist.empty() );
  m_indexlist.back()->setIndex( m_dataref, 0 );
  int level = m_indexlist.back()->Level();
  delete m_indexlist.back();
  m_indexlist.pop_back();
  if( m_dataref->GetNumberOfLevels() -1 == level ){
    m_dataref -> ClearDimensionIndizes(); // FIX IT: we should clear level indexes!
  }
}

/* --------------------------------------------------------------------------- */
/* getNumDimensions --                                                         */
/* --------------------------------------------------------------------------- */

int XferDataItem::getNumDimensions()const{
  return m_dataref!=0? m_dataref->GetNumberOfDataDimensions():0;
}

/* --------------------------------------------------------------------------- */
/* lastLevelHasIndices --                                                      */
/* --------------------------------------------------------------------------- */

bool XferDataItem::lastLevelHasIndices(){
  DataItemIndexList::reverse_iterator ii = m_indexlist.rbegin();
  if( ii != m_indexlist.rend() && m_dataref != 0 ){
    if( (*ii)->Level() == ( m_dataref->GetNumberOfLevels() -1 ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

int XferDataItem::getDimensionSize(int dim)const{
//   cerr << "XferDataItem::getDimensionSize(int dim)" << std::endl;
  return getSize( dim );
}

/* --------------------------------------------------------------------------- */
/* setDataReference --                                                         */
/* --------------------------------------------------------------------------- */

void XferDataItem::setDataReference( DataReference *dataref ){
  if( m_dataref != 0 ){
    delete m_dataref;
  }
  m_dataref = dataref;
}

/* --------------------------------------------------------------------------- */
/* getDataType --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType XferDataItem::getDataType(){
  switch( m_paramtype ){
  case XferDataItem::param_Default:
    return m_dataref->getDataType();

  case XferDataItem::param_ComplexReal:
  case XferDataItem::param_ComplexImag:
  case XferDataItem::param_ComplexAbs:
  case XferDataItem::param_ComplexArg:
    return DataDictionary::type_Real;

  default:
    assert( false );
  }
  return DataDictionary::type_Bad;
}

/* --------------------------------------------------------------------------- */
/* checkDimensionIndizes --                                                    */
/* --------------------------------------------------------------------------- */

bool XferDataItem::checkDimensionIndizes(){
  return false;
}

/* --------------------------------------------------------------------------- */
/* getNumOfWildcards --                                                        */
/* --------------------------------------------------------------------------- */

int XferDataItem::getNumOfWildcards() const{
  int n = 0;
  DataItemIndexList::const_iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( (*ii)->isWildcard() ) n++;
  }
  return n;
}

/* --------------------------------------------------------------------------- */
/* getDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::getDataItemIndex( const std::string &name ){
  BUG_PARA( BugXfer, "getDataItemIndex", "name=" << name );

  DataItemIndexList::iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( (*ii)->IndexName() == name ){
      return (*ii);
    }
  }
  BUG_EXIT( "not found" );
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::getDataItemIndex( int pos ){
  if( pos >= 0 && pos < m_indexlist.size() )
    return     m_indexlist[pos];
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataItemIndexWildcard --                                                 */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::getDataItemIndexWildcard( int x ) const{
  int n = 0;
  DataItemIndexList::const_iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( (*ii)->isWildcard() ){
      if( x == ++n ){
        return (*ii);
      }
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataItemIndexIndexed --                                                  */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *XferDataItem::getDataItemIndexIndexed( int x ){
  int n = 0;
  DataItemIndexList::iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( (*ii)->isIndexed() ){
      if( x == ++n ){
        return (*ii);
      }
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setDimensionIndizes --                                                      */
/* --------------------------------------------------------------------------- */

void XferDataItem::setDimensionIndizes(){
  BUG_PARA( BugXfer, "setDimensionIndizes", "Name=" << m_dataref->fullName(true) );

  int level = 0,num = 0;
  int *indices = new int [ (int)m_indexlist.size() ];

  DataItemIndexList::iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    BUG_MSG( "Index: Level " << (*ii)->Level() << ", Number " << (*ii)->IndexNum());
    if( (*ii)->Level() != level ){
      assert( (*ii)->Level() > level );
      if( num > 0 ){
        BUG_MSG("Indizes = " << num << ", Level = " << level );
        m_dataref->SetDimensionIndizes( level, num, indices );
      }
      level = (*ii)->Level();
      num = 0;
    }
    if( num != (*ii)->IndexNum() ) {
      BUG_FATAL( "XferDataItem::setDimensionIndizes PANIC: "
		 << getFullName(true) << " Number of collected index "
		 << num << " is not equal to index number "
		 << (*ii)->IndexNum() );
      print( std::cerr );
      assert( false );
    }
    if( (*ii)->isWildcard() ){
      indices[num++] = DataReference::WildcardIndex;
    }
    else{
      indices[num++] = (*ii)->InitialIndex();
    }
  }
  if( num > 0 ){
    BUG_MSG("Indizes = " << num << ", Level = " << level );
    m_dataref->SetDimensionIndizes( level, num, indices );
  }
  BUG_EXIT( "Indices set: " << m_dataref->fullName(true) );
  delete [] indices;
}

/* --------------------------------------------------------------------------- */
/* setDimensionIndizes --                                                      */
/* --------------------------------------------------------------------------- */

void XferDataItem::setDimensionIndizes( int level ){
  BUG_PARA( BugXfer, "XferDataItem::setDimensionIndizes", "level= " << level );

  if( level < 0 ){
    setDimensionIndizes();
    return;
  }
  int num = 0;
  int *indices = new int [ (int)m_indexlist.size() ];
  DataItemIndexList::iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    BUG_DEBUG("Index: Level " << (*ii)->Level() << ", Number " << (*ii)->IndexNum());
    if( (*ii)->Level() == level ){
      if( (*ii)->isWildcard() ){
	indices[num++] = DataReference::WildcardIndex;
      }
      else{
	indices[num++] = (*ii)->InitialIndex();
      }
    }
  }
  if( num > 0 ){
    BUG_DEBUG("Indizes = " << num << ", Level = " << level );
    m_dataref->SetDimensionIndizes( level, num, indices );
  }
  delete [] indices;
}

/* --------------------------------------------------------------------------- */
/* setDataRefIndices --                                                        */
/* --------------------------------------------------------------------------- */

void XferDataItem::setDataRefIndices() const{
  setDataRefIndices( *m_dataref );
}

/* --------------------------------------------------------------------------- */
/* setDataRefIndices --                                                        */
/* --------------------------------------------------------------------------- */

void XferDataItem::setDataRefIndices( DataReference &ref ) const{
  BUG( BugXfer, "setDataRefIndices" );

  int level = 0,num = 0;
  int *indices = new int [ (int)m_indexlist.size() ];

  DataItemIndexList::const_iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    BUG_MSG( "Level " << (*ii)->Level() << ", Number " << (*ii)->IndexNum() );

    if( (*ii)->Level() != level ){
      assert( (*ii)->Level() > level );
      if( num > 0 ){
        BUG_MSG( "Indizes = " << num << ", Level = " << level );
        ref.SetDimensionIndizes( level, num, indices );
      }
      level = (*ii)->Level();
      num = 0;
    }

    if( num != (*ii)->IndexNum() ) {
      BUG_FATAL( "PANIC: " << getFullName(true) <<
		 ": Number of collected index " << num <<
		 " is not equal to index number " << (*ii)->IndexNum() );
      print( std::cerr );
      assert( false );
    }

    if( (*ii)->isIndexed() || (*ii)->isWildcard() ){
      indices[num++] = DataReference::WildcardIndex;
    }
    else{
      indices[num++] = (*ii)->InitialIndex();
    }
  }

  if( num > 0 ){
    BUG_MSG( "Indizes = " << num << ", Level = " << level );
    ref.SetDimensionIndizes( level, num, indices );
  }
  delete [] indices;
}

/* --------------------------------------------------------------------------- */
/* getFullName --                                                              */
/* --------------------------------------------------------------------------- */

const std::string XferDataItem::getFullName( const DataItemIndexList& indexlist ) const {
  DataItemIndexList::const_iterator iter;
  if( m_dataref == 0 )
    return "";
  std::string name = m_dataref->fullName(true);
  for( iter=indexlist.begin();iter!=indexlist.end();++iter ){
    if( (*iter)->IndexName().empty() )
      continue;
    std::string::size_type idx = 0;
    for( int i = 0; i < (*iter)->Level(); ++i ){
      idx=i>0?++idx:idx;
      idx=name.find( '.',idx );
    }
    idx=name.find( '[',++idx );
    for( int i = 0; i < (*iter)->IndexNum(); ++i ){
      idx=name.find( ',',++idx );
    }
    std::string::size_type idx2 = 0;
    idx2=name.find_first_of( ",]",++idx );
    name.replace(idx,idx2-idx,(*iter)->IndexOrFgName() );
  }
  return name;
}

/* --------------------------------------------------------------------------- */
/* Data --                                                                     */
/* --------------------------------------------------------------------------- */

DataReference *XferDataItem::Data(){
  return m_dataref;
}

/* --------------------------------------------------------------------------- */
/* refreshDataItemIndexedList --                                                 */
/* --------------------------------------------------------------------------- */

void XferDataItem::refreshDataItemIndexedList(){
  DataItemIndexList::iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if (GuiElement::findElement((*ii)->IndexName()) && (*ii)->isIndexed()) {
      GuiElement* elem = GuiElement::findElement((*ii)->IndexName());
      if (elem) setIndex((*ii)->IndexName(), elem->getGuiIndex()->getIndex());
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getUserAttr --                                                              */
/* --------------------------------------------------------------------------- */

UserAttr *XferDataItem::getUserAttr(){
  return static_cast<UserAttr*>( m_dataref->getUserAttr() );
}


/* --------------------------------------------------------------------------- */
/* userAttr --                                                                 */
/* --------------------------------------------------------------------------- */

UserAttr &XferDataItem::userAttr(){
  return static_cast<UserAttr&>( m_dataref->userAttr() );
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

size_t XferDataItem::getSize()const{
  return m_dataref->GetDataDimensionSize();
}

/* --------------------------------------------------------------------------- */
/* getLastSize --                                                              */
/* --------------------------------------------------------------------------- */

size_t XferDataItem::getLastSize()const{
  DataContainer::SizeList dimslist;
  int ndims = m_dataref->GetAllDataDimensionSize ( dimslist );
  return dimslist.back();
}

/* --------------------------------------------------------------------------- */
/* StreamTargetsAreCleared --                                                  */
/* --------------------------------------------------------------------------- */

bool XferDataItem::StreamTargetsAreCleared(std::string& error_msg){
  return !DataPoolIntens::Instance().checkTargetStreams( m_dataref, error_msg );
}

/* --------------------------------------------------------------------------- */
/* clearTargetStreams --                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItem::clearTargetStreams(){
  DataPoolIntens::Instance().clearTargetStreams( m_dataref );
}

/* --------------------------------------------------------------------------- */
/* isUpdated --                                                                */
/* --------------------------------------------------------------------------- */

bool XferDataItem::isUpdated( TransactionNumber num, bool gui ){
  if( gui ){ // GUI-update
    // Bei einem Gui-update müssen Werte, Attribute und Rollbacks geprüft werden!!
    return m_dataref->isDataContainerUpdated_PreInx( DataReference::GuiUpdated, num );
  }

  return m_dataref->isDataContainerUpdated_PreInx( DataReference::ValueUpdated, num );
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool XferDataItem::isDataItemUpdated( TransactionNumber num, bool gui ){
  BUG_PARA( BugXfer, "isDataItemUpdated"
	    , "TransactionNumber=" << num << ", gui=" << BugBool(gui) );

  // Das GUI muss auch nach einem TTrail-Rollback wieder auf den aktuellen
  // Stand gebracht werden. Ein update() während einer TTrail-Transaction
  // hat seine Tücken, da nach einem Rollback auch alle Timestamps wieder
  // die Alten sind.
  if( gui ){
    if( m_dataref->getDataItemLastRollback() >= num ){
      BUG_EXIT( "update after last rollback" );
      return true;
    }
  }

  // Bei einem GUI-Update interessieren uns auch geänderte Attribute!
  DataReference::UpdateStatus update_status = gui
    ? DataReference::GuiUpdated
    : DataReference::ValueUpdated;

  // Zuerst suchen wir die erste Wildcard um den ersten Level mit variablen
  // Indizes zu ermitteln. Sind keine Wildcards vorhanden, nehmen wir den
  // aktuellen Level.
  XferDataItemIndex *index = getDataItemIndexWildcard( 1 );
  int current_level = m_dataref->GetNumberOfLevels() -1;
  int level = 0;
  if( index == 0 ){
    level = current_level;
  }
  else{
    level = index->Level();
  }
  assert( level >= 0 && level <= current_level );

  bool rslt = false;

  if( level == current_level ){
    // Die Kontrolle findet auf dem aktuellen Level statt. Eine einfache Sache.

#ifdef HAVE_LOG4CPLUS
    if(__logger__.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL)){
      if( m_dataref->isDataItemUpdated( update_status, num ) ){
        BUG_DEBUG(  m_dataref->fullName( true )
                    << " is updated" );
      }
      else
        BUG_DEBUG(  m_dataref->fullName( true )
                    << " is NOT updated" );
    }
#endif

    rslt = m_dataref->isDataItemUpdated( update_status, num );
    BUG_EXIT( m_dataref->fullName(true) << ": updated (current level) = " << BugBool(rslt) );
    return rslt;
  }

  DataReference::Ptr ref = DataPool::newDataReference( *m_dataref );
  while( current_level > level ){
    ref->ConvertToParentReference();
    current_level--;
  }

#ifdef HAVE_LOG4CPLUS
  if(__logger__.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL)){
    if( ref->isDataItemUpdated( update_status, num ) ){
      BUG_DEBUG( ref->fullName( true ) << " is updated" );
    }
    else {
      BUG_DEBUG( ref->fullName( true ) << " is NOT updated" );
    }
  }
#endif

  rslt = ref->isDataItemUpdated( update_status, num );
  BUG_EXIT( ref->fullName(true) << ": updated (level=" << level << ") = " << BugBool(rslt) );
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::getValue( int &i )const{
  switch( m_paramtype ){
  case param_Default:
    return m_dataref->GetValue_PreInx( i );
  case param_ComplexReal:
  case param_ComplexImag:
  case param_ComplexAbs:
  case param_ComplexArg:
    return false;
  default:
    assert( false );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */

bool XferDataItem::getValues( DataVector &i )const{
  switch( m_paramtype ){
  case param_Default:
    return m_dataref->getDataVector( i );
  case param_ComplexReal:
  case param_ComplexImag:
  case param_ComplexAbs:
  case param_ComplexArg:
    return false;
  default:
    assert( false );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getDataSetInputValue --                                                     */
/* --------------------------------------------------------------------------- */
// if data item subvarname has a DataSet:
// - set input to input string that belongs to value
// - return true

bool XferDataItem::getDataSetInputValue(std::string &input, const std::string &value) {
  if(getUserAttr()->DataSetName().empty()) {
    // kein DataSet
    return false;
  }

  DataSet* ds = DataPoolIntens::Instance().getDataSet(getUserAttr()->DataSetName());
  int opos = ds->getOutputPosition(value);
  if(ds->getInputValue(input, opos)) {
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::setValue( int i ){
  if( isReadOnly() ) assert( false );
  if( !m_dataref->SetValue_PreInx( i ) ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool XferDataItem::isValid( )const{
  BUG_DEBUG("XferDataItem::isValid()");
  return m_dataref->GetItemValid_PreInx( );
}

/* --------------------------------------------------------------------------- */
/* GetAllDataDimensionSize --                                                  */
/* --------------------------------------------------------------------------- */

int  XferDataItem::getAllDataDimensionSize( DataContainer::SizeList &dimsize ) const{
  return m_dataref->GetAllDataDimensionSize( dimsize );
}

/* --------------------------------------------------------------------------- */
/* setInvalid --                                                               */
/* --------------------------------------------------------------------------- */

bool XferDataItem::setInvalid( ){
  BUG_DEBUG("XferDataItem::setInvalid()");
  return m_dataref->SetItemInvalid_PreInx( );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::getValue( double &d )const{
    BUG_DEBUG("XferDataItem::getValue(double &d)");
  switch( m_paramtype ){
  case param_Default:
    return m_dataref->GetValue_PreInx( d );

  case param_ComplexReal:{
      dComplex c;
      if( !m_dataref->GetValue_PreInx( c ) ){
        return false;
      }
      d = c.real();
      return true;
    }

  case param_ComplexAbs:{
      dComplex c;
      if( !m_dataref->GetValue_PreInx( c ) ){
        return false;
      }
      if( !NumLim::isFinite( c.real() ) || !NumLim::isFinite( c.imag() ) ){
	d=NumLim::infinity();
	//	d = numeric_limits<double>::infinity();
      }
      else
	d = std::abs(c);
      return true;
    }

  case param_ComplexImag:{
      dComplex c;
      if( !m_dataref->GetValue_PreInx( c ) ){
        return false;
      }
      d = c.imag();
      return true;
    }

  case param_ComplexArg:{
      dComplex c;
      if( !m_dataref->GetValue_PreInx( c ) ){
        return false;
      }
      if( NumLim::isFinite( c.real() ) ){
	if( NumLim::isFinite( c.imag() ) ){
	  d=std::arg(c);
	  return true;
	}
	d = c.imag()>0? std::numbers::pi/2 : -std::numbers::pi/2;
	return true;
      }
      d = c.real()>0 ? std::numbers::pi/2 : -std::numbers::pi;
      return true;
    }

  default:
    assert( false );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::setValue( double d ){
  BUG_DEBUG( "XferDataItem::setValue( double ) double( " << d << " )");

  switch( m_paramtype ){
  case param_Default:
    if( !m_dataref->SetValue_PreInx( d ) ){
      return false;
    }
    break;
  case param_ComplexAbs:
    setComplexAbsValue( d );
    break;
  case param_ComplexArg:
    setComplexArgValue( d );
    break;
  case param_ComplexReal:
    setComplexRealValue( d );
    break;
  case param_ComplexImag:
    setComplexImagValue( d );
    break;
  default:
    assert( false );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setValues --                                                                */
/* --------------------------------------------------------------------------- */

bool XferDataItem::setValues( const DataVector & d ){
  BUG_DEBUG( "XferDataItem::setValue( std::vector<double> )");

  switch( m_paramtype ){
  case param_Default:
    if( !m_dataref->setDataVector( d ) ){
      return false;
    }
    break;
  case param_ComplexAbs:
  case param_ComplexArg:
  case param_ComplexReal:
  case param_ComplexImag:
  default:
    assert( false );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::getValue( std::string &s )const{
    BUG_DEBUG(  "XferDataItem::getValue( std::string & ) "
		   << m_paramtype );
  switch( m_paramtype ){
  case param_Default:
    return m_dataref->GetValue_PreInx( s );
  case param_ComplexAbs:
  case param_ComplexArg:
  case param_ComplexReal:
  case param_ComplexImag:
    return false;
  default:
    assert( false );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::setValue( const std::string &s ){
  BUG_DEBUG("XferDataItem::setValue string( " << s << " )");
  if( isReadOnly() ) assert( false );
  if( !m_dataref->SetValue_PreInx( s ) ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::getValue( dComplex &c )const{
    BUG_DEBUG("XferDataItem::getValue( dComplex )" << m_paramtype );
  switch( m_paramtype ){
  case param_Default:
    return m_dataref->GetValue_PreInx( c );
  case param_ComplexAbs:
  case param_ComplexArg:
  case param_ComplexReal:
  case param_ComplexImag:
    return false;
  default:
    assert( false );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::setValue( const dComplex &c ){
  if( !m_dataref->SetValue_PreInx( c ) ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setComplexAbsValue --                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItem::setComplexAbsValue( double d ){
  dComplex c;
  if( m_dataref->GetValue_PreInx( c ) ){
    double f = d / std::abs( c );
    c = dComplex( c.real() * f, c.imag() * f );
  }
  else{
    c = dComplex( d, 0.0 );
  }
  setValue( c );
}

/* --------------------------------------------------------------------------- */
/* setComplexArgValue --                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItem::setComplexArgValue( double d ){
  dComplex c;
  if( !m_dataref->GetValue_PreInx( c ) ){
    c = dComplex( 1.0, 1.0 );
  }
  double phi = std::arg( c );
  double delta = d - phi;
  double real = c.real() * cos( delta ) - c.imag() * sin( delta );
  double imag = c.real() * sin( delta ) + c.imag() * cos( delta );
  setValue( dComplex( real, imag ) );
}

/* --------------------------------------------------------------------------- */
/* setComplexRealValue --                                                      */
/* --------------------------------------------------------------------------- */

void XferDataItem::setComplexRealValue( double d ){
  dComplex c;
  if( m_dataref->GetValue_PreInx( c ) ){
    c = dComplex( d, c.imag() );
  }
  else{
    c = dComplex( d, 0.0 );
  }
  setValue( c );
}

/* --------------------------------------------------------------------------- */
/* setComplexImagValue --                                                      */
/* --------------------------------------------------------------------------- */

void XferDataItem::setComplexImagValue( double d ){
  dComplex c;
  if( m_dataref->GetValue_PreInx( c ) ){
    c = dComplex( c.real(), d );
  }
  else{
    c = dComplex( 0.0, d );
  }
  setValue( c );
}

/* --------------------------------------------------------------------------- */
/* setDbItemTimestamp --                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItem::setDbItemTimestamp( TransactionNumber trans, int level ){
  BUG_PARA( BugDBTimestamp, "setDbItemTimestamp", getFullName(true) );

  if( m_dataref == 0 ){
    return;
  }

  SAY("XferDataItem::setDbItemTimestamp(" << trans << ", " << level << ")");
  SAY(" -- Ref: " << m_dataref->fullName(true));

  ItemAttr *itemattr = 0;
  int current_level = m_dataref->GetNumberOfLevels() -1;

  if( level < 0 || level == current_level ){
    SAY("  current level");
    BUG_MSG( "set " << trans << " in " << m_dataref->fullName( true ) );
    DataReference::Ptr ref = DataPool::newDataReference( *m_dataref );
    setDataRefIndices( *ref );
    DataAlterSetDbModified modify( trans, false );
    ref->alterData( modify );
  }
  else{
    SAY(" -- current level=" <<current_level << ", level=" << level);
    DataReference::Ptr ref = DataPool::newDataReference( *m_dataref );

    while( current_level > level ){
      ref->ConvertToParentReference();
      current_level--;
      BUG_MSG( "convert to parent" );
    }
    SAY(" -- set " << ref->fullName(true));

    BUG_MSG( "set " << trans << " on level " << level << " in " << m_dataref->fullName( true ) );
    getItemAttrForUpdates( *ref ).setDataBaseTimestamp( trans );
  }
  BUG_EXIT( "DbTimestamp " << trans << " set" );
}

/* --------------------------------------------------------------------------- */
/* isDbItemUpdated --                                                          */
/* --------------------------------------------------------------------------- */

bool XferDataItem::isDbItemUpdated(int level) const{
  BUG( BugDBTimestamp, "isDbItemUpdated" );

  if( m_dataref == 0 ){
    BUG_EXIT( "no DataReference => false" );
    return false;
  }
  BUG_MSG( "DataItem: " << m_dataref->fullName( true )  );

  TransactionNumber ts = getTransactionNumber(level);
  if( ts == 0 ){
    BUG_EXIT( "no DataBaseTimestamp available = > false" );
    // Wenn das Item nicht als DbItem markiert ist, kann es auch nicht
    // verändert worden sein.
    return false;
  }

  bool updated = false;
  TransactionNumber ts_item=0;
  int current_level=m_dataref->GetNumberOfLevels() -1;
  if( (level < 0) || (level==current_level) ){
    updated = m_dataref->isDataItemUpdated( DataReference::ValueUpdated, ts );
    ts_item = m_dataref->getDataItemUpdated( DataReference::ValueUpdated );
  }
  else {
    DataReference::Ptr ref = DataPool::newDataReference( *m_dataref );
    while( current_level > level ){
      ref->ConvertToParentReference();
      current_level--;
    }
    updated = ref->isDataItemUpdated( DataReference::ValueUpdated, ts );
    ts_item = ref->getDataItemUpdated( DataReference::ValueUpdated );
  }

  BUG_MSG( "Timestamp of item: " << ts_item << ", of Loadtime:" << ts );
  BUG_EXIT( "return " << BugBool( updated ) );
  return updated;
}

/* --------------------------------------------------------------------------- */
/* getTransactionNumber --                                                     */
/* --------------------------------------------------------------------------- */

TransactionNumber XferDataItem::getTransactionNumber(int level) const {
  if( m_dataref == 0 ){
    return 1;
  }

  SAY("XferDataItem::getTransactionNumber(" << level << ")");

  const ItemAttr *attr = 0;
  int current_level = m_dataref->GetNumberOfLevels() -1;
  if( level < 0 || level == current_level ){
    SAY(" -- current level " << m_dataref->fullName(true));
    attr = getItemAttr( *m_dataref );
  }
  else{
    DataReference::Ptr ref = DataPool::newDataReference( *m_dataref );
    while( current_level > level ){
      ref->ConvertToParentReference();
      SAY(" -- covert to " << ref->fullName(true));
      current_level--;
    }
    attr = getItemAttr( *ref );
  }
  if( attr ){
    return attr->DataBaseTimestamp();
  }
  SAY(" -- no itemattr");
  return 2;
}

/* --------------------------------------------------------------------------- */
/* isDbValueUpdated --                                                         */
/* --------------------------------------------------------------------------- */

bool XferDataItem::isDbValueUpdated() const{
  BUG( BugDBTimestamp, "isDbValueUpdated" );

  if( m_dataref == 0 ){
    BUG_EXIT( "no DataReference => false" );
    return false;
  }
  BUG_MSG( "DataItem: " << m_dataref->fullName( true )  );

  TransactionNumber ts = getTransactionNumber();
  if( ts == 0 ){
    BUG_EXIT( "no DataBaseTimestamp available => false" );
    // Wenn das Item nicht als DbItem markiert ist, kann es auch nicht
    // verändert worden sein.
     return false;
  }

  bool updated = m_dataref->isDataContainerUpdated_PreInx( DataReference::ValueUpdated, ts );
  SAY("XferDataItem::isDbValueUpdated == " << updated);
  if( updated ){
    BUG_EXIT( "Value is updated since " << ts );
  }
  else{
    BUG_EXIT( "Value is NOT updated since " << ts );
  }
  return updated;
}

/* --------------------------------------------------------------------------- */
/* getLabel --                                                                 */
/* --------------------------------------------------------------------------- */

const std::string &XferDataItem::getLabel(){
  return userAttr().Label( m_dataref );
}

/* --------------------------------------------------------------------------- */
/* getUnit  --                                                                 */
/* --------------------------------------------------------------------------- */

const std::string XferDataItem::getUnit(){
  return userAttr().Unit();
}

/* --------------------------------------------------------------------------- */
/* isAncestorOf --                                                             */
/* --------------------------------------------------------------------------- */

bool XferDataItem::isAncestorOf( const XferDataItem *xfer ){
  BUG(BugGui, "XferDataItem::isAncestorOf");

  bool ret = false;
  XferDataItem *copy_xfer = new XferDataItem( *xfer );

  if( getNumberOfLevels() > copy_xfer->getNumberOfLevels() ){
    delete copy_xfer;
    BUG_EXIT("conversion to parent not possible");
    return false;
  }

  while( getNumberOfLevels()  < copy_xfer->getNumberOfLevels() ){
    DataReference * dref = copy_xfer->Data();
    if ( !dref->ConvertToParentReference()) {
      delete copy_xfer;
      BUG_EXIT("conversion to parent reference failed");
      return false;
    }
  }

  // sorry beat, mein check geht nur ueber dem FullName
  // das kannst du sicher besser
  if ( !getNumOfWildcards()  && !copy_xfer->getNumOfWildcards() ){
    if (getFullName(true) == copy_xfer->getFullName(true) ){
      ret = true;
    }
  }
  else{
    if (getFullName(false) == copy_xfer->getFullName(false) ){
      ret = true;
    }
  }
  BUG_EXIT("Return '"<<ret<<"' own varname ["<<getFullName(true)<<"] xfer["<<copy_xfer->getFullName(true)<<"]");
  delete copy_xfer;
  return ret;
}

/* --------------------------------------------------------------------------- */
/* isAncestorDbTransient --                                                    */
/* --------------------------------------------------------------------------- */

bool XferDataItem::isAncestorDbTransient() {
  BUG(BugGui, "XferDataItem::isAncestorDbTransient");

  if( getNumberOfLevels() == 0 ){
    return false;
  }

  XferDataItem *copy_xfer = new XferDataItem( *this ); // copy

  while( copy_xfer->getNumberOfLevels() > 1 ){
    DataReference * dref = copy_xfer->Data();
    if ( !dref->ConvertToParentReference() ){
      delete copy_xfer;
      BUG_MSG("conversion to parent reference failed");
      return false;
    }

    // check db transient
    if (copy_xfer->userAttr().IsDbTransient()) {
      delete copy_xfer;
      BUG_EXIT("ancestor item '"<<copy_xfer->getFullName(true)<<"' is db transient.");
      return true;
    }
  }

  BUG_EXIT("varname ["<<getFullName(true)<<"] xfer["<<copy_xfer->getFullName(true)<<"]");
  delete copy_xfer;
  return false;
}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void XferDataItem::clearRange(){
  BUG_DEBUG("XferDataItem::clearRange");

  if( m_indexlist.empty() ){
    // Wenn keine Indexliste vorhanden ist, wird das von der DataReference
    // referenzierte Item gelöscht.
    m_dataref->eraseAllElements();
    return;
  }

  int minlevel = INT_MAX;
  DataItemIndexList::iterator ii;
  for( ii = m_indexlist.begin(); ii != m_indexlist.end(); ++ii ){
    if( (*ii)->isWildcard() ){
      int l=(*ii)->Level();
      if( l < minlevel ){
	minlevel = l;
      }
    }
  }

  if( minlevel == INT_MAX ){
    // Es gibt keine Wildcard-Indizes.
    m_dataref->eraseAllElements();
  }
  else {
    clearRange( minlevel );
  }
  setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void XferDataItem::clearRange( int level, bool incl_itemattr ){
  if( level < 0 ){
    return;
  }

  DataReference::Ptr ref = DataPool::newDataReference( *m_dataref );
  while( ref->GetNumberOfLevels() > level+1 ){
    ref->ConvertToParentReference();
  }
  ref->eraseAllElements(incl_itemattr);
}

/* --------------------------------------------------------------------------- */
/* clearRangeMaxLevel --                                                       */
/* --------------------------------------------------------------------------- */

void XferDataItem::clearRangeMaxLevel( bool incl_itemattr ){
  BUG_PARA( BugXfer, "clearRangeMaxLevel", "incl_itemattr=" << BugBool(incl_itemattr) );

  setDataRefIndices();
  m_dataref->clearDataItem( incl_itemattr );

  setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* setDbItemsNotModified --                                                    */
/* --------------------------------------------------------------------------- */

void XferDataItem::setDbItemsNotModified(){
  DataAlterSetDbModified modify;
  m_dataref->alterData( modify );
}

/* --------------------------------------------------------------------------- */
/* doEditData --                                                               */
/* --------------------------------------------------------------------------- */

void XferDataItem::doEditData( XferEditDataItemFunctor &func
                             , const std::string &rowname
			     , const std::string &colname
			     , int inx )
{
  BUG_DEBUG("XferDataItem::doEditData Index = " << inx);

  XferDataItemIndex *rowindex = 0;
  if( !rowname.empty() ){
    rowindex = getDataItemIndex( rowname );
  }
  XferDataItemIndex *colindex = getDataItemIndex( colname );
  if( colindex == 0 ){
    return;
  }

  // Wir kontrollieren hier ganz genau. Es soll sich hier kein Fehler
  // mit unerklärlichem Phänomen einschleichen.
  int wc = getNumOfWildcards();
  if( rowindex == 0 ){
    assert( wc == 1 );
  }
  else{
    assert( wc == 2 );
  }

  if( rowindex == 0 ){ // ein Vektor
    func( *colindex, inx, *m_dataref );
    return;
  }

  // eine Matrix
  int rowsize = 0;
  DataReference::Ptr ref = DataPool::newDataReference( *m_dataref );
  int level    = 0;
  int indexnum = 0;

  // Wir ermitteln die Grösse der Row. Dies ist unter Umständen etwas
  // aufwendig, da wir jede erdenkliche Situation unterstützen wollen.
  if( ( rowindex->Level() * 1000 + rowindex->IndexNum() ) <
      ( colindex->Level() * 1000 + colindex->IndexNum() ) ){
    rowsize = rowindex->getDimensionSize( m_dataref );
  }
  else{
    int colsize = colindex->getDimensionSize( m_dataref );
    if( colindex->Upperbound() > 0 && colindex->Upperbound() < colsize ){
      colsize = colindex->Upperbound() + 1;
    }

    level     = colindex->Level();
    indexnum  = colindex->IndexNum();
    int rowsz = 0;

    for( int i = colindex->Lowerbound(); i < colsize; i++ ){
      ref->SetDimensionIndexOfLevel( i, level, indexnum );
      rowsz = rowindex->getDimensionSize( ref.get_ptr() );
      rowsize = rowsz > rowsize ? rowsz : rowsize;
    }
  }

  if( rowindex->Upperbound() > 0 && rowindex->Upperbound() < rowsize ){
    rowsize = rowindex->Upperbound() + 1;
  }

  // Nun ist alles klar. Wir rufen für jede mögliche row den Functor auf.
  level    = rowindex->Level();
  indexnum = rowindex->IndexNum();

  for( int i = rowindex->Lowerbound(); i < rowsize; i++ ){
    ref->SetDimensionIndexOfLevel( i, level, indexnum );
    func( *colindex, inx, *ref );
  }
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool XferDataItem::writeXML( std::ostream &os, std::vector<std::string> &attrs
	                   , int level
			   , bool debug ){
  if( m_dataref ){
    setDimensionIndizes(level);
    return m_dataref->writeXML( os, attrs, level, debug );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* writeJSON --                                                                */
/* --------------------------------------------------------------------------- */

bool XferDataItem::writeJSON( std::ostream &os,
                              int level,
                              int indent,
                              const SerializableMask flags )
{
  BUG_PARA( BugXfer, "writeJSON", "level=" << level );

  if( m_dataref ){
    setDimensionIndizes(level);
    return m_dataref->writeJSON( os, level, indent, flags );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

std::ostream & XferDataItem::print( std::ostream &o )const{
  if( !DataPoolIntens::Instance().getDataPool().DataPoolAllocated()){
    return o;
  }
  DataItemIndexList::const_iterator I;
  for( I = m_indexlist.begin(); I != m_indexlist.end(); ++I ){
    //    o << " Level: " << (*I)->Level() << " Index: " << (*I)->IndexNum() << std::endl;
    (*I)->print( o ); o << std::endl;
  }
  if( m_dataref != 0 )
    m_dataref -> PrintItem( true, o );
  return o;
}

/* --------------------------------------------------------------------------- */
/* operator << --                                                              */
/* --------------------------------------------------------------------------- */

std::ostream &operator<<( std::ostream &o, const XferDataItem &x ){
  return x.print(o);
}

/* --------------------------------------------------------------------------- */
/* getDict                                                                     */
/* --------------------------------------------------------------------------- */

DataDictionary *XferDataItem::getDict(){
  if( m_dataref == 0 )
    return 0;
  return m_dataref->GetDict();
}

/* --------------------------------------------------------------------------- */
/* getItemAttr --                                                              */
/* --------------------------------------------------------------------------- */

const ItemAttr *XferDataItem::getItemAttr( DataReference &ref ) const{
  return static_cast<const ItemAttr *>( ref.getDataItemAttr() );
}

/* --------------------------------------------------------------------------- */
/* getItemAttrForUpdates --                                                    */
/* --------------------------------------------------------------------------- */

ItemAttr &XferDataItem::getItemAttrForUpdates( DataReference &ref ){
  return static_cast<ItemAttr &>( ref.getDataItemAttrForUpdates() );
}

bool XferDataItem::alterData( DataAlterFunctor &f ){
  m_dataref->alterData( f );
  return true;
}
