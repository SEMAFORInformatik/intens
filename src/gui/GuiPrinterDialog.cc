
#include "app/HardCopyListener.h"
#include "gui/GuiPrinterDialog.h"
#include "gui/GuiElement.h"

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiPrinterDialog::GuiPrinterDialog()
  : m_data_index( 0 )
  , m_dialog( 0 )
  , m_print_menu_with_remaining_entries( true )
  , m_currentListener( 0 ){
}

/* --------------------------------------------------------------------------- */
/* getHardcopy --                                                              */
/* --------------------------------------------------------------------------- */
HardCopyListener *GuiPrinterDialog::getHardCopy( const std::string &name ){
  HardCopyList::iterator hi = m_hardcopylist.find( name );
  if( hi == m_hardcopylist.end() ){
    return 0;
  }
  // Der Parser braucht diese Funktion zum Generieren des gewuenschten Menus.
  (*hi).second->setMenuInstalled();
  return (*hi).second;
}

/* --------------------------------------------------------------------------- */
/* insertHardcopy --                                                           */
/* --------------------------------------------------------------------------- */
void GuiPrinterDialog::insertHardcopy( const std::string &name, HardCopyListener *hdc ){
  HardCopyList::iterator hi = m_hardcopylist.find( name );
  if( hi == m_hardcopylist.end() ){
    hdc->setIndex( ++m_data_index );
    m_hardcopylist.insert( HardCopyList::value_type( name, hdc ) );
  }
}

/* --------------------------------------------------------------------------- */
/* removeHardcopy --                                                           */
/* --------------------------------------------------------------------------- */
void GuiPrinterDialog::removeHardcopy( HardCopyListener *hdc ){
  HardCopyList::iterator hi;
  for( hi = m_hardcopylist.begin(); hi != m_hardcopylist.end(); ++hi ){
    if( (*hi).second == hdc ){
      m_hardcopylist.erase( hi );
      return;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* setHardCopy --                                                              */
/* --------------------------------------------------------------------------- */
void GuiPrinterDialog::setHardCopy( const std::string &name ){
  HardCopyListener *hdc = 0;
  HardCopyList::iterator hi = m_hardcopylist.find( name );
  if( hi != m_hardcopylist.end() ){
    hdc = (*hi).second;
  }
  if( m_dialog == 0 ){
    m_currentListener = hdc;
  }
  else{
    selectHardCopyObject( hdc );
  }
}

/* --------------------------------------------------------------------------- */
/* createPrintMenu --                                                          */
/* --------------------------------------------------------------------------- */
void GuiPrinterDialog::createPrintMenu( GuiPulldownMenu *menu, bool preview ){
  MyEventData *event = 0;
  if( preview )
    event = new MyEventData( ReportGen::PREVIEW );
  else
    event = new MyEventData( ReportGen::PRINT );
  installRemainingHardCopies();
  m_hardcopytree->createMenu( menu, event );
}

/* --------------------------------------------------------------------------- */
/* installRemainingHardCopies --                                               */
/* --------------------------------------------------------------------------- */
void GuiPrinterDialog::installRemainingHardCopies(){
  if( !m_print_menu_with_remaining_entries ){
    return;
  }

  HardCopyList::const_iterator hi;
  for( hi = hardcopylist().begin(); hi != hardcopylist().end(); ++hi ){
    if( !(*hi).second->isMenuInstalled() ){
      assert( m_hardcopytree != 0 );
      m_hardcopytree->addObject( "", (*hi).second );
      (*hi).second->setMenuInstalled();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* getFolder --                                                                */
/* --------------------------------------------------------------------------- */

HardCopyFolder *GuiPrinterDialog::getFolder( bool with_remaining ){
  m_print_menu_with_remaining_entries = with_remaining;
  return m_hardcopytree->folder();
}
