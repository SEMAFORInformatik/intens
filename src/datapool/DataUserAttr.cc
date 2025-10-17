
#include "datapool/DataUserAttr.h"

/*******************************************************************************/
/*                              DataUserAttr                                   */
/*******************************************************************************/

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataUserAttr::DataUserAttr()
: m_rundung( -1 )
, m_sortkey( false ){
}

DataUserAttr::DataUserAttr( DataDictionary *pDict )
: m_rundung( -1 )
, m_sortkey( false ){
}

DataUserAttr::~DataUserAttr(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* setRundung --                                                               */
/* --------------------------------------------------------------------------- */

void DataUserAttr::setRundung( int rnd ){
  m_rundung = rnd;
}

/* --------------------------------------------------------------------------- */
/* getRundung --                                                               */
/* --------------------------------------------------------------------------- */

int DataUserAttr::getRundung() const{
  return m_rundung;
}

/* --------------------------------------------------------------------------- */
/* setSortKey --                                                               */
/* --------------------------------------------------------------------------- */

void DataUserAttr::setSortKey(){
  m_sortkey = true;
}

/* --------------------------------------------------------------------------- */
/* isSortKey --                                                                */
/* --------------------------------------------------------------------------- */

bool DataUserAttr::isSortKey(){
  return m_sortkey;
}

/*******************************************************************************/
/*                          DefaultDataUserAttr                                */
/*******************************************************************************/

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DefaultDataUserAttr::DefaultDataUserAttr(){}
DefaultDataUserAttr::DefaultDataUserAttr( DataDictionary * pDict ){}
DefaultDataUserAttr::DefaultDataUserAttr( const DefaultDataUserAttr &attr ){}
DefaultDataUserAttr::~DefaultDataUserAttr(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* NewClass  --                                                                */
/* --------------------------------------------------------------------------- */

DataUserAttr *DefaultDataUserAttr::NewClass( DataDictionary *dict ) const{
  if( dict ){
    return new DefaultDataUserAttr( dict );
  }
  else{
    return new DefaultDataUserAttr();
  }
}

/* --------------------------------------------------------------------------- */
/* copyClass  --                                                               */
/* --------------------------------------------------------------------------- */

DataUserAttr *DefaultDataUserAttr::copyClass() const{
  return new DefaultDataUserAttr( *this );
}

/* --------------------------------------------------------------------------- */
/* fixupAttributes --                                                          */
/* --------------------------------------------------------------------------- */

void DefaultDataUserAttr::fixupAttributes( DataItem &item ){
  // amg: 2007-08-16 : wenn alle drei flags ungesetzt sind, sollte zumindest
  // das DATAIsReadOnly gesetzt sein

  //if( item.dictionary().GetDataType() != DataDictionary::type_StructVariable ){
  //  if( (m_attr_mask & DATAIsEditable) == 0 &&
  //      (m_attr_mask & DATAIsReadOnly) == 0 &&
  //      (m_attr_mask & DATAeditable) == 0      ) {
  //    setAttributes( DATAIsReadOnly, DATAIsEditable );
  //}
}
