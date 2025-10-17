
#include "utils/utils.h"
#include "datapool/DataException.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataStructValue.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataItemContainer.h"
#include "datapool/DataItem.h"
#include "datapool/DataReference.h"
#include "datapool/DataAlterFunctor.h"
#include "datapool/DataInx.h"
#include "datapool/DataLogger.h"

#define __CLASSNAME__ "DataStructElement"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataStructElement::DataStructElement( DataItem *parent, DataDictionary &dict, bool update )
  : m_Value( parent, dict, update )
  , m_dict( dict ){
  LOG_DEBUG( "Name=" << dict.getName() );
}

DataStructElement::DataStructElement(const DataStructElement &ref)
  : DataElement( ref )
  , m_Value( ref.m_Value )
  , m_dict( ref.m_dict ){
    LOG_DEBUG("copying");
}

DataStructElement::~DataStructElement(){
  LOG_DEBUG( "destroyed");
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getElementType  --                                                          */
/* --------------------------------------------------------------------------- */

DataDictionary::DataType DataStructElement::getElementType() const{
  return DataDictionary::type_StructVariable;
}

/* --------------------------------------------------------------------------- */
/* operator=  --                                                               */
/* --------------------------------------------------------------------------- */

bool DataStructElement::operator=( const DataElement &ref ){
  LOG_DEBUG("assigning");
  assert( ref.isStructElement() );

  if( !ref.isStructElement() ){
    // Das zu kopierende Element ref ist nicht einmal eine Struktur.
    // Wie soll man sich das nur vorstellen ?!?
    return true;
  }

  if( !m_dict.isSameDataType( ref.getStructElement().m_dict ) ){
    // Ganz schlecht. Bei einer Struktur kennen wir kein Pardon. Sie
    // muessen das selbe Dictionary haben.
    assert( false );
    return true;
  }

  m_Value = ref.getStructElement().m_Value;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getStructElement --                                                         */
/* --------------------------------------------------------------------------- */

const DataStructElement &DataStructElement::getStructElement() const{
  return *this;
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */

DataContainer *DataStructElement::clone() const{
  return new DataStructElement( *this );
}

/* --------------------------------------------------------------------------- */
/* DataDictionaryModified --                                                   */
/* --------------------------------------------------------------------------- */

void DataStructElement::DataDictionaryModified( DataPool &datapool
                                              , DataDictionary *pDict
                                              , bool AddOrDelete
                                              , bool GlobalFlag
                                              , bool FirstCycle
                                              , DataItem **pFirstCycleItem )
{
  m_Value.DataDictionaryModified( datapool, &m_dict, pDict, AddOrDelete,GlobalFlag,FirstCycle,pFirstCycleItem);
}

/* --------------------------------------------------------------------------- */
/* getDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataValue *DataStructElement::getDataValue() const {
  return new DataStructValue( this, getAttributes() );
}

/* --------------------------------------------------------------------------- */
/* setDataValue --                                                             */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::setDataValue( DataValue *d ){
  if( d ){
    if( d->isValid() && d->getType() == type_StructVariable ){
      const DataStructElement *el = d->getStructValue().getStructElement();
      if( el == this ){
        // Eine Zuweisung auf sich selbst muss verhindert werden.
        return NoUpdate;
      }

      assignDataElement( *el );
      resetAttributes( d->getAttributes() );
      return ValueUpdated;
    }
  }

  return clearValue();
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataStructElement::isValid() const {
  return m_Value.hasValidElements( false );
}

/* --------------------------------------------------------------------------- */
/* hasValidElements --                                                         */
/* --------------------------------------------------------------------------- */

bool DataStructElement::hasValidElements( bool incl_itemattr ) const{
  return m_Value.hasValidElements( incl_itemattr );
}

/* --------------------------------------------------------------------------- */
/* hasSerializableValidElements --                                             */
/* --------------------------------------------------------------------------- */

bool DataStructElement::hasSerializableValidElements( const SerializableMask flags ) const{
  return m_Value.hasSerializableValidElements( flags );
}

/* --------------------------------------------------------------------------- */
/* item --                                                                     */
/* --------------------------------------------------------------------------- */

DataItem &DataStructElement::item( int index ) const{
  DataItem *item = m_Value.GetItem( index );
  assert( item != 0 );
  return *item;
}

/* --------------------------------------------------------------------------- */
/* getItem --                                                                  */
/* --------------------------------------------------------------------------- */

DataItem *DataStructElement::getItem( int index ) const{
  return m_Value.GetItem( index );
}

/* --------------------------------------------------------------------------- */
/* getItem --                                                                  */
/* --------------------------------------------------------------------------- */

DataItem *DataStructElement::getItem( const std::string &name ) const{
  return m_Value.getItem( name );
}

/* --------------------------------------------------------------------------- */
/* getItemByDbAttr --                                                          */
/* --------------------------------------------------------------------------- */

DataItem *DataStructElement::getItemByDbAttr( const std::string &dbAttr ) const{
  return m_Value.getItemByDbAttr( dbAttr );
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize --                                                  */
/* --------------------------------------------------------------------------- */

int DataStructElement::getAllDataDimensionSize( DataContainer::SizeList &dimsize,
						int dim_not_used,
						bool incl_attr ) const
{
  // Auf der Ebene DataElement müssen wir nur angeben, ob das Element gültige
  // Werte hat oder nicht. Ein StructElement ist dann gültig, wenn irgend eines
  // seiner DataItem's irgendwo einen gültigen Wert hat.
  BUG( BugRef, "GetAllDataDimensionSize" );

  int items = m_Value.getNumberOfItems();

  for( int i = 0; i < items; i++ ){
    DataContainer::SizeList dimsz;
    if( m_Value.GetItem(i)->getAllDataDimensionSize( dimsz, incl_attr ) > 0 ){
      for( int inx = 0; inx < (int)dimsz.size(); inx++ ){
        if( dimsz[inx] > 0 ){
          return 1;
        }
      }
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getValidDimensionSize --                                                    */
/* --------------------------------------------------------------------------- */

int DataStructElement::getValidDimensionSize( bool incl_itemattr ) const{
  return m_Value.hasValidElements( incl_itemattr ) ? 1 : 0;
}

/* --------------------------------------------------------------------------- */
/* clearValue --                                                               */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::clearValue(){
  return clearContainer( false );
}

/* --------------------------------------------------------------------------- */
/* clearElement --                                                             */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::clearElement() {
  return clearContainer( false );
}

/* --------------------------------------------------------------------------- */
/* clearContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::clearContainer( bool incl_itemattr ){
  BUG( BugRef, "clearContainer" );
  return m_Value.clearContainer( incl_itemattr );
}

/* --------------------------------------------------------------------------- */
/* clearContainerExceptSortKey --                                              */
/* --------------------------------------------------------------------------- */

void DataStructElement::clearContainerExceptSortKey(){
  m_Value.clearContainerExceptSortKey();
}

/* --------------------------------------------------------------------------- */
/* eraseContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::eraseContainer(){
  BUG( BugRef, "eraseContainer" );
  return m_Value.eraseContainer();
}

/* --------------------------------------------------------------------------- */
/* assignDataElement --                                                        */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::assignDataElement( const DataElement &el )
{
  if( el.getElementType() != DataDictionary::type_StructVariable ){
    // Das zu kopierende Element ref ist nicht einmal eine Struktur.
    // Wie soll man sich das nur vorstellen ?!?
    assert( false );
  }

  if( this != &el.getStructElement() ){
    if( !m_dict.isSameDataType( el.getStructElement().m_dict ) ){
      // Ganz schlecht. Bei einer Struktur kennen wir kein Pardon. Sie
      // muessen beide dasselbe Dictionary haben.
      assert( false );
    }
  }
  return m_Value.assignContainer( el.getStructElement().m_Value );
}

/* --------------------------------------------------------------------------- */
/* assignCorrespondingDataItem --                                              */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::assignCorrespondingDataItem( const DataElement &el ){
  LOG_DEBUG( "assignCorrespondingDataItem" );

  if( el.getElementType() != DataDictionary::type_StructVariable ){
    // Das zu kopierende Element ref ist nicht einmal eine Struktur.
    // Wie soll man sich das nur vorstellen ?!?
    assert( false );
  }

  return m_Value.assignCorrespondingDataItem( el.getStructElement().m_Value );
}

/* --------------------------------------------------------------------------- */
/* clearCorrespondingDataItem --                                               */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::clearCorrespondingDataItem( const DataDictionary &dict ){
  LOG_DEBUG( "clearCorrespondingDataItem" );

  return m_Value.clearCorrespondingDataItem( dict );
}

/* --------------------------------------------------------------------------- */
/* copyContainer --                                                            */
/* --------------------------------------------------------------------------- */

DataContainer *DataStructElement::copyContainer( DataPool &datapool ) const{
  DataStructElement *el = new DataStructElement( 0, m_dict, false );

  el->assignDataElement( *this );
  return el;
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStructElement::writeXML( std::ostream &ostr
                                , std::vector<std::string> &attrs
                                , bool is_structitem
                                , int level
                                , bool debug
                                , bool isScalar )
{
  LOG_DEBUG(  "writing");
  return m_Value.writeXML( ostr, attrs, level, debug );
}

/* --------------------------------------------------------------------------- */
/* writeJSON --                                                                */
/* --------------------------------------------------------------------------- */

bool DataStructElement::writeJSON( std::ostream &ostr,
                                   bool is_structitem,
                                   int level,
                                   int indentation,
                                   bool isScalar,
                                   double scale,
                                   const SerializableMask flags )
{
  return m_Value.writeJSON( ostr,
                            level,
                            indentation,
                            flags );
}

/* --------------------------------------------------------------------------- */
/* writeDP --                                                                  */
/* --------------------------------------------------------------------------- */

void DataStructElement::writeDP( std::ostream &ostr, int i, int index ) const{
  indent( i, ostr ) << "<EL";
  if( index >= 0 ){
    ostr << " index=\"" << index << "\"";
  }
  writeDPattributes( ostr );
  ostr << ">" << std::endl;
  m_Value.writeDP( ostr, i + 2 );
  indent( i, ostr ) << "</EL>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::alterData( DataItem &previous_item
                                                      , DataAlterFunctor &func
                                                      , DataInx &index )
{
  BUG( BugRef, "alterData" );

  UpdateStatus status = NoUpdate;

  func.alterStructData( *this );

  if( index.isLastLevel() ){
    status = func.alterData( *this );

    // Wir sind auf dem letzten spezifizierten Level. Auf Wunsch hangeln
    // wir uns durch alle Items der Struktur.
    index.freezeLastLevel();
    for( int iteminx = 0;
	 iteminx < m_Value.getNumberOfItems() && func.allStructItems();
	 iteminx++ ){
      DataItem &item = m_Value.Item( iteminx );
      status = maximum( status, item.alterData( func, index ) );
    }

    if( func.updateElements() ){
      markDataContainerUpdated( status );
    }
  }
  else{
    // Der Parameter numinx ist in diesem Fall der Item-Index. Der erste Eintrag
    // in der Indexliste enthält die Anzahl der Indizes.
    index.nextLevel();
    int iteminx = index.getItemIdnr();
    if( iteminx < 0 ){
      std::cerr << index.print() << std::endl;
      ThrowDpException( "alterData", "invalid Item-index " << iteminx << " from DataInx-Object" );
    }

    DataItem &item = m_Value.Item( iteminx );
    BUG_MSG( "Item #" << iteminx << " = " << item.getName() );
    status = item.alterData( func, index );
    if( func.updateElements() ){
      markDataContainerUpdated( status );
      item.markDataItemUpdated( status );
    }
  }

  BUG_EXIT( "status = " << status );
  return status;
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataStructElement::compare( const DataContainer &cont
                                            , int fall
                                            , int max_faelle )
{
  const DataElement *el = cont.getDataElement();
  assert( el );
  assert( el->getContainerType() == getContainerType() );
  assert( el->isStructElement() );
  const DataStructElement &struct_el = el->getStructElement();
  if( !m_dict.isSameDataType( struct_el.m_dict ) ){
    assert( false );
  }

  DataCompareResult rslt( m_Value.compare( struct_el.m_Value, fall, max_faelle ) );
  setCompareAttribute( rslt.getResult() );
  return rslt.getResult();
}

/* --------------------------------------------------------------------------- */
/* setCompareAttributesOfAllElements --                                        */
/* --------------------------------------------------------------------------- */

bool DataStructElement::setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle ){
  bool rslt = false;
  setCompareAttribute( mask );
  if( m_Value.setCompareAttributesOfAllElements( mask, fall, max_faelle ) ){
    rslt = true;
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* setCompareAttribute --                                                      */
/* --------------------------------------------------------------------------- */

void DataStructElement::setCompareAttribute( DATAAttributeMask mask ){
  resetAttribute( DATAisEqual | DATAisDifferent | DATAisLeftOnly | DATAisRightOnly );
  setAttribute( mask );
}

/* --------------------------------------------------------------------------- */
/* getSortKey --                                                               */
/* --------------------------------------------------------------------------- */

bool DataStructElement::getSortKey( std::string &key ) const{
  return m_Value.getSortKey( key );
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

void *DataStructElement::getPython(){
#if defined(HAVE_PYTHON)
  return m_Value.getPython();
#else
  return 0;
#endif
}

/* --------------------------------------------------------------------------- */
/* getPython --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataStructElement::setPython( const PythonObject &obj ){
#if defined(HAVE_PYTHON)
  LOG_DEBUG("DataStructElement");
  return m_Value.setPython( obj );
#endif
  return NoUpdate;
}
