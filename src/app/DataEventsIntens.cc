
#include "app/DataEventsIntens.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataEventsIntens::DataEventsIntens(){}

DataEventsIntens::~DataEventsIntens(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* writeLogMessage  --                                                         */
/* --------------------------------------------------------------------------- */

void DataEventsIntens::writeLogMessage( const std::string &txt ){
  GuiFactory::Instance()->getLogWindow()->writeText( txt );
}

/* --------------------------------------------------------------------------- */
/* updateUserInterface  --                                                     */
/* --------------------------------------------------------------------------- */

void DataEventsIntens::updateUserInterface(){
  GuiManager::Instance().update( GuiElement::reason_Always );
}

/* --------------------------------------------------------------------------- */
/* dispatchPendingEvents  --                                                   */
/* --------------------------------------------------------------------------- */

void DataEventsIntens::dispatchPendingEvents(){
  GuiManager::Instance().dispatchPendingEvents();
}
