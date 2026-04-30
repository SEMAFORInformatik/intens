
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#include "job/JobIncludes.h"
#include "job/JobCodeClearTextWindow.h"
#include "job/JobEngine.h"
#include "gui/GuiScrolledText.h"

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeClearTextWindow::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeClearTextWindow::execute");
  if( _tw )
    _tw -> clearText();
  return op_Ok;
}