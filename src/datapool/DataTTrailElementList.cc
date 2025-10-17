
#include <assert.h>

#include "datapool/DataTTrailElementList.h"
#include "datapool/DataTTrailElement.h"
#include "datapool/DataLogger.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataTTrailElementList);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailElementList::DataTTrailElementList()
  : m_max_elements( 0 ){
  BUG_INCR_COUNT;
}

DataTTrailElementList::~DataTTrailElementList(){
  clear();
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setMaxElements --                                                           */
/* --------------------------------------------------------------------------- */

void DataTTrailElementList::setMaxElements( int anzahl ){
  m_max_elements = anzahl;
}

/* --------------------------------------------------------------------------- */
/* insertElement --                                                            */
/* --------------------------------------------------------------------------- */

bool DataTTrailElementList::insertElement( DataTTrailElement *el_new ){
  //  BUG( BugTTrail, "insertElement" );

  bool rslt = m_element_list.empty();

  m_element_list.push_back( el_new );
  if( m_max_elements > 0 ){
    if( m_max_elements < (int)m_element_list.size() ){
      DataTTrailElement *el_old = (*m_element_list.begin());
      // BUG_MSG( "stack overflow: MaxElements = " << m_max_elements
      // 	       << " and " << m_element_list.size() << " Elements stacked."
      // 	       << " Element " << el_old->name() << " deleted" );
      delete el_old;
      m_element_list.pop_front();
    }
  }
  // BUG_EXIT( m_element_list.size() << " Elements stacked" );
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* getElement --                                                               */
/* --------------------------------------------------------------------------- */

DataTTrailElement *DataTTrailElementList::getElement(){
  BUG( BugTTrail, "getElement" );
  assert( !m_element_list.empty() );

  DataTTrailElement *el = (*m_element_list.rbegin());
  assert( el != 0 );

  m_element_list.pop_back();
  BUG_EXIT( "Element " << el->name() << " grabbed" );
  return el;
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataTTrailElementList::clear(){
  //  BUG( BugTTrail, "clear" );

  if( m_element_list.empty() ){
    //    BUG_EXIT( "empty list" );
    return false;
  }

  for( ElementList::iterator it = m_element_list.begin(); it != m_element_list.end(); ++it ){
    //    BUG_MSG( "delete element @" << (*it) );
    delete (*it);
  }
  m_element_list.clear();
  return true;
}

/* --------------------------------------------------------------------------- */
/* isEmpty --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataTTrailElementList::isEmpty() const{
  return m_element_list.empty();
}

/* --------------------------------------------------------------------------- */
/* isntEmpty --                                                                */
/* --------------------------------------------------------------------------- */

bool DataTTrailElementList::isntEmpty() const{
  return !m_element_list.empty();
}

/* --------------------------------------------------------------------------- */
/* size --                                                                     */
/* --------------------------------------------------------------------------- */

int DataTTrailElementList::size() const{
  return (int)m_element_list.size();
}
