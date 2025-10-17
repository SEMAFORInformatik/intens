
#include <assert.h>

#include "datapool/DataLogger.h"
#include "datapool/DataTTrailCycleDelete.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataTTrailCycleDelete);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailCycleDelete::DataTTrailCycleDelete( DataItem *cycle, int num )
  : m_cycle( cycle )
  , m_cyclenum( num ){
}

DataTTrailCycleDelete::~DataTTrailCycleDelete(){
    LOG_DEBUG("deleted" );
    if( m_cycle != 0 ){
	DataItem::deleteItem( m_cycle );
	LOG_DEBUG("Saved Cycle Deleted");
    }
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* restore --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailCycleDelete::restore( DataPool &dp ){
    LOG_DEBUG( "Cycle #" << m_cyclenum );
  dp.TTInsertCycle( m_cycle, m_cyclenum );
  m_cycle = 0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrailCycleDelete::print( std::ostream &ostr ) const{
  ostr << "Delete Cycle #" << m_cyclenum;
}

/* --------------------------------------------------------------------------- */
/* name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string DataTTrailCycleDelete::name() const{
  return "DeleteCycle";
}
