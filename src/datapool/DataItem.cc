#include <sstream>

#include "utils/utils.h"
#include "utils/Separator.h"
#include "utils/Debugger.h"
#include "datapool/DataException.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataItemAttr.h"
#include "datapool/DataContainer.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataIntegerElement.h"
#include "datapool/DataRealElement.h"
#include "datapool/DataStringElement.h"
#include "datapool/DataComplexElement.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataDimension.h"
#include "datapool/DataReference.h"
#include "datapool/DataVector.h"
#include "datapool/DataAlterFunctor.h"
#include "datapool/DataInx.h"
#include "app/DataPoolIntens.h"
#include "gui/UnitManager.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

#define __CLASSNAME__ "DataItem"
INIT_LOGGER();

/*=============================================================================*/
/* Initialisation							       */
/*=============================================================================*/

BUG_INIT_COUNT(DataItem);

/*=============================================================================*/
/* Constructor / Destructor						       */
/*=============================================================================*/

DataItem::DataItem( DataItem *parent, DataDictionary *dict, bool update )
  : m_ReferenceCounter( 0 )
  , m_dict( *dict )
  , m_container( 0 )
  , m_DataLastUpdated( 0 )
  , m_ValueLastUpdated( 0 )
  , m_FullUpdated( 0 )
  , m_LastRollback( 0 )
  , m_pDataItemAttr( 0 ){
  BUG_INCR_COUNT;
  BUG_PARA( BugRef, "Constructor"
	    , "@" << this << ": Name=" << dictionary().getName() << ", update=" << BugBool(update) );

  resetAttributes( dictionary().GetAttr()->getAttributes() );

  if( parent != 0 ){
    DataAttributes par_attr( parent->getAttributes() );
    // Wir erben das editable-Attribut vom Parent-DataItem (#729/24.8.2017/bh)
    if( par_attr.isAttributeSet( DATAeditable ) ){
      setAttribute( DATAeditable );
    }
  }

  // Der erste leere Container wird nicht mehr hier, sondern auf Anfrage
  // zu einem späteren Zeitpunkt erstellt.
  // -- setContainer( newContainer() );

  if( update ){
    markDataItemFullUpdated();
  }
  else{
    // Dies ist nur für das GUI für Updates von Bedeutung, da in seltenen
    // Fällen der Hinweis auf eine Veränderung fehlt. (17.3.2017/hob)
    markDataItemRollback();
  }

  dictionary().GetAttr()->fixupAttributes( *this );
  BUG_EXIT( "created: " << dictionary().getName() );
}

DataItem::DataItem( const DataItem &item ) // Copy Constructor
  : DataAttributes( item )
  , m_ReferenceCounter( 0 )
  , m_dict( item.m_dict )
  , m_container( 0 )
  , m_DataLastUpdated( item.m_DataLastUpdated )
  , m_ValueLastUpdated( item.m_ValueLastUpdated )
  , m_FullUpdated( item.m_FullUpdated )
  , m_LastRollback( item.m_LastRollback )
  , m_pDataItemAttr( 0 ){
  BUG_INCR_COUNT;
  BUG( BugRef, "Copy-Constructor" );

  if( item.hasContainer() ){
    setContainer( item.container().clone() );
  }

  if( item.m_pDataItemAttr != 0 ){
    m_pDataItemAttr = item.m_pDataItemAttr->Clone();
    assert(m_pDataItemAttr != 0);
  }
  BUG_EXIT( "created: " << dictionary().getName() );
}

DataItem::~DataItem(){
  BUG_DECR_COUNT;
  BUG_PARA( BugRef, "Destructor", dictionary().getName() );

  // Für ein globales Objekt mit ReferenceCounter > 0
  // darf kein 'delete' durchgeführt werden.
  assert( m_ReferenceCounter == 0 );

  deleteContainer();
  if( m_pDataItemAttr != 0 ){
    delete m_pDataItemAttr;
    m_pDataItemAttr = 0;
  }
}

/*=============================================================================*/
/* member functions							       */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getDictionary --							       */
/* --------------------------------------------------------------------------- */

DataDictionary *DataItem::getDictionary() const{
  return &m_dict;
}

/* --------------------------------------------------------------------------- */
/* dictionary --							       */
/* --------------------------------------------------------------------------- */

DataDictionary &DataItem::dictionary() const{
  return m_dict;
}

/* --------------------------------------------------------------------------- */
/* datapool--								       */
/* --------------------------------------------------------------------------- */

DataPool &DataItem::datapool() const{
  return dictionary().datapool();
}

/* --------------------------------------------------------------------------- */
/* getDataType --							       */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataItem::getDataType() const{
  return dictionary().getDataType();
}

/* --------------------------------------------------------------------------- */
/* container --								       */
/* --------------------------------------------------------------------------- */

DataContainer &DataItem::container(){
  BUG_SILENT( BugRef, "container" );
  if( m_container == 0 ){
    setContainer( newContainer( false ) );
    BUG_MSG( "===== new container created =====" );
  }
  return *m_container;
}

/* --------------------------------------------------------------------------- */
/* container --								       */
/* --------------------------------------------------------------------------- */

const DataContainer &DataItem::container() const{
  assert( m_container != 0 );
  return *m_container;
}

/* --------------------------------------------------------------------------- */
/* setContainer --							       */
/* --------------------------------------------------------------------------- */

void DataItem::setContainer( DataContainer *c, bool delete_old ){
  assert( c );

  BUG_PARA( BugRef, "setContainer"
	    , "type=" << c->getContainerType() << ", delete_old=" << BugBool(delete_old) );

  if( delete_old ){
    BUG_MSG( "delete old container" );
    deleteContainer();
  }
  BUG_EXIT( "installed" );
  m_container = c;
}

/* --------------------------------------------------------------------------- */
/* hasContainer --							       */
/* --------------------------------------------------------------------------- */

bool DataItem::hasContainer() const{
  return m_container != 0;
}

/* --------------------------------------------------------------------------- */
/* newContainer --							       */
/* --------------------------------------------------------------------------- */

DataContainer *DataItem::newContainer( bool update ){
  BUG_PARA( BugRef,"newContainer", "Name=" << dictionary().getName() );

  assert( DataDictionary::TypeNeedsAlloc( getDataType() ) );
  DataContainer *cont = 0;

  switch( getDataType() ){
  case DataDictionary::type_Integer:
    cont = new DataIntegerElement;
    break;

  case DataDictionary::type_Real:
    cont = new DataRealElement;
    break;

  case DataDictionary::type_String:
    cont = new DataStringElement;
    break;

  case DataDictionary::type_Complex:
    cont = new DataComplexElement;
    break;

  case DataDictionary::type_CharData:
    cont = new DataStringElement(DataDictionary::type_CharData);
    break;

  case DataDictionary::type_StructVariable:
    assert( dictionary().hasStructureNode() );
    cont = new DataStructElement( this, dictionary().structure(), update );
    break;

  default:
    assert(false);  // Darf nicht passieren
    break;
  }
  assert( cont != 0 );

  if( update ){
    if( !hasContainer() ){
      // An diesem Item befinden sich noch keine Datenelemente. Beim Anlegen
      // des ersten ungueltigen Elements setzen wir den Timestamp nicht, da
      // auch vorher keine Daten vorhanden waren.
    }
    else{
      cont->markDataContainerDataUpdated( *this );
    }
  }

  return cont;
}

/* --------------------------------------------------------------------------- */
/* element --								       */
/* --------------------------------------------------------------------------- */

DataElement &DataItem::element(){
  assert( container().getContainerType() == DataContainer::ContainerIsElement );
  return static_cast<DataElement &>( container() );
}

/* --------------------------------------------------------------------------- */
/* dimension --								       */
/* --------------------------------------------------------------------------- */

DataDimension &DataItem::dimension(){
  assert( container().getContainerType() == DataContainer::ContainerIsDimension );
  return static_cast<DataDimension &>( container() );
}

/* --------------------------------------------------------------------------- */
/* getName --								       */
/* --------------------------------------------------------------------------- */

std::string DataItem::getName() const{
  return dictionary().getName();
}

/* --------------------------------------------------------------------------- */
/* getDbAttr --								                                                 */
/* --------------------------------------------------------------------------- */

std::string DataItem::getDbAttr() const{
  const UserAttr *attr = dynamic_cast<const UserAttr *>(getDataUserAttr());
  if(!attr) {
    return "";
  }
  return attr->DbAttr();
}

/* --------------------------------------------------------------------------- */
/* getDbUnit --								                                                 */
/* --------------------------------------------------------------------------- */

std::string DataItem::getDbUnit() const{
  const UserAttr *attr = dynamic_cast<const UserAttr *>(getDataUserAttr());
  if(!attr) {
    return "";
  }
  return attr->DbUnit();
}

/* --------------------------------------------------------------------------- */
/* markDataItemUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::markDataItemUpdated( UpdateStatus status ){
  switch( status ){
  case NoUpdate:
    break;
  case DataUpdated:
    setDataItemDataUpdated( DataPool::getTransactionNumber() );
    break;
  case ValueUpdated:
    setDataItemValueUpdated( DataPool::getTransactionNumber() );
    break;
  default:
    assert( false ); // FATAL
  }
}

/* --------------------------------------------------------------------------- */
/* clearDataItemUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::clearDataItemUpdated(){
  m_FullUpdated	     = 0;
  m_ValueLastUpdated = 0;
  m_DataLastUpdated  = 0;
}

/* --------------------------------------------------------------------------- */
/* markDataItemDataUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::markDataItemDataUpdated( bool yes ){
  setDataItemDataUpdated( yes ? DataPool::getTransactionNumber() : 0 );
}

/* --------------------------------------------------------------------------- */
/* markDataItemValueUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::markDataItemValueUpdated( bool yes ){
  setDataItemValueUpdated( yes ? DataPool::getTransactionNumber() : 0 );
}

/* --------------------------------------------------------------------------- */
/* isDataItemDataUpdated --						       */
/* --------------------------------------------------------------------------- */

bool DataItem::isDataItemDataUpdated( TransactionNumber t ) const{
  return m_DataLastUpdated >= t;
}

/* --------------------------------------------------------------------------- */
/* isDataItemValueUpdated --						       */
/* --------------------------------------------------------------------------- */

bool DataItem::isDataItemValueUpdated( TransactionNumber t ) const{
  return m_ValueLastUpdated >= t;
}

/* --------------------------------------------------------------------------- */
/* markDataItemFullUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::markDataItemFullUpdated(){
  m_FullUpdated = DataPool::getTransactionNumber();
  m_DataLastUpdated  = m_FullUpdated;
  m_ValueLastUpdated = m_FullUpdated;
}

/* --------------------------------------------------------------------------- */
/* markDataItemRollback --						       */
/* --------------------------------------------------------------------------- */

void DataItem::markDataItemRollback(){
  m_LastRollback = DataPool::getTransactionNumber();
}

/* --------------------------------------------------------------------------- */
/* isDataItemFullUpdated --						       */
/* --------------------------------------------------------------------------- */

bool DataItem::isDataItemFullUpdated( TransactionNumber t ) const{
  return m_FullUpdated >= t;
}

/* --------------------------------------------------------------------------- */
/* isDataItemGuiUpdated --						       */
/* --------------------------------------------------------------------------- */

bool DataItem::isDataItemGuiUpdated( TransactionNumber t ) const{
  return getDataItemGuiUpdated() >= t;
}

/* --------------------------------------------------------------------------- */
/* setDataItemDataUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::setDataItemDataUpdated( TransactionNumber t ){
  m_DataLastUpdated = t;
}

/* --------------------------------------------------------------------------- */
/* setDataItemValueUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::setDataItemValueUpdated( TransactionNumber t ){
  m_ValueLastUpdated = t;
  // Der Wert (Value) ist Teil der Daten (Data). In diesem Fall werden daher
  // beide Timestamps neu gesetzt.
  m_DataLastUpdated = m_ValueLastUpdated;
}

/* --------------------------------------------------------------------------- */
/* setDataItemFullUpdated --						       */
/* --------------------------------------------------------------------------- */

void DataItem::setDataItemFullUpdated( TransactionNumber t ){
  m_FullUpdated = t;
}

/* --------------------------------------------------------------------------- */
/* getDataItemDataUpdated --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataItem::getDataItemDataUpdated() const {
  return m_DataLastUpdated;
}

/* --------------------------------------------------------------------------- */
/* getDataItemValueUpdated --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataItem::getDataItemValueUpdated() const {
  return m_ValueLastUpdated;
}

/* --------------------------------------------------------------------------- */
/* getDataItemFullUpdated --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataItem::getDataItemFullUpdated() const {
  return m_FullUpdated;
}

/* --------------------------------------------------------------------------- */
/* getDataItemGuiUpdated --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataItem::getDataItemGuiUpdated() const {
  return maximum( m_DataLastUpdated, m_FullUpdated, m_LastRollback );
}

/* --------------------------------------------------------------------------- */
/* getLastRollbackTimestamp --						       */
/* --------------------------------------------------------------------------- */

TransactionNumber DataItem::getLastRollbackTimestamp() const {
  return m_LastRollback;
}

/* --------------------------------------------------------------------------- */
/* deleteItem --							       */
/* --------------------------------------------------------------------------- */

DataItem *DataItem::deleteItem( DataItem * pItem ){
  BUG( BugRef, "deleteItem" );

  if( pItem->m_ReferenceCounter > 0 ){
    pItem->m_ReferenceCounter--;
    BUG_EXIT( "Item " << pItem->getName() <<
              " not deleted: ReferenceCounter = " << pItem->m_ReferenceCounter );
    return pItem;
  }
  BUG_EXIT( "Item " << pItem->getName() << " deleted" );
  delete pItem;
  return 0;
}

/* --------------------------------------------------------------------------- */
/* eraseItem --								       */
/* --------------------------------------------------------------------------- */

DataItem *DataItem::eraseItem( DataItem * pItem ){
  BUG_PARA( BugRef, "eraseItem", pItem->getName() );

  pItem->deleteContainer();
  return pItem;
}

/* --------------------------------------------------------------------------- */
/* duplicateItem --							       */
/* --------------------------------------------------------------------------- */

DataItem *DataItem::duplicateItem(){
  BUG_DEBUG("duplicateItem()");
  // -------------------------------------------------------------------------
  // Diese Funktion wird vom TTrail benötigt. Es entsteht ein echtes Duplikat.
  // Die Funktion eignet sich nur zum Sichern von Daten. Im Gegensatz zu
  // cloneItem() werden auch globale Items kopiert.
  // -------------------------------------------------------------------------
  return new DataItem( *this );
}

/* --------------------------------------------------------------------------- */
/* cloneItem --								       */
/* --------------------------------------------------------------------------- */

DataItem* DataItem::cloneItem(){
  BUG_DEBUG( "cloneItem(): ReferenceCounter=" << m_ReferenceCounter <<
	     " ItemIsGlobal=" << dictionary().ItemIsGlobal() );

  if( dictionary().ItemIsGlobal() ){
    // -----------------------------------------------------------------
    // Globale DataItems werden nicht gecloned. Mit den ReferenceCounter
    // wird sichergestellt, dass immer nur ein Objekt existiert.
    // -----------------------------------------------------------------
    m_ReferenceCounter++;
    BUG_DEBUG("Inc RefCounter. New Counter = " << m_ReferenceCounter );
    return this;
  }

  DataItem * pTemp = new DataItem( *this );
  BUG_DEBUG("rslt=" << pTemp);
  return pTemp;
}

/* --------------------------------------------------------------------------- */
/* restoreItem --							       */
/* --------------------------------------------------------------------------- */

void DataItem::restoreItem( DataItem &item ){
  assert( getDictionary() == item.getDictionary() );

  exchange( m_container	       , item.m_container );
  exchange( m_pDataItemAttr    , item.m_pDataItemAttr );
  exchange( m_DataLastUpdated  , item.m_DataLastUpdated );
  exchange( m_ValueLastUpdated , item.m_ValueLastUpdated );
  exchange( m_FullUpdated      , item.m_FullUpdated );

  m_LastRollback = datapool().GetCurrentTransaction();
}

/* --------------------------------------------------------------------------- */
/* DataDictionaryModified --						       */
/* --------------------------------------------------------------------------- */

void DataItem::DataDictionaryModified( DataDictionary *pDict
				     , bool AddOrDelete
				     , bool GlobalFlag
				     , bool FirstCycle
				     , DataItem **pFirstCycleItem ){
  BUG_DEBUG( "DataDictionaryModified: AddOrDelete=" << AddOrDelete <<
	     " Name=" << pDict->getName() );

  assert( pDict != getDictionary() ); // Muss im DataItemContainer schon abgehandelt werden

  GlobalFlag |= dictionary().ItemIsGlobal();

  if( getDataType() == DataDictionary::type_StructVariable &&
      (GlobalFlag ? FirstCycle : true)			   ){
    /***********************************************************/
    /* Nur im Falle einer Struktur-Variablen muss ich in die   */
    /* Tiefe tauchen und dies auch nur bei nicht globalen oder */
    /* beim ersten globalen Item.			       */
    /***********************************************************/
    container().DataDictionaryModified( datapool()
				      , pDict
				      , AddOrDelete
				      , GlobalFlag
				      , FirstCycle
				      , pFirstCycleItem);
  }

  return;
}

/* --------------------------------------------------------------------------- */
/* getDataUserAttr --							       */
/* --------------------------------------------------------------------------- */

const DataUserAttr *DataItem::getDataUserAttr() const{
  return dictionary().getDataUserAttr();
}

/* --------------------------------------------------------------------------- */
/* isScalar --								       */
/* --------------------------------------------------------------------------- */

bool DataItem::isScalar() const{
  const DataUserAttr *attr = getDataUserAttr();
  if( attr == 0 ){
    return false;
  }
  return attr->IsScalar();
}

/* --------------------------------------------------------------------------- */
/* getDataItemAttr --							       */
/* --------------------------------------------------------------------------- */

DataItemAttr *DataItem::getDataItemAttr( bool createAttrIfNotThere ){
  if( createAttrIfNotThere && m_pDataItemAttr == 0 ){
    createItemAttr();
  }
  return m_pDataItemAttr;
}

/* --------------------------------------------------------------------------- */
/* hasItemAttr --							       */
/* --------------------------------------------------------------------------- */

bool DataItem::hasItemAttr() const{
  return m_pDataItemAttr != 0;
}

/* --------------------------------------------------------------------------- */
/* dataItemAttr --							       */
/* --------------------------------------------------------------------------- */

const DataItemAttr &DataItem::dataItemAttr() const{
  assert( m_pDataItemAttr != 0 );
  return *m_pDataItemAttr;
}

/* --------------------------------------------------------------------------- */
/* dataItemAttr --							       */
/* --------------------------------------------------------------------------- */

DataItemAttr &DataItem::dataItemAttr(){
  assert( m_pDataItemAttr != 0 );
  return *m_pDataItemAttr;
}

/* --------------------------------------------------------------------------- */
/* hasValidDataItemAttr --						       */
/* --------------------------------------------------------------------------- */

bool DataItem::hasValidDataItemAttr() const{
  if( m_pDataItemAttr != 0 ){
    if( m_pDataItemAttr->hasValidAttributes() ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* clearDataItemAttr --							       */
/* --------------------------------------------------------------------------- */

bool DataItem::clearDataItemAttr(){
  if( m_pDataItemAttr != 0 ){
    if( m_pDataItemAttr->hasValidAttributes() ){
      m_pDataItemAttr->clearValidAttributes();
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* createItemAttr --							       */
/* --------------------------------------------------------------------------- */

void DataItem::createItemAttr(){
  assert( m_pDataItemAttr == 0 );

  m_pDataItemAttr = datapool().GetDataItemAttr()->NewClass( getDictionary(), this );
  assert( m_pDataItemAttr != 0 );
}

/* --------------------------------------------------------------------------- */
/* createDimensions --							       */
/* --------------------------------------------------------------------------- */

void DataItem::createDimensions( int dims ){
  BUG_PARA( BugRef, "createDimensions", "Item=" << dictionary().getName() << ", dims=" << dims );
  if( dims > 0 ){
    BUG_MSG("Dimensions=" << container().getNumberOfDimensions() );
    while( container().getNumberOfDimensions() < dims ){
      setContainer( container().addNewDimension( *this ), false );
      BUG_MSG("new Dimensions=" << container().getNumberOfDimensions() );
    }
  }
  BUG_EXIT( "final Dimensions=" << container().getNumberOfDimensions() );
}

/* --------------------------------------------------------------------------- */
/* getNumberOfDimensions --						       */
/* --------------------------------------------------------------------------- */

int DataItem::getNumberOfDimensions() const{
  if( hasContainer() ){
    return container().getNumberOfDimensions();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataDimensionSizeOf --						       */
/* --------------------------------------------------------------------------- */

int DataItem::getDataDimensionSizeOf( int NumDim ) const{
  if( !hasContainer() ){
    return 0;
  }
  if( NumDim >= container().getNumberOfDimensions() ){
    return 1;
  }
  return container().getDataDimensionSizeOf( NumDim );
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize --						       */
/* --------------------------------------------------------------------------- */

int DataItem::getAllDataDimensionSize( DataContainer::SizeList &dimsize,
                                       bool incl_itemattr ) const
{
  BUG( BugRef, "GetAllDataDimensionSize" );

  dimsize.clear();
  int min_size = incl_itemattr && hasItemAttr() ? dataItemAttr().hasValidAttributes() : 0;
  int numDim = 0;

  if( !hasContainer() ){ // Es gibt keine Elemente
    dimsize.push_back( min_size );
    BUG_EXIT( "Keine Container vorhanden" );
    return 1;
  }

  numDim = getNumberOfDimensions();

  if( numDim == 0 ){
    // Nur ein einzelnes Element ist vorhanden.
    numDim = 1;
    dimsize.push_back( maximum( min_size,
                                container().getAllDataDimensionSize( dimsize,
                                                                     0,
                                                                     incl_itemattr ) ) );
  }
  else{
    for( int i=0; i < numDim; i++ ){
      dimsize.push_back( min_size );
    }
    container().getAllDataDimensionSize( dimsize, 0, incl_itemattr );
  }

  if( numDim > 1 && dimsize[0] == 1 ){
    // ----------------------------------------------------------------------
    // Führende Einer werden eliminiert. Die physische Anzahl von Dimensionen
    // interessiert uns nicht.
    // ----------------------------------------------------------------------
    int i;
    DataContainer::SizeList::iterator it = dimsize.begin();
    for( i=1; i < numDim-1 && dimsize[i] == 1; i++ ){
      ++it;
    }
    ++it;
    numDim -= i;
    dimsize.erase( dimsize.begin(), it );
    assert( dimsize.size() == numDim );
  }

  BUG_EXIT( "Dimensions=" << numDim );
  return numDim;
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize_PreInx --					       */
/* --------------------------------------------------------------------------- */

int DataItem::getAllDataDimensionSize_PreInx( DataContainer::SizeList &dimsize
					    , DataInx &index ) const
{
  BUG( BugRef, "getAllDataDimensionSize_PreInx" );
  dimsize.clear();
  int num_wildcards  = index.remainingWildcards();

  if( !hasContainer() ){ // Es gibt keine Elemente
    dimsize.push_back( 0 );
    complete_head( dimsize, num_wildcards, 0 );
    BUG_EXIT( "Keine Container vorhanden" );
    return (int)dimsize.size();
  }

  int num_dimensions = maximum( 1, getNumberOfDimensions() );

  while( index.remainingIndicesInIndexList() > num_dimensions ){
    if( index.isWildcard() || index.getIndex() == 0 ){
      index.nextIndex();
    }
    else{
      // Der indizierte Bereich ist nicht vorhanden
      dimsize.push_back( 0 );
      complete_head( dimsize, num_wildcards, 0 );
      return (int)dimsize.size();
    }
  }

  if( index.remainingIndicesInIndexList() == 0 ){
    while( num_dimensions > index.remainingIndicesInIndexList() ){
      index.newFrontWildcard();
    }
  }
  else{
    while( num_dimensions > index.remainingIndicesInIndexList() ){
      index.newFrontIndex();
    }
  }

  bool no_wildcards  = index.hasntRemainingWildcards();

  assert( num_dimensions == index.remainingIndicesInIndexList() );

  DataContainer::SizeList wildcardlist;
  index.positionOfRemainingWildcards( wildcardlist );

  DataContainer::SizeList dimsizelist;
  dimsizelist.resize( num_dimensions, 0 );

  if( container().getAllDataDimensionSize( dimsizelist, 0, index ) == 0 ){
    dimsize.resize( 1, 0 );
    complete_head( dimsize, num_wildcards, 0 );
    return (int)dimsize.size();
  }

  if( no_wildcards ){
    // Mehr als 0 oder 1 ist nicht vorhanden, da wir genau einen Wert
    // beurteilt haben.
    dimsize.resize( 1, dimsizelist.at( 0 ) );
    return 1;
  }

  for( int i=0; i < (int)dimsizelist.size(); i++ ){
    if( wildcardlist.at( i ) == 1 ){
      dimsize.push_back( dimsizelist.at( i ) );
    }
  }

  complete_head( dimsize, num_wildcards, 1 );
  return (int)dimsize.size();
}

/* --------------------------------------------------------------------------- */
/* complete_head --							       */
/* --------------------------------------------------------------------------- */

void DataItem::complete_head( DataContainer::SizeList &dimsize, int wildcards, int val ) const{
  while( (int)dimsize.size() < wildcards ){
    dimsize.insert( dimsize.begin(), val );
  }
}

/* --------------------------------------------------------------------------- */
/* assignDataItem --							       */
/* --------------------------------------------------------------------------- */

DataItem::UpdateStatus DataItem::assignDataItem( const DataItem *pSource ){
  BUG_DEBUG("assignDataItem");

  bool old_data_is_valid = false;

  if( hasContainer() ){
    old_data_is_valid = container().hasValidElements( false );
  }

  UpdateStatus status = NoUpdate;

  if( pSource == 0 ){
    // Es ist kein DataItem zum Kopieren vorhanden. Wir löschen einen
    // eventuell vorhandenen DataContainer.
    status = deleteContainer();
  }
  else{
    // Dies ist der Normalfall: Ein DataItem mit DataContainer muss
    // kopiert werden.
    if( pSource->hasContainer() ){
      if( pSource->container().hasValidElements( false ) ){
	// Ein DataItem mit gültigen Daten muss	 kopiert werden.
	DataContainer *data = pSource->container().copyContainer( datapool() );
	setContainer( data );
	status = ValueUpdated;
      }
      else{
	if( !old_data_is_valid ){
	  // Source- und Destination-Daten sind nicht gültig. Es gibt
	  // nichts zu tun.
	  markDataItemFullUpdated();
	  return NoUpdate;
	}
	// Nachher gibt es keine gültigen Daten.
	status = deleteContainer();
      }
    }
    else{
      // Source invalid aber Destination war valid. Das wird nun geloescht
      if( old_data_is_valid ){
	status = deleteContainer();
      }
      else{ // TODO
	if(  pSource->hasContainer() )
	markDataItemFullUpdated();
      }
    }
  }

  if( status != NoUpdate ){
    markDataItemFullUpdated();
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* writeXML --								       */
/* --------------------------------------------------------------------------- */

bool DataItem::writeXML( std::ostream &ostr
		       , std::vector<std::string> &attrs
		       , int level
		       , bool debug )
{
  BUG_DEBUG("writeXML");

  if( !hasValidElements( false ) ){
    BUG_DEBUG(" - no valid elements");
    return false;
  }
  if( dictionary().isInternalName() ){
    BUG_DEBUG(" - internal name with '@'");
    return false;
  }

  bool rslt = false;
  int ind = debug ? level : 0;
  const std::string &name = dictionary().getName();
  bool is_structitem = getDataType() == DataDictionary::type_StructVariable;
  if( name == "Root" ){
    BUG_DEBUG(	"ROOT" );
    bool scalar = false;
    return container().writeXML( ostr, attrs, is_structitem, level, debug, scalar );
  }

  indent( ind, ostr ) << "<" << name;
  bool header = !attrs.empty();
  while( level == 0 && header ){
    std::string attr = attrs.back();
    if( attr.substr(0,5) == "xmlns" ||
	attr.substr(0,3)=="xsi"	    ||
	attr.substr(0,7)=="version" ){
      ostr << "\n" << attr;
      attrs.pop_back();
      header = !attrs.empty();
    }
    else{
      header=false;
    }
  }

  const DataUserAttr *attr = getDataUserAttr();
  if( !attrs.empty() ){
    if( attr != 0 ){
      attr->write( ostr, attrs );
    }
  }
  ostr << ">\n";

  bool scalar = false;
  // if( attr != 0 ){
  //   scalar = attr->IsScalar();
  // }

  rslt = container().writeXML( ostr, attrs, is_structitem, 2 + level, debug, scalar );
  indent( ind, ostr ) << "</" << name << ">\n";
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* isValid --								       */
/* --------------------------------------------------------------------------- */

bool DataItem::isValid() const {
  return hasContainer() && container().hasValidElements( false );
}

/* --------------------------------------------------------------------------- */
/* hasValidElements --							       */
/* --------------------------------------------------------------------------- */

bool DataItem::hasValidElements( bool incl_itemattr ) const{
  if( incl_itemattr ){
    if( hasValidDataItemAttr() ){
      return true;
    }
  }
  return hasContainer() && container().hasValidElements( incl_itemattr );
}

/* --------------------------------------------------------------------------- */
/* hasSerializableValidElements --					       */
/* --------------------------------------------------------------------------- */

bool DataItem::hasSerializableValidElements( const SerializableMask flags ) const{
  if( !hasContainer() ){
    // Keine Daten vorhanden
    return false;
  }

  const DataUserAttr *attr = getDataUserAttr();
  if( attr != 0 ){
    if( !attr->isSerializable( flags ) ){
      // Wir müssen keine gültigen Elemente suchen.
      // Das DataItem ist NICHT serializable.
      return false;
    }
  }

  return container().hasSerializableValidElements( flags );
}

/* --------------------------------------------------------------------------- */
/* writeJSON --								       */
/* --------------------------------------------------------------------------- */

bool DataItem::writeJSON( std::ostream &ostr,
                          int level,
                          int indentation,
                          Separator &sep,
                          const SerializableMask flags )
{
  BUG_DEBUG("Begin of DataItem::writeJSON");

  bool rslt = false;
  std::string name = dictionary().getName();
  double scale = 1;

  if(flags & HIDEtransient) {  // stream hides TRANSIENT items, use DBATTR if given
    std::string dbAttr = getDbAttr();
    if(!dbAttr.empty()) {  // DBATTR is given
      name = dbAttr;
    }
  }
  if(flags & APPLYdbUnit) {  // stream apply DBUNIT to real und integer values
    std::string dbUnit = getDbUnit();
    if(AppData::Instance().hasUnitManagerFeature() && !dbUnit.empty()) {  // DBUNIT is given
      UnitManager::Unit* db_unit = UnitManager::Instance().getUnitData(dbUnit);
      if (db_unit && db_unit->factor != 1.) {
        BUG_INFO("DbUnit(" << name << ") => (" << db_unit->factor << ")");
        db_unit->print();
        scale = db_unit->factor;
      }
    }
  }

  if( name.find_first_of( '@' ) != std::string::npos ){
    BUG_DEBUG("End of DataItem::writeJSON: Omit Items with @...");
    return false;
  }
  if( !hasContainer() ){
    BUG_DEBUG("End of DataItem::writeJSON: No Container");
    return false;
  }

  bool is_structitem = getDataType() == DataDictionary::type_StructVariable;
  if( name == "Root" ){
    BUG_DEBUG(	"ROOT" );
    bool scalar = false;
    rslt = container().writeJSON( ostr,
                                  is_structitem,
                                  level,
                                  indentation,
                                  scalar,
                                  scale,
                                  flags );
    BUG_DEBUG("End of DataItem::writeJSON: Root");
    return rslt;
  }

  int i = level * indentation;
  ostr << sep << std::endl;

  // comment this in case you want to see the item name
  // on lowest level:
  if( level > 0 ){
    indent( i, ostr ) << "\"" << name << "\": ";
  }

  bool scalar = false;
  const DataUserAttr *attr=getDataUserAttr();
  if( attr != 0 ){
    scalar = attr->IsScalar();
  }
  // !scalar (vectors) are always written as lists ([ ])
  // DataDimension writes [ ] for vectors
  // for DataElements, they are writen here
  if ( !scalar && container().getContainerType() == DataContainer::ContainerIsElement ) {
    ostr << "[" << std::endl;
    indent( i + indentation, ostr );
    level++;
  }
  rslt = container().writeJSON( ostr,
                                is_structitem,
                                level,
                                indentation,
                                scalar,
                                scale,
                                flags );
  if ( !scalar && container().getContainerType() == DataContainer::ContainerIsElement ) {
    ostr << std::endl;
    indent( i, ostr ) << "]";
  }
  BUG_DEBUG("End of DataItem::writeJSON");
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* writeDP --								       */
/* --------------------------------------------------------------------------- */

void DataItem::writeDP( std::ostream &ostr, int i ) const{
  indent( i, ostr ) << "<ITEM name=\"" << getName() << "\"";

#if !defined(__OMIT_DP_TIMESTAMPS__)
  if( getDataItemDataUpdated() > 0 ){
    ostr << " dat_ts=\"" << getDataItemDataUpdated() << "\"";
  }
  if( getDataItemValueUpdated() > 0 ){
    ostr << " val_ts=\"" << getDataItemValueUpdated() << "\"";
  }
  if( getDataItemFullUpdated() > 0 ){
    ostr << " full_ts=\"" << getDataItemFullUpdated() << "\"";
  }
  if( getDataItemGuiUpdated() > 0 ){
    ostr << " gui_ts=\"" << getDataItemGuiUpdated() << "\"";
  }
  if( getLastRollbackTimestamp() > 0 ){
    ostr << " rollback_ts=\"" << getLastRollbackTimestamp() << "\"";
  }
#endif
  if( hasItemAttr() ){
    dataItemAttr().writeDP( ostr );
  }
  else{
    ostr << " attr=\"none\"";
  }
  writeDPattributes( ostr );
  // Eine schlechte Idee bei Vergleich von Datapool-Output (8.6.2020/hob)
  // ostr << " this=\"" << this << "\"";
  if( hasContainer() ){
    ostr << ">" << std::endl;
    container().writeDP( ostr, i + 2, -1 );
    indent( i, ostr ) << "</ITEM>" << std::endl;
  }
  else{
    ostr << "/>" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* getAttributes --							       */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataItem::getAttributes( TransactionNumber t ){
  if( isDataItemGuiUpdated( t ) ){
    return DataAttributes::getAttributes() | DATAisGuiUpdated;
  }
  return DataAttributes::getAttributes();
}

/* --------------------------------------------------------------------------- */
/* getAttributes --							       */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataItem::getAttributes(){
  return DataAttributes::getAttributes();
}

/* --------------------------------------------------------------------------- */
/* clearContainer --							       */
/* --------------------------------------------------------------------------- */

DataItem::UpdateStatus DataItem::clearContainer( bool incl_itemattr ){
  if( incl_itemattr && hasItemAttr() ){
    dataItemAttr().clearValidAttributes();
  }

  if( !hasContainer() ){
    // Keine Daten vorhanden.
    return NoUpdate;
  }
  if( getDataType() != DataDictionary::type_StructVariable ){
    return eraseContainer();
  }
  UpdateStatus status = container().clearContainer( incl_itemattr );
  switch( status ){
  case DataUpdated:
    markDataItemDataUpdated();
    break;
  case ValueUpdated:
    markDataItemValueUpdated();
    break;
  default:
    break;
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* resizeContainer --							       */
/* --------------------------------------------------------------------------- */

void DataItem::resizeContainer( const DataItem &item ){
  // Wir stellen sicher das die Anzahl der Dimensionen mindestens so gross ist
  // wie beim Parameter-DataItem.
  createDimensions( item.getNumberOfDimensions() );
}

/* --------------------------------------------------------------------------- */
/* eraseContainer --							       */
/* --------------------------------------------------------------------------- */

DataItem::UpdateStatus DataItem::eraseContainer( bool incl_itemattr ){
  if( incl_itemattr && hasItemAttr() ){
    dataItemAttr().clearValidAttributes();
  }

  if( !hasContainer() ){
    // Keine Daten vorhanden.
    return NoUpdate;
  }
  // Felder mit dem Attribut Global oder Protected werden nur bei Cycle-Operationen
  // vor dem Löschen geschützt. (26.2.2016/bh)
  // ---------------------------------------------------------------------------
  //if( dictionary().ItemIsGlobal() || dictionary().ItemIsProtected() ){
  //  // Globale und geschützte Daten bleiben unangetastet.
  //  return NoUpdate;
  //}
  // ---------------------------------------------------------------------------
  return deleteContainer();
}

/* --------------------------------------------------------------------------- */
/* deleteContainer --							       */
/* --------------------------------------------------------------------------- */

DataItem::UpdateStatus DataItem::deleteContainer(){
  UpdateStatus status = NoUpdate;
  if( hasContainer() ){
    if( container().hasValidElements( false ) ){
      markDataItemFullUpdated();
      status = ValueUpdated;
    }
    else{
      // Das letzte gültige Element wurde bereits beim letzten Update des
      // DataItem's gelöscht. Wir setzen den 'fullUpdated'-Timestamp auf
      // den letzten 'dataUpdated'-Timestamp.
      m_FullUpdated = m_DataLastUpdated;
      markDataItemUpdated( DataUpdated );
      status = DataUpdated;
    }
    delete m_container;
    m_container = 0;
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* alterData --								       */
/* --------------------------------------------------------------------------- */

DataItem::UpdateStatus DataItem::alterData( DataAlterFunctor &func
					  , DataInx &inx )
{
  BUG_PARA( BugRef, "alterData", "@" << this << ": Name=" << dictionary().getName() );

  UpdateStatus status = NoUpdate;
  bool last_level = false;

  if( !hasContainer() ){
    // Keine Daten vorhanden.
    if( !func.createElements() ){
      // Es sollen auch keine Daten erstellt werden.
      // Das indizierte Element ist nicht vorhanden
      func.alterItem( *this, status, inx, true );
      if( func.updateElements() ){
	markDataItemUpdated( status );
      }
      BUG_EXIT( "Kein Container vorhanden und kein create befohlen" );
      return status;
    }
  }

  if( inx.isLastLevel() && inx.isIndexListEmpty() ){
    // Wenn auf dem letzten Level keine Indizes vorhanden sind, werden alle
    // Elemente über alle Dimensionen bearbeitet.
    BUG_MSG( "Kein Index auf dem letzten Level" );
  }
  else{
    // Führende Indizes mit Wert 0 werden überlesen.
    int num_inx	 = inx.getDimensions();
    int num_dims = container().getNumberOfDimensions();
    BUG_MSG( "Indices=" << num_inx << ", Dimensions=" << num_dims );

    bool ignore_leading_zeros = func.ignoreLeadingZeroIndices();
    if( !ignore_leading_zeros ){
      if( num_inx == 1 && inx.isZero() && !inx.isWildcard() && isScalar() ){
	ignore_leading_zeros = true;
      }
    }

    if( ignore_leading_zeros ){
      // Ein einzelner Index mit Wert 0 wird ignoriert.
      while( num_dims < num_inx && inx.isZero() ){
	num_inx--;
	if( !inx.nextIndex() ){
	  if( num_inx > 0 ){
	    BUG_FATAL( "ABORT: Indices=" << num_inx << ", Dimensions=" << num_dims );
	    ThrowDpException( "alterData", "Indices=" << num_inx << ", Dimensions=" << num_dims );
	  }
	}
      }
      BUG_MSG( "remaining indices: " << inx.printRemainingIndices() );
    }

    if( func.createElements() ){
      // Wir stellen sicher, dass die erforderliche Anzahl von Dimensionen
      // auch physisch vorhanden sind.
      createDimensions( num_inx );
    }
    else{
      if( num_dims < num_inx ){
	// Es sind mehr Dimensionen indiziert worden als vorhanden sind.
	// Falls keine Wildcards folgen, ist die Arbeit beendet.
	BUG_MSG( "Kein vorhandenes Element indiziert" );

	// Das indizierte Element ist nicht vorhanden
	func.alterItem( *this, status, inx, true );
	if( func.updateElements() ){
	  markDataItemUpdated( status );
	}
	BUG_EXIT( "indiziertes Element nicht vorhanden" );
	return status;
      }
    }
  }

  if( !func.alterItem( *this, status, inx ) ){
    if( func.updateElements() ){
      markDataItemUpdated( status );
    }
    BUG_EXIT( "work interrupted by functor" );
    return status;
  }

  switch( container().getContainerType() ){
  case DataContainer::ContainerIsDimension:
    BUG_MSG( "start of work thru dimensions" );
    if( func.startDimension( container(), inx ) ){
      status = container().alterData( *this, func, inx );
    }
    break;

  case DataContainer::ContainerIsElement:
    BUG_MSG( "start of work of single element" );
    if( (last_level=inx.isLastLevel()) ){
      func.startOneElement();
    }
    status = container().alterData( *this, func, inx );
    if( last_level ){
      func.endOneElement();
    }
    break;

  default:
    BUG_FATAL( "Ungültiger Container-Typ" );
    ThrowDpException( "alterData", "Ungültiger Container-Typ" );
  }

  if( func.updateElements() ){
    markDataItemUpdated( status );
  }

  BUG_EXIT( "status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* getDataElement --							       */
/* --------------------------------------------------------------------------- */

DataElement &DataItem::getDataElement( DataIndexList &inxlist ){
  BUG_DEBUG( "getDataElement: " << getName() << inxlist.indices() );

  // Führende Indizes mit Wert 0 werden überlesen.
  int num_inx = inxlist.getDimensions();
  int num_dims = container().getNumberOfDimensions();
  int index = 0;
  while( num_dims < num_inx && index < num_inx ){
    int inx = inxlist.index( index ).getIndex();
    if( inx == 0 ){
      index++;
    }
    else{
      num_dims = num_inx;
    }
  }
  return container().getDataElement( *this, inxlist, index );
}

/* --------------------------------------------------------------------------- */
/* getDataContainer --							       */
/* --------------------------------------------------------------------------- */

DataContainer &DataItem::getDataContainer( DataIndexList &inxlist, int sz ){
  int num_inx = inxlist.getDimensions();
  createDimensions( num_inx );
  return container().getDataContainer( *this, inxlist, 0, sz );
}

/* --------------------------------------------------------------------------- */
/* prepareCompareResult --						       */
/* --------------------------------------------------------------------------- */

void DataItem::prepareCompareResult( const DataElement &el_left ){
  eraseContainer();

  // Als Resultat-Struktur wird die linke Seite kopiert und mit der rechten
  // Seite verglichen. el_left wird dabei nicht mehr verwendet. Die Resultat-
  // Struktur wird laufend angepasst und enthalt am Schluss der Untersuchung
  // alle Informationen zum Auswerten.
  DataElement *el_rslt = container().getDataElement();
  assert( el_rslt );
  el_rslt->assignDataElement( el_left );
}

/* --------------------------------------------------------------------------- */
/* compare --								       */
/* --------------------------------------------------------------------------- */

DataCompareResult DataItem::compare( const DataElement &el_right
				   , int fall
				   , int max_faelle )
{
  assert( fall >= 1 );

  markDataItemValueUpdated();
  DataElement *el_rslt = container().getDataElement();
  assert( el_rslt != 0 );

  // Nun wird verglichen.
  DataCompareResult rslt( DataAttributes::getAttributes() );
  rslt.setResult( el_rslt->compare( el_right, fall, max_faelle ) );

  setCompareAttribute( rslt.getResult() );
  return rslt.getResult();
}

/* --------------------------------------------------------------------------- */
/* compare --								       */
/* --------------------------------------------------------------------------- */

DataCompareResult DataItem::compare( const DataItem &item
				   , int fall
				   , int max_faelle )
{
  // Bei einem StructItem kann das Abarbeiten der Elemente mit oder ohne Vorsortierung
  // geschehen. Hier wird es etwas aufwändiger.

  DataCompareResult rslt( DataAttributes::getAttributes() );
  markDataItemValueUpdated();

  if( item.hasContainer() ){
    // Make sure the number of dimensions is big enough
    createDimensions( item.getNumberOfDimensions() );
    DataContainer *cont = 0;

    if( getDataType() == type_StructVariable ){
      if( container().getNumberOfDimensions() == 1 && item.container().getNumberOfDimensions() == 1 ){
	// we have a simple vector: use the key which might connect elements to be compared
	if( dictionary().getStructure()->hasSortKey() ){
	  rslt.setResult( container().getDataDimension()
                      ->compareSorted(*item.container().getDataDimension(), fall, max_faelle ) );
	  setCompareAttribute( rslt.getResult() );
	  return rslt;
	}
      }
      cont = &container();
    }
    else{
      if( !container().isCompareDimension() ){
	// Diese Dimension dient nur dem Abbilden von verschiedenen Vergeichsfällen.
	// Jeder Index auf der untersten Ebene entspricht einem Fall.
	setContainer( container().addNewDimension( *this, max_faelle ), false );
	container().setCompareDimension();
      }
      // Wir kopieren die Werte dieses Falles in die Resultatstruktur und lassen sie
      // dort in Ruhe. Der Vergleich geht über das Original.
      container().getDataDimension()->setContainer( fall, item.container().clone() );
      cont = container().getDataDimension()->container( 0 );
    }

    rslt.setResult( cont->compare( item.container(), fall, max_faelle ) );
    setCompareAttribute( rslt.getResult() );
    return rslt;
  }
  else{
    if( !hasContainer() ){
      setCompareAttribute( DATAisEqual );
      return DATAisEqual; // no data on both sides
    }

    setCompareAttributesOfAllElements( DATAisLeftOnly, 0, max_faelle );
    return DATAisLeftOnly;
  }
}

/* --------------------------------------------------------------------------- */
/* setCompareAttributesOfAllElements --					       */
/* --------------------------------------------------------------------------- */

bool DataItem::setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle ){
  bool rslt = false;
  setCompareAttribute( mask );
  if( hasContainer() ){
    if( getDataType() != type_StructVariable ){
      if( !container().isCompareDimension() ){
	// Diese Dimension dient nur dem Abbilden von verschiedenen Vergeichsfällen.
	// Jeder Index auf der untersten Ebene entspricht einem Fall.
	setContainer( container().addNewDimension( *this, max_faelle ), false );
	container().setCompareDimension();
	if( fall != 0 ){
	  container().getDataDimension()->moveContainer( 0, fall, newContainer() );
	}
      }
    }
    if( container().setCompareAttributesOfAllElements( mask, fall, max_faelle ) ){
      rslt = true;
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* setCompareAttribute --						       */
/* --------------------------------------------------------------------------- */

void DataItem::setCompareAttribute( DATAAttributeMask mask ){
  resetAttribute( DATAisEqual | DATAisDifferent | DATAisLeftOnly | DATAisRightOnly );
  setAttribute( mask );
}

/* --------------------------------------------------------------------------- */
/* isSortKey --								       */
/* --------------------------------------------------------------------------- */

bool DataItem::isSortKey() const{
  return dictionary().isSortKey();
}

/* --------------------------------------------------------------------------- */
/* getSortKey --							       */
/* --------------------------------------------------------------------------- */

bool DataItem::getSortKey( std::string &key ) const{
  if( isSortKey() && hasContainer() ){
    assert( getDataType() != DataDictionary::type_StructVariable );

    const DataContainer *cont = &container();
    const DataContainer *comp_dim = 0;

    if( cont->getNumberOfDimensions() > 0 ){
      if( cont->isCompareDimension() ){
	// Wir müssen unter Umständen alle Einträge absuchen, da der Key nicht
	// überall vorhanden sein muss. Die vorhandenen SortKeys sind jedoch
	// alle gleich. Somit reicht uns der erste verfügbare Key.
	for( int inx=0; inx < cont->getNumberOfElements(); inx++ ){
	  if( cont->getDataDimension()->container( inx ) != 0 ){
	    if( getSortKey( cont->getDataDimension()->container( inx ), key ) ){
	      return true;
	    }
	  }
	}
	return false;
      }
    }

    return getSortKey( cont, key );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getSortKey --							       */
/* --------------------------------------------------------------------------- */

bool DataItem::getSortKey( const DataContainer *cont, std::string &key ) const{
  while( cont->getNumberOfDimensions() > 0 ){
    assert( cont != 0 );
    cont = cont->getDataDimension()->container( 0 );
  }
  assert( cont != 0 );
  const DataElement *el = cont->getDataElement();
  return el->getValue( key );
}

/* --------------------------------------------------------------------------- */
/* getPython --								       */
/* --------------------------------------------------------------------------- */

void *DataItem::getPython(){
#if defined(HAVE_PYTHON)
  PyObject *obj = 0;
  if( hasContainer() ){
    obj = (PyObject*)container().getPython();
  }
  else{
    obj = PythonObject::getEmptyObject();
  }
  assert( obj != 0 );
  return obj;
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* setPython --								       */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataItem::setPython( const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  assert( obj.hasPyObject() );

  if( obj.isEmptyObject() ){
    // bei leeren Objekten machen wir nichts. Der Wert bleibt ungültig.
    return NoUpdate;
  }

  DataElement::UpdateStatus status = deleteContainer();
  int dims = getNumberOfDimensions();
  assert( dims == 0 );
  dims = getPythonDimensions( obj );
  createDimensions( dims );

  return maximum( container().setPython( obj, *this, dims ), status );
#else
  return NoUpdate;
#endif
}

/* --------------------------------------------------------------------------- */
/* getPythonDimensions --						       */
/* --------------------------------------------------------------------------- */

int DataItem::getPythonDimensions( const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  if( PyList_Check( obj ) ){
    int dims = 0;
    Py_ssize_t sz = PyList_Size( obj );
    for( Py_ssize_t x = 0; x < sz; x++ ){
      PyObject *sub_obj = PyList_GetItem( obj, x );
      dims = maximum( dims, 1 + getPythonDimensions( sub_obj ) );
    }
    return dims;
  }

  if( PyTuple_Check( obj ) ){
    int dims = 0;
    Py_ssize_t sz = PyTuple_Size( obj );
    for( Py_ssize_t x = 0; x < sz; x++ ){
      PyObject *sub_obj = PyTuple_GetItem( obj, x );
      dims = maximum( dims, 1 + getPythonDimensions( sub_obj ) );
    }
    return dims;
  }
#endif
  return 0;
}
