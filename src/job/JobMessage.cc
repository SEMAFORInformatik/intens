
#include "job/JobIncludes.h"

#include "job/JobEngine.h"
#include "job/JobMessage.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "operator/MessageQueueThreads.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

void JobMessage::setElement( GuiElement *el ) {
  m_element = el;
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobMessage::startJobAction(){
  BUG(BugJob,"JobMessage::executeJobController" );

  GuiFactory::Instance()->showDialogInformation( m_element
                               , m_title.empty() ? "Inform" : m_title
	         		                 , m_message
			                         , this
			                         );
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobMessage::stopJobAction(){
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* confirmOkButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void JobMessage::informOkButtonPressed(){
  BUG(BugJob,"JobMessage::informOkButtonPressed" );
  endJobAction( JobAction::job_Ok_ignore );
}
