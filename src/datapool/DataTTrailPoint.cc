
#include <assert.h>

#include "datapool/DataTTrailPoint.h"
#include "datapool/DataPool.h"
#include "datapool/DataLogger.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataTTrailPoint);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailPoint::DataTTrailPoint( int level, DataPool &datapool, TransactionOwner *owner )
  : m_level( level )
  , m_owner( owner ){
  BUG_INCR_COUNT;
  m_transaction_num = datapool.GetCurrentTransaction();
}

DataTTrailPoint::~DataTTrailPoint(){
  LOG_DEBUG("destroying level " << m_level);
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* isOwner --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailPoint::isOwner( TransactionOwner *owner ){
  return m_owner == owner;
}

/* --------------------------------------------------------------------------- */
/* getOwner --                                                                 */
/* --------------------------------------------------------------------------- */

TransactionOwner *DataTTrailPoint::getOwner() const{
  assert( m_owner != 0 );
  return m_owner;
}

/* --------------------------------------------------------------------------- */
/* restore --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailPoint::restore( DataPool &dp ){
  dp.RestoreCurrentTransaction( m_transaction_num );
  return false;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrailPoint::print( std::ostream &ostr ) const{
  ostr << "Point: " << m_level;
}

/* --------------------------------------------------------------------------- */
/* name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string DataTTrailPoint::name() const{
  return "Point";
}
