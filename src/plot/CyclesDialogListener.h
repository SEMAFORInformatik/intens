
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#ifndef CYCLES_DIALOG_LISTENER_H
#define CYCLES_DIALOG_LISTENER_H

class GuiEventData;

class CyclesDialogListener{
 public:
  virtual void closeEvent( GuiEventData *event ) = 0;
  virtual void cyclesUpdate() = 0;
 private:
};

#endif