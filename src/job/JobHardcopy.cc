
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobHardcopy.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"

/*=============================================================================*/
/* Constructor                                                                 */
/*=============================================================================*/

JobHardcopy::JobHardcopy()
  : m_event(0)
  , m_element(0) {
  setLogOff();
}

/*=============================================================================*/
/* Destructor                                                                  */
/*=============================================================================*/

JobHardcopy::~JobHardcopy() {
  delete m_event;
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

void JobHardcopy::setElement( GuiElement* e ) {
  m_element = e;
}

void JobHardcopy::setHardCopyListener( HardCopyListener* hcl ) {
  m_hcl = hcl;
}

void JobHardcopy::setMode( ReportGen::Mode mode ) {
  if (AppData::Instance().HeadlessWebMode()) {
    mode = ReportGen::SAVE;
  }
  m_event = new GuiPrinterDialog::MyEventData( mode );
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobHardcopy::startJobAction(){
  BUG(BugJob,"JobHardcopy::executeJobController" );
  GuiFactory::Instance()->createPrinterDialog()->showDialog( m_hcl, m_element, m_event, this );
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */
void JobHardcopy::stopJobAction(){
   assert( false );
}
