
#include <iostream>
#include "datapool/DataEvents.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataEvents::DataEvents(){}

DataEvents::~DataEvents(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* writeLogMessage  --                                                         */
/* --------------------------------------------------------------------------- */

void DataEvents::writeLogMessage( const std::string &txt ){
#if defined(__INTENS_CODE__)
  GuiFactory::Instance()->getLogWindow()->writeText( txt );
#else
  std::cerr << txt << std::endl;
#endif
}

/* --------------------------------------------------------------------------- */
/* updateUserInterface  --                                                     */
/* --------------------------------------------------------------------------- */

void DataEvents::updateUserInterface(){
#if defined(__INTENS_CODE__)
  GuiManager::Instance().update( GuiElement::reason_Always );
#endif
}

/* --------------------------------------------------------------------------- */
/* dispatchPendingEvents  --                                                   */
/* --------------------------------------------------------------------------- */

void DataEvents::dispatchPendingEvents(){
#if defined(__INTENS_CODE__)
  GuiManager::Instance().dispatchPendingEvents();
#endif
}
