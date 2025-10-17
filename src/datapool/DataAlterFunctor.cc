
#include <assert.h>

#include "datapool/DataAlterFunctor.h"
#include "datapool/DataContainer.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataAlterFunctor);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterFunctor::DataAlterFunctor(){
  BUG_INCR_COUNT;
}

DataAlterFunctor::~DataAlterFunctor(){
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterFunctor::check( DataReference &ref ){
  return false; // ok
}

/* --------------------------------------------------------------------------- */
/* writeTTrail --                                                              */
/* --------------------------------------------------------------------------- */

bool DataAlterFunctor::writeTTrail() const{
  return updateElements();
}

/* --------------------------------------------------------------------------- */
/* writeTTrail --                                                              */
/* --------------------------------------------------------------------------- */

bool DataAlterFunctor::startDimension( const DataContainer &cont, DataInx &inx ){
  if( cont.isDataDimension() ){
    return startDimension( *cont.getDataDimension(), inx );
  }
  return true;
}
