
#include "utils/utils.h"
#include "datapool/DataPool.h"
#include "datapool/DataReference.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataItem.h"
#include "datapool/DataDimension.h"
#include "datapool/DataElement.h"
#include "datapool/DataAlterFunctor.h"
#include "datapool/DataItemAttr.h"
#include "datapool/DataValue.h"
#include "datapool/DataVector.h"
#include "datapool/DataMatrix.h"
#include "datapool/DataEvents.h"
#include "datapool/DataLogger.h"


/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

DataPool *DataPool::s_instance = 0;
DataEvents *DataPool::s_events = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataPool::DataPool(DataUserAttr *pUserAttr, DataItemAttr *pItemAttr)
  : m_ActualCycle( -1 )
  , m_DataDict( this, pUserAttr )
  , m_ActualTransaction( 0 )
  , m_ClearTransaction( 0 )
  , m_pItemAttr( 0 )
  , m_ttrail( *this ){
  LOG_DEBUG("(constructor)");

  m_pItemAttr = (pItemAttr == 0 ) ? DefaultDataItemAttr().NewClass()
                                  : pItemAttr->NewClass();
}

DataPool::~DataPool(){
  BUG( BugRef, "destructor");

  m_ttrail.destroy();

  CycleList::iterator it;
  for( it = m_Cycles.begin(); it != m_Cycles.end(); it++ ){
    if( *it != 0 ){
      DataItem *item = DataItem::deleteItem( *it );
      if( item != 0 ){
        std::cout << "Item '" << item->getName() << "' not deleted" << std::endl;
      }
      *it = 0;
    }
  }
#if defined _DEBUG
  DataTTrail::showMemoryUsage( std::cout );
  DataPool::showMemoryUsage( std::cout );
#endif
}

/*=============================================================================*/
/* Functions                                                                   */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

DataPool *DataPool::Instance(DataUserAttr *pUserAttr, DataItemAttr *pItemAttr ){
  if( s_instance == 0 ){
    s_instance = new DataPool(pUserAttr,pItemAttr);
  }
  return s_instance;
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

DataPool &DataPool::getDatapool(){
  assert( s_instance != 0 );
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* Destroy --                                                                  */
/* --------------------------------------------------------------------------- */

void DataPool::Destroy(){
  if( s_instance != 0 ){
    delete s_instance;
    s_instance = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* operator==  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataPool::operator==( const DataPool &dp ){
  return this == &dp;
}

/* --------------------------------------------------------------------------- */
/* operator!=  --                                                              */
/* --------------------------------------------------------------------------- */

bool DataPool::operator!=( const DataPool &dp ){
  return this != &dp;
}

/* --------------------------------------------------------------------------- */
/* newDataReference --                                                         */
/* --------------------------------------------------------------------------- */

DataReference *DataPool::newDataReference( const char *addr ){
  return DataReference::newDataReference( addr );
}

/* --------------------------------------------------------------------------- */
/* newDataReference --                                                         */
/* --------------------------------------------------------------------------- */

DataReference *DataPool::newDataReference( const DataReference &ref, const char *addr ){
  return DataReference::newDataReference( ref, addr );
}

/* --------------------------------------------------------------------------- */
/* NewTransaction --                                                           */
/* --------------------------------------------------------------------------- */

TransactionNumber DataPool::NewTransaction(){
  return ++m_ActualTransaction;
}

/* --------------------------------------------------------------------------- */
/* GetCurrentTransaction --                                                    */
/* --------------------------------------------------------------------------- */

TransactionNumber DataPool::GetCurrentTransaction() const{
  return m_ActualTransaction;
}

/* --------------------------------------------------------------------------- */
/* getTransactionNumber --                                                     */
/* --------------------------------------------------------------------------- */

TransactionNumber DataPool::getTransactionNumber(){
  if( s_instance != 0 ){
    return s_instance->GetCurrentTransaction();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getLastClearTransaction --                                                  */
/* --------------------------------------------------------------------------- */

TransactionNumber DataPool::getLastClearTransaction() const{
  return m_ClearTransaction;
}

/* --------------------------------------------------------------------------- */
/* getLastClearTransaction --                                                  */
/* --------------------------------------------------------------------------- */

bool DataPool::isLastClearTransaction( TransactionNumber num ){
  if( s_instance != 0 ){
    return s_instance->getLastClearTransaction() == num;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* RestoreCurrentTransaction --                                                */
/* --------------------------------------------------------------------------- */

void DataPool::RestoreCurrentTransaction( TransactionNumber num ){
  m_ActualTransaction = num;
}

/* --------------------------------------------------------------------------- */
/* GetRootDict --                                                              */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPool::GetRootDict(){
  assert(m_DataDict.getDataType() == DataDictionary::type_StructDefinition);
  return &m_DataDict;
}

/* --------------------------------------------------------------------------- */
/* GetRootDictVar --                                                           */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPool::GetRootDictVar(){
  assert( m_DataDict.getNext()!= 0 );
  assert( m_DataDict.getNext()->getDataType() == DataDictionary::type_StructVariable );
  return m_DataDict.getNext();
}

/* --------------------------------------------------------------------------- */
/* GetDataItemAttr --                                                          */
/* --------------------------------------------------------------------------- */

DataItemAttr *DataPool::GetDataItemAttr(){
  assert(m_pItemAttr != 0);
  return  m_pItemAttr;
}

/* --------------------------------------------------------------------------- */
/* AddToDictionary --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPool::AddToDictionary( const char *pStructMember,
                                           const std::string &Name,
                                           const DataDictionary::DataType Type,
                                           const char *pStuctVar,
                                           bool ItemIsGlobal,
                                           bool ItemIsInternal )
{
  return m_DataDict.AddToDictionary( pStructMember,
                                     Name,
                                     Type,
                                     pStuctVar,
                                     ItemIsGlobal,
                                     ItemIsInternal );
}

/* --------------------------------------------------------------------------- */
/* AddToDictionary --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPool::AddToDictionary( DataDictionary *pStructMemberDict,
                                           const std::string &Name,
                                           const DataDictionary::DataType Type,
                                           DataDictionary *pStuctVarDict,
                                           bool ItemIsGlobal,
                                           bool ItemIsInternal )
{
  return m_DataDict.AddToDictionary( pStructMemberDict,
                                     Name,
                                     Type,
                                     pStuctVarDict,
                                     ItemIsGlobal,
                                     ItemIsInternal );
}

/* --------------------------------------------------------------------------- */
/* RemoveFromDictionary --                                                     */
/* --------------------------------------------------------------------------- */

bool DataPool::RemoveFromDictionary( const char *pName ){
  assert(pName != 0 && *pName != 0);
  return RemoveFromDictionary( FindDictEntry( pName ) );
}

/* --------------------------------------------------------------------------- */
/* RemoveFromDictionary --                                                     */
/* --------------------------------------------------------------------------- */

bool DataPool::RemoveFromDictionary( DataDictionary *pDict ){
  assert(pDict != 0);
  return DynamicDatapoolDelete( pDict );
}

/* --------------------------------------------------------------------------- */
/* FindDictEntry --                                                            */
/* --------------------------------------------------------------------------- */

DataDictionary *DataPool::FindDictEntry( const char *pAddr ){
  return m_DataDict.FindDictEntry(pAddr);
}

/* --------------------------------------------------------------------------- */
/* PrintDataDictionary --                                                      */
/* --------------------------------------------------------------------------- */

void DataPool::PrintDataDictionary(){
  GetRootDict()->PrintDataDictionary();
}

/* --------------------------------------------------------------------------- */
/* NewCycle --                                                                 */
/* --------------------------------------------------------------------------- */

int DataPool::NewCycle(){
  LOG_DEBUG("NewCycle");

  int i = NumCycles();
  m_Cycles.resize(i+1,0);
  if (m_ActualCycle < 0) {
    m_Cycles[i] = new DataItem( 0, GetRootDictVar(), false );
  }
  else{
    m_Cycles[i] = m_Cycles[m_ActualCycle]->cloneItem();
  }
  assert(m_Cycles[i] != 0);

  m_ttrail.saveCycleBeforeNew( m_ActualCycle, i );
  m_ActualCycle = i;

  LOG_DEBUG("Actual Cycle=" << m_ActualCycle);
  return m_ActualCycle;
}

/* --------------------------------------------------------------------------- */
/* SetCycle --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataPool::SetCycle( int CycleIndex, bool clearUndoStack ){
  LOG_DEBUG("CycleIndex=" << CycleIndex);

  if (CycleIndex == m_ActualCycle){
    LOG_DEBUG("cycle already set");
    return true;
  }
  if (CycleIndex < 0              ||
      CycleIndex >= NumCycles()   ||
      m_Cycles[CycleIndex] == 0     ){
    LOG_DEBUG("rslt=false (no cycle)");
    return false;
  }
  if( clearUndoStack ){
    m_ttrail.saveCycleNumber( m_ActualCycle );
  }
  m_ActualCycle = CycleIndex;

  LOG_DEBUG("rslt=true");
  return true;
}

/* --------------------------------------------------------------------------- */
/* ClearCycle --                                                               */
/* --------------------------------------------------------------------------- */

bool DataPool::ClearCycle( int CycleIndex ){
  LOG_DEBUG("CycleIndex=" << CycleIndex);

  if( CycleIndex < 0 || CycleIndex >= NumCycles() ){
    LOG_DEBUG("rslt=false (no cycle)");
    return false;
  }

  DataStructElement *structelement = GetRootElement( CycleIndex );
  assert( structelement != 0 );
  DataItemContainer *cont = structelement->getItemContainer();
  cont->clearCycle( m_ttrail );

  // Da nach einem ClearCycle alle DataItem-Timestamps initialisiert sind,
  // benötigen wir diesem Timestamp für einen sauberen GUI-Update.
  m_ClearTransaction = GetCurrentTransaction();

  LOG_DEBUG("rslt=true");
  return true;
}

/* --------------------------------------------------------------------------- */
/* RemoveCycle --                                                              */
/* --------------------------------------------------------------------------- */

bool DataPool::RemoveCycle( int CycleIndex ){
  LOG_DEBUG("CycleIndex=" << CycleIndex);

  if( CycleIndex < 0 || CycleIndex >= NumCycles() ){
    LOG_DEBUG("rslt=false (no cycle)");
    return false;
  }

  m_ttrail.saveCycleNumber( m_ActualCycle );

  if( m_Cycles[CycleIndex] != 0 ){
    m_ttrail.saveDeletedCycle( m_Cycles[CycleIndex], CycleIndex );
    m_Cycles[CycleIndex] = 0;
  }

  m_Cycles.erase( m_Cycles.begin() + CycleIndex );

  if( m_ActualCycle >= CycleIndex ){
    m_ActualCycle--;
    if( m_ActualCycle < 0 && NumCycles() > 0 ){
      /*****************************************/
      /* Wir haben cycle 0 elimiert. Dies war  */
      /* auch der aktuelle Cycle.              */
      /* Wir setzten desshalb den aktuellen    */
      /* Cycle wieder auf 0                    */
      /*****************************************/
      m_ActualCycle = 0;
    }
  }

  LOG_DEBUG("rslt=true");
  return true;
}

/* --------------------------------------------------------------------------- */
/* TTRemoveCycle --                                                            */
/* Diese Funktion benoetigt der TTrail zum Restaurieren eines alten Zustands.  */
/* --------------------------------------------------------------------------- */

bool DataPool::TTRemoveCycle( int currentcycle, int newcycle ){
  LOG_DEBUG("current=" << currentcycle );

  if( currentcycle < 0 || currentcycle >= NumCycles() ){
    LOG_DEBUG("rslt=false (no cycle)");
    return false;
  }

  if( m_Cycles[currentcycle] != 0 ){
    DataItem::deleteItem( m_Cycles[currentcycle] );
    m_Cycles[currentcycle] = 0;
  }

  m_Cycles.erase( m_Cycles.begin() + currentcycle );

  if( newcycle < 0 || newcycle >= NumCycles() ){
    m_ActualCycle = 0;
  }
  else{
    m_ActualCycle = newcycle;
  }
  LOG_DEBUG("rslt=true");
  return true;
}

/* --------------------------------------------------------------------------- */
/* TTInsertCycle --                                                            */
/* Diese Funktion benoetigt der TTrail zum Restaurieren eines alten Zustands.  */
/* --------------------------------------------------------------------------- */

void DataPool::TTInsertCycle( DataItem *cycle, int cyclenum ){
  LOG_DEBUG("Cycle #" << cyclenum );

  int i = NumCycles();
  assert( cyclenum >= 0 && cyclenum <= i );
  m_Cycles.resize( i+1 ,0 );

  while( i > cyclenum ){
    m_Cycles[i] = m_Cycles[i-1];
    i--;
  }
  m_Cycles[cyclenum] = cycle;
}

/* --------------------------------------------------------------------------- */
/* NumCycles --                                                                */
/* --------------------------------------------------------------------------- */

int DataPool::NumCycles() const{
  return m_Cycles.size();
}

/* --------------------------------------------------------------------------- */
/* GetCurrentCycle --                                                          */
/* --------------------------------------------------------------------------- */

int DataPool::GetCurrentCycle() const{
  return m_ActualCycle;
}

/* --------------------------------------------------------------------------- */
/* DataPoolAllocated --                                                        */
/* --------------------------------------------------------------------------- */

bool DataPool::DataPoolAllocated() const{
  return m_ActualCycle >= 0;
}

/* --------------------------------------------------------------------------- */
/* GetRootItem --                                                              */
/* --------------------------------------------------------------------------- */

DataItem * DataPool::GetRootItem( int cyclenum ){
  LOG_DEBUG("GetRootItem");

  assert( GetCurrentCycle() > -1 );
  assert( GetCurrentCycle() < NumCycles() );

  if( cyclenum < 0 ){
    return m_Cycles[ GetCurrentCycle() ];
  }
  if( cyclenum < NumCycles() ){
    return m_Cycles[ cyclenum ];
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* GetRootElement --                                                           */
/* --------------------------------------------------------------------------- */

DataStructElement * DataPool::GetRootElement( int cyclenum ){
  LOG_DEBUG("GetRootElement");

  DataItem *pIt = GetRootItem( cyclenum );
  if( pIt == 0 ){
    return 0;
  }

  DataStructElement *pStrElem = 0;
  assert(pIt != 0);
  DataContainer &cont = pIt->container();
  assert( cont.getContainerType() == DataContainer::ContainerIsElement );

  pStrElem = reinterpret_cast<DataStructElement *>( &cont );
  LOG_DEBUG("rslt=" << pStrElem);
  return pStrElem;
}

/* --------------------------------------------------------------------------- */
/* BeginDataPoolTransaction --                                                 */
/* --------------------------------------------------------------------------- */

void DataPool::BeginDataPoolTransaction( TransactionOwner *owner ){
  LOG_DEBUG("BeginDataPoolTransaction");
  assert( m_ActualCycle >= 0 );

  m_ttrail.startTransaction( owner );
}

/* --------------------------------------------------------------------------- */
/* CommitDataPoolTransaction --                                                */
/* --------------------------------------------------------------------------- */

void DataPool::CommitDataPoolTransaction( TransactionOwner *owner, bool all ){
  LOG_DEBUG("CommitDataPoolTransaction");
  assert( m_ActualCycle >= 0 );

  if( !m_ttrail.inTransaction() ){
    LOG_DEBUG(  "No open Transactions" );
    return;
  }

  if( all ){
    m_ttrail.commitAllTransactions();
  }
  else{
    m_ttrail.commitTransaction( owner );
  }
}

/* --------------------------------------------------------------------------- */
/* RollbackDataPoolTransaction --                                              */
/* --------------------------------------------------------------------------- */

void DataPool::RollbackDataPoolTransaction( TransactionOwner *owner, bool all ){
  LOG_DEBUG("RollbackDataPoolTransaction");
  assert( m_ActualCycle >= 0 );

  if( !m_ttrail.inTransaction() ){
    LOG_DEBUG(  "No open Transactions" );
    return;
  }

  if( all ){
    m_ttrail.abortAllTransactions();
  }
  else{
    m_ttrail.abortTransaction( owner );
  }
}

/* --------------------------------------------------------------------------- */
/* inDataPoolTransaction --                                                    */
/* --------------------------------------------------------------------------- */

bool DataPool::inDataPoolTransaction() const{
  return m_ttrail.inTransaction();
}

/* --------------------------------------------------------------------------- */
/* writeTransactionTrail --                                                    */
/* --------------------------------------------------------------------------- */

void DataPool::writeTransactionTrail( DataItemContainer *cont, int inx ){
  m_ttrail.saveDataItem( cont, inx );
}

/* --------------------------------------------------------------------------- */
/* activateUndo --                                                             */
/* --------------------------------------------------------------------------- */

void DataPool::activateUndo(){
  m_ttrail.TTrailUndo().activate();
}

/* --------------------------------------------------------------------------- */
/* isUndoActivated --                                                          */
/* --------------------------------------------------------------------------- */

bool DataPool::isUndoActivated(){
  return m_ttrail.TTrailUndo().isActivated();
}

/* --------------------------------------------------------------------------- */
/* attachUndoListener --                                                       */
/* --------------------------------------------------------------------------- */

void DataPool::attachUndoListener( DataTTrailUndoListener *lsnr ){
  m_ttrail.TTrailUndo().attachUndoListener( lsnr );
}

/* --------------------------------------------------------------------------- */
/* attachRedoListener --                                                       */
/* --------------------------------------------------------------------------- */

void DataPool::attachRedoListener( DataTTrailUndoListener *lsnr ){
  m_ttrail.TTrailUndo().attachRedoListener( lsnr );
}

/* --------------------------------------------------------------------------- */
/* saveTTrailActionElement --                                                  */
/* --------------------------------------------------------------------------- */

void DataPool::saveTTrailActionElement( DataTTrailElement *el ){
  m_ttrail.saveActionElement( el );
}

/* --------------------------------------------------------------------------- */
/* undo --                                                                     */
/* --------------------------------------------------------------------------- */

void DataPool::undo(){
  m_ttrail.TTrailUndo().undo( *this );
}

/* --------------------------------------------------------------------------- */
/* redo --                                                                     */
/* --------------------------------------------------------------------------- */

void DataPool::redo(){
  m_ttrail.TTrailUndo().redo( *this );
}

/* --------------------------------------------------------------------------- */
/* enableUndo --                                                               */
/* --------------------------------------------------------------------------- */

void DataPool::enableUndo(){
  m_ttrail.TTrailUndo().enable();
}

/* --------------------------------------------------------------------------- */
/* disableUndo --                                                              */
/* --------------------------------------------------------------------------- */

void DataPool::disableUndo(){
  m_ttrail.TTrailUndo().disable();
}

/* --------------------------------------------------------------------------- */
/* allowUndo --                                                                */
/* --------------------------------------------------------------------------- */

void DataPool::allowUndo(){
  BUG( BugUndo, "allowUndo" );

  m_ttrail.TTrailUndo().enable( true );
}

/* --------------------------------------------------------------------------- */
/* disallowUndo --                                                             */
/* --------------------------------------------------------------------------- */

bool DataPool::disallowUndo(){
  BUG( BugUndo, "disallowUndo" );

  if( m_ttrail.TTrailUndo().isDisabled() ){
    return false;
  }
  m_ttrail.TTrailUndo().disable( true );
  return true;
}

/* --------------------------------------------------------------------------- */
/* commitAllTransactions --                                                    */
/* --------------------------------------------------------------------------- */

void DataPool::commitAllTransactions(){
  LOG_DEBUG( "commitAllTransactions" );
  m_ttrail.commitAllTransactions();
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataPool::write( std::ostream &ostr, std::vector<std::string> &attrs
                      , int level, bool debug ){
  ostr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  CycleList::iterator it;
  int cycle = 0;
  for( it = m_Cycles.begin(); it != m_Cycles.end(); it++, ++cycle ){
    ostr << "<datapool cycle = \"" << cycle << "\">\n";
    if( *it != 0 ){
      (*it)->writeXML( ostr, attrs, level, debug );
    }
    ostr << "</datapool>\n";
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDP --                                                                  */
/* --------------------------------------------------------------------------- */

void DataPool::writeDP( std::ostream &ostr, int i, int cycle ){
ostr << "========== Begin of Datapool ==========" << std::endl;
  if( cycle == -9 ){
    cycle = 0;
    for( CycleList::iterator it = m_Cycles.begin(); it != m_Cycles.end(); it++, ++cycle ){
      indent( i, ostr ) << "<DATAPOOL cycle = \"" << cycle << "\">\n";
      if( *it != 0 ){
        (*it)->writeDP( ostr, i+2 );
      }
      ostr << "</DATAPOOL>" << std::endl;
    }
  }
  else{
    int cyc = cycle >= 0 ? cycle : m_ActualCycle;
    if( cyc < 0 || cyc >= (int)m_Cycles.size() ){
      ostr << "<DATAPOOL cycle = \"" << "none" << "\">" << std::endl;
    }
    else{
      indent( i, ostr ) << "<DATAPOOL cycle = \"" << cyc << "\">" << std::endl;
      m_Cycles[cyc]->writeDP( ostr, i+2 );
    }
    indent( i, ostr ) << "</DATAPOOL>" << std::endl;
  }
  ostr << "========== End of Datapool ==========" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* writeDD --                                                                  */
/* --------------------------------------------------------------------------- */

void DataPool::writeDD( std::ostream &ostr, int i ){
  ostr << "========== Begin of Dictionary ==========" << std::endl;
  m_DataDict.writeDD( ostr, i );
  ostr << "========== End of Dictionary ==========" << std::endl;
}


/*********************************************************************/
/* DynamicDatapoolAlloc                                              */
/*********************************************************************/

bool DataPool::DynamicDatapoolAlloc(DataDictionary * pDict)
{
  bool rslt = false;
  LOG_DEBUG("Name=" << pDict->getName()
          );

  assert( DataDictionary::IsValidType( pDict->getDataType() ) );
  assert( pDict->GetParent() != 0 );

  if( !DataPoolAllocated() ){
    /*****************************************/
    /* Dies ist kein Fehler, wir haben nur   */
    /* nichts zu tun. Also waren wir erfolg- */
    /* reich (auf die billige tour)          */
    /*****************************************/
    LOG_DEBUG("rslt true (not yet allocated)");
    return true;
  }

  if( !DataDictionary::TypeNeedsAlloc( pDict->getDataType() ) ){
    /*****************************************/
    /* gleicher Fall wie oben. Dies IST ein  */
    /* erfolg fuer diese Funktion.           */
    /*****************************************/
    LOG_DEBUG("rslt true (no Alloc needed\n");
    return true;
  }

  DataDictionaryModified(pDict,true);
  rslt = true;

  LOG_DEBUG("rslt=" << rslt);
  return rslt;
}


/*********************************************************************/
/* DynamicDatapoolDelete                                             */
/*********************************************************************/

bool DataPool::DynamicDatapoolDelete(DataDictionary * pDict)
{
  bool rslt = false;
  LOG_DEBUG("DynamicDatapoolDelete: Name=" << pDict->getName() );

  assert( DataDictionary::IsValidType(pDict->getDataType() ) );
  assert( DataDictionary::TypeNeedsAlloc(pDict->getDataType() ) );
  assert( pDict->GetParent() != 0 );

  if( DataPoolAllocated() ){
    DataDictionaryModified( pDict, false );
  }

  pDict->MarkNodeDeleted();
  rslt = true;

  LOG_DEBUG( "rslt=" << rslt);
  return rslt;
}

/*********************************************************************/
/* DataDictionaryModified                                            */
/*********************************************************************/

void DataPool::DataDictionaryModified(DataDictionary *pDict ,bool AddOrDelete)
{
  LOG_DEBUG("Name=" << pDict->getName() << " AddOrDelete=" << AddOrDelete );

  if( m_ttrail.inTransaction() ) assert( false );

  // CommitDataPoolTransaction();    // Das machts uns etwas leichter
  CycleList::iterator it;
  bool FirstCycle = true;
  DataItem *pFirstCycleItem = 0;

  for(it = m_Cycles.begin(); it != m_Cycles.end(); it++) {
    if (*it != 0){
      (*it)->DataDictionaryModified( pDict,AddOrDelete,false,FirstCycle,&pFirstCycleItem);
      FirstCycle = false;
    }
  }
  return;
}

/* --------------------------------------------------------------------------- */
/* setDataEvents --                                                            */
/* --------------------------------------------------------------------------- */

void DataPool::setDataEvents( DataEvents *e ){
  assert( e != 0 );
  if( e != 0 ){
    delete s_events;
  }
  s_events = e;
}

/* --------------------------------------------------------------------------- */
/* getDataEvents --                                                            */
/* --------------------------------------------------------------------------- */

DataEvents &DataPool::getDataEvents(){
  assert( s_events != 0 );
  return *s_events;
}

/* --------------------------------------------------------------------------- */
/* showMemoryUsage --                                                          */
/* --------------------------------------------------------------------------- */

void DataPool::showMemoryUsage( std::ostream &ostr ){
#if defined _DEBUG
  bool show_counts = true;
  int obj = 0;
  obj += BUG_SHOW_COUNT( DataDictionary    , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataReference     , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataItem          , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataItemContainer , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataDimension     , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataElement       , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataAlterFunctor  , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataValue         , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataVector        , ostr, show_counts );
  obj += BUG_SHOW_COUNT( DataMatrix        , ostr, show_counts );
  ostr << "==> Total: " << obj << " Data-Objects in DataPool allocated <==" << std::endl;
#endif
}
