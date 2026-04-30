
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(CONFIRMATION_LISTENER_H)
#define CONFIRMATION_LISTENER_H

class ConfirmationListener
{
/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed() = 0;
  virtual void confirmNoButtonPressed() = 0;
  virtual void confirmCancelButtonPressed() {}
};

#endif