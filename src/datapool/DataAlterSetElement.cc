
#include "datapool/DataReference.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataAlterSetElement.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetElement::DataAlterSetElement( bool valid )
  : m_valid( valid )
  , m_dict( 0 ){
}

DataAlterSetElement::~DataAlterSetElement(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterSetElement::check( DataReference &ref ){
  BUG_DEBUG("check '" << ref.fullName(true) << "'");
  return ref.hasWildcards();
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterSetElement::alterItem( DataItem &item
                                   , UpdateStatus &status
                                   , DataInx &inx
                                   , bool final )
{
  BUG_DEBUG( "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_DEBUG("indexed range does not exist." );
    assert( !m_valid );
    return false;
  }

  if( inx.isLastLevel() ){
    m_dict = item.getDictionary();
  }

  BUG_DEBUG( "continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterSetElement::UpdateStatus DataAlterSetElement::alterData( DataElement &el ){
  BUG_DEBUG( "alterData" );

  if( m_valid ){
    return setValue( el );
  }
  else{
    return el.clearElement();
  }
}

/* --------------------------------------------------------------------------- */
/* dictionary --                                                               */
/* --------------------------------------------------------------------------- */

const DataDictionary &DataAlterSetElement::dictionary() const{
  assert( m_dict != 0 );
  return *m_dict;
}
