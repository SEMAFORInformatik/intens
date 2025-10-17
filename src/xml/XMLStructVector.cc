
#include <qstring.h>

#include "xfer/XferDataItemIndex.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferConverter.h"
#include "app/DataPoolIntens.h"
#include "xfer/XferDataItem.h"
#include "datapool/DataReference.h"
#include "gui/GuiManager.h"

#include "xml/XMLValue.h"
#include "xml/XMLContents.h"
#include "xml/XMLStructVector.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLStructVector::XMLStructVector( XMLElement *parent, XferDataItem *xfer, bool isRoot )
  : XMLElement( parent )
  , m_xferDataItem( xfer )
  , m_isRoot( isRoot ){
}

XMLStructVector::~XMLStructVector(){
  if( m_isRoot ){
    DataPoolIntens::Instance().goCycle( 0 );
    GuiManager::Instance().update( GuiElement::reason_Cycle );
  }
  delete m_xferDataItem;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void XMLStructVector::setValue( const std::string & value ){
  if( ignoreValues() ){
    return;
  }

  assert( m_xferDataItem != 0 );
  if( m_xferDataItem->getDataType() == DataDictionary::type_String ){
    if( !m_xferDataItem->setValue( value.c_str() ) ){
      m_xferDataItem->setInvalid();
    }
  }
  else{
    if( !m_xferDataItem->setValue( value ) ){
      m_xferDataItem->setInvalid();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* newElement --                                                               */
/* --------------------------------------------------------------------------- */

XMLElement *XMLStructVector::newElement( const std::string &element ){
  if( !m_isRoot ){
    if( element == "contents" ){
      XferDataItemIndex *index = m_xferDataItem->getLastIndex();
      index->setLowerbound( m_elements );
      m_xferDataItem->setDimensionIndizes();
      ++m_elements;
      return new XMLContents( this );
    }

    if( element == "vector" ){
      XferDataItemIndex *index = m_xferDataItem->getLastIndex();
      index->setLowerbound( m_elements );
      m_xferDataItem->setDimensionIndizes();
      XferDataItem *xfer = new XferDataItem( *m_xferDataItem );
      index = xfer->newDataItemIndex();
      index->setLowerbound( 0 );
      xfer->setDimensionIndizes();
      ++m_elements;
      return new XMLStructVector( this, xfer );
    }

    return new XMLElement( this );
  }

  // Es handelt sich um den Datapool oder zumindest um einen kompletten Cycle
  if( element != "contents" )
    return new XMLElement( this );

  DataPoolIntens &dpi = DataPoolIntens::Instance();
  if( m_elements > 0 ){
    // Hier erzeugen wir einen neuen Cycle
    // Da das eine Kopie ist, löschen wir den Inhalt gleich wieder.
    dpi.newCycle( "" );
    dpi.clearCycle( dpi.currentCycle() );
  }
  ++m_elements;
  return m_parent->newElement( element );
}
