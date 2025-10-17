
#if !defined(GUI_DIALOG_INCLUDED_H)
#define GUI_DIALOG_INCLUDED_H


#include <string>
#include <stack>

class GuiElement;

class GuiDialog
{
public:
  GuiDialog() :
    m_application_modal( false )
    , m_parent_dialog( 0 )
    , m_waiting( false )
    , m_wait_installer( 0 ){}
  ~GuiDialog() { }

  virtual void setApplicationModal();
  virtual void resetApplicationModal();
  bool isApplicationModal() { return m_application_modal; }

  void installWaitCursor(){
    installWaitCursor( this ); }
  virtual void installWaitCursor( GuiDialog *installer );

  void removeWaitCursor(){
    removeWaitCursor( this ); }
  virtual void removeWaitCursor( GuiDialog *installer );

  void removeParentWaitCursor(){
    removeParentWaitCursor( this ); }
  void removeParentWaitCursor( GuiDialog *installer );

  void installParentWaitCursor(){
    installParentWaitCursor( this ); }
  void installParentWaitCursor( GuiDialog *installer );

  virtual void setWaitCursorState( bool state ) = 0;
  virtual bool isWaiting() { return m_waiting; }
  void setWaiting( bool waiting ){ m_waiting = waiting; }

  virtual void setDialogTitle( const std::string & );
  const std::string&  getDialogTitle();

  void setParentDialog( GuiDialog *dialog );
  GuiDialog *parentDialog(){ return m_parent_dialog; }

/*=============================================================================*/
/* public member functions for static parent dialog list                       */
/*=============================================================================*/
  /** Der GuiDialog dient als Container des parent GuiDialog für modale Dialoge,
      welche nicht GuiForm sind. Diese holen sich mit getParentDialog() den
      parent. Der Aufrufer eines solchen Dialogs muss sich zuerst als parent an-
      gemeldet haben.
  */
  static void pushParentDialog( GuiDialog *dialog );
  static void popParentDialog( GuiDialog *dialog );
  static GuiDialog *getParentDialog( GuiElement *el = 0 );

protected:
  bool hasWaitInstaller(){ return m_wait_installer != 0; }
  void setWaitInstaller( void * i ){ m_wait_installer = i; }
  void *getWaitInstaller(){ return m_wait_installer; }

private:
  bool          m_application_modal;
  bool          m_waiting;
  std::string   m_title;
  GuiDialog    *m_parent_dialog;
  void         *m_wait_installer;

  /*=============================================================================*/
  /* private Data                                                                */
  /*=============================================================================*/
private:
  typedef std::stack<GuiDialog *> GuiDialogStack;
  static GuiDialogStack   s_dialog_stack;

};

class DialogUnmapListener
{
public:
  DialogUnmapListener() {}
  virtual ~DialogUnmapListener() {}
  virtual void DialogIsUnmapped() = 0;
};

#endif
