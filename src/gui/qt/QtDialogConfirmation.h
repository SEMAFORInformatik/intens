
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(QTDIALOG_CONFIRMATION_H)
#define QTDIALOG_CONFIRMATION_H

#include "gui/ConfirmationListener.h"
#include "gui/GuiElement.h"

class QtDialogConfirmation
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  QtDialogConfirmation( ConfirmationListener *listener );
  virtual ~QtDialogConfirmation() {}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static GuiElement::ButtonType showDialog( GuiElement *, const std::string &, const std::string &
                                            , ConfirmationListener *
                                            , const std::map<GuiElement::ButtonType, std::string>& buttonText
                                            , bool withCancelBtn=false
                                            , int defaultBtn=1 );
};

#endif