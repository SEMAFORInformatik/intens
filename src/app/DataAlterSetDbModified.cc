
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataReference.h"
#include "app/ItemAttr.h"
#include "app/UserAttr.h"
#include "app/DataAlterSetDbModified.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetDbModified::DataAlterSetDbModified( TransactionNumber trans, bool new_trans )
  : m_db_timestamp( trans )
  , m_new_transaction( new_trans ){
  if( trans == 0 ){
    m_db_timestamp = DataPool::getTransactionNumber();
  }
}

DataAlterSetDbModified::~DataAlterSetDbModified(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterSetDbModified::check( DataReference &ref ){
  BUG_DEBUG(">>>>> Begin DataAlterSetDbModified: " << ref.fullName(true) );
  return false; // Es kann auch mit Wildcards gesucht werden.
}

/* --------------------------------------------------------------------------- */
/* epilog --                                                                   */
/* --------------------------------------------------------------------------- */

void DataAlterSetDbModified::epilog( bool result ){
  BUG_DEBUG("<<<<< End DataAlterSetDbModified: new_transaction=" << m_new_transaction );
  if( m_new_transaction ){
    DataPool::getDatapool().NewTransaction();
  }
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterSetDbModified::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final )
{
  BUG_DEBUG( "Begin alterItem: '" << item.getName() << "'");

  status = NoUpdate;

  const UserAttr *userattr = static_cast<const UserAttr *>( item.getDataUserAttr() );
  if( userattr != 0 ){
    if( userattr->IsDbTransient() ){
      BUG_DEBUG( "End alterItem: Item is transient" );
      return false; // transiente Items interessieren uns nicht
    }
  }

  if( !inx.isLastLevel() ){
    if( final ){
      // Der indizierte Bereich ist nicht vorhanden. Wir brechen die
      // Verarbeitung ab.
      BUG_DEBUG("End alterItem: Indizierter Bereich nicht vorhanden." );
      return false; // abbrechen
    }

    BUG_DEBUG("End alterItem: Weitere Level folgen" );
    return true; // ok
  }

  // Letzter Level erreicht
  ItemAttr *itemattr = static_cast<ItemAttr *>( item.getDataItemAttr( true ) );
  assert( itemattr != 0 );
  BUG_DEBUG(" -- set timestamp");
  itemattr->setDataBaseTimestamp( m_db_timestamp );

  if( item.getDataType() != DataDictionary::type_StructVariable ){
    // Auf dem letzten angegebenen Level ist keine Struktur-Variable.
    // In diesem Fall ist die Arbeit beendet.
    BUG_DEBUG("End alterItem: Ende erreicht" );
    return false; // abbrechen
  }

  BUG_DEBUG( "End alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterSetDbModified::UpdateStatus DataAlterSetDbModified::alterData( DataElement &el ){
  return NoUpdate; // kein update einzelner Werte
}

/* --------------------------------------------------------------------------- */
/* inclusiveValidItemAttributes --                                             */
/* --------------------------------------------------------------------------- */

bool DataAlterSetDbModified::inclusiveValidItemAttributes() const{
  BUG_DEBUG("-- inclusive valid Item-Attributes");
  return true;
}
