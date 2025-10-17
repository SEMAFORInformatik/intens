
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataAlterGetItemAttr.h"
#include "datapool/DataReference.h"
#include "datapool/DataInx.h"

#define SAY(X) // std::cout << "SAY: " << X << std::endl;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetItemAttr::DataAlterGetItemAttr( bool update )
  : m_update( update )
  , m_item(0) {
}

DataAlterGetItemAttr::~DataAlterGetItemAttr(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItemAttr::check( DataReference &ref ){
  SAY("ataAlterGetItemAttr: " << ref.fullName(true));
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItemAttr::alterItem( DataItem &item,
				      UpdateStatus &status,
				      DataInx &inx,
				      bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( inx.isLastLevel() ){
    m_item = &item;
    if( updateElements() ){
      status = DataUpdated;
    }
    BUG_EXIT( "alterItem: ok");
    return false; // Ziel erreicht
  }

  if( final ){
    // create an element if requested.
    BUG_EXIT("indexed range does not exist." );
    return false;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetItemAttr::UpdateStatus DataAlterGetItemAttr::alterData( DataElement &el ){
  // wird nicht benötigt
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* updateElements --                                                           */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItemAttr::updateElements() const{
  return m_update;
}

/* --------------------------------------------------------------------------- */
/* createElements --                                                           */
/* --------------------------------------------------------------------------- */

bool DataAlterGetItemAttr::createElements() const{
  return m_update;
}

/* --------------------------------------------------------------------------- */
/* getItem --                                                                  */
/* --------------------------------------------------------------------------- */

const DataItemAttr *DataAlterGetItemAttr::getConstItemAttr() const{
  assert( !updateElements() );
  if( m_item != 0 ){
    return m_item->getDataItemAttr( true );
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getItemNoUpdate --                                                          */
/* --------------------------------------------------------------------------- */

DataItemAttr *DataAlterGetItemAttr::getItemAttr() const{
  assert( updateElements() );
  assert( m_item != 0 );
  return m_item->getDataItemAttr( true );
}
