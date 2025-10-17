#include "datapool/DataReference.h"
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterClearDataItem.h"

#define SAY(X) // std::cerr << X << std::endl;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterClearDataItem::DataAlterClearDataItem( bool incl_itemattr )
  : m_incl_itemattr( incl_itemattr ){
}

DataAlterClearDataItem::~DataAlterClearDataItem(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterClearDataItem::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( m_incl_itemattr && inx.isLastLevel() ){
    SAY( "clear attributes of item '" << item.getName() << "'" );
    item.clearDataItemAttr();
  }

  if( final ){
    // no values to clear
    BUG_EXIT("indexed range does not exist." );
    return false; //
  }

  if( inx.isLastLevel() ){
    if( inx.isIndexListEmpty() ){
      // no index. clear all data elements
      SAY( "clear item '" << item.getName() << "'" );
      status = item.clearContainer( m_incl_itemattr );
      return false;
    }
    // Nun funktionieren wir wie DataAlterClearElement
    return true;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterClearDataItem::UpdateStatus DataAlterClearDataItem::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );
  return el.clearElement();
}

/* --------------------------------------------------------------------------- */
/* inclusiveValidItemAttributes --                                             */
/* --------------------------------------------------------------------------- */

bool DataAlterClearDataItem::inclusiveValidItemAttributes() const{
  return m_incl_itemattr;
}
