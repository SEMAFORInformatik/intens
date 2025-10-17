
#include "job/JobIncludes.h"

#include "job/JobConfirmation.h"
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "operator/MessageQueueThreads.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobConfirmation::startJobAction(){

  GuiFactory::Instance()->showDialogConfirmation( m_element
						  , "Confirm"
						  , m_message
						  , this
						  , m_buttonText
						  , m_cancelBtn
				  );
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobConfirmation::stopJobAction(){
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void JobConfirmation::confirmYesButtonPressed(){
  endJobAction( JobAction::job_Ok );
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void JobConfirmation::confirmNoButtonPressed(){
  endJobAction( JobAction::job_Nok );
}

/* --------------------------------------------------------------------------- */
/* confirmCancelButtonPressed --                                               */
/* --------------------------------------------------------------------------- */

void JobConfirmation::confirmCancelButtonPressed(){
  endJobAction( JobAction::job_Canceled );
}
