
#include "utils/utils.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataInx.h"
#include "datapool/DataStructElement.h"
#include "datapool/DataReference.h"
#include "datapool/DataAlterGetStylesheet.h"
#include "app/DataPoolIntens.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetStylesheet::DataAlterGetStylesheet( TransactionNumber t )
  : m_element_gui_timestamp( 0 )
  , m_element_back_timestamp( 0 )
  , m_transactionnumber( t ){
}

DataAlterGetStylesheet::~DataAlterGetStylesheet(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetStylesheet::check( DataReference &ref ){
  if( ref.hasWildcards() ){
    return true; // nicht möglich
  }

  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetStylesheet::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  DataAttributes attr( item.getAttributes() );

  const std::string& sh = item.getStylesheet();
  if (sh.size()) {
    m_stylesheet += (m_stylesheet.size() ? "\n" : "") + sh;
  }

  // Der Rollback-Timestamp ist nur für das Erkennen eines nötigen GUI-Updates zu verwenden.
  m_element_back_timestamp = maximum( m_element_back_timestamp, item.getLastRollbackTimestamp() );

  // Der GUI-Timestamp ist nur auf dem letzten verfügbaren Level relevant, da Updates
  // in einem Seitenast nicht beachtet werden dürfen! Der Timestamp wird daher immer wieder
  // überschrieben.
  m_element_gui_timestamp  = item.getDataItemGuiUpdated();

  if( final ){
    BUG_FATAL( "indexed range does not exist." );
    return false; // abbrechen
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterStructData --                                                          */
/* --------------------------------------------------------------------------- */

void DataAlterGetStylesheet::alterStructData( DataStructElement &el ){
  const std::string& sh = el.getStylesheet();
  if (sh.size()) {
    m_stylesheet += (m_stylesheet.size() ? "\n" : "") + sh;
  }
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetStylesheet::UpdateStatus DataAlterGetStylesheet::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  const std::string& sh = el.getStylesheet();
  if (sh.size()) {
    m_stylesheet += (m_stylesheet.size() ? "\n" : "") + sh;
  }

  // Wenn des Element vorhanden ist, interessiert uns der GuiTimestamp des DataItems
  // nicht mehr, ausser das Element wurde noch nie verändert.
  if( el.getDataContainerDataUpdated() > 0 ){
    m_element_gui_timestamp = maximum( el.getDataContainerDataUpdated(), m_element_back_timestamp );
  }
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* get --                                                                      */
/* --------------------------------------------------------------------------- */

const std::string& DataAlterGetStylesheet::get(bool& updateGui) const{
  updateGui = false;
  if( m_element_gui_timestamp >= m_transactionnumber ){
    updateGui = true;
  }
  return m_stylesheet;
}
