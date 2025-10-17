
#include <qtimer.h>

#include <assert.h>
#include "gui/qt/QtSimpleAction.h"
#include "gui/qt/GuiQtManager.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

QtSimpleAction::QtSimpleAction(  SimpleAction::Object *obj )
  : SimpleAction( obj ){
  connect( this, SIGNAL( timeout() ), this, SLOT( workproc() ) );
  setSingleShot( true );
  start( 0 );
}

/* --------------------------------------------------------------------------- */
/* workproc --                                                                 */
/* --------------------------------------------------------------------------- */

void QtSimpleAction::workproc() {
  remove();
}
