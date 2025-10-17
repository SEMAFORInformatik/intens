
#include "datapool/DataCompareResult.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

 DATAAttributeMask DataCompareResult::s_mask_all = DATAisEqual | DATAisDifferent | DATAisLeftOnly | DATAisRightOnly;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataCompareResult::DataCompareResult()
  : m_mask( 0 ){
}

DataCompareResult::DataCompareResult( const DataCompareResult &rslt )
  : m_mask( rslt.m_mask ){
}

DataCompareResult::DataCompareResult( DATAAttributeMask mask )
  : m_mask( (mask & s_mask_all) ){
}

DataCompareResult::~DataCompareResult(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setResult  --                                                               */
/* --------------------------------------------------------------------------- */

void DataCompareResult::setResult( const DataCompareResult &rslt ){
  setResult( rslt.m_mask );
}

/* --------------------------------------------------------------------------- */
/* setResult  --                                                               */
/* --------------------------------------------------------------------------- */

void DataCompareResult::setResult( DATAAttributeMask mask ){
  // Unerwartete Bits werden gefiltert.
  if( isSet( DATAisLeftOnly ) && isSet( mask, DATAisLeftOnly ) ){
    m_mask = m_mask | DATAisDifferent;
  }
  if( isSet( DATAisRightOnly ) && isSet( mask, DATAisRightOnly ) ){
    m_mask = m_mask | DATAisDifferent;
  }
  m_mask = m_mask | (mask & s_mask_all);
}

/* --------------------------------------------------------------------------- */
/* getCompareResultMask  --                                                    */
/* --------------------------------------------------------------------------- */

DATAAttributeMask DataCompareResult::getResult() const{
  if( isNotSet( s_mask_all ) ){
    return 0; // Kein Flag ist gesetzt
  }

  if( isSet( DATAisDifferent ) ){
    return DATAisDifferent;
  }
  if( isSet( DATAisEqual ) ){
    if( isSet( DATAisLeftOnly | DATAisRightOnly ) ){
      return DATAisDifferent;
    }
    else{
      return DATAisEqual;
    }
  }
  if( isSet( DATAisLeftOnly ) ){
    if( isSet( DATAisRightOnly ) ){
      return DATAisDifferent;
    }
    else{
      return DATAisLeftOnly;
    }
  }
  else{
    if( isSet( DATAisRightOnly ) ){
      return DATAisRightOnly;
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* isSet --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataCompareResult::isSet( DATAAttributeMask mask ) const{
  return (m_mask & mask) != 0;
}

/* --------------------------------------------------------------------------- */
/* isNotSet --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataCompareResult::isNotSet( DATAAttributeMask mask ) const{
  return (m_mask & mask) == 0;
}

/* --------------------------------------------------------------------------- */
/* isSet --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataCompareResult::isSet( DATAAttributeMask mask1, DATAAttributeMask mask2 ) const{
  return (mask1 & mask2) != 0;
}
