
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(GUI_CYCLEDIALOG_INCLUDED_H)
#define GUI_CYCLEDIALOG_INCLUDED_H

#include "gui/GuiButtonListener.h"
#include "gui/GuiScrolledlistListener.h"

/**The GuiCycleDialog creates, clears or deletes Cycles
  */
class GuiCycleDialog : public GuiButtonListener, public GuiScrolledlistListener{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiCycleDialog();
  ~GuiCycleDialog();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  JobAction* getAction(){ return 0; }
/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
};


#endif