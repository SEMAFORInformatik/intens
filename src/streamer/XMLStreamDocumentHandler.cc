
#include "utils/Debugger.h"
#include "streamer/XMLStreamDocumentHandler.h"
#include "xfer/XferDataItem.h"
#include "xml/XMLStructure.h"
#include "xml/XMLValue.h"
#include "app/DataPoolIntens.h"

INIT_LOGGER();

//========================================================================
// Constructor / Destructor
//========================================================================

XMLStreamDocumentHandler::XMLStreamDocumentHandler()
  : m_currentCycle( 0 )
  , m_rootItem( 0 )
  , m_allCycles( false ){
}

XMLStreamDocumentHandler::~XMLStreamDocumentHandler(){
  clear();
  delete m_rootItem;
}

// ---------------------------------------------------------------------------
// startDocument --
// ---------------------------------------------------------------------------

void XMLStreamDocumentHandler::startDocument(){
}

// ---------------------------------------------------------------------------
// endDocument --
// ---------------------------------------------------------------------------

void XMLStreamDocumentHandler::endDocument(){
  clear();
}

// ---------------------------------------------------------------------------
// startElement --
// ---------------------------------------------------------------------------

void XMLStreamDocumentHandler::startElement( const std::string &element,
                                             XMLAttributeMap &attrs ){
  BUG_DEBUG("startElement(" << element << ")");

  XMLElement *elem = 0;
  if( !m_elements.empty() ){
    elem = m_elements.back()->newElement( element );
    if( !isDatapoolStream() ){
      if( element == INTERNAL_CYCLE_LIST ||
          element == INTERNAL_CYCLE_NAME ){
        elem->setIgnoreValues();
      }
    }
    if( elem->ignoreValues() ){
      BUG_DEBUG("=== Begin IGNORE " << element << " ===");
    }
    else {
      BUG_DEBUG("clearRange of " << element);
      elem -> clearRange();
    }
    m_elements.push_back( elem );
    return;
  }

  if( element == "DATAPOOL" ){
    if( isDatapoolStream() ){
      BUG_DEBUG("==> START DATAPOOL <==");
    }
    else if( isCycleStream() ){
      BUG_DEBUG("==> START CYCLE <==");
    }
    m_elements.push_back( createRoot( element ) );
    return;
  }

  if( m_rootItem != 0 ){
    m_elements.push_back( createRoot( element ) );
    elem = m_elements.back()->newElement( element );
    BUG_DEBUG("clearRange of " << element);
    elem -> clearRange();
    m_elements.push_back( elem );
    return;
  }

  // Das XML Document passt nicht zu uns
  XMLElement *parent = 0;
  elem = new XMLElement( parent );
  elem->clearDatapool( m_allCycles );
  m_elements.push_back( elem );
}

// ---------------------------------------------------------------------------
// endElement --
// ---------------------------------------------------------------------------

void XMLStreamDocumentHandler::endElement( const std::string &element ){
  BUG_DEBUG("endElement(" << element << ")");
  assert( !m_elements.empty() );
  XMLElement *elem = m_elements.back();
  if( elem ){
    if( elem->ignoreValues() ){
      BUG_DEBUG("=== End IGNORE " << element << " ===");
    }
  }

  if( m_elements.back() != 0 ){
    m_elements.back()->end();
    delete m_elements.back();
  }
  m_elements.pop_back();
}

// ---------------------------------------------------------------------------
// characters --
// ---------------------------------------------------------------------------

void XMLStreamDocumentHandler::characters( const std::string &value ){
  BUG_DEBUG("characters(" << value << ")");

  assert( !m_elements.empty() );
  m_elements.back()->characters( value );
}

// ---------------------------------------------------------------------------
// setRoot --
// ---------------------------------------------------------------------------

void XMLStreamDocumentHandler::setRoot( XferDataItem *xfer ){
  BUG_DEBUG("setRoot");
  m_currentCycle = 0;
  if( xfer != 0 ) {
    if (m_rootItem) delete m_rootItem;
    m_rootItem = new XferDataItem( *xfer );
  }
  clear();
}

// ---------------------------------------------------------------------------
// clear --
// ---------------------------------------------------------------------------

void XMLStreamDocumentHandler::clear(){
  std::vector<XMLElement *>::iterator iter;
  for ( iter = m_elements.begin(); iter != m_elements.end(); ++iter )
    delete *iter;
  m_elements.clear();
}

// ---------------------------------------------------------------------------
// createRoot --
// ---------------------------------------------------------------------------

XMLElement *XMLStreamDocumentHandler::createRoot( const std::string &element ){
  XferDataItem *newItem = 0;
  XMLElement *parent = 0;
  bool isRoot = true;
  if( m_rootItem != 0 ){
    newItem = new XferDataItem( *m_rootItem );
  }
  return new XMLStructure( parent, newItem, isRoot, m_allCycles );
}

/* --------------------------------------------------------------------------- */
/* isElementStream --                                                          */
/* --------------------------------------------------------------------------- */

bool XMLStreamDocumentHandler::isElementStream(){
  return m_rootItem != 0;
}

/* --------------------------------------------------------------------------- */
/* isDatapoolStream --                                                         */
/* --------------------------------------------------------------------------- */

bool XMLStreamDocumentHandler::isDatapoolStream(){
  if( isElementStream() ){
    return false;
  }
  return m_allCycles;
}

/* --------------------------------------------------------------------------- */
/* isCycleStream --                                                            */
/* --------------------------------------------------------------------------- */

bool XMLStreamDocumentHandler::isCycleStream(){
  if( isElementStream() ){
    return false;
  }
  return !m_allCycles;
}
