
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


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