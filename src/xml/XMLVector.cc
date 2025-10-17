
#include "utils/Debugger.h"
#include "utils/utils.h"
#include "datapool/DataVector.h"
#include "xml/XMLVector.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferDataItemIndex.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLVector::XMLVector( XferDataItem *xfer )
  : m_xferDataItem( xfer )
  , m_max_index( 0 ){
  assert( m_xferDataItem != 0 );
}

XMLVector::~XMLVector(){
  delete m_xferDataItem;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* insertValue --                                                              */
/* --------------------------------------------------------------------------- */

void XMLVector::insertValue( int inx, DataValue *data ){
  BUG_DEBUG("insertValue()");

  m_value_list.insert( ValueList::value_type( inx, data ) );
  m_max_index = maximum( inx, m_max_index );
}

/* --------------------------------------------------------------------------- */
/* setValues --                                                                */
/* --------------------------------------------------------------------------- */

void XMLVector::setValues( bool ignoreValues ){
  BUG_DEBUG("setValues()");

  if( !ignoreValues ){
    m_values.resizeVector( m_max_index + 1 );
    for( ValueList::iterator it =  m_value_list.begin(); it != m_value_list.end(); ++it ){
      DataValue::Ptr ptr = it->second;
      m_values.setValue( it->first, ptr );
    }
    m_xferDataItem->setValues( m_values );
  }
  m_values.clearVector();
  m_value_list.clear();
}

/* --------------------------------------------------------------------------- */
/* setLastIndexWildcard --                                                     */
/* --------------------------------------------------------------------------- */

void XMLVector::setLastIndexWildcard(){
  XferDataItemIndex *index = m_xferDataItem->getLastIndex();
  assert( index != 0 );
  index->setWildcard();
  m_xferDataItem->setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* setLastIndexLowerbound --                                                   */
/* --------------------------------------------------------------------------- */

void XMLVector::setLastIndexLowerbound( int inx ){
  XferDataItemIndex *index = m_xferDataItem->getLastIndex();
  index->setLowerbound( inx );
  m_xferDataItem->setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* getDataItem --                                                              */
/* --------------------------------------------------------------------------- */

XferDataItem *XMLVector::getDataItem(){
  return m_xferDataItem;
}

/* --------------------------------------------------------------------------- */
/* newDataItem --                                                              */
/* --------------------------------------------------------------------------- */

XferDataItem *XMLVector::newDataItem(){
  XferDataItem *xfer = new XferDataItem( *m_xferDataItem );
  XferDataItemIndex *index = xfer->newDataItemIndex();
  index->setLowerbound( 0 );
  xfer->setDimensionIndizes();
  return xfer;
}
