
#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataAlterSetStylesheet.h"
#include "datapool/DataInx.h"
#include "app/UserAttr.h"
#include "datapool/DataReference.h"
#include "gui/GuiManager.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterSetStylesheet::DataAlterSetStylesheet(const std::string& stylesheet)
  : m_stylesheet(stylesheet)
  , m_create( false ){
}

DataAlterSetStylesheet::~DataAlterSetStylesheet(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterSetStylesheet::check( DataReference &ref ){
  if( ref.numberOfWildcards() == 0 ){
    m_create = true;
  }
  // Es können auch für mehrere Elemente dieselben Attribute gesetzt werden.
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */
bool DataAlterSetStylesheet::alterItem( DataItem &item
                                     , UpdateStatus &status
                                     , DataInx &inx
                                     , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;
  if( final ){
    BUG_EXIT( "indexed range does not exist." );
    return false; // abbrechen
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */
DataAlterSetStylesheet::UpdateStatus DataAlterSetStylesheet::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );
  if( el.setStylesheet(m_stylesheet) ){
    el.markDataContainerDataUpdated();
    return DataUpdated;
  }

  // Das Item wird durch alterData() immer mit dem neusten Stylesheet versehen.
  return NoUpdate; // kein update
}
