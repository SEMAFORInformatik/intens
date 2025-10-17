
#include <string>

// #include <X11/Intrinsic.h>
#include "app/DataPoolIntens.h"
// #include "gui/GuiInitWorker.h"
#include "job/JobManager.h"

#include "xfer/XferDataItem.h"
#include "xml/XMLStructure.h"
#include "xml/XMLVariable.h"
#include "xml/XMLContents.h"

//========================================================================
// end
//========================================================================
void XMLContents::end(){
  std::string value;
  if( getValue( value ) )
    m_parent->setValue( value );
}

//========================================================================
// newElement
//========================================================================
XMLElement *XMLContents::newElement( const std::string &element ){
  if( !m_isRoot ){
    XMLElement *newElement = 0;
    XferDataItem *xfer = XMLElement::getXferDataItem( element );
    if( xfer != 0 ){
      switch( xfer->Data()->getDataType() ){
      case DataDictionary::type_StructVariable :{
	newElement =  new XMLStructure( this, xfer );
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
  // Es handelt sich um den Datapool oder zumindest um einen kompletten Cycle
  // Da wir kein Xfer haben, überlassen wir die Arbeit dem Root

  //  GuiInitWorker *init = JobManager::Instance().getInitialFunctionWorker( "INIT" );
  //  if( init != 0 ){
    // Der InitWorker hängt sich als WorkProc ein.
  //    init->startInitialWork();
  //  }

  return m_parent->newElement( element );
}
