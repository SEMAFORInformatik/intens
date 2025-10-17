#include "datapool/DataElement.h"
#include "datapool/DataPool.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataVector.h"
#include "datapool/DataAlterGetVector.h"
#include "datapool/DataReference.h"
#include "datapool/DataIntegerValue.h"
#include "datapool/DataRealValue.h"
#include "datapool/DataStringValue.h"
#include "datapool/DataComplexValue.h"
#include "datapool/DataStructValue.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataAlterGetVector::DataAlterGetVector( DataVector &v )
  : m_type( type_None )
  , m_vector( v ){
}

DataAlterGetVector::~DataAlterGetVector(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* check --                                                                    */
/* --------------------------------------------------------------------------- */

bool DataAlterGetVector::check( DataReference &ref ){
  m_type = ref.getDataType();

  if( ref.hasWildcardsNotLastLevel() ){
    return true; // nicht möglich
  }
  if( ref.numberOfWildcards() != 1 ){
    return true; // nicht möglich
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* alterItem --                                                                */
/* --------------------------------------------------------------------------- */

bool DataAlterGetVector::alterItem( DataItem &item
                                  , UpdateStatus &status
                                  , DataInx &inx
                                  , bool final )
{
  BUG( BugRef, "alterItem" );

  status = NoUpdate;

  if( final ){
    BUG_EXIT("indexed range does not exist." );
    return false; // abbrechen
  }

  BUG_EXIT( "alterItem: continue" );
  return true;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataAlterGetVector::UpdateStatus DataAlterGetVector::alterData( DataElement &el ){
  BUG( BugRef, "alterData" );

  BUG_EXIT("Element is available");

  DataValue::Ptr data( el.getDataValue() );
  m_vector.appendValue( data );
  return NoUpdate; // no update
}

/* --------------------------------------------------------------------------- */
/* nullElement --                                                              */
/* --------------------------------------------------------------------------- */

void DataAlterGetVector::nullElement( DataInx &inx, int num_dim ){
  DataValue::Ptr data;
  m_vector.appendValue( data ); // leer
}

  //switch( m_type ){
  //case type_Integer:
  //  data = new DataIntegerValue();
  //  break;
  //case type_Real:
  //  data = new DataRealValue();
  //  break;
  //case type_String:
  //case type_CharData:
  //  data = new DataStringValue();
  //  break;
  //case type_Complex:
  //  data = new DataComplexValue();
  //  break;
  //case type_StructVariable:
  //  data = new DataComplexValue();
  //  break;
  //default:
  //
  //}
