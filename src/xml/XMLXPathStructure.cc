
#include "xfer/XferDataItemIndex.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataReference.h"

#include "xml/XMLVariable.h"
#include "xml/XMLStructVector.h"
#include "xml/XMLXPathStructure.h"
#include "xml/XMLContents.h"
#include "xml/XMLXPathStructure.h"

//========================================================================
// Constructor / Destructor
//========================================================================
XMLXPathStructure::XMLXPathStructure( XMLElement *parent, XferDataItem *xfer )
  : XMLElement( parent )
  , m_xferDataItem( xfer ){
}

XMLXPathStructure::~XMLXPathStructure(){
  delete m_xferDataItem;
}

//========================================================================
// clearRange
//========================================================================
void XMLXPathStructure::clearRange(){
  if( !ignoreValues() ){
    assert( m_xferDataItem != 0 );
    m_xferDataItem->clearRange();
  }
}

//========================================================================
// newElement
//========================================================================
XMLElement *XMLXPathStructure::newElement( const std::string &element ){
  assert( m_xferDataItem != 0 );
  if( element == "contents" )
    return new XMLContents( this );
  else if( element == "vector" ){
    XferDataItem *xfer = new XferDataItem( *m_xferDataItem );
    XferDataItemIndex *index = xfer->newDataItemIndex();
    index->setLowerbound( 0 );
    xfer->setDimensionIndizes();
    return new XMLStructVector( this, xfer );
  }

  /** additionals for xpath query results **/
  XMLElement *newElement = 0;
  XferDataItem *xfer = XMLElement::getXferDataItem( element );
  if( xfer != 0 ){
    switch( xfer->Data()->getDataType() ){
    case DataDictionary::type_StructVariable :{
      XferDataItem *xfer = new XferDataItem( *m_xferDataItem );
      XferDataItemIndex *index = xfer->newDataItemIndex();
      index->setLowerbound( 0 );
      xfer->setDimensionIndizes();
      newElement =  new XMLXPathStructure( this, xfer );
      break;
    }
    case DataDictionary::type_Integer :
    case DataDictionary::type_Real:
    case DataDictionary::type_String:
    case DataDictionary::type_Complex:
      newElement =  new XMLVariable( this, xfer );
      break;
    default :
      return new XMLElement( this );
      break;
    }
    return newElement;
  } else {
    std::cout << "Not Got xferDataitem: " << m_xferDataItem->getFullName(true) << "  el: " << element << std::endl;
  }

  return new XMLElement( this );
}
