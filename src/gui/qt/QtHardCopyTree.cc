
#include "gui/GuiPulldownMenu.h"
#include "app/UiManager.h"
#include "gui/qt/QtHardCopyTree.h"
#include "gui/qt/GuiQtSeparator.h"

/* --------------------------------------------------------------------------- */
/* addFolder --                                                                */
/* --------------------------------------------------------------------------- */

HardCopyFolder *QtHardCopyFolder::addFolder( const std::string &label ){
  HardCopyFolder *folder = new QtHardCopyFolder( label, this, tree() );
  HardCopyFolder::addElement( folder );
  return folder;
}

/* --------------------------------------------------------------------------- */
/* addObject --                                                                */
/* --------------------------------------------------------------------------- */

void QtHardCopyFolder::addObject( const std::string &label, HardCopyListener *listener ){
  assert( listener != 0 );
  if( !listener->showMenu() )
    return;
  std::string menu_label;
  // Der Parameter uebersteuert das MenuLabel des HardCopyListener's.
  // Er wird in der Regel vom Parser gesetzt.
  if( !label.empty() )
    menu_label = label;
  else
    menu_label = listener->MenuLabel();

  HardCopyObject *object = new QtHardCopyObject( menu_label, this, listener, tree() );
  listener->setHardCopyObject( object );
  HardCopyFolder::addElement( object );
}

/* --------------------------------------------------------------------------- */
/* addSeparator --                                                             */
/* --------------------------------------------------------------------------- */

void QtHardCopyFolder::addSeparator(){
  HardCopyFolder::addElement( new QtHardCopySeparator );
}

/* --------------------------------------------------------------------------- */
/* createMenu --                                                               */
/* --------------------------------------------------------------------------- */

void QtHardCopySeparator::createMenu( GuiPulldownMenu *menu, GuiEventData *event ){
  menu->getElement()->attach( new GuiQtSeparator(menu->getElement()) );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void QtHardCopyTree::serializeXML(std::ostream &os, bool recursive){
  HardCopyTree::serializeXML(os, recursive);
}
