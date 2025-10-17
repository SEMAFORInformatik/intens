
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
