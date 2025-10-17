
#ifndef SCALE_DIALOG_LISTENER_H
#define SCALE_DIALOG_LISTENER_H

class GuiEventData;

class ScaleDialogListener{
 public:
  virtual void resetEvent( GuiEventData *event ) = 0;
  virtual void closeEvent( GuiEventData *event ) = 0;
  virtual void scaleUpdate() = 0;
 private:
};

#endif
