#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterAssignCorrDataItem.h"
#include "datapool/DataReference.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterAssignCorrDataItem::DataAlterAssignCorrDataItem( const DataElement *el, const DataDictionary &dict )
  : m_element( el )
  , m_dictionary( dict ){
}

DataAlterAssignCorrDataItem::~DataAlterAssignCorrDataItem(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignCorrDataItem::check( DataReference &ref ){
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignCorrDataItem::alterItem( DataItem &item
                                           , UpdateStatus &status
                                           , DataInx &inx
                                           , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    // indexed range does not exist.
    BUG_FATAL("indexed range does not exist" );
    assert( !isValid() );
    return false;
  }

  BUG_EXIT( "continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterAssignCorrDataItem::UpdateStatus DataAlterAssignCorrDataItem::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  if( isValid() ){
    return el.assignCorrespondingDataItem( *m_element );
  }
  else{
    return el.clearCorrespondingDataItem( m_dictionary );
  }
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignCorrDataItem::createElements() const{
  return isValid();
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterAssignCorrDataItem::isValid() const{
  return m_element != 0;
}
