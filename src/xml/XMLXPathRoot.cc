
#include "xfer/XferDataItem.h"
#include "app/DataPoolIntens.h"

#include "xml/XMLVariable.h"
#include "xml/XMLXPathStructure.h"
#include "xml/XMLXPathRoot.h"

// -----------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------
XMLXPathRoot::XMLXPathRoot( XferDataItem *xfer, int cycle )
  : XMLElement( 0 )
  , m_xfer( xfer )
  , m_cycle( cycle ){
}

XMLXPathRoot::~XMLXPathRoot(){
  delete m_xfer;
}

// -----------------------------------------------------------------------
// newElement
// -----------------------------------------------------------------------
XMLElement *XMLXPathRoot::newElement( const std::string &element ){
  XMLElement *newElement = 0;
  XferDataItem *xfer = getXferDataItem( element );
  if( xfer != 0 ){
    switch(  xfer->Data()->getDataType() ){
    case DataDictionary::type_StructVariable :{
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
  }
  return new XMLElement( this );
}


// -----------------------------------------------------------------------
//  getXferDataItem
// -----------------------------------------------------------------------
XferDataItem *XMLXPathRoot::getXferDataItem( const std::string &name ){
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
