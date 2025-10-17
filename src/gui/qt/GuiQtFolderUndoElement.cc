
#include "app/AppData.h"
#include "gui/qt/GuiQtFolderUndoElement.h"
#include "app/DataPoolIntens.h"

bool GuiQtFolderUndo::s_activ = false;

/*=============================================================================*/
/* Constructor / Destructor of GuiQtFolderUndo                                 */
/*=============================================================================*/

GuiQtFolderUndo::GuiQtFolderUndo( GuiQtFolder *fol
				, int new_page
				, int old_page )

  : m_activated( false )
  , m_folder( fol )
  , m_new_page( new_page )
  , m_old_page( old_page )
  , m_action( GuiQtFolderUndoElement::SWITCH ){
  BUG( BugGuiFolder, "Constructor of GuiQtFolderUndo" );
  if( !s_activ ){
    m_activated = true;
    s_activ     = true;
    BUG_EXIT( "activated" );
  }
}

GuiQtFolderUndo::~GuiQtFolderUndo(){
  BUG( BugGuiFolder, "Destructor of GuiQtFolderUndo" );
  if( m_activated ){
    s_activ = false;
    if( m_folder == 0 ){
      BUG_EXIT( "no folder" );
    }
    else{
      GuiQtFolderUndoElement *undo =
	new GuiQtFolderUndoElement( m_folder, m_old_page, m_new_page, m_action );
      DataPoolIntens::getDataPool().saveTTrailActionElement( undo );
      BUG_EXIT( "deactivated" );
    }
  }
}

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtFolderUndoElement::GuiQtFolderUndoElement( GuiQtFolder *fol
					      , int new_page
					      , int old_page
					      , Action a )
  : m_folder( fol )
  , m_new_page( new_page )
  , m_old_page( old_page )
  , m_action( a ){
  BUG( BugGuiFolder, "Constructor of GuiQtFolderUndoElement" );
}

GuiQtFolderUndoElement::~GuiQtFolderUndoElement(){
  BUG( BugGuiFolder, "Destructor of GuiQtFolderUndoElement" );
}

/*=============================================================================*/
/* Member Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* restore --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtFolderUndoElement::restore( DataPool &dp ){
  BUG( BugGuiFolder, "restore" );

  // Block Undo
  GuiQtFolderUndo undo( 0, 0, 0 );
  int page = m_old_page;

  switch( m_action ){
  case SWITCH:
    BUG_MSG( "switch from page #" << m_old_page << " to #" << m_new_page );
    m_folder->activatePage( m_new_page, GuiFolderGroup::omit_Default, false );
    // exchange page
    m_old_page = m_new_page;
    m_new_page = page;
    break;
  case MAP:
    BUG_MSG( "map page #" << m_old_page );
    m_folder->activatePage( m_old_page, GuiFolderGroup::omit_Default, false );
    m_action = UNMAP;
    break;
  case UNMAP:
    BUG_MSG( "unmap page #" << m_old_page );
    m_folder->hidePage( m_old_page );
    if( m_new_page !=  m_old_page &&  m_new_page >= 0 ){
      m_folder->activatePage( m_new_page, GuiFolderGroup::omit_Default, false );
    }
    m_action = MAP;
    break;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtFolderUndoElement::print( std::ostream &ostr ) const{
  ostr << "change Folder from page #" << m_old_page << " to #" << m_new_page;
}

/* --------------------------------------------------------------------------- */
/* name --                                                                     */
/* --------------------------------------------------------------------------- */

const std::string GuiQtFolderUndoElement::name() const{
  std::ostringstream msg;
  print( msg );
  return msg.str();
}
