
#include <assert.h>

#include "datapool/DataLogger.h"
#include "datapool/DataTTrailCycleSwitch.h"
#include "datapool/DataPool.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataTTrailCycleSwitch);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailCycleSwitch::DataTTrailCycleSwitch( int num )
  : m_cyclenum( num ){
  BUG_INCR_COUNT;
}
DataTTrailCycleSwitch::~DataTTrailCycleSwitch(){
  BUG_DECR_COUNT;
}


/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* restore --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailCycleSwitch::restore( DataPool &dp ){
  LOG_DEBUG( "Cycle #" << m_cyclenum );
  dp.SetCycle( m_cyclenum );
  return true;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrailCycleSwitch::print( std::ostream &ostr ) const{
  ostr << "Switch to Cycle: " << m_cyclenum;
}

/* --------------------------------------------------------------------------- */
/* name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string DataTTrailCycleSwitch::name() const{
  return "NewCycle";
}
