
#include <assert.h>

#include "datapool/DataTTrailUndo.h"
#include "datapool/DataTTrailUndoDisable.h"
#include "datapool/DataTTrailElement.h"
#include "datapool/DataTTrailUndoListener.h"
#include "datapool/DataPool.h"
#include "datapool/DataEvents.h"
#include "datapool/DataLogger.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataTTrailUndo::DataTTrailUndo()
  : m_is_activated( false )
  , m_is_enabled( false )
  , m_undo_lsnr( 0 )
  , m_redo_lsnr( 0 ){
  setMaxElements( 5 );
}

DataTTrailUndo::~DataTTrailUndo(){
  m_undo_list.clear();
  m_redo_list.clear();
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* activate --                                                                 */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::activate(){
  BUG( BugUndo, "activate" );

  sendToUndoListener( false );
  sendToRedoListener( false );

  m_is_activated = true;
}

/* --------------------------------------------------------------------------- */
/* isActivated --                                                              */
/* --------------------------------------------------------------------------- */

bool DataTTrailUndo::isActivated() const{
  return m_is_activated;
}

/* --------------------------------------------------------------------------- */
/* isntActivated --                                                            */
/* --------------------------------------------------------------------------- */

bool DataTTrailUndo::isntActivated() const{
  return !m_is_activated;
}

/* --------------------------------------------------------------------------- */
/* setMaxElements --                                                           */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::setMaxElements( int anzahl ){
  BUG_PARA( BugUndo, "setMaxElements", "Anzahl=" << anzahl );
  m_undo_list.setMaxElements( anzahl );
  m_redo_list.setMaxElements( anzahl );
}

/* --------------------------------------------------------------------------- */
/* attachUndoListener --                                                       */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::attachUndoListener( DataTTrailUndoListener *lsnr ){
  BUG( BugUndo, "attachUndoListener" );
  m_undo_lsnr = lsnr;
}

/* --------------------------------------------------------------------------- */
/* attachRedoListener --                                                       */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::attachRedoListener( DataTTrailUndoListener *lsnr ){
  BUG( BugUndo, "attachRedoListener" );
  m_redo_lsnr = lsnr;
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::enable( bool temporary ){
  BUG_PARA( BugUndo, "enable", "temporary = " << BugBool( temporary ) );

  if( isntActivated() ){
    BUG_EXIT( "Undo/Redo not activated" );
    return;
  }

  if( !m_is_enabled  ){
    m_is_enabled = true;

    if( !temporary ){
      m_undo_list.clear();
      sendUndoStatus( false );

      m_redo_list.clear();
      sendRedoStatus( false );
    }
    BUG_EXIT( "enabled" );
    return;
  }

  BUG_EXIT( "already enabled" );
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::disable( bool temporary ){
  BUG_PARA( BugUndo, "disable", "temporary = " << BugBool( temporary ) );

  if( isntActivated() ){
    BUG_EXIT( "Undo/Redo not activated" );
    return;
  }

  m_is_enabled = false;
  if( !temporary ){
    clear();
  }
  BUG_EXIT( "disabled" );
}

/* --------------------------------------------------------------------------- */
/* clear --                                                                    */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::clear(){
  BUG( BugUndo, "clear" );

  if( isActivated() ){
    if( m_undo_list.clear() ){
      sendUndoStatus( false );
    }
    if( m_redo_list.clear() ){
      sendRedoStatus( false );
    }
    BUG_EXIT( "Undo/Redo-Lists cleared" );
  }
}

/* --------------------------------------------------------------------------- */
/* isEnabled --                                                                */
/* --------------------------------------------------------------------------- */

bool DataTTrailUndo::isEnabled() const{
  if( isActivated() ){
    return m_is_enabled;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isDisabled --                                                               */
/* --------------------------------------------------------------------------- */

bool DataTTrailUndo::isDisabled() const{
  if( isActivated() ){
    return !m_is_enabled;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* insertElement --                                                            */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::insertElement( DataTTrailElement *el ){
  BUG_PARA( BugUndo, "insertElement", "[" << el->name() << "]" );

  if( isActivated() ){
    if( isEnabled() ){
      if( m_undo_list.insertElement( el ) ){
	sendUndoStatus( true );
      }
      if( m_redo_list.clear() ){
	sendRedoStatus( false );
      }
      BUG_EXIT( "element inserted. "
		<< m_undo_list.size() << " elements on the stack" );
    }
    else{
      BUG_EXIT( "Undo/Redo not enabled" );
    }
  }
  else{
    BUG_EXIT( "Undo/Redo not activated" );
  }
}

/* --------------------------------------------------------------------------- */
/* undo --                                                                     */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::undo( DataPool &datapool ){
  BUG( BugUndo, "undo" );

  if( isntActivated() ){
    // Der Undo-Button hat noch nicht gemerkt, dass es nichts zu tun gibt.
    // Undo-Redo ist nicht aktiviert.
    sendToUndoListener( false );
    sendToRedoListener( false );
    BUG_EXIT( "Undo/Redo not activated" );
    return;
  }
  if( m_undo_list.isEmpty() ){
    // Der Undo-Button hat noch nicht gemerkt, dass es nichts zu tun gibt.
    // Das sollte eigentlich nicht sein, aber wir sind geduldig mit ihm.
    sendToUndoListener( false );
    sendToRedoListener( m_redo_list.isntEmpty() );
    BUG_EXIT( "nothing to do" );
    return;
  }

  // Während dieser Aktion sollen keine Elemente im Undo-Stack landen !
  disable( true );

  DataTTrailElement *el = m_undo_list.getElement();
  BUG_MSG( "call restore for element [" << el->name() << "]" );
  el->restore( datapool );

  enable(true ); // Undo wieder aktiv

  if( m_undo_list.isEmpty() ){
    sendUndoStatus( false );
  }

  // Nun befindet sich der alte Zustand des DataItem's im Element. Dieses
  // ist bereit für ein Redo.
  if( m_redo_list.insertElement( el ) ){
    sendRedoStatus( true );
  }
  BUG_MSG( "Element " << el->name() << " restored. "
	   << m_undo_list.size() << " remaining elements" );

  // Beim Gui-Update werden unter Umständen auch im Datapool Daten verändert.
  // Diese dürfen nicht auf dem undo-Stack landen.
  DataTTrailUndoDisable undo_disabler(true);
  DataPool::getDataEvents().updateUserInterface();
}

/* --------------------------------------------------------------------------- */
/* redo --                                                                     */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::redo( DataPool &datapool ){
  BUG( BugUndo, "redo" );

  if( isntActivated() ){
    // Der Undo-Button hat noch nicht gemerkt, dass es nichts zu tun gibt.
    // Undo-Redo ist nicht aktiviert.
    sendToUndoListener( false );
    sendToRedoListener( false );
    BUG_EXIT( "Undo/Redo not activated" );
    return;
  }
  if( m_redo_list.isEmpty() ){
    // Der Redo-Button hat noch nicht gemerkt, dass es nichts zu tun gibt.
    // Das sollte eigentlich nicht sein, aber wir sind geduldig mit ihm.
    sendToRedoListener( false );
    sendToUndoListener( m_undo_list.isntEmpty() );
    BUG_EXIT( "nothing to do" );
    return;
  }

  // Während dieser Aktion sollen keine Elemente im Undo-Stack landen !
  disable( true );

  DataTTrailElement *el = m_redo_list.getElement();
  el->restore( datapool );

  enable(true ); // Undo wieder aktiv

  if( m_redo_list.isEmpty() ){
    sendRedoStatus( false );
  }

  // Nun befinden sich der alte Zustand des DataItem's im Element. Dieses
  // ist nun bereit für ein neues Undo.
  if( m_undo_list.insertElement( el ) ){
    sendUndoStatus( true );
  }
  BUG_MSG( "Element " << el->name() << " restored. "
	   << m_redo_list.size() << " remaining elements" );

  // Beim Gui-Update werden unter Umständen auch im Datapool Daten verändert.
  // Diese dürfen nicht auf dem undo-Stack landen.
  DataTTrailUndoDisable undo_disabler(true);
  DataPool::getDataEvents().updateUserInterface();
}

/* --------------------------------------------------------------------------- */
/* hasUndoData --                                                              */
/* --------------------------------------------------------------------------- */

bool DataTTrailUndo::hasUndoData() const{
  return m_undo_list.isntEmpty();
}

/* --------------------------------------------------------------------------- */
/* getLastUndoElement --                                                       */
/* --------------------------------------------------------------------------- */

DataTTrailElement *DataTTrailUndo::getLastUndoElement(){
  if( m_undo_list.isEmpty() ){
    return 0;
  }
  return m_undo_list.getElement();
}

/* --------------------------------------------------------------------------- */
/* hasRedoData --                                                              */
/* --------------------------------------------------------------------------- */

bool DataTTrailUndo::hasRedoData() const{
  return m_redo_list.isntEmpty();
}

/* --------------------------------------------------------------------------- */
/* sendUndoStatus --                                                           */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::sendUndoStatus( bool enabled ){
  BUG_PARA( BugUndo, "sendUndoStatus", "enabled=" << BugBool( enabled ) );

  if( isActivated() ){
    sendToUndoListener( enabled );
  }
  else{
    BUG_EXIT( "not activated" );
  }
}

/* --------------------------------------------------------------------------- */
/* sendToUndoListener --                                                       */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::sendToUndoListener( bool enabled ){
  BUG_PARA( BugUndo, "sendToUndoListener", "enabled=" << BugBool( enabled ) );

  if( m_undo_lsnr != 0 ){
    m_undo_lsnr->undoStatus( enabled );
    BUG_EXIT( "status sent" );
  }
}

/* --------------------------------------------------------------------------- */
/* sendRedoStatus --                                                           */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::sendRedoStatus( bool enabled ){
  BUG_PARA( BugUndo, "sendRedoStatus", "enabled=" << BugBool( enabled ) );

  if( isActivated() ){
    sendToRedoListener( enabled );
  }
  else{
    BUG_EXIT( "not activated" );
  }
}

/* --------------------------------------------------------------------------- */
/* sendToRedoListener --                                                       */
/* --------------------------------------------------------------------------- */

void DataTTrailUndo::sendToRedoListener( bool enabled ){
  BUG_PARA( BugUndo, "sendToRedoListener", "enabled=" << BugBool( enabled ) );

  if( m_redo_lsnr != 0 ){
    m_redo_lsnr->redoStatus( enabled );
    BUG_EXIT( "status sent" );
  }
}
