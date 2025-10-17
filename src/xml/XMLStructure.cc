
#include "utils/Debugger.h"
#include "app/DataPoolIntens.h"
#include "datapool/DataReference.h"
#include "xfer/XferDataItemIndex.h"
#include "xfer/XferDataItem.h"
#include "xml/XMLVariable.h"
#include "xml/XMLStructVector.h"
#include "xml/XMLStructure.h"
#include "xml/XMLContents.h"
#include "xml/XMLStructure.h"

INIT_LOGGER();

//========================================================================
// Constructor / Destructor
//========================================================================

XMLStructure::XMLStructure( XMLElement *parent,
                            XferDataItem *xfer,
                            bool isRoot,
                            bool allCycles )
  : XMLElement( parent )
  , m_xfer( xfer )
  , m_isRoot( isRoot )
  , m_allCycles( allCycles ){

  // Ist xfer 0, handelt es sich um den ganzen DATAPOOL oder einen CYCLE
  // Bei Bedarf räumen wir den ganzen Datapool auf
  if( xfer == 0  ){
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
}

XMLStructure::~XMLStructure(){
  delete m_xfer;
}

// ---------------------------------------------------------------------------
// clearRange --
// ---------------------------------------------------------------------------

void XMLStructure::clearRange(){
  if( !ignoreValues() ){
    assert( m_xfer != 0 );
    BUG_DEBUG("clearRange()");
    m_xfer->clearRange();
  }
}

// ---------------------------------------------------------------------------
// newElement --
// ---------------------------------------------------------------------------

XMLElement *XMLStructure::newElement( const std::string &element ){
  if( !m_isRoot ){
    assert( m_xfer != 0 );
    if( element == "contents" )
      return new XMLContents( this );
    else if( element == "vector" ){
      XferDataItem *xfer = new XferDataItem( *m_xfer );
      XferDataItemIndex *index = xfer->newDataItemIndex();
      index->setLowerbound( 0 );
      xfer->setDimensionIndizes();
      return new XMLStructVector( this, xfer );
    }
    return new XMLElement( this );
  }
  // Es handelt sich um den Datapool oder zumindest um einen kompletten Cycle
  else if( element == "contents" )
    return new XMLContents( this, m_isRoot );
  // Ein vector ist nur beim kompletten Datapool möglich
  else if( element == "vector" && m_allCycles )
    return new XMLStructVector( this, 0, m_isRoot );
  else{
    XferDataItem *xfer = getXferDataItem( element );
    if( xfer != 0 ){
      switch(  xfer->Data()->getDataType() ){
      case DataDictionary::type_StructVariable :{
	return  new XMLStructure( this, xfer );
	break;
      }
      case DataDictionary::type_Integer :
      case DataDictionary::type_Real:
      case DataDictionary::type_String:
      case DataDictionary::type_Complex:
	return  new XMLVariable( this, xfer );
	break;
      default :
	break;
      }
    }
  }
  return new XMLElement( this );
}

// -----------------------------------------------------------------------
//  getXferDataItem
// -----------------------------------------------------------------------

XferDataItem *XMLStructure::getXferDataItem( const std::string &name ){
  if( !m_isRoot )
    return XMLElement::getXferDataItem( name );
  XferDataItem *xfer = 0;
  if( getXferDataItem() != 0 ){
    if( getXferDataItem()->getName() == name )
      xfer = new XferDataItem( *m_xfer );
  }
  else{
    DataReference *ref = DataPoolIntens::Instance().getDataReference( name );
    if( ref != 0 )
      xfer = new XferDataItem( ref );
  }
  return xfer;
}
