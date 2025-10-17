
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
