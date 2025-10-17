#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataReference.h"
#include "app/ItemAttr.h"
#include "app/UserAttr.h"
#include "app/DataAlterGetDbModified.h"

INIT_LOGGER();

#define CERR_WRITE(X) // std::cerr << X << std::endl;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetDbModified::DataAlterGetDbModified()
  : m_db_modified( false ){
}

DataAlterGetDbModified::~DataAlterGetDbModified(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetDbModified::check( DataReference &ref ){
  CERR_WRITE( ">>>>> Begin DataAlterGetDbModified: " << ref.fullName( true ) );
  //ref.writeDP( std::cerr, 4 );
  return false; // wildcard search is possible
}

/* --------------------------------------------------------------------------- */
/* epilog --                                                                   */
/* --------------------------------------------------------------------------- */

void DataAlterGetDbModified::epilog( bool result ){
  if( !m_db_modified ){
    CERR_WRITE( "<<<<< End DataAlterGetDbModified: NOT modified" );
  }
  else{
    CERR_WRITE( "<<<<< End DataAlterGetDbModified: IS modified");
  }
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetDbModified::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final )
{
  BUG_PARA( BugDBTimestamp, "alterItem", item.getName() );

  status = NoUpdate;

  const UserAttr *userattr = static_cast<const UserAttr *>( item.getDataUserAttr() );
  if( userattr != 0 ){
    if( userattr->IsDbTransient() ){
      CERR_WRITE( "Item " << item.name() << " is transient" );
      BUG_EXIT( "Item is transient" );
      return false; // transient items are ignored
    }
  }

  // check modification even when DB-Timestamp missing
  ItemAttr *itemattr = static_cast<ItemAttr *>( item.getDataItemAttr() );
  TransactionNumber db_ts = 0;
  if( itemattr != 0 ){
    db_ts = itemattr->DataBaseTimestamp();
  }

  TransactionNumber full_ts = 0;

  if( final ){
    // the indexed range does not exist, stop processing.
    full_ts = item.getDataItemFullUpdated();
    BUG_MSG( "db_ts=" << db_ts << ", full_ts=" << full_ts );
    if( db_ts < full_ts ){
      CERR_WRITE( item.name() << ": final> db_ts=" << db_ts << ", full_ts=" << full_ts << ", modified" );
      m_db_modified = true;
      BUG_MSG( "item '" << item.getName() << "' is modified" );
      BUG_DEBUG( "item '" << item.dictionary().getStaticName() << "' is modified" );
    }

    BUG_EXIT("indexed range does not exist." );
    return false; // abbrechen
  }

  if( !inx.isLastLevel() ){
    BUG_EXIT("more levels will follow" );
    return true; // ok
  }

  BUG_MSG("Type = " << item.getDataType() );
  full_ts = item.getDataItemFullUpdated();

  if( db_ts < full_ts ){
    CERR_WRITE( item.name() << ": last> db_ts=" << db_ts << ", full_ts=" << full_ts << ", modified" );
    m_db_modified = true;
    BUG_EXIT( "item '" << item.getName() << "' is modified" );
    BUG_DEBUG( "item '" << item.dictionary().getStaticName() << "' is modified" );
    return false;
  }

  if( item.getDataType() == DataDictionary::type_StructVariable ){
    // last level has a struct var, continue searching
    BUG_EXIT("StructItem: Weitere Level suchen" );
    return true; // ok
  }

  TransactionNumber up_ts = item.getDataItemValueUpdated();

  if( db_ts < up_ts ){
    CERR_WRITE( item.name() << ": last> db_ts=" << db_ts << ", up_ts=" << up_ts << ", modified" );
    m_db_modified = true;
    BUG_EXIT( "item '" << item.getName() << "' is modified" );
    BUG_DEBUG( "item '" << item.dictionary().getStaticName() << "' is modified" );
    return false;
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetDbModified::UpdateStatus DataAlterGetDbModified::alterData( DataElement &el ){
  return NoUpdate;
}
