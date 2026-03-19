
#include "datapool/DataElement.h"
#include "datapool/DataReference.h"
#include "datapool/DataAlterGetElementValid.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetElementValid::DataAlterGetElementValid()
  : m_isValid( false ){
}

DataAlterGetElementValid::~DataAlterGetElementValid(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElementValid::check( DataReference &ref ){
  if( ref.hasWildcardsNotLastLevel() ){
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElementValid::alterItem( DataItem &item
                                          , UpdateStatus &status
                                          , DataInx &inx
                                          , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_FATAL("indexed range does not exist." );
    return false;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetElementValid::UpdateStatus DataAlterGetElementValid::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  m_isValid  |= el.isValid();
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* getUpdated --                                                               */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElementValid::isValid() const{
  return  m_isValid;
}
