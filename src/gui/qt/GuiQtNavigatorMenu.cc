
#include <QMenu>

#include <assert.h>
#include "gui/GuiNavigator.h"
#include "gui/GuiFactory.h"
#include "gui/qt/GuiQtNavigatorMenu.h"

/*=============================================================================*/
/* Copy Constructor GuiQtNavigatorMenu                                         */
/*=============================================================================*/

GuiQtNavigatorMenu::GuiQtNavigatorMenu( const GuiQtNavigatorMenu& menu )
  : GuiQtPopupMenu( menu )
  , GuiNavigatorMenu( menu ) {
}

/*=============================================================================*/
/* create                                                                      */
/*=============================================================================*/

void GuiQtNavigatorMenu::create( GuiNavigator *parent, XferDataItem *xfer ){
  assert( parent != 0 );
  GuiElement::setParent( parent->getElement() );
  for(GuiElementList::iterator it = m_container.begin(); it != m_container.end(); ++it) {
    (*it)->setParent(this);
  }
  setXfer( xfer );
  GuiQtPopupMenu::create();
}

/*=============================================================================*/
/* create                                                                      */
/*=============================================================================*/
void GuiQtNavigatorMenu::popup( int x, int y, XferDataItem *xferSrc ){
  setXYPosition( x, y );
  setXferSource( xferSrc );
  m_xferSrcList.clear();
  GuiQtPopupMenu::popup();
}

/*=============================================================================*/
/* create                                                                      */
/*=============================================================================*/
void GuiQtNavigatorMenu::popup( int x, int y, const std::vector<XferDataItem*> xferSrcList ){
  setXYPosition( x, y );
  m_xferSrcList = xferSrcList;
  setXferSource( 0 );
  GuiQtPopupMenu::popup();
}
