
#include "datapool/DataContainer.h"
#include "datapool/DataDimension.h"
#include "datapool/DataItem.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataLogger.h"
#if defined(HAVE_PYTHON)
#include "python/PythonObject.h"
#endif

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataContainer::DataContainer()
  : m_DataLastUpdated( 0 )
  , m_ValueLastUpdated( 0 )
{
    LOG_DEBUG("(constructor)");
}

DataContainer::~DataContainer(){
  LOG_DEBUG("(destructor)");
}

DataContainer::DataContainer (const DataContainer &ref )
  : m_DataLastUpdated( ref.m_DataLastUpdated )
  , m_ValueLastUpdated( ref.m_ValueLastUpdated )
{
  LOG_DEBUG("(Copy-constructor)");
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* addNewDimension --                                                          */
/* --------------------------------------------------------------------------- */

DataContainer *DataContainer::addNewDimension( DataItem &item, int reserve ){
  DataDimension *pDim = new DataDimension( item, this );
  assert( pDim != 0 );
  pDim->setReserve( reserve );
  pDim->setContainer( this );

  return pDim;
}

/* --------------------------------------------------------------------------- */
/* markDataContainerDataUpdated --                                             */
/* --------------------------------------------------------------------------- */

void DataContainer::markDataContainerDataUpdated( DataItem &item ){
  setDataContainerDataUpdated( DataPool::getTransactionNumber() );
  item.markDataItemDataUpdated();
}

/* --------------------------------------------------------------------------- */
/* markDataContainerValueUpdated --                                            */
/* --------------------------------------------------------------------------- */

void DataContainer::markDataContainerValueUpdated( DataItem &item ){
  setDataContainerValueUpdated( DataPool::getTransactionNumber() );
  item.markDataItemValueUpdated();
}

/* --------------------------------------------------------------------------- */
/* markDataContainerUpdated --                                                 */
/* --------------------------------------------------------------------------- */

void DataContainer::markDataContainerUpdated( UpdateStatus status ){
  switch( status ){
  case NoUpdate:
    break;
  case DataUpdated:
    setDataContainerDataUpdated( DataPool::getTransactionNumber() );
    break;
  case ValueUpdated:
    setDataContainerValueUpdated( DataPool::getTransactionNumber() );
    break;
  default:
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* markDataContainerDataUpdated --                                             */
/* --------------------------------------------------------------------------- */

void DataContainer::markDataContainerDataUpdated( bool yes ){
  setDataContainerDataUpdated( yes ? DataPool::getTransactionNumber() : 0 );
}

/* --------------------------------------------------------------------------- */
/* markDataContainerValueUpdated --                                            */
/* --------------------------------------------------------------------------- */

void DataContainer::markDataContainerValueUpdated( bool yes ){
  setDataContainerValueUpdated( yes ? DataPool::getTransactionNumber() : 0 );
}

/* --------------------------------------------------------------------------- */
/* isDataContainerDataUpdated --                                               */
/* --------------------------------------------------------------------------- */

bool DataContainer::isDataContainerDataUpdated( TransactionNumber t ) const{
  return m_DataLastUpdated >= t;
}

/* --------------------------------------------------------------------------- */
/* isDataContainerValueUpdated --                                              */
/* --------------------------------------------------------------------------- */

bool DataContainer::isDataContainerValueUpdated( TransactionNumber t ) const{
  return m_ValueLastUpdated >= t;
}

/* --------------------------------------------------------------------------- */
/* getDataContainerDataUpdated --                                              */
/* --------------------------------------------------------------------------- */

TransactionNumber DataContainer::getDataContainerDataUpdated() const{
  return m_DataLastUpdated;
}

/* --------------------------------------------------------------------------- */
/* getDataContainerValueUpdated --                                             */
/* --------------------------------------------------------------------------- */

TransactionNumber DataContainer::getDataContainerValueUpdated() const{
  return m_ValueLastUpdated;
}

/* --------------------------------------------------------------------------- */
/* setDataContainerDataUpdated --                                              */
/* --------------------------------------------------------------------------- */

void DataContainer::setDataContainerDataUpdated( TransactionNumber t ){
  m_DataLastUpdated = t;
}

/* --------------------------------------------------------------------------- */
/* setDataContainerValueUpdated --                                             */
/* --------------------------------------------------------------------------- */

void DataContainer::setDataContainerValueUpdated( TransactionNumber t ){
  m_ValueLastUpdated = t;
  // Der Wert (Value) ist Teil der Daten (Data). In diesem Fall werden daher
  // beide Timestamps neu gesetzt.
  m_DataLastUpdated  = t;
}

/* --------------------------------------------------------------------------- */
/* setPython --                                                                */
/* --------------------------------------------------------------------------- */

DataContainer::UpdateStatus DataContainer::setPython( const PythonObject &obj, DataItem &item, int dims ){
  LOG_DEBUG("DataContainer::setPython: dims=" << dims);
  return setPython( obj );
}
