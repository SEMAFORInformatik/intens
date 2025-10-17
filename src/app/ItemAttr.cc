
#include "app/ItemAttr.h"
#include "datapool/DataDictionary.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

ItemAttr::ItemAttr()
  : m_database_timestamp( 0 ){
}

ItemAttr::ItemAttr( DataDictionary * pDict, DataItem * pItem )
  : m_database_timestamp( 0 ){
}

ItemAttr::ItemAttr( const ItemAttr & attr )
  : m_database_timestamp( attr.m_database_timestamp ){
}

ItemAttr::~ItemAttr(){
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* NewClass --                                                                 */
/* --------------------------------------------------------------------------- */

DataItemAttr *ItemAttr::NewClass( DataDictionary *pDict, DataItem *pItem) const {
  return (pDict != 0 || pItem != 0) ? new ItemAttr( pDict, pItem )
                                    : new ItemAttr();
}

/* --------------------------------------------------------------------------- */
/* Clone --                                                                    */
/* --------------------------------------------------------------------------- */

DataItemAttr *ItemAttr::Clone(){
  ItemAttr *attr = new ItemAttr( *this );
  assert( attr != 0 );
  return attr;
}

/* --------------------------------------------------------------------------- */
/* setDataBaseTimestamp --                                                     */
/* --------------------------------------------------------------------------- */

void ItemAttr::setDataBaseTimestamp( TransactionNumber ts ){
  m_database_timestamp = ts;
}

/* --------------------------------------------------------------------------- */
/* DataBaseTimestamp --                                                        */
/* --------------------------------------------------------------------------- */

TransactionNumber ItemAttr::DataBaseTimestamp() const{
  return m_database_timestamp;
}

/* --------------------------------------------------------------------------- */
/* hasValidAttributes --                                                       */
/* --------------------------------------------------------------------------- */

bool ItemAttr::hasValidAttributes() const{
  return m_database_timestamp > 0;
}

/* --------------------------------------------------------------------------- */
/* clearValidAttributes --                                                     */
/* --------------------------------------------------------------------------- */

void ItemAttr::clearValidAttributes(){
  m_database_timestamp = 0;
}

/* --------------------------------------------------------------------------- */
/* writeDP --                                                                  */
/* --------------------------------------------------------------------------- */

void ItemAttr::writeDP( std::ostream &ostr ) const{
  ostr << " db_ts=\"" << m_database_timestamp << "\"";
}
