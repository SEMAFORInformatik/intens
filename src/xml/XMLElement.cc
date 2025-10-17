
#include <string>
#include <algorithm>

#include "datapool/DataDictionary.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataReference.h"
#include "app/DataPoolIntens.h"

#include "xml/XMLElement.h"

XMLElement::XMLElement( XMLElement *parent )
  : m_parent( parent )
  , m_elements( 0 )
  , m_ignore( false ){
  if( parent ){
    m_ignore = parent->ignoreValues();
  }
}

// -----------------------------------------------------------------------
//  clearDatapool
// -----------------------------------------------------------------------

void XMLElement::clearDatapool( bool allCycles ){
  assert( m_parent == 0 );
  DataPoolIntens &dpi = DataPoolIntens::Instance();
  if( allCycles ){
    dpi.goCycle( 0 );
    int numCycles = dpi.numCycles();
    for( int i =1; i < numCycles; ++i ){
      dpi.removeCycle( 0 );
    }
  }
  dpi.clearCycle( dpi.currentCycle() );
}

// -----------------------------------------------------------------------
//  getXferDataItem
// -----------------------------------------------------------------------

XferDataItem *XMLElement::getXferDataItem( const std::string &name ){
  DataReference *dataRef = 0;
  XferDataItem *xfer = 0;
  if( getXferDataItem() != 0 ){
    dataRef = DataPoolIntens::getDataReference( getXferDataItem()->Data(), name );
    if( dataRef != 0 )
      xfer = new XferDataItem( *getXferDataItem(), dataRef );
  }
  return xfer;
}

// -----------------------------------------------------------------------
//  newElement
// -----------------------------------------------------------------------

XMLElement *XMLElement::newElement( const std::string &element ){
  return new XMLElement( this );
}
