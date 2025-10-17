
#include "xfer/XferDataParameter.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* converterSetScaleIndexes --                                                 */
/* --------------------------------------------------------------------------- */

void XferDataParameter::converterSetScaleIndexes( Converter &conv ){
  for ( int wildcardNumberForwards=1;
	wildcardNumberForwards <= m_dataitem->getNumOfWildcards();
	++wildcardNumberForwards ) {
    int value = m_dataitem->getDataItemIndexWildcard( wildcardNumberForwards )->getValue();
    BUG_DEBUG("wildcardNumberForwards " << wildcardNumberForwards << " value : " << value );
    int wildcardNumberBackwards = m_dataitem->getNumOfWildcards() - wildcardNumberForwards + 1;
    conv.setScaleIndex( wildcardNumberBackwards, value );
  }
}
