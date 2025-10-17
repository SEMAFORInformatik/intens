
#if !defined(GUI_QT_DIALOG_INCLUDED_H)
#define GUI_QT_DIALOG_INCLUDED_H

#include <assert.h>

#include <string>
#include "gui/GuiDialog.h"

class QDialog;

class GuiQtDialog : public GuiDialog
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtDialog();
  virtual ~GuiQtDialog() {}

/*=============================================================================*/
/* public member functions of GuiDialog                                        */
/*=============================================================================*/

  virtual void installWaitCursor( GuiDialog * );
  virtual void removeWaitCursor( GuiDialog * );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
 public:
  bool hasWaitInstaller(){ return m_wait_installer.size() > 0; }
  void setWaitCursorState( bool set ){ assert(false ); } // use install/removeWaitCursor
  virtual QDialog* getDialogWidget() = 0;
  virtual std::string getDialogName() = 0;

  static void installDialogsWaitCursor(GuiDialog* diag_modal=0);
  static void removeDialogsWaitCursor(GuiDialog* diag_modal=0);
  void readSettings();
  bool readFromSettings();
  bool getSettingsSize(int& width, int& height);
  void overrideSettingsSize(int width, int height);
  bool extendSize(int width, int height);
  int  getQtWindowStates() { return m_qtWindowStates; }
 protected:
  void setQtWindowStates(int state) { m_qtWindowStates = state; }
private:
  std::vector<GuiDialog*> m_wait_installer;
  bool  m_readFromSettings;
  int   m_readSettingWidth;
  int   m_readSettingHeight;
  int   m_qtWindowStates;
};

#endif
