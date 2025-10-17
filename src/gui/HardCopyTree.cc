
#include "gui/GuiPulldownMenu.h"
#include "app/UiManager.h"
#include "gui/HardCopyTree.h"
#include "gui/GuiEventData.h"

/* --------------------------------------------------------------------------- */
/* createMenu --                                                               */
/* --------------------------------------------------------------------------- */

void HardCopyTree::createMenu( GuiPulldownMenu *menu, GuiEventData *event ){
  HardCopyElement::ElementList::const_iterator it;
  for( it = folder()->elements().begin(); it != folder()->elements().end(); ++it ){
    (*it)->createMenu( menu, event );
  }
}

/* --------------------------------------------------------------------------- */
/* createMenu --                                                               */
/* --------------------------------------------------------------------------- */

void HardCopyFolder::createMenu( GuiPulldownMenu *menu, GuiEventData *event ){
  menu = UImanager::Instance().createNewSubMenu( menu->getElement(), label() );

  ElementList::iterator it;
  for( it = m_elements.begin(); it != m_elements.end(); ++it ){
    (*it)->createMenu( menu, event );
  }
}

/* --------------------------------------------------------------------------- */
/* createMenu --                                                               */
/* --------------------------------------------------------------------------- */

void HardCopyObject::createMenu( GuiPulldownMenu *menu, GuiEventData *event ){
  m_listener->setLabel( label() );
  m_listener->attach( menu->getElement(), event ); // GuiMenuButtonListener
}

/* --------------------------------------------------------------------------- */
/* addElement --                                                               */
/* --------------------------------------------------------------------------- */

void HardCopyFolder::addElement( HardCopyElement *e ){
  m_elements.push_back( e );
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void HardCopyTree::serializeXML(std::ostream &os, bool recursive){
}
