
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/GuiElement.h"

GuiFactory *   GuiFactory::s_factory = 0;

/* --------------------------------------------------------------------------- */
/* getTopLevelElement --                                                       */
/* --------------------------------------------------------------------------- */

GuiElement* GuiFactory::getTopLevelElement() {
  return GuiManager::Instance().getElement();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiFactory::update( GuiElement::UpdateReason reason, bool omitAfterUpdateForms) {
  if (omitAfterUpdateForms) GuiManager::Instance().omitNextAfterUpdateForms();
  GuiManager::Instance().update( reason );
}

/* --------------------------------------------------------------------------- */
/* haveFolder --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiFactory::haveFolder(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* haveList --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiFactory::haveList(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* haveTable --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiFactory::haveTable(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* have3dPlot --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiFactory::have3dPlot(){
  return true;
}
