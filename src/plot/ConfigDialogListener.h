
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#ifndef CONFIG_DIALOG_LISTENER_H
#define CONFIG_DIALOG_LISTENER_H

class GuiEventData;

class ConfigDialogListener{
 public:
  virtual void resetEvent( GuiEventData *event ) = 0;
  virtual void closeEvent( GuiEventData *event ) = 0;
 private:
};

#endif