
#include "datapool/DataTTrailUndoDisable.h"
#include "datapool/DataPool.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailUndoDisable::DataTTrailUndoDisable( bool doit )
  : m_doit( doit ){
  BUG_PARA( BugUndo, "Constructor of DataTTrailUndoDisable", "Doit is " << BugBool(doit) );
  if( m_doit ){
    m_doit = DataPool::Instance()->disallowUndo();
  }
}

DataTTrailUndoDisable::~DataTTrailUndoDisable(){
  BUG( BugUndo, "Destructor of DataTTrailUndoDisable" );

  if( m_doit){
    DataPool::Instance()->allowUndo();
  }
}
