
#include "datapool/DataItemAttr.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataItemAttr::DataItemAttr(){
}

DataItemAttr::DataItemAttr( const DataItemAttr & attr ){
}

DataItemAttr::DataItemAttr( DataDictionary *dict, DataItem *item ){
}

DataItemAttr::~DataItemAttr(){
}

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DefaultDataItemAttr::DefaultDataItemAttr(){
}

DefaultDataItemAttr::DefaultDataItemAttr( const DefaultDataItemAttr & attr ){
}

DefaultDataItemAttr::DefaultDataItemAttr( DataDictionary *dict, DataItem *item ){
}

DefaultDataItemAttr::~DefaultDataItemAttr(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* NewClass --                                                                 */
/* --------------------------------------------------------------------------- */

DataItemAttr *DefaultDataItemAttr::NewClass( DataDictionary *dict, DataItem *item ) const{
  return ( dict != 0 || item != 0 ) ? new DefaultDataItemAttr( dict, item )
                                    : new DefaultDataItemAttr();
}

/* --------------------------------------------------------------------------- */
/* Clone --                                                                    */
/* --------------------------------------------------------------------------- */

DataItemAttr *DefaultDataItemAttr::Clone(){
  return new DefaultDataItemAttr( *this );
}
