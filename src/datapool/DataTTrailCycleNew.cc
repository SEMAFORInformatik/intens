
#include <assert.h>

#include "datapool/DataLogger.h"
#include "datapool/DataTTrailCycleNew.h"
#include "datapool/DataPool.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataTTrailCycleNew);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailCycleNew::DataTTrailCycleNew( int oldnum, int newnum )
  : m_old_cyclenum( oldnum )
  , m_new_cyclenum( newnum ){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* restore --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailCycleNew::restore( DataPool &dp ){
  LOG_DEBUG("Cycle #" << m_new_cyclenum );
  dp.TTRemoveCycle( m_new_cyclenum, m_old_cyclenum );
  return true;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrailCycleNew::print( std::ostream &ostr ) const{
  ostr << "Delete Cycle #" << m_new_cyclenum << " and switch to Cycle #" << m_old_cyclenum;
}

/* --------------------------------------------------------------------------- */
/* name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string DataTTrailCycleNew::name() const{
  return "NewCycle";
}
