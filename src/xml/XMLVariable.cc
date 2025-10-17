
#include <qstring.h>

#include "utils/Debugger.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataItemIndex.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferConverter.h"
#include "datapool/DataReference.h"

#include "xml/XMLRealVector.h"
#include "xml/XMLIntegerVector.h"
#include "xml/XMLStringVector.h"
#include "xml/XMLComplexVector.h"
#include "xml/XMLValue.h"
#include "XMLVariable.h"

INIT_LOGGER();

//========================================================================
// Constructor / Destructor
//========================================================================

XMLVariable::XMLVariable( XMLElement *parent, XferDataItem *xfer )
  : XMLElement( parent )
  , m_xferDataItem( xfer ){
  BUG_DEBUG("Constructor of XMLVariable");
  assert( m_xferDataItem != 0 );
  if( m_xferDataItem->getDict()->isInternalName() ){
    BUG_DEBUG("-- ignore internal name --");
    setIgnoreValues();
  }
}

XMLVariable::~XMLVariable(){
  delete m_xferDataItem;
}

// ---------------------------------------------------------------------------
// clearRange --
// ---------------------------------------------------------------------------

void XMLVariable::clearRange(){
  if( !ignoreValues() ){
    BUG_DEBUG("clearRange()");
    assert( m_xferDataItem != 0 );
    m_xferDataItem->clearRange();
  }
}

// ---------------------------------------------------------------------------
// setValue --
// ---------------------------------------------------------------------------

void XMLVariable::setValue( const std::string & value ){
  if( ignoreValues() ){
    return;
  }

  BUG_DEBUG("setValue(" << value << ")");

  assert( m_xferDataItem != 0 );
  switch( m_xferDataItem->getDataType() ){
  case DataDictionary::type_Complex:{
    ComplexConverter conv( 0, -1, 0, false, true );
    std::istringstream is( value );
    dComplex dc;
    conv.read( is, dc );
    if( !m_xferDataItem->setValue( dc ) )
      m_xferDataItem->setInvalid();
    break;
  }
  case DataDictionary::type_String :{
    if( !m_xferDataItem->setValue( value ) )
      m_xferDataItem->setInvalid();
    break;
  }
  default:
    if( !m_xferDataItem->setValue( value ) )
      m_xferDataItem->setInvalid();
    break;
  }
}

// ---------------------------------------------------------------------------
// newElement --
// ---------------------------------------------------------------------------

XMLElement *XMLVariable::newElement( const std::string &element ){
  assert( m_xferDataItem != 0 );
  if( element == "value" || element == "v" ){
    return new XMLValue( this );
  }
  else if( element == "vector" ){
    XferDataItem *xfer = new XferDataItem( *m_xferDataItem );
    XferDataItemIndex *index = xfer->newDataItemIndex();
    index->setLowerbound( 0 );
    xfer->setDimensionIndizes();
    switch( m_xferDataItem->getDataType() ){
    case DataDictionary::type_Complex:
      return new XMLComplexVector( this, xfer );
      break;
    case DataDictionary::type_String :
      return new XMLStringVector( this, xfer );
      break;
    case DataDictionary::type_Integer :
      return new XMLIntegerVector( this, xfer );
      break;
    case DataDictionary::type_Real :
      return new XMLRealVector( this, xfer );
      break;

    default :
      assert( false );
      break;
    }
  }
  return new XMLElement( this );
}
