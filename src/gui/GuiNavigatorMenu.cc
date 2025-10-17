
#include "utils/Debugger.h"
#include "gui/GuiNavigatorMenu.h"
#include "xfer/XferDataItem.h"

/*=============================================================================*/
/* constructor / destructor                                                    */
/*=============================================================================*/
GuiNavigatorMenu::GuiNavigatorMenu()
  : m_xfer( 0 )
  , m_xferSrc( 0 )
  , m_xpos( -1 )
  , m_ypos( -1 ){
}

GuiNavigatorMenu::GuiNavigatorMenu( const GuiNavigatorMenu& menu )
  : m_xfer( 0 )
  , m_xferSrc( menu.m_xferSrc )
  , m_xpos( -1 )
  , m_ypos( -1 ){
  if (menu.m_xfer) {
    setXfer( menu.m_xfer );
  }
}

GuiNavigatorMenu::~GuiNavigatorMenu(){
  if( m_xfer ){
    delete m_xfer;
    m_xfer = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* setXfer --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiNavigatorMenu::setXfer( XferDataItem *xfer ){
  assert( xfer );
  // Es wird immer eine Kopie angelegt, damit es keine fatalen Side-Effekte
  // gibt, wenn der Besitzer des Objekts etwas anderes vorhat.
  if( m_xfer ){
    delete m_xfer;
    m_xfer = 0;
  }
  m_xfer = new XferDataItem( *xfer );
}

/* --------------------------------------------------------------------------- */
/* getXfer --                                                                  */
/* --------------------------------------------------------------------------- */
XferDataItem *GuiNavigatorMenu::getXfer(){
  assert( m_xfer != 0 );
  // Das ist eine unschöne Sache, da wir die Kontrolle verlieren und nicht
  // wissen, was mit dem Objekt passiert. (Design-Fehler !!)
  return m_xfer;
}

/* --------------------------------------------------------------------------- */
/* setXferSource --                                                            */
/* --------------------------------------------------------------------------- */
void GuiNavigatorMenu::setXferSource( XferDataItem *xfer ){
  // Es wird immer eine Kopie angelegt, damit es keine fatalen Side-Effekte
  // gibt, wenn der Besitzer des Objekts etwas anderes vorhat.
  if( m_xferSrc ){
    delete m_xferSrc;
    m_xferSrc = 0;
  }
  if( xfer ){
    m_xferSrc = new XferDataItem( *xfer );
  }
}

/* --------------------------------------------------------------------------- */
/* getXferSource --                                                            */
/* --------------------------------------------------------------------------- */
XferDataItem *GuiNavigatorMenu::getXferSource(){
  // Das ist eine unschöne Sache, da wir die Kontrolle verlieren und nicht
  // wissen, was mit dem Objekt passiert. (Design-Fehler !!)
  return m_xferSrc;
}

/* --------------------------------------------------------------------------- */
/* setXYPosition --                                                            */
/* --------------------------------------------------------------------------- */
void GuiNavigatorMenu::setXYPosition( int x, int y ){
  m_xpos = x;
  m_ypos = y;
}
