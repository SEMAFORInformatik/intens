
#include "datapool/DataElement.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataItem.h"
#include "datapool/DataDimension.h"
#include "datapool/DataAlterReadJSON.h"
#include "datapool/DataReference.h"

//#define LOGGER "intens.datapool"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterReadJSON::DataAlterReadJSON( std::string name, Json::Value &value )
  : DataAlterSetElement( true )
  , m_name( name )
  , m_root( value ){
}

DataAlterReadJSON::~DataAlterReadJSON(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterReadJSON::check( DataReference &ref ){
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterReadJSON::alterItem( DataItem &item,
				   UpdateStatus &status,
                                   DataInx &inx,
                                   bool final )
{
  if( m_root.type() == Json::nullValue ){
    // Keine Daten vorhanden
    BUG_DEBUG( "No Data in Json-stream (null)" );
    return false;
  }
  if( m_root.type() == Json::arrayValue ){
    if( m_root.size() == 0 ){
      // Keine Daten vorhanden
      BUG_DEBUG( "No Data in Json-stream (empty list)" );
      return false;
    }
  }

  status = NoUpdate;

  if( final ){
    BUG_ERROR("indexed range does not exist." );
    return false; // abbrechen
  }

  if( !inx.isLastLevel() ){
    return true;
  }

  DataDictionary *dict = item.getDictionary();

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

  if ( m_root.isObject() && m_root.isMember( m_name.c_str() ) ) {
    // stream with multiple elements
    BUG_DEBUG( "stream with multiple elements: name=[" << m_name << "]" );
    status = read( m_root[m_name], item, *indexlist, *dict );
  }
  else{
    BUG_DEBUG( "stream with one element: name=[" << m_name << "]" );
    status = read( m_root, item, *indexlist, *dict );
  }
  delete indexlist;

  BUG_DEBUG( "alterItem " << item.getName() << " is " <<
             (status != ValueUpdated ? "NOT ":"") << "updated" );
  return false;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

DataAlterReadJSON::UpdateStatus DataAlterReadJSON::read( Json::Value &value,
							 DataItem &item,
							 DataIndexList &inxlist,
							 DataDictionary &dict )
{
  BUG_DEBUG( "read: " << item.getName() << inxlist.indices() );

  if( value.type() == Json::objectValue ){
    DataElement &el = item.getDataElement( inxlist );
    if( !el.isStructElement() ){
      // Keine Struktur ?!?
      return NoUpdate;
    }
    return readObject( value, item, el, dict );
  }
  if( value.type() == Json::arrayValue ){
    return readArray( value, item, inxlist, dict );
  }
  return readValue( value, item, inxlist, dict );
}

/* --------------------------------------------------------------------------- */
/* readObject --                                                               */
/* --------------------------------------------------------------------------- */

DataAlterReadJSON::UpdateStatus DataAlterReadJSON::readObject( Json::Value &value,
							       DataItem &item,
							       DataElement &el,
							       DataDictionary &dict )
{
  BUG_DEBUG( "readObject: " << item.getName() );

  UpdateStatus status = NoUpdate;
  if( dict.getDataType() != type_StructVariable ){
    return status;
  }

  const DataStructElement &str_el = el.getStructElement();

  Json::Value::Members members( value.getMemberNames() );
  for ( Json::Value::Members::iterator it = members.begin();
	it != members.end();
	++it ) {
    const std::string &name = *it;
    DataItem *next_item = str_el.getItem( name );
    if( next_item ){
      BUG_DEBUG( "DataItem " << name << " found" );
      DataDictionary *next_dict = next_item->getDictionary();
      DataIndexList next_inxlist;
      UpdateStatus s = read( value[name], *next_item, next_inxlist, *next_dict );
      BUG_DEBUG( "DataItem " << name
                 << (s == NoUpdate ? " not ":" is ")
                 << "updated" );
      next_item->markDataItemUpdated( s );
      if( s != NoUpdate ){
        status = s;
      }
    }
    else{
      BUG_DEBUG( "Item " << name << " NOT in structure" );
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* readArray --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterReadJSON::UpdateStatus DataAlterReadJSON::readArray( Json::Value &value,
							      DataItem &item,
							      DataIndexList &inxlist,
							      DataDictionary &dict )
{
  BUG_DEBUG( "readArray: " << item.getName() << inxlist.indices() );

  UpdateStatus status = NoUpdate;
  Json::ValueType child_type = getChildType( value );
  if( child_type == Json::nullValue ){
    // Der Array ist leer
    return status;
  }

  DataIndex &inx = inxlist.newIndex( 0 );
  int size = value.size();

  if( child_type != Json::arrayValue ){
    // Die Elemente in der Liste sind keine Listen.
    DataContainer &cont = item.getDataContainer( inxlist, size );
    assert( cont.isDataDimension() );
    DataDimension *dim = cont.getDataDimension();
    assert( dim );
    for( int i = 0; i < size; i++ ){
      BUG_DEBUG( " -- Element (" << i << ") found" );
      DataElement &el = dim->getDataElement( item, i );
      UpdateStatus s = NoUpdate;
      if( child_type == Json::objectValue ){
        s = readObject( value[i], item, el, dict );
      }
      else{
        s = setValue( value[i], el, dict );
      }
      if( s != NoUpdate ){
        status = s;
      }
    }
    inxlist.deleteLastIndex();
    return status;
  }

  for( int index = 0; index < size; index++ ){
    inx.setIndex( index );
    if( value[index].type() == Json::nullValue ||
        value[index].type() != child_type ){
      continue; // ignore
    }
    UpdateStatus s = readArray( value[index], item, inxlist, dict );
    if( s != NoUpdate ){
      status = s;
    }
  }
  inxlist.deleteLastIndex();
  return status;
}

/* --------------------------------------------------------------------------- */
/* readValue --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterReadJSON::UpdateStatus DataAlterReadJSON::readValue( Json::Value &value,
							      DataItem &item,
							      DataIndexList &inxlist,
							      DataDictionary &dict )
{
  BUG_DEBUG( "readValue: " << item.getName() << inxlist.indices() );

  DataElement &el = item.getDataElement( inxlist );
  return setValue( value, el, dict );
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterReadJSON::UpdateStatus DataAlterReadJSON::alterData( DataElement &el ){
  assert( false );
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

DataAlterReadJSON::UpdateStatus DataAlterReadJSON::setValue( Json::Value &value,
							     DataElement &el,
							     DataDictionary &dict )
{
  const DataUserAttr * attr = dict.getDataUserAttr();
  int rundung = attr != 0 ? attr->getRundung() : -1;

  switch( value.type() ){
  case Json::nullValue:
    return el.clearElement();

  case Json::intValue:
    if( el.setValue( (int)value.asInt(), rundung ) ){
      return ValueUpdated;
    }
    break;

  case Json::uintValue:
    if( el.setValue( (int)value.asUInt(), rundung ) ){
      return ValueUpdated;
    }
    break;

  case Json::realValue:
    if( el.setValue( value.asDouble(), rundung ) ){
      return ValueUpdated;
    }
    break;

  case Json::stringValue:
    if( el.setValue( value.asString(), rundung ) ){
      return ValueUpdated;
    }
    break;

  case Json::booleanValue:
    if( el.setValue( (int)value.asBool(), rundung ) ){
      return ValueUpdated;
    }
    break;

  case Json::arrayValue:
    BUG_WARN( "WARNING: unexpected type 'arrayValue'" );
    break;

  case Json::objectValue:
    BUG_ERROR( "FATAL ERROR: unexpected type 'objectValue'" );
    assert( false );
    break;

  default:
    BUG_WARN( "FATAL ERROR: unknown type" );
    break;
  }
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* getChildType --                                                             */
/* --------------------------------------------------------------------------- */

Json::ValueType DataAlterReadJSON::getChildType( Json::Value &value ){
  int size = value.size();
  for ( int index =0; index < size; ++index ){
    if ( value[index].type() == Json::nullValue){
      continue;
    }
    return value[index].type();
  }
  return Json::nullValue;
}
