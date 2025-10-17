
#include "datapool/DataElementMap.h"
#include "datapool/DataDimension.h"
#include "datapool/DataElement.h"
#include "datapool/DataStructElement.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataElementMap);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataElementMap::DataElementMap(){
  BUG_INCR_COUNT;
}

DataElementMap::~DataElementMap(){
  BUG_DECR_COUNT;
  m_el_map.clear();
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* insert --                                                                   */
/* --------------------------------------------------------------------------- */

bool DataElementMap::insert( DataElement *el ){
  assert( el->isStructElement() );
  std::string key;
  if( !el->getStructElement().getSortKey( key ) ){
    return false; // Kein Key ?!?
  }

  return m_el_map.insert( ElementMap::value_type( key, el ) ).second;
}

/* --------------------------------------------------------------------------- */
/* get --                                                                      */
/* --------------------------------------------------------------------------- */

DataElementMap::ElementMap::iterator DataElementMap::get( const std::string &key ){
  return m_el_map.find( key );
}

/* --------------------------------------------------------------------------- */
/* compare --                                                                  */
/* --------------------------------------------------------------------------- */

DataCompareResult DataElementMap::compare( DataElementMap &m, int fall, int max_faelle ){
  DataCompareResult rslt;
  ElementMap::iterator it_m;
  for( ElementMap::iterator it = m_el_map.begin(); it != m_el_map.end(); it++ ){
    it_m = m.get( it->first );
    if( it_m == m.m_el_map.end() ){
      it->second->setCompareAttributesOfAllElements( DATAisLeftOnly, 0, max_faelle );
      rslt.setResult( DATAisLeftOnly );
    }
    else{ // gefunden
      rslt.setResult( it->second->compare( *(it_m->second), fall, max_faelle ) );
      it_m->second = 0;
    }
  }
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* append --                                                                   */
/* --------------------------------------------------------------------------- */

bool DataElementMap::append( DataDimension &dim, int fall, int max_faelle ){
  bool diff = false;
  for( ElementMap::iterator it = m_el_map.begin(); it != m_el_map.end(); it++ ){
    if( it->second != 0 ){
      DataContainer *cont = it->second->clone();
      cont->setCompareAttributesOfAllElements(  DATAisRightOnly, fall, max_faelle );
      cont->clearContainerExceptSortKey();
      dim.appendContainer( cont );
      diff = true;
    }
  }
  return diff;
}
