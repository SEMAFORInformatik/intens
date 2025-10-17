
#include <assert.h>
#include <string>
#include <sstream>

#include "utils/utils.h"
#include "utils/Separator.h"
#include "datapool/DataItemContainer.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataItem.h"
#include "datapool/DataLogger.h"
#include "datapool/DataPool.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataItemContainer);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataItemContainer::DataItemContainer( DataItem *parent, DataDictionary &dict, bool update ){
  BUG_INCR_COUNT;
  BUG( BugRef, "Constructor" );

  bool rslt = buildItemContainer( parent, dict, update );
  assert(rslt);
}

DataItemContainer::DataItemContainer( const DataItemContainer &ref )
  : m_itemlist( ref.m_itemlist ){
  BUG_INCR_COUNT;
  BUG( BugRef, "Copy-Constructor" );

  // Der Container enthaelt Pointers auf DataItems. Diese DataItems müssen nun
  // auch kopiert werden.
  DataItemList::iterator it;
  for( it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( *it != 0 ){
      *it = (*it)->cloneItem();
    }
  }
}

DataItemContainer::~DataItemContainer(){
  BUG_DECR_COUNT;
  BUG( BugRef, "Destructor" );
  if( m_itemlist.capacity() != m_itemlist.size() ){
    std::cout << "FATAL: memory wasted in DataItemContainer:"
              << " capacity=" << m_itemlist.capacity()
              << ", size=" << m_itemlist.size()
              << std::endl;
  }

  deleteContainer();
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* GetItem --                                                                  */
/* --------------------------------------------------------------------------- */

DataItem *DataItemContainer::GetItem( int inx ) const{
  assert( inx >= 0 && inx < (int)m_itemlist.size() );
  return m_itemlist[inx];
}

/* --------------------------------------------------------------------------- */
/* getItem --                                                                  */
/* --------------------------------------------------------------------------- */

DataItem *DataItemContainer::getItem( const std::string &name ) const{
  for( DataItemList::const_iterator it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it) ){
      if( (*it)->getName() == name ){
        return *it;
      }
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getItemByDbAttr --                                                                  */
/* --------------------------------------------------------------------------- */

DataItem *DataItemContainer::getItemByDbAttr( const std::string &dbAttr ) const{
  for( DataItemList::const_iterator it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it) ){
      if( (*it)->getDbAttr() == dbAttr ){
        return *it;
      }
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* Item --                                                                     */
/* --------------------------------------------------------------------------- */

DataItem &DataItemContainer::Item( int inx ) const{
  DataItem *item = GetItem( inx );
  assert( item != 0 );
  return *item;
}

/* --------------------------------------------------------------------------- */
/* SetItem --                                                                  */
/* --------------------------------------------------------------------------- */

void DataItemContainer::SetItem( int inx, DataItem *item ){
  assert( inx >= 0 && inx < (int)m_itemlist.size() );
  if( m_itemlist[inx] != 0 ){
    // Das ist etwas seltsam !
    std::cout << "ALERT: DataItem '" << m_itemlist[inx]->getName() << "' deleted" << std::endl;
    delete m_itemlist[inx];
  }
  m_itemlist[inx] = item;
}

/* --------------------------------------------------------------------------- */
/* getNumberOfItems --                                                         */
/* --------------------------------------------------------------------------- */

int DataItemContainer::getNumberOfItems() const{
  return m_itemlist.size();
}

/* --------------------------------------------------------------------------- */
/* hasValidElements --                                                         */
/* --------------------------------------------------------------------------- */

bool DataItemContainer::hasValidElements( bool incl_itemattr ) const{
  for( DataItemList::const_iterator it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it)->hasValidElements( incl_itemattr ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasSerializableValidElements --                                             */
/* --------------------------------------------------------------------------- */

bool DataItemContainer::hasSerializableValidElements( const SerializableMask flags ) const{
  for( DataItemList::const_iterator it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it)->hasSerializableValidElements( flags ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* resizeItemList --                                                           */
/* --------------------------------------------------------------------------- */

void DataItemContainer::resizeItemList( int sz ){
  if( sz != getNumberOfItems() ){
    m_itemlist.reserve( sz );
    m_itemlist.resize( sz, 0 );
    if( m_itemlist.capacity() != m_itemlist.size() ){
      std::cout << "ALERT: Memory wasted in DataItemContainer" << std::endl;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* operator=  --                                                               */
/* --------------------------------------------------------------------------- */

DataItemContainer& DataItemContainer::operator=( const DataItemContainer &ref ){
  LOG_DEBUG("operator=");
  assert( m_itemlist.size() == ref.m_itemlist.size() );

  // Zuerst werden alle Membervariablen der Struktur gelöscht.
  deleteContainer();

  DataItemList::const_iterator iSrc = ref.m_itemlist.begin();
  // Und nun werden alle Source-Membervariablen gecloned.
  for( DataItemList::iterator iDest = m_itemlist.begin(); iDest != m_itemlist.end(); ++iDest, ++iSrc ){
    if( *iSrc != 0 ){
      *iDest = (*iSrc)->cloneItem();
    }
  }
  return *this;
}

/* --------------------------------------------------------------------------- */
/* buildItemContainer --                                                       */
/* --------------------------------------------------------------------------- */

bool DataItemContainer::buildItemContainer( DataItem *parent, DataDictionary &dict, bool update ){
  BUG( BugRef, "buildItemContainer" );

  assert( dict.getDataType() == DataDictionary::type_StructDefinition );

  DataDictionary *pStr = dict.getStructure();
  assert( pStr != 0 );
  int HighestIndex = pStr->GetHighestIndex();

  if( HighestIndex < 0 ){
    LOG_DEBUG("rslt=true (nothing to do)");
    return true;
  }

  resizeItemList( HighestIndex + 1 );

  // Jetzt werden die Items erstellt. Wenn es sich um Struktur-Items handelt,
  // werden auch diese rekursiv erstellt.

  while( pStr ){
    if( DataDictionary::TypeNeedsAlloc( pStr->getDataType() ) ){
      DataItem * pIt = new DataItem( parent, pStr, update );
      assert( pIt != 0 );

      int Index = pStr->GetItemIndex();
      assert( Index < (int)m_itemlist.size() );
      assert( Index >= 0 );
      assert( m_itemlist[Index] == 0 );
      assert( pIt != 0 );
      SetItem( Index, pIt );
    }
    pStr = pStr->getNext();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* DataDictionaryModified --                                                   */
/* --------------------------------------------------------------------------- */

void DataItemContainer::DataDictionaryModified(DataPool       &datapool
                                              ,DataDictionary *pThisDict
                                              ,DataDictionary *pDict
                                              ,bool            AddOrDelete
                                              ,bool            GlobalFlag
                                              ,bool            FirstCycle
                                              ,DataItem      **pFirstCycleItem
                                              ){
  LOG_DEBUG("Name=" << pDict->getName() << " AddOrDelete =" << AddOrDelete );
  DataItemList::iterator it;

  if ( pThisDict != pDict->GetParent() ){
    /**************************************************/
    /* Nichts zu tun hier. Nur weiterreichen.         */
    /**************************************************/
    for(it = m_itemlist.begin(); it != m_itemlist.end(); it++){
      if (*it != 0){
        (*it)->DataDictionaryModified( pDict,AddOrDelete,GlobalFlag,FirstCycle,pFirstCycleItem);
      }
    }
    LOG_DEBUG( "Not mine");
    return;
  }

  /****************************************************/
  /* Dies geht mich an. Jetzt muss ich tun, was zu tun*/
  /* ist.                                             */
  /****************************************************/

  int inx = pDict->GetItemIndex();

  if ( AddOrDelete ){
    /*************************************************/
    /* Add-Fall. Dies kann entweder bedeuten, dass   */
    /* das Item hinten angefuegt worden ist. In dem  */
    /* Fall muss ich den Vektor vergroessern, oder   */
    /* es kann ein Item zwischendurch neu ueber-     */
    /* schrieben worden sein. Dazu muss das Item     */
    /* den Type type_None haben aber das geht uns    */
    /* hier nichts an, da dieser Teil des Lunches    */
    /* schon gegessen worden ist!!!                  */
    /*                                               */
    /*************************************************/
    DataItem * pIt = 0;
    resizeItemList( inx + 1 );
    assert( GetItem(inx) == 0 );

    if ( pDict->ItemIsGlobal() && ! FirstCycle ) {
      /**************************************************/
      /* Hinzufuegen von globalen Items. Dies ist aber  */
      /* nicht der erste Cycle. Wir muessen also schon  */
      /* einen Cycle erstellt haben irgendwo.           */
      /* auf diesen kann via den Pointer pFirstCycleItem*/
      /* zugegriffen werden.                            */
      /**************************************************/
      if( !pDict->isParentNodeRoot() ){
        /***********************************************/
        /* Hinzufuegen von Struktur-Member mit dem     */
        /* global-flag ist momentan nicht unterstuetzt.*/
        /***********************************************/
        assert(false);
        LOG_DEBUG("Global already done");
        return;
      }
      assert(*pFirstCycleItem != 0);
      pIt = (*pFirstCycleItem)->cloneItem();
    }
    else {
      /**************************************************/
      /* Item Hinzufuegen.                              */
      /**************************************************/
      pIt = new DataItem( 0, pDict, false );
      if (pDict->ItemIsGlobal()) {
        assert(FirstCycle);
        assert(*pFirstCycleItem == 0);
        *pFirstCycleItem = pIt;
      }
    }
    assert(pIt != 0);
    SetItem(inx,pIt);
  }
  else {
    /*************************************************/
    /* Delete-Fall                                   */
    /* Wir muessen das Item nun loeschen. Eigentlich */
    /* kein Problem.                                 */
    /*************************************************/
    assert(GetItem(inx) != 0);
    DataItem::deleteItem(GetItem(inx));
    SetItem(inx,0);
  }
}

/* --------------------------------------------------------------------------- */
/* clearContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataItemContainer::UpdateStatus DataItemContainer::clearContainer( bool incl_itemattr ){
  BUG( BugRef, "clearContainer" );

  UpdateStatus status = NoUpdate;
  for( DataItemList::iterator i = m_itemlist.begin(); i != m_itemlist.end(); ++i ){
    if( *i != 0 ){
      status = maximum( status, (*i)->clearContainer( incl_itemattr ) );
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* clearContainerExceptSortKey --                                              */
/* --------------------------------------------------------------------------- */

void DataItemContainer::clearContainerExceptSortKey(){
  BUG( BugRef, "clearContainer" );

  for( DataItemList::iterator i = m_itemlist.begin(); i != m_itemlist.end(); ++i ){
    if( *i != 0 ){
      if( !(*i)->isSortKey() ){
        (*i)->clearContainer( false );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* eraseContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataItemContainer::UpdateStatus DataItemContainer::eraseContainer(){
  BUG( BugRef, "eraseContainer" );

  UpdateStatus status = NoUpdate;
  for( DataItemList::iterator i = m_itemlist.begin(); i != m_itemlist.end(); ++i ){
    if( *i != 0 ){
      status = maximum( status, (*i)->eraseContainer() );
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* deleteContainer --                                                          */
/* --------------------------------------------------------------------------- */

DataItemContainer::UpdateStatus DataItemContainer::deleteContainer(){
  BUG( BugRef, "deleteContainer" );

  UpdateStatus status = NoUpdate;
  for( DataItemList::iterator i = m_itemlist.begin(); i != m_itemlist.end(); ++i ){
    if( *i != 0 ){
      // Globale DataItems geben unter Umständen den Pointer zurück, da noch
      // andere Container einen Pointer darauf haben.
      *i = DataItem::deleteItem( *i );
      *i = 0;
      status = ValueUpdated;
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* clearCycle --                                                               */
/* --------------------------------------------------------------------------- */

void DataItemContainer::clearCycle( DataTTrail &ttrail ){
  int inx = 0;
  DataDictionary *dict = 0;

  for( DataItemList::iterator i = m_itemlist.begin(); i != m_itemlist.end(); ++i ){
    if( *i != 0 ){
      dict = (*i)->getDictionary();
      if( !(dict->ItemIsGlobal() || dict->ItemIsProtected()) ){
        // ACHTUNG: DataItem-Objekte mit den Attributen 'Global' oder 'Protected'
        // werden bei einem clear eines Cycles nicht angerührt.
        if( ttrail.inTransaction() ){
          ttrail.saveDataItem( this, inx );
        }

        (*i)->deleteContainer();
        // Nach einem werden die Timestamps initialisiert.
        (*i)->clearDataItemUpdated();
      }
    }
    inx++;
  }
}

/* --------------------------------------------------------------------------- */
/* assignContainer --                                                          */
/* --------------------------------------------------------------------------- */

DataItemContainer::UpdateStatus DataItemContainer::assignContainer(const DataItemContainer &cont ){
  DataItemList::iterator iDest;
  DataItemList::const_iterator iSrc = cont.m_itemlist.begin();
  UpdateStatus status = NoUpdate;

  // Und nun werden alle Source-Membervariablen gecloned.
  for( iDest = m_itemlist.begin(); iDest != m_itemlist.end(); ++iDest, ++iSrc ){
    assert( *iSrc != 0 );
    assert( *iDest != 0 );
    status = maximum( status, (*iDest)->assignDataItem( *iSrc ) );
  }

  return status;
}

/* --------------------------------------------------------------------------- */
/* assignCorrespondingDataItem --                                              */
/* --------------------------------------------------------------------------- */

DataItemContainer::UpdateStatus DataItemContainer::assignCorrespondingDataItem( const DataItemContainer &cont ){
  const DataItem *Src;
  DataItemList::const_iterator iDest;
  UpdateStatus status = NoUpdate;

  // Und nun werden alle korrespondierenden Source-Membervariablen gecloned.
  for( iDest = m_itemlist.begin(); iDest != m_itemlist.end(); ++iDest ){
    assert( *iDest != 0 );
    if( ( Src=cont.getCorrespondingDataItem( *(*iDest) ) ) != 0 ){
      status = maximum( status, (*iDest)->assignDataItem( Src ) );
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* getCorrespondingDataItem --                                                 */
/* --------------------------------------------------------------------------- */

const DataItem *DataItemContainer::getCorrespondingDataItem( const DataItem &dest ) const {
  DataItemList::const_iterator iSrc;
  for( iSrc = m_itemlist.begin(); iSrc != m_itemlist.end(); ++iSrc ){
    if( dest.dictionary().isSameDataTypeAndName( (*iSrc)->dictionary()) ){
      return (*iSrc);
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* clearCorrespondingDataItem --                                               */
/* --------------------------------------------------------------------------- */

DataItemContainer::UpdateStatus DataItemContainer::clearCorrespondingDataItem( const DataDictionary &var_dict ){
  LOG_DEBUG( "clearCorrespondingDataItem" );

  assert( var_dict.getDataType() == DataDictionary::type_StructVariable );
  DataDictionary *dict = var_dict.getStructure();
  DataDictionary *next = 0;
  UpdateStatus status = NoUpdate;

  DataItemList::iterator iDest;
  for( iDest = m_itemlist.begin(); iDest != m_itemlist.end(); ++iDest ){
    assert( *iDest != 0 );
    if( getCorrespondingDictionary( dict, (*iDest)->getDictionary() ) != 0 ){
      *iDest = DataItem::eraseItem( *iDest );
      status = ValueUpdated;
    }
  }
  return status;
}

/* --------------------------------------------------------------------------- */
/* getCorrespondingDictionary --                                               */
/* --------------------------------------------------------------------------- */

DataDictionary *DataItemContainer::getCorrespondingDictionary( DataDictionary *str
                                                             , DataDictionary *itm )
{
  assert( str->getDataType() == DataDictionary::type_StructDefinition );
  DataDictionary *d = str->getStructure();
  while( d != 0 ){
    if( itm->isSameDataTypeAndName( *d ) ){
      return d;
    }
    d = d->getNext();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataItemContainer::writeXML( std::ostream &ostr
                                  , std::vector<std::string> &attrs
                                  , int level
                                  , bool debug )
{
  if( debug ) indent( level, ostr );

  ostr << "<contents>\n";
  DataItemList::iterator it;
  assert( !m_itemlist.empty() );
  for( it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it) != 0 )
      (*it)->writeXML( ostr, attrs, 2 + level, debug );
  }
  if( debug ) indent( level, ostr );
  ostr << "</contents>\n";
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeJSON --                                                                */
/* --------------------------------------------------------------------------- */

bool DataItemContainer::writeJSON( std::ostream &ostr
                                 , int level
                                 , int indentation
                                 , const SerializableMask flags )
{
  int i = level * indentation;
  ostr << "{";
  Separator sep( 0, ',', 0 );

  DataItemList::iterator it;
  assert( !m_itemlist.empty() );
  for( it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it) != 0 && (*it)->hasSerializableValidElements( flags ) ){
      (*it)->writeJSON( ostr, level+1, indentation, sep, flags );
    }
  }

  ostr << std::endl;
  indent( i, ostr ) << "}";
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDP --                                                                  */
/* --------------------------------------------------------------------------- */

void DataItemContainer::writeDP( std::ostream &ostr, int i ) const{
  for( DataItemList::const_iterator it( m_itemlist.begin() ); it != m_itemlist.end(); ++it ){
    if( (*it) != 0 ){
      (*it)->writeDP( ostr, i );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataItemContainer::compare( const DataItemContainer &cont
                                            , int fall
                                            , int max_faelle )
{
  DataCompareResult rslt;
  int sz = getNumberOfItems();
  for( int i=0; i < sz; i++ ){
    rslt.setResult( Item(i).compare( cont.Item(i), fall, max_faelle ) );
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* setCompareAttributesOfAllElements --                                        */
/* --------------------------------------------------------------------------- */

bool DataItemContainer::setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle ){
  bool rslt = false;
  for( DataItemList::iterator it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it) != 0 ){
      if( (*it)->setCompareAttributesOfAllElements( mask, fall, max_faelle ) ){
        rslt = true;
      }
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* getSortKey --                                                               */
/* --------------------------------------------------------------------------- */

bool DataItemContainer::getSortKey( std::string &key ) const{
  key.clear();
  std::ostringstream valstream;
  Separator sep( 0, '-', 0 );
  std::string val;
  for( DataItemList::const_iterator it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it) != 0 ){
      if( (*it)->getSortKey( val ) ){
        valstream << sep << val;
      }
    }
  }
  key = valstream.str();
  return key.size() > 0;
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

void *DataItemContainer::getPython(){
#if defined(HAVE_PYTHON)
  PyObject *dict = PyDict_New();
  PyObject *key  = 0;
  PyObject *item = 0;
  for( DataItemList::const_iterator it = m_itemlist.begin(); it != m_itemlist.end(); ++it ){
    if( (*it) != 0 ){
#if PY_MAJOR_VERSION < 3
      key  = PyString_FromString( (*it)->getName().c_str() );
#else
      key  = PyUnicode_FromString( (*it)->getName().c_str() );
#endif
      item = (PyObject*)(*it)->getPython();
      assert( key != 0 );
      assert( item != 0 );
      PyDict_SetItem( dict, key, item );
    }
  }
  return dict;
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataItemContainer::UpdateStatus DataItemContainer::setPython(  const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  LOG_DEBUG("DataItemContainer::setPython");
  assert( obj.hasPyObject() );

  if( PyDict_Check( obj ) != 0 ) {
    DataItemContainer::UpdateStatus status = NoUpdate;
    Py_ssize_t sz = PyDict_Size( obj );
    Py_ssize_t x  = 0;
    PyObject *key = 0;
    PyObject *arg = 0;
    while( PyDict_Next( obj, &x, &key, &arg ) ){
#if PY_MAJOR_VERSION < 3
      if( PyString_Check( key ) ){
        std::string name( PyString_AsString( key ) );
#else
      if( PyUnicode_Check( key ) ){
        std::string name( PyUnicode_AsUTF8( key ) );
#endif
        LOG_DEBUG("DataItemContainer::setPython: search for " << name );
        DataItem *item = getItem( name );
        if( item != 0 ){
          status = maximum( item->setPython( arg ), status );
        }
        else{
          LOG_DEBUG("Item '" << name << "' not found");
        }
      }
      else{
        LOG_DEBUG("Itemname isnt a string");
      }
    }
    return status;
  }
  if( obj == Py_None ){
    LOG_DEBUG("empty");
  }
  else{
    LOG_DEBUG("DataItemContainer::setPython: Dict-Object or Py_None expected");
  }
#endif
  return NoUpdate;
}
