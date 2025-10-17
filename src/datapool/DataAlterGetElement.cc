#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataAlterGetElement.h"
#include "datapool/DataReference.h"
#include "datapool/DataInx.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetElement::DataAlterGetElement()
  : m_element( 0 ){
}

DataAlterGetElement::~DataAlterGetElement(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElement::check( DataReference &ref ){
  // Mit Wildcards kann kein Element gesucht werden.
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetElement::alterItem( DataItem &item
                                   , UpdateStatus &status
                                   , DataInx &inx
                                   , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    // create element if requested
    BUG_EXIT("indexed range does not exist." );
    return false;
  }

  if( inx.isLastLevel() ){
    m_item = &item;
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetElement::UpdateStatus DataAlterGetElement::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  m_element = &el;
  BUG_EXIT("Element is available");
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */

const DataElement *DataAlterGetElement::getElement() const{
  return m_element;
}

/* --------------------------------------------------------------------------- */
/* getItem --                                                                  */
/* --------------------------------------------------------------------------- */

const DataItem *DataAlterGetElement::getItem() const{
  return m_item;
}
