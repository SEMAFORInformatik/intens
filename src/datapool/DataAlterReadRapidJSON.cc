
#include "datapool/DataElement.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataRealElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataItem.h"
#include "datapool/DataDimension.h"
#include "datapool/DataAlterReadRapidJSON.h"
#include "datapool/DataReference.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterReadRapidJSON::DataAlterReadRapidJSON()
  : DataAlterSetElement( true )
  , m_status( NoUpdate ){
}

DataAlterReadRapidJSON::~DataAlterReadRapidJSON(){
  BUG_DEBUG("delete DataAlterReadRapidJSON: remaining items: " << m_stack.size());

  while( m_stack.size() > 0 ){
    ItemInfo *info = m_stack.top();
    info->setStatus( m_status );
    info->markItemUpdated();
    delete info;
    m_stack.pop();
  }

  while( m_item_stack.size() > 0 ){
    DataItem *item = m_item_stack.top();
    item->markDataItemUpdated( m_status );
    m_item_stack.pop();
  }
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::check( DataReference &ref ){
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::alterItem( DataItem &item,
                                        UpdateStatus &status,
                                        DataInx &inx,
                                        bool final )
{
  status = NoUpdate;

  if( final ){
    BUG_ERROR("indexed range does not exist." );
    return false; // abbrechen
  }

  if( !inx.isLastLevel() ){
    m_item_stack.push( &item );
    return true;
  }

  BUG_DEBUG("Begin of DataAlterReadRapidJSON::alterItem: last level");
  DataDictionary &dict = item.dictionary();

  // Die letzte Indexliste wird kopiert
  int dims = inx.getDimensions();
  DataIndexList *indexlist = inx.cloneIndexList();

  // Alle führenden Nuller werden eliminiert.
  int inx_zero = 0;
  for( int index = 0; index < dims; index++ ){
    int inx = indexlist->index( index ).getIndex();
    if( inx == 0 ){
      inx_zero++;
    }
    else{
      dims = 0; // end
    }
  }
  for( int index = 0; index < inx_zero; index++ ){
    indexlist->deleteFirstIndex();
  }
  BUG_DEBUG(" --> push Object on stack");
  m_stack.push( new ItemInfoExt( item,
                                 indexlist,
                                 dict,
                                 1, // Level
                                 ItemInfo::type_Object ) ); // Base Object

  BUG_DEBUG("End of DataAlterReadRapidJSON::alterItem: " << item.getName());
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterReadRapidJSON::UpdateStatus DataAlterReadRapidJSON::alterData( DataElement &el ){
  assert( false ); // unused
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* setString --                                                                */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setString( std::string &value ){
  assert( m_stack.size() );
  m_stack.top()->setString( value );
}

/* --------------------------------------------------------------------------- */
/* setInteger --                                                               */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setInteger( int value ){
  assert( m_stack.size() );
  m_stack.top()->setInteger( value );
}

/* --------------------------------------------------------------------------- */
/* setUInteger --                                                              */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setUInteger( unsigned int value ){
  assert( m_stack.size() );
  m_stack.top()->setUInteger( value );
}

/* --------------------------------------------------------------------------- */
/* setDouble --                                                                */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setDouble( double value ){
  assert( m_stack.size() );
  m_stack.top()->setDouble( value );
}

/* --------------------------------------------------------------------------- */
/* setInvalid --                                                               */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setInvalid(){
  assert( m_stack.size() );
  m_stack.top()->setInvalid();
}

/* --------------------------------------------------------------------------- */
/* setStartObject --                                                           */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::setStartObject( int level ){
  BUG_DEBUG("Begin of DataAlterReadRapidJSON::setStartObject: level " << level);
  assert( m_stack.size() );
  ItemInfo *info = m_stack.top();
  if( info->doOverride( level + 1 ) ){
    BUG_DEBUG("End of DataAlterReadRapidJSON::setStartObject: override already set");
    return true;
  }
  ItemInfo *clone = info->clone( level, ItemInfo::type_Object );
  if( !info->isStructItem() ){
    clone->setOverride();
    BUG_DEBUG("End of DataAlterReadRapidJSON::setStartObject: not a structure => override");
    return true;
  }
  m_stack.push( clone );
  BUG_DEBUG("End of DataAlterReadRapidJSON::setStartObject");
  return false;
}

/* --------------------------------------------------------------------------- */
/* setKey --                                                                   */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::setKey( std::string &key, int level, bool tryDbAttrAsKey ){
  BUG_DEBUG("Begin of DataAlterReadRapidJSON::setKey: '" << key << "', level " << level << "', tryDbAttrAsKey " << tryDbAttrAsKey);
  assert( m_stack.size() );
  ItemInfo *info = m_stack.top();

  if( info->doOverride( level ) ){
    BUG_DEBUG("End of DataAlterReadRapidJSON::setKey: override already set");
    return true; // override
  }

  if( info->isInfoType( ItemInfo::type_Key ) ){
    BUG_DEBUG(" --> the previous key of the same Object");
    UpdateStatus status = info->markItemUpdated();
    delete info;
    m_stack.pop();
    info = m_stack.top();
    info->setStatus( status );
    setStatus( status );
  }

  assert( info->isInfoType( ItemInfo::type_Object ) );

  DataElement &el = info->dataElement();
  const DataStructElement &str_el = el.getStructElement();

  DataItem *item(0);
  if(tryDbAttrAsKey) {
    item = str_el.getItemByDbAttr( key );
  }
  if(!item) {
    item = str_el.getItem( key );
    if(item && tryDbAttrAsKey && !item->getDbAttr().empty() && item->getDbAttr() != key){
      BUG_DEBUG("DataAlterReadRapidJSON::setKey: key " << key
                << " ignored, item has dbattr " << item->getDbAttr() << "!");
      item = 0;
    }
  }

  if( !item ){
    BUG_DEBUG("End of DataAlterReadRapidJSON::setKey: '" << key << "' doesnt exist");
    m_stack.push( new ItemInfo( level, key ) );
    return true; // ignore all values of this item
  }

  m_stack.push( new ItemInfoExt( *item,
                                 new DataIndexList(),
                                 item->dictionary(),
                                 level,
                                 ItemInfo::type_Key ) );
  BUG_DEBUG("End of DataAlterReadRapidJSON::setKey: Dataitem '" << key << "' is ok");
  return false; // ok. item available
}

/* --------------------------------------------------------------------------- */
/* setEndKey --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::setEndKey( int level ){
  BUG_DEBUG("Begin of DataAlterReadRapidJSON::setEndKey of level " << level);
  assert( m_stack.size() );
  ItemInfo *info = m_stack.top();

  if( info->doOverride( level ) ){
    BUG_DEBUG("End of DataAlterReadRapidJSON::setEndKey: override already set");
    return true; // ignore
  }

  if( info->isInfoType( ItemInfo::type_Key ) ){
    UpdateStatus status = info->markItemUpdated();
    delete info;
    m_stack.pop();
    info = m_stack.top();
    info->setStatus( status );
    setStatus( status );
    BUG_DEBUG("End of DataAlterReadRapidJSON::setEndKey: previous key removed from stack");
    return false; // ok
  }

  BUG_DEBUG("End of DataAlterReadRapidJSON::setEndKey: no key to remove");
  return false; // ok
}

/* --------------------------------------------------------------------------- */
/* setEndObject --                                                             */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::setEndObject( int level ){
  BUG_DEBUG("Begin of DataAlterReadRapidJSON::setEndObject: level " << level);
  assert( m_stack.size() );
  ItemInfo *info = m_stack.top();

  if( info->isInfoType( ItemInfo::type_Key ) ){
    if( info->getLevel() == level + 1 ){
      BUG_DEBUG(" --> remove the last key of the Object from stack");
      UpdateStatus status = info->markItemUpdated();
      delete info;
      m_stack.pop();
      info = m_stack.top();
      info->setStatus( status );
      setStatus( status );
    }
    else{
      assert( info->getLevel() <= level );
    }
  }

  if( info->doOverride( level + 1 ) ){
    BUG_DEBUG("End of DataAlterReadRapidJSON::setEndObject: override already set");
    return true; // override
  }

  BUG_DEBUG(" --> remove object from stack");
  BUG_DEBUG(" --> type=" << info->itemType()
            << ", level=" << info->getLevel()
            << ", name=" << info->getName());

  assert( info->isInfoType( ItemInfo::type_Object ) );

  UpdateStatus status = info->markItemUpdated();
  delete info;
  m_stack.pop();
  if( m_stack.size() ){
    info = m_stack.top();
    info->setStatus( status );
    setStatus( status );
  }
  BUG_DEBUG("End of DataAlterReadRapidJSON::setEndObject");
  return false; // ok
}

/* --------------------------------------------------------------------------- */
/* setStartArray --                                                            */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setStartArray( int level ){
  BUG_DEBUG("Begin of DataAlterReadRapidJSON::setStartArray: level " << level);
  assert( m_stack.size() );
  ItemInfo *info = m_stack.top();
  if( info->doOverride( level ) ){
    BUG_DEBUG("End of DataAlterReadRapidJSON::setStartArray: override already set");
    return;
  }

  BUG_DEBUG(" --> start with new index");
  ItemInfo *clone = info->clone( level, ItemInfo::type_List );
  clone->startNewIndex();
  m_stack.push( clone );
  BUG_DEBUG("End of DataAlterReadRapidJSON::setStartArray: level " << level);
}

/* --------------------------------------------------------------------------- */
/* setEndArray --                                                              */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setEndArray(){
  BUG_DEBUG("Begin of DataAlterReadRapidJSON::setEndArray");
  assert( m_stack.size() );
  ItemInfo *info = m_stack.top();

  assert( info->isInfoType( ItemInfo::type_List ) );
  BUG_DEBUG(" --> remove last index");
  UpdateStatus status = info->markItemUpdated();
  delete info;
  m_stack.pop();
  if( m_stack.size() ){
    info = m_stack.top();
    info->setStatus( status );
    setStatus( status );
  }
  BUG_DEBUG("End of DataAlterReadRapidJSON::setEndArray");
}

/* --------------------------------------------------------------------------- */
/* doOverride --                                                               */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::doOverride( int level ) const{
  ItemInfo *info = m_stack.top();
  return info->doOverride( level );
}

/* --------------------------------------------------------------------------- */
/* setStatus --                                                                */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::setStatus( DataGlobals::UpdateStatus s ){
  if( s == ValueUpdated ){
    m_status = s;
  }
}


int DataAlterReadRapidJSON::ItemInfo::s_lnr = 1;

/*=============================================================================*/
/* Constructor / Destructor of ItemInfo                                        */
/*=============================================================================*/

DataAlterReadRapidJSON::ItemInfo::ItemInfo( int level,
                                            std::string name,
                                            ItemType type )
  : m_level( level )
  , m_name( name )
  , m_type( type )
  , m_override( true )
  , m_id( s_lnr ){
  s_lnr++;
}

DataAlterReadRapidJSON::ItemInfoExt::ItemInfoExt( DataItem &item,
                                                  DataIndexList *inxlist,
                                                  DataDictionary &dict,
                                                  int level,
                                                  ItemType type )
  : ItemInfo( level, item.getName(), type )
  , m_item( item )
  , m_status( NoUpdate )
  , m_inxlist( inxlist )
  , m_dict( dict )
  , m_dim( 0 ){
  m_override = false;

  BUG_DEBUG(">>>>>>>>>> NEW ItemInfo " <<
            itemType() << " [" << m_level << "] (" << m_id << ") " << m_item.getName() <<
            m_inxlist->indices() << " >>>>>>>>>>");
}

DataAlterReadRapidJSON::ItemInfo::~ItemInfo(){
  BUG_DEBUG("<<<<<<<<<< DELETE ItemInfo Type=" << itemType()
            << " Level=" << m_level
            << ", id=" << m_id
            << ", name= " << m_name
            << ", override=" << m_override
            << " <<<<<<<<<<");
}

DataAlterReadRapidJSON::ItemInfoExt::~ItemInfoExt(){
  assert( m_inxlist );
  delete m_inxlist;
}

/*=============================================================================*/
/* member functions of ItemInfo                                                */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* ItemInfo.markItemUpdated --                                                 */
/* --------------------------------------------------------------------------- */

DataGlobals::UpdateStatus DataAlterReadRapidJSON::ItemInfo::markItemUpdated(){
  return NoUpdate;
}

DataGlobals::UpdateStatus DataAlterReadRapidJSON::ItemInfoExt::markItemUpdated(){
  m_item.markDataItemUpdated( m_status );
  return m_status;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.setStatus --                                                       */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::setStatus( DataGlobals::UpdateStatus s ){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::setStatus( DataGlobals::UpdateStatus s ){
  if( s == ValueUpdated ){
    m_status = s;
  }
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.clone --                                                           */
/* --------------------------------------------------------------------------- */

DataAlterReadRapidJSON::ItemInfo *DataAlterReadRapidJSON::ItemInfo::clone( int level,
                                                                           ItemType type ){
  assert( false );
  return 0;
}

DataAlterReadRapidJSON::ItemInfo *DataAlterReadRapidJSON::ItemInfoExt::clone( int level,
                                                                              ItemType type ){
  ItemInfo *info = new ItemInfoExt( m_item, new DataIndexList(*m_inxlist), m_dict, level, type );
  if( m_type == type_List ){
    BUG_DEBUG(" --> increment index of list");
    m_inxlist->lastIndex().incrementIndex();
  }
  return info;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.getRundung --                                                      */
/* --------------------------------------------------------------------------- */

int DataAlterReadRapidJSON::ItemInfo::getRundung(){
  assert( false );
  return 0;
}

int DataAlterReadRapidJSON::ItemInfoExt::getRundung(){
  const DataUserAttr * attr = m_dict.getDataUserAttr();
  return attr != 0 ? attr->getRundung() : -1;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.dataElement --                                                     */
/* --------------------------------------------------------------------------- */

DataElement &DataAlterReadRapidJSON::ItemInfo::dataElement(){
  assert( false );
  // the following dummy code is never executed (because of above assert)
  // but it avoids the compiler warning: no return statement
  DataElement *el = new DataRealElement();
  return *el;
}

DataElement &DataAlterReadRapidJSON::ItemInfoExt::dataElement(){
  assert( m_inxlist );
  assert( !m_override );
  BUG_DEBUG("ItemInfo::dataElement: get " << m_item.getName() << m_inxlist->indices());
  DataElement &el = m_item.getDataElement( *m_inxlist );
  if( m_type == type_List ){
    BUG_DEBUG(" --> increment index of list");
    m_inxlist->lastIndex().incrementIndex();
  }
  return el;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.startNewIndex --                                                   */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::startNewIndex(){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::startNewIndex(){
  assert( m_inxlist );
  assert( m_type == type_List );
  m_inxlist->newIndex( 0 );
  DataContainer &cont = m_item.getDataContainer( *m_inxlist, 1 );
  assert( cont.isDataDimension() );
  m_dim = cont.getDataDimension();
  assert( m_dim );
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.endLastIndex --                                                    */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::endLastIndex(){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::endLastIndex(){
  assert( m_inxlist );
  assert( !m_inxlist->isEmpty() );
  assert( m_type == type_List );
  m_inxlist->deleteLastIndex();
  m_dim = 0;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.setString --                                                       */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::setString( std::string &value ){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::setString( std::string &value ){
  BUG_DEBUG("setString(" << value << ")");
  DataElement &el = dataElement();
  if( el.setValue( value, -1 ) ){
    m_status = ValueUpdated;
  }
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.setInteger --                                                      */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::setInteger( int value ){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::setInteger( int value ){
  BUG_DEBUG("setInteger(" << value << ")");
  if( dataElement().setValue( value, -1 ) ){
    m_status = ValueUpdated;
  }
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.setUInteger --                                                      */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::setUInteger( unsigned int value ){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::setUInteger( unsigned int value ){
  BUG_DEBUG("setUInteger(" << value << ")");
  if( dataElement().setValue( value, -1 ) ){
    m_status = ValueUpdated;
  }
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.setDouble --                                                       */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::setDouble( double value ){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::setDouble( double value ){
  BUG_DEBUG("setDouble(" << value << ")");
  if( dataElement().setValue( value, getRundung() ) ){
    m_status = ValueUpdated;
  }
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.setInvalid --                                                      */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::setInvalid(){
  assert( false );
}

void DataAlterReadRapidJSON::ItemInfoExt::setInvalid(){
  BUG_DEBUG("setInvalid");
  if( dataElement().clearElement() == ValueUpdated ){
    m_status = ValueUpdated;
  }
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.isStructItem --                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::ItemInfo::isStructItem(){
  assert( false );
  return false;
}

bool DataAlterReadRapidJSON::ItemInfoExt::isStructItem(){
  return m_dict.getDataType() == type_StructVariable;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.isInfoType --                                                      */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::ItemInfo::isInfoType( ItemType type ){
  return m_type == type;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.getLevel --                                                        */
/* --------------------------------------------------------------------------- */

int DataAlterReadRapidJSON::ItemInfo::getLevel(){
  return m_level;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.getName --                                                         */
/* --------------------------------------------------------------------------- */

std::string &DataAlterReadRapidJSON::ItemInfo::getName(){
  return m_name;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.setOverride --                                                     */
/* --------------------------------------------------------------------------- */

void DataAlterReadRapidJSON::ItemInfo::setOverride(){
  m_override = true;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.doOverride --                                                      */
/* --------------------------------------------------------------------------- */

bool DataAlterReadRapidJSON::ItemInfo::doOverride( int level ){
  if( m_override ){
    if( level > m_level ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* ItemInfo.itemType --                                                        */
/* --------------------------------------------------------------------------- */

std::string DataAlterReadRapidJSON::ItemInfo::itemType(){
  switch( m_type ){
  case type_Base:
    return "BaseObject";
  case type_Key:
    return "KeyItem";
  case type_Object:
    return "Object";
  case type_List:
    return "List";
  default:
    return "unknown";
  }
}
