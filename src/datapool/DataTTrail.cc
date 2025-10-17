
#include <assert.h>

#include "datapool/DataPool.h"
#include "datapool/DataTTrail.h"
#include "datapool/DataTTrailPoint.h"
#include "datapool/DataTTrailItem.h"
#include "datapool/DataTTrailItemList.h"
#include "datapool/DataTTrailCycleSwitch.h"
#include "datapool/DataTTrailCycleNew.h"
#include "datapool/DataTTrailCycleDelete.h"
#include "datapool/TransactionOwner.h"
#include "datapool/DataItemContainer.h"
#include "datapool/DataItem.h"
#include "datapool/DataLogger.h"
#include "datapool/DataDebugger.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrail::DataTTrail( DataPool &datapool )
  : m_datapool( datapool )
  , m_transactions( 0 )
  , m_recovering( false ){
}

DataTTrail::~DataTTrail(){
  clearTrail();
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* inTransaction (old style) --                                                */
/* --------------------------------------------------------------------------- */

bool DataTTrail::inTransaction() const{
  return m_transactions > 0;
}

/* --------------------------------------------------------------------------- */
/* isInTransaction --                                                          */
/* --------------------------------------------------------------------------- */

bool DataTTrail::isInTransaction() const{
  return m_transactions > 0;
}

/* --------------------------------------------------------------------------- */
/* isntInTransaction --                                                        */
/* --------------------------------------------------------------------------- */

bool DataTTrail::isntInTransaction() const{
  return m_transactions == 0;
}

/* --------------------------------------------------------------------------- */
/* startTransaction --                                                         */
/* --------------------------------------------------------------------------- */

void DataTTrail::startTransaction( TransactionOwner *owner ){
  BUG_PARA( BugTTrail, "startTransaction"
	    , "Current Transaction=" << m_transactions << ", owner=" << owner->ownerId() );

  owner->setFinalTransaction( isntInTransaction() );

  // Die Position des Transaction-Points wird gespeichert, damit verschachtelte
  // Transaktionen einfach zu bearbeiten sind.
  m_transactions++;
  DataTTrailPoint *point = new DataTTrailPoint( m_transactions, m_datapool, owner );
  m_trail_points.push_back( m_trail.insert( m_trail.end(), point ) );

  assert( m_trail_points.size() == m_transactions );
  BUG_EXIT( "Transaction " << m_transactions << " started" );
}

/* --------------------------------------------------------------------------- */
/* commitTransaction --                                                        */
/* --------------------------------------------------------------------------- */

void DataTTrail::commitTransaction( TransactionOwner *owner ){
  BUG_PARA( BugTTrail, "commitTransaction"
	    , "Current Transaction=" << m_transactions << ", owner=" << owner->ownerId() );

  assert( isInTransaction() );
  assert( m_trail.size() > 0 );
  assert( m_trail_points.size() == m_transactions );

   // Wenn der commit nicht vom Owner der Transaktion aufgerufen wird, gehen wir
  // davon aus, dass die laufende Transaktion durch eine äussere Transaktion
  // mit einem commit abgesegnet wird. Ist diese Annahme falsch, wird sich dies
  // bald zeigen.

  if( !ownerIsInTransaction( owner ) ){
    // Der Owner ist nicht mehr vorhanden in der Liste der laufenden Transaktionen.
    // Er wird einfach ignoriert. Sicher ist er durch eine übergeordnete Transaktion
    // bereits eliminiert worden (siehe while-Schlaufe unten). Solche Ungereimtheiten
    // in der Reihenfolge können vorkommen.

    if( owner->isFinalTransaction() ){
      // Da es sich beim Owner um denjenigen der äussersten finalen Transaktion
      // handelt, sieht die Sache schlecht aus. Das darf nicht geschehen !!
      BUG_FATAL(  "FATAL: async TTrail: final owner not found" );
      assert( false );
    }

    BUG_EXIT( "unknown TransactionOwner => commit ignored" );
    return;
  }

  // Nun nehmen wir die jüngste Transaktion vom Stack.
  TTrail::iterator it = m_trail_points.back(); // current

  // Im Normalfall passt der Owner zu dieser Transaktion und die Schlaufe wird
  // nicht durchlaufen.
  while( !(*it)->isOwner( owner ) ){
    assert( (*it)->typ() == DataTTrailElement::POINT );
    // Wir führen auch für untergeordnete Transaktionen den commit() aus. Falls
    // sich diese Owner noch melden, werden diese zu spät gekommenen oben
    // abgefertigt.
    BUG_MSG( "async Transaction of Owner " << (*it)->getOwner()->ownerId() << " found" );
    commit();
    BUG_MSG( "async Transaction commited" );

    if( isntInTransaction() ){
      // Das darf nicht geschehen. Die finale Transaktion wird immer vom
      // jeweiligen Owner beendet !!
      BUG_FATAL(  "FATAL: async TTrail: Owner not found ?!?" );
      assert( false );
    }
    it = m_trail_points.back(); // current
  }

  commit();

  if( isntInTransaction() ){
    // Dies war die äusserste Transaction. Wir machen ein paar Kontrollen.
    BUG_MSG( "No remaining Transactions" );
    assert( m_trail.empty() );
    assert( m_trail_points.empty() );
    return;
  }
  BUG_EXIT("commit ok" );
}

/* --------------------------------------------------------------------------- */
/* ownerIsInTransaction --                                                     */
/* --------------------------------------------------------------------------- */

bool DataTTrail::ownerIsInTransaction( TransactionOwner *owner ){
  for( TTrailPoints::iterator it = m_trail_points.begin(); it !=  m_trail_points.end(); ++it ){
    if( (**it)->isOwner( owner ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* commitAllTransactions --                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrail::commitAllTransactions(){
  BUG_PARA( BugTTrail, "commitAllTransaction", "#" << m_transactions );

  assert( isInTransaction() );
  assert( m_trail.size() > 0 );
  assert( m_trail_points.size() == m_transactions );

  // Alle Elemente werden eliminiert. Wir machen kurzen Prozess.
  clearTrail();
  BUG_EXIT( "All transactions commited" );
}

/* --------------------------------------------------------------------------- */
/* abortTransaction --                                                         */
/* --------------------------------------------------------------------------- */

void DataTTrail::abortTransaction( TransactionOwner *owner ){
  BUG_PARA( BugTTrail, "abortTransaction"
	    , "Current Transaction=" << m_transactions << ", owner=" << owner->ownerId() );

  assert( isInTransaction() );
  assert( !m_trail.empty() );
  assert( m_trail_points.size() == m_transactions );


  if( !ownerIsInTransaction( owner ) ){
    // Der Owner ist nicht mehr vorhanden in der Liste der laufenden Transaktionen.
    // In diesem Fall ist es nicht möglich, ein abort() auszuführen. Wir gehen
    // davon aus, dass eine übergeordnete Transaktion dies hoffentlich bereits
    // getan hat.

    if( owner->isFinalTransaction() ){
      // Da es sich beim Owner um denjenigen der äussersten finalen Transaktion
      // handelt, sieht die Sache schlecht aus. Das darf nicht geschehen !!
      BUG_FATAL(  "FATAL: async TTrail: final owner not found" );
      assert( false );
    }

    BUG_EXIT( "unknown TransactionOwner => abort ignored" );
    return;
  }

  // Alle Daten werden bis zum Transaction-Point des Owners rekonstruiert. Die
  // Elemente werden nach Gebrauch gelöscht.
  TTrail::iterator it = m_trail_points.back(); // current

  // Im Normalfall passt der Owner zu dieser Transaktion und die Schlaufe wird
  // nicht durchlaufen.
  while( !(*it)->isOwner( owner ) ){
    BUG_MSG( "async Transaction of Owner " << (*it)->getOwner()->ownerId() << " found" );
    abort();
    BUG_MSG( "async Transaction aborted" );

    if( isntInTransaction() ){
      // Das darf nicht geschehen. Die finale Transaktion wird immer vom
      // jeweiligen Owner beendet !!
      BUG_FATAL( "FATAL ERROR: async TTrail: Owner not found ?!?" );
      assert( false );
    }
    it = m_trail_points.back(); // current
  }

  abort();

  if( isntInTransaction() ){
    // Dies war die äusserste Transaction. Wir machen ein paar Kontrollen.
    BUG_MSG( "No remaining Transactions" );
    assert( m_trail.empty() );
    assert( m_trail_points.empty() );
    return;
  }
  BUG_EXIT( "abort ok" );
}

/* --------------------------------------------------------------------------- */
/* abortAllTransactions --                                                     */
/* --------------------------------------------------------------------------- */

void DataTTrail::abortAllTransactions(){
  BUG_PARA( BugTTrail, "abortAllTransaction", "Current Transaction=" << m_transactions );

  assert( m_trail_points.size() == m_transactions );

  while( isInTransaction() ){
    abort();
  }

  assert( m_trail.empty() );
  assert( m_trail_points.empty() );
}

/* --------------------------------------------------------------------------- */
/* saveDataItem --                                                             */
/* --------------------------------------------------------------------------- */

void DataTTrail::saveDataItem( DataItemContainer *container, int itemNum ){
  assert( container != 0 );
  if( m_recovering ){
    // Siehe Ticket 695. Beim Rollback kann diese Methode aufgerufen werden.
    return;
  }

  if( isntActive() ){
    return;
  }

  DataItem *item = container->GetItem( itemNum );
  assert( item != 0 );

  if( item->dictionary().ItemIsWithoutTTrail() ){
    // Das Item ist sowohl für Transaktionen als auch für Undo deaktiviert.
    return;
  }

  if( isInTransaction() ){
    // Während einer Transaktion wird im TransactionTrail aufgezeichnet. Erst
    // am efolgreichen Ende einer Transaktion wird undo() wieder aktiv.
    if( searchDataItem( item ) ){
      LOG_DEBUG(  "item already exists" );
      return;
    }
  }

  BUG_PARA( BugTTrail, "saveDataitem", "Item " << item->getName() );

  item = item->duplicateItem();
  assert( item != 0 );
  DataTTrailItem *el = new DataTTrailItem( container, item ,itemNum );

  if( isInTransaction() ){
    m_trail.push_back( el );
    BUG_EXIT( "In Transaction: Item saved in TTrail" );
    return;
  }

  if( m_undo_data.isEnabled() ){
    m_undo_data.insertElement( el );
    BUG_EXIT( "Undo aktiv: Item inserted" );
    return;
  }

  // FATAL: Das darf nicht sein !
  assert( false ); // Notbremse
}

/* --------------------------------------------------------------------------- */
/* saveActionElement --                                                        */
/* --------------------------------------------------------------------------- */

void DataTTrail::saveActionElement( DataTTrailElement *el ){
  BUG( BugTTrail, "saveActionElement" );

  assert( el != 0 );
  assert( !m_recovering );

  if( isntActive() ){
    delete el;
    BUG_EXIT( "isnt active" );
    return;
  }

  if( isInTransaction() ){
    m_trail.push_back( el );
    BUG_EXIT( "element saved" );
    return;
  }

  if( !m_undo_data.isEnabled() ){
    delete el;
    BUG_EXIT( "Undo/Redo not enabled" );
    return;
  }
  m_undo_data.insertElement( el );
  BUG_EXIT( "Undo/Redo element inserted" );
}

/* --------------------------------------------------------------------------- */
/* saveCycleNumber --                                                          */
/* --------------------------------------------------------------------------- */

void DataTTrail::saveCycleNumber( int cyclenum ){
  if( m_recovering ){
    return;
  }

  // Über einen Cycle-Switch funktioniert undo() nicht.
  m_undo_data.clear();

  if( isntInTransaction() ){
    return;
  }

  LOG_DEBUG(  "Cycle #" << cyclenum );
  DataTTrailCycleSwitch *el = new DataTTrailCycleSwitch( cyclenum );
  m_trail.push_back( el );
}

/* --------------------------------------------------------------------------- */
/* saveCycleBeforeNew --                                                       */
/* --------------------------------------------------------------------------- */

void DataTTrail::saveCycleBeforeNew( int old_cyclenum, int new_cyclenum ){
  assert( !m_recovering );

  // Über einen Cycle-Switch funktioniert undo() nicht.
  m_undo_data.clear();

  if( isntInTransaction() ){
    return;
  }

  LOG_DEBUG(  "Cycle #" << old_cyclenum );
  DataTTrailCycleNew *el = new DataTTrailCycleNew( old_cyclenum, new_cyclenum );
  m_trail.push_back( el );
}

/* --------------------------------------------------------------------------- */
/* saveDeletedCycle --                                                         */
/* --------------------------------------------------------------------------- */

void DataTTrail::saveDeletedCycle( DataItem *cycle, int cyclenum ){
  assert( !m_recovering );
  assert( cycle != 0 );

  // Über einen gelöschten Cycle funktioniert undo() nicht.
  m_undo_data.clear();

  if( isntInTransaction() ){
    DataItem::deleteItem( cycle );
    return;
  }

  LOG_DEBUG( "Cycle #" << cyclenum );
  DataTTrailCycleDelete *el = new DataTTrailCycleDelete( cycle, cyclenum );
  m_trail.push_back( el );
}

/* --------------------------------------------------------------------------- */
/* TTrailUndo --                                                               */
/* --------------------------------------------------------------------------- */

DataTTrailUndo &DataTTrail::TTrailUndo(){
  return m_undo_data;
}

/* --------------------------------------------------------------------------- */
/* operator <<                                                                 */
/* --------------------------------------------------------------------------- */

std::ostream &operator << ( std::ostream &ostr, const DataTTrailElement &el ){
  el.print( ostr );
  return ostr;
}

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* isInFinalTransaction --                                                     */
/* --------------------------------------------------------------------------- */

bool DataTTrail::isInFinalTransaction() const{
  return m_transactions == 1;
}

/* --------------------------------------------------------------------------- */
/* isInInnerTransaction --                                                     */
/* --------------------------------------------------------------------------- */

bool DataTTrail::isInInnerTransaction() const{
  return m_transactions > 1;
}

/* --------------------------------------------------------------------------- */
/* commit --                                                                   */
/* --------------------------------------------------------------------------- */

void DataTTrail::commit(){
  BUG( BugTTrail, "commit" );

  if( isInFinalTransaction() ){
    // Dies muss die äusserste Transaktion sein.
    commitFinalTransaction();
    return;
  }

  if( isInInnerTransaction() ){
    // Wir befinden uns noch in einer inneren Transaktion.
    commitInnerTransaction();
    return;
  }

  BUG_FATAL( "Fatal Error: Not in Transaction-State" );
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* commitInnerTransaction --                                                   */
/* --------------------------------------------------------------------------- */

void DataTTrail::commitInnerTransaction(){
  BUG_PARA( BugTTrail, "commitInnerTransaction", "#" << m_transactions );

  assert( isInInnerTransaction() );
  assert( m_trail_points.size() == m_transactions );

  TTrail::iterator ic = m_trail_points.back(); // current
  m_trail_points.pop_back();
  TTrail::iterator ip = m_trail_points.back(); // prior
  TTrail::iterator it = ic;

  // Alle Elemente, welche bereits in der nächst äusseren Transaktion gesichert
  // wurden, werden eliminiert. Damit ist gewährleistet, dass die Daten im Falle
  // eines abort wieder hergestellt werden können.

  it++;
  while( it != m_trail.end() ){
    if( resident( (*it), ip, ic ) ){
      BUG_MSG( "existing Item deleted" );
      delete (*it);
      it = m_trail.erase( it );
    }
    else{
      ++it;
    }
  }

  delete (*ic);
  m_trail.erase( ic ); // delete Transaction Point

  m_transactions--;
}

/* --------------------------------------------------------------------------- */
/* commitFinalTransaction --                                                   */
/* --------------------------------------------------------------------------- */

void DataTTrail::commitFinalTransaction(){
  BUG_PARA( BugTTrail, "commitFinalTransaction", "Current Transaction=" << m_transactions );

  assert( isInFinalTransaction() );
  assert( m_trail_points.size() == m_transactions );

  if( m_undo_data.isDisabled() ){
    // Ohne Undo-Feature gibt es nicht viel zu tun.
    clearTrail();
    BUG_EXIT( "TransactionTrail cleared" );
    return;
  }

  m_trail_points.clear();
  m_transactions = 0;

  if( m_trail.empty() ){
    BUG_EXIT( "TransactionTrail is empty" );
    return;
  }

  DataTTrailItemList *itemlist = new DataTTrailItemList();
  DataTTrailItem *item = 0;
  bool undo_is_possible = true;

  for( TTrail::iterator it = m_trail.begin(); it != m_trail.end(); ++it ){
    switch( (*it)->typ() ){
    case DataTTrailElement::ITEM:
      item = (*it)->getItem();
      assert( item );
      itemlist->insertItem( item );
      BUG_MSG( "Item @" << item << " in ItemList inserted" );
      break;

    case DataTTrailElement::CYCLE:
      // Sobald ein DataTTrailElement im Transactiontrail vorkommt welches
      // kein DataTTrailItem ist, muss ein mögliches Undo verhindert werden.
      // Dort wird nur ein einfaches Wiederherstellen von Daten unterstützt
      // ohne Cycle-Manipulation und Ähnliches.
      undo_is_possible = false;
      delete (*it);
      BUG_MSG( "CYCLE-Action: Undo not possible" );
      break;

    case DataTTrailElement::POINT:
      delete (*it);
      BUG_MSG( "POINT: ok -> ignored" );
      break;

    case DataTTrailElement::ACTION:
      itemlist->insertItem( (*it) );
      break;

    default:
      BUG_MSG( "other Typ " << (*it)->typ() << " ignored" );
      delete (*it);
      break;
    }
  }

  m_trail.clear();
  BUG_MSG( "Der TransactionTrail ist nun leer" );

  if( undo_is_possible ){
    if( itemlist->isntEmpty() ){
      BUG_MSG( "insert ItemList" );
      m_undo_data.insertElement( itemlist );
      BUG_EXIT( "ItemList on Undo-Stack" );
    }
    else{
      delete itemlist;
      BUG_EXIT( "ItemList is empty => delete" );
    }
    return;
  }

  // Die Transaction ist fertig und ein Undo ist nicht möglich. Alle
  // verbleibenden Daten werden gelöscht.
  delete itemlist;
  m_undo_data.clear();
  BUG_EXIT( "Undo not possible: ItemList deleted" );
}

/* --------------------------------------------------------------------------- */
/* abort  --                                                                   */
/* --------------------------------------------------------------------------- */

void DataTTrail::abort(){
  BUG( BugTTrail, "abort" );

  m_recovering = true;
  TTrail::iterator ip = m_trail_points.back();
  TTrail::iterator it = m_trail.end();
  --it;

  while( it != ip ){
    (*it)->restore( m_datapool );
    delete (*it);
    --it;
  }
  delete (*ip);

  m_trail.erase( ip, m_trail.end() );
  m_trail_points.pop_back();
  m_transactions--;
  m_recovering = false;
}

/* --------------------------------------------------------------------------- */
/* clearTrail --                                                               */
/* --------------------------------------------------------------------------- */

void DataTTrail::clearTrail(){
  LOG_DEBUG(  "clear" );
  m_trail_points.clear();

  for( TTrail::iterator it = m_trail.begin(); it != m_trail.end(); ++it ){
    delete (*it);
  }
  m_trail.clear();
  m_transactions = 0;
 }

/* --------------------------------------------------------------------------- */
/* searchDataItem --                                                           */
/* --------------------------------------------------------------------------- */

bool DataTTrail::searchDataItem( DataItem *item ){
  for( TTrail::iterator it = m_trail_points.back(); it != m_trail.end(); ++it ){
    if( (*it)->isEqual( item ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* resident --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataTTrail::resident( DataTTrailElement *el
			 , TTrail::iterator from
                         , const TTrail::iterator to )
{
  for( ++from; from != to; ++from ){
    if( el->isEqual( *(*from) ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isntActive --                                                               */
/* --------------------------------------------------------------------------- */

bool DataTTrail::isntActive() const{
  if( inTransaction() ){
    return false;
  }
  if( m_undo_data.isEnabled() ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

void DataTTrail::destroy(){
  clearTrail();
  m_undo_data.clear();
}

/* --------------------------------------------------------------------------- */
/* showMemoryUsage --                                                          */
/* --------------------------------------------------------------------------- */

void DataTTrail::showMemoryUsage( std::ostream &ostr ){
#if defined _DEBUG
  bool show_counts = true;
  int obj = 0;
  obj += BUG_SHOW_COUNT( DataTTrailItem       , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataTTrailItemList   , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataTTrailPoint      , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataTTrailCycleDelete, ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataTTrailCycleNew   , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataTTrailCycleSwitch, ostr, show_counts );
  ostr << "==> Total: " << obj << " TTrail-Objects in DataPool allocated <==" << std::endl;
#endif
}
