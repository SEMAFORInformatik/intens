
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "app/DataPoolUndoListener.h"
#include "utils/Debugger.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataPoolUndoListener::DataPoolUndoListener( bool undo )
  : m_is_undo_listener( undo ){
}

DataPoolUndoListener::~DataPoolUndoListener(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* undoStatus --                                                               */
/* --------------------------------------------------------------------------- */


void DataPoolUndoListener::undoStatus( bool enabled ){
  BUG_PARA( BugUndo, "undoStatus", "enabled=" << BugBool( enabled ) );

  if( enabled ){
    allow();
  }
  else{
    disallow();
  }
}

/* --------------------------------------------------------------------------- */
/* redoStatus --                                                               */
/* --------------------------------------------------------------------------- */


void DataPoolUndoListener::redoStatus( bool enabled ){
  BUG_PARA( BugUndo, "redoStatus", "enabled=" << BugBool( enabled ) );

  if( enabled ){
    allow();
  }
  else{
    disallow();
  }
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */


void DataPoolUndoListener::ButtonPressed(){
  BUG( BugUndo, "ButtonPressed" );

  if( m_is_undo_listener ){
    DataPoolIntens::getDataPool().undo();
  }
  else{
    DataPoolIntens::getDataPool().redo();
  }
}
