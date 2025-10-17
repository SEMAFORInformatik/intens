
#include <assert.h>
#include <sstream>

#include "datapool/DataLogger.h"
#include "datapool/DataTTrailItemList.h"
#include "datapool/DataTTrailItem.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataTTrailItemList);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailItemList::DataTTrailItemList(){
  BUG_SILENT( BugTTrail, "DataTTrailItemList" );
  BUG_INCR_COUNT;
  BUG_MSG( "DataTTrailItemList constructed @" << this );
}

DataTTrailItemList::~DataTTrailItemList(){
  BUG_SILENT( BugTTrail, "~DataTTrailItemList" );
  for( ItemList::iterator it = m_item_list.begin(); it != m_item_list.end(); ++it ){
    delete (*it);
  }
  m_item_list.clear();
  BUG_DECR_COUNT;
  BUG_MSG( "DataTTrailItemList destructed @" << this );
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* isEqual --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailItemList::isEqual( DataTTrailElement &el ){
  // Die ItemList wird nur für das Undo-Feature verwendet. Diese Abfrage
  // ist nicht zugelassen.
  assert( false );
  return false;
}

/* --------------------------------------------------------------------------- */
/* isEqual --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailItemList::isEqual( DataItem *item ){
  // Die ItemList wird nur für das Undo-Feature verwendet. Diese Abfrage
  // ist nicht zugelassen.
  assert( false );
  return false;
}

/* --------------------------------------------------------------------------- */
/* restore --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailItemList::restore( DataPool &datapool ){
  for( ItemList::iterator it = m_item_list.begin(); it != m_item_list.end(); ++it ){
    (*it)->restore( datapool );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* insertItem --                                                               */
/* --------------------------------------------------------------------------- */

void DataTTrailItemList::insertItem( DataTTrailElement *item ){
  BUG_PARA( BugUndo, "insertItem", "[" << item->name() << "]" );

  m_item_list.push_back( item );
}

/* --------------------------------------------------------------------------- */
/* isntEmpty --                                                                */
/* --------------------------------------------------------------------------- */

bool DataTTrailItemList::isntEmpty() const{
  return !m_item_list.empty();
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrailItemList::print( std::ostream &ostr ) const{
  for( ItemList::const_iterator it = m_item_list.begin(); it != m_item_list.end(); ++it ){
    (*it)->print( ostr );
  }
}

/* --------------------------------------------------------------------------- */
/* name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string DataTTrailItemList::name() const{
  std::ostringstream msg;
  msg << "ItemList:";
  ItemList::const_iterator it = m_item_list.begin();
  if( it == m_item_list.end() ){
    msg << "<no items>";
  }
  else{
    msg << (*it)->name();
    for( ++it; it != m_item_list.end(); ++it ){
      msg << "," << (*it)->name();
    }
  }
  return msg.str();
}
