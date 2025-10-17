
#ifndef GUI_TEXTDIALOG_LISTENER_INCLUDED_H
#define GUI_TEXTDIALOG_LISTENER_INCLUDED_H

/**
   Das Interface 'GuiTextDialogListener' dient als Listener fuer die Klasse
   'GuiTextDialogListener'.
*/
class GuiTextDialogListener {
public:
  /// Wird aufgerufen wenn der Button 'Ok' gedrueckt wurde
  virtual void ButtonOkPressed( char** text, int lines ) = 0;
  /// Wird aufgerufen wenn der Button 'Apply' gedrueckt wurde
  virtual void ButtonApplyPressed( char** text, int lines ) = 0;
  /// Wird aufgerufen wenn der Button 'Cancel' gedrueckt wurde
  virtual void ButtonCancelPressed() = 0;
};

#endif
