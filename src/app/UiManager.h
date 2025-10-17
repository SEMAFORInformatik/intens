
#if !defined(UIMANAGER_H)
#define UIMANAGER_H

#include <map>
#include <vector>

#include "utils/Debugger.h"
#include "gui/GuiElement.h"
#include "gui/GuiToggleListener.h"
#include "gui/GuiMenuButtonListener.h"
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "app/DataPoolUndoListener.h"
#include "job/JobActionSimple.h"

class GuiForm;
class GuiApplHeader;
class GuiMenubar;
class GuiButtonbar;
class GuiPulldownMenu;
class HardCopyListener;
class JobTask;
class ProcessGroup;
class DbFunction;

typedef std::map<std::string, GuiMenuButtonListener *> FileButtonList;

/** Der UImanager ist ein Singleton-Object. Er kontrolliert die gesammte
    Applikation INTENS.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: UiManager.h,v 1.43 2006/03/08 14:56:02 amg Exp $
*/
class UImanager
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  UImanager();
  virtual ~UImanager();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static UImanager &Instance();
  void setApplicationTitle( const std::string & title );

  /** Diese Funktion createNewMenu() erstellt ein neues Menu. Gleichzeitig
      wird es in der Menubar der Main-Form angehängt.
      @param title Beschriftung des Menus
      @return Pointer auf das neue GuiPulldownMenu-Objekt
  */
  GuiPulldownMenu *createNewMenu( const std::string &title );

  /** Diese Funktion createNewMenu() erstellt ein neues Menu. Es wird nirgends
      angehängt. Dies muss nachgeholt werden. Das neue Menu wird für mögliche
      spätere Zugriffe dem GuiManager übergeben.
      @param name Eindeutiger Name des Menus für den GuiManager
      @param title Beschriftung des Menus
      @return Pointer auf das neue GuiPulldownMenu-Objekt
  */
  GuiPulldownMenu *createNewMenu( const std::string &name, const std::string &title );

  /** Diese Funktion liefert den Pointer auf ein bereits vorhandenes Menu oder
      SubMenu. Somit wird dem Parser ermöglicht, bereits vorhandene Menus zu
      erweitern.
      @param name Eindeutiger Name des Menus für den GuiManager
      @return Pointer auf das vorhandene GuiPulldownMenu-Objekt
  */
  GuiPulldownMenu *getExistingMenu( const std::string &name );

  /** Diese Funktion createNewSubMenu() erstellt ein neues Menu und hängt es an
      das mitgelieferte Menu.
      @param parent Pointer auf das Parent-GuiPulldownMenu-Objekt
      @param title Beschriftung des Menus
      @return Pointer auf das neue GuiPulldownMenu-Objekt
  */
  GuiPulldownMenu *createNewSubMenu( GuiElement *parent, const std::string &title );

  /** Diese Funktion createNewSubMenu() erstellt ein neues Menu und hängt es an
      das mitgelieferte Menu. Das neue Menu wird für mögliche
      spätere Zugriffe dem GuiManager übergeben.
      @param parent Pointer auf das Parent-GuiPulldownMenu-Objekt
      @param name Eindeutiger Name des Menus
      @param title Beschriftung des Menus
      @return Pointer auf das neue GuiPulldownMenu-Objekt
  */
  GuiPulldownMenu *createNewSubMenu( GuiElement *parent, const std::string &name, const std::string &title );

  /** Die Funktion attachMenu() hängt ein Menu an die Menubar der Main-Form,
      sofern das Menu nicht bereits anderswo installiert ist.
      @param menu Pointer auf das GuiPulldownMenu-Objekt
  */
  void attachMenu( GuiElement *menu );

  bool addFormMenuButton( GuiElement *, const std::string &, const std::string &, bool );
  bool addFunctionMenuButton( GuiElement *, const std::string &, const std::string & );
  bool addFunctionNavigatorMenuButton( GuiElement *, const std::string &, const std::string & );
  bool addTaskMenuButton( GuiElement *, const std::string &, const std::string & );
  void addTaskActionButton( JobTask *task );
  bool addProcessMenuButton( GuiElement *, const std::string &, const std::string & );
  void addProcessActionButton( ProcessGroup *group );

  GuiPulldownMenu *getOpenMenu( bool with_remaining = true );
  GuiPulldownMenu *getSaveMenu( bool with_remaining = true );
  GuiPulldownMenu *getPrintMenu() { return m_print_menu; }
  GuiPulldownMenu *getPreviewMenu() { return m_preview_menu; }

  bool addOpenButton( const std::string &name, GuiMenuButtonListener *listener );
  FileButtonList *getOpenButtonList();
  bool addSaveButton( const std::string &name, GuiMenuButtonListener *listener );
  FileButtonList *getSaveButtonList();
  bool createFileButton( GuiElement*, const std::string &name, const std::string &label, FileButtonList * );

  bool addHardCopy( const std::string &, HardCopyListener *hdc );

  void addHelp( const std::string &name, GuiMenuButtonListener *lsnr );
  //  ActionButton *addHelpCallback( const std::string &, ActionButtonProc, XtPointer );

  void initApplication( int *argc, char **argv, const std::string &classname );
  void createApplication();
  void startApplication();
  void closeDescriptionFunction();
  /** add description function argument used by descripton file logging */

  std::string addDescriptionFunctionArgument(std::string& key, std::string& value);
  /** set description function name used by descripton file logging */
  void setDesFuncName(const std::string& funcName) { m_desFuncName = funcName; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void createBasicElements();
  void createFileMenu( GuiMenubar * );
  void createEditMenu( GuiMenubar * );
  void createFormMenu( GuiMenubar * );
  void createProcessMenu( GuiMenubar * );
  void createOptionMenu();
  void createHelpMenu();
  void createOpenMenuButtons();
  void createSaveMenuButtons();
  void createPreviewMenuButtons();
  void createPrintMenuButtons();
  void setMainTitle();
  void addCopyrightDialog();
  void addOptions();
  void installQuitApplication();
  void finalAttachments();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class UnitManagerListener : public GuiButtonListener{
   public:
    UnitManagerListener() : m_element(0) {}
     virtual ~UnitManagerListener() {}

    virtual void ButtonPressed();
    JobAction* getAction(){ return 0; }
   private:
    GuiElement *m_element;

  };
  class HelpmsgListener : public GuiToggleListener
  {
  public:
  HelpmsgListener(AppData::HelpMessageType type) : m_helpMessageType(type) {}
    virtual void ToggleStatusChanged( bool state );
  private:
    AppData::HelpMessageType m_helpMessageType;
  };
  class DebugDescriptionFileListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state );
  };
  class DebugPythonLogFileListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state );
  };
  class DebugGuiListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Gui", state ); }
  };
  class DebugGuiFldListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "GuiFld", state ); }
  };
  class DebugGuiFolderListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "GuiFolder", state ); }
  };
  class DebugGuiTableListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "GuiTable", state ); }
  };
  class DebugGuiTgrListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "GuiTrigger", state ); }
  };
  class DebugGuiIconListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "GuiIcon", state ); }
  };
  class DebugGuiMgrListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "GuiMgr", state ); }
  };
  class DebugGuiWorkerListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "GuiWorker", state ); }
  };
  class DebugXferListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Xfer", state ); }
  };
  class DebugDBManagerListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "DBManager", state ); }
  };
  class DebugDBFilterListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "DBFilter", state ); }
  };
  class DebugDBCuratorListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "DBCurator", state ); }
  };
  class DebugDBTimestampListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "DBTimestamp", state ); }
  };
  class DebugOperatorListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Operator", state ); }
  };
  class DebugStreamerListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Streamer", state ); }
  };
  class DebugTargetListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Targets", state );
        if( state ) DataPoolIntens::Instance().printTargetContainer();
      }
  };
  class DebugJobListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Job", state ); }
  };
  class DebugJobStartListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "JobStart", state ); }
  };
  class DebugJobCodeListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "JobCode", state ); }
  };
  class DebugJobStackListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "JobStack", state ); }
  };
  class DebugPlotListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Plot", state ); }
  };
  class DebugDataRefListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Ref", state ); }
  };
  class DebugDatapoolListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Data", state ); }
  };
  class DebugUndoListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "Undo", state ); }
  };
  class DebugTTrailListener : public GuiToggleListener
  {
    virtual void ToggleStatusChanged( bool state )
      { Debugger::ModifyDebugFlag( "TTrail", state ); }
  };
  class DebugTTrailMemListener : public GuiMenuButtonListener
  {
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  };
  class DebugPrintDataListener : public GuiMenuButtonListener
  {
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  };
  class DebugPrintDictListener : public GuiMenuButtonListener
  {
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  };
  class QtDialogListener : public GuiMenuButtonListener
  {
  public:
    QtDialogListener( const char* dialog ) : m_dialog(dialog) {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    std::string    m_dialog;
  };

  // Qt Style Listener
  class QtStyleListener : public GuiToggleListener
  {
  public:
    QtStyleListener( const std::string& stylename ) : m_styleString(stylename) {}
    virtual void ToggleStatusChanged( bool state );
  private:
    std::string    m_styleString;
  };

  // Qt StyleSheet Listener
  class QtStyleSheetListener : public GuiToggleListener
  {
  public:
    QtStyleSheetListener( const std::string& stylesheetname ) : m_styleSheetString(stylesheetname) {}
    virtual void ToggleStatusChanged( bool state );
  private:
    std::string    m_styleSheetString;
  };

  class ActionEnableUndo : public JobActionSimple
  {
  public:
    ActionEnableUndo() : JobActionSimple("Enable Undo"){}
    ~ActionEnableUndo(){}
  protected:
    virtual void executeSimpleAction();
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  static UImanager  *s_instance;

  GuiForm           *m_main_form;
  GuiApplHeader     *m_main_titlebar;
  GuiMenubar        *m_main_menubar;
  GuiPulldownMenu   *m_file_menu;
  GuiPulldownMenu   *m_open_menu;
  GuiPulldownMenu   *m_save_menu;
  GuiPulldownMenu   *m_print_menu;
  GuiPulldownMenu   *m_preview_menu;
  GuiPulldownMenu   *m_edit_menu;
  GuiPulldownMenu   *m_form_menu;
  GuiPulldownMenu   *m_process_menu;
  GuiPulldownMenu   *m_option_menu;
  GuiPulldownMenu   *m_help_menu;
  GuiButtonbar      *m_main_buttonbar;
  UnitManagerListener m_unit_manager_listener;

  HelpmsgListener    m_helpmsg_none_listener;
  HelpmsgListener    m_helpmsg_menubar_listener;
  HelpmsgListener    m_helpmsg_tooltip_listener;

  FileButtonList     m_openbutton_list;
  FileButtonList     m_savebutton_list;
  bool               m_open_menu_with_remaining_entries;
  bool               m_save_menu_with_remaining_entries;

  DataPoolUndoListener     m_data_undo_listener;
  DataPoolUndoListener     m_data_redo_listener;

  DebugDescriptionFileListener m_debug_desfile;
  DebugPythonLogFileListener m_pylogger_file;

  DebugGuiListener         m_debug_gui;
  DebugGuiFldListener      m_debug_guifld;
  DebugGuiFolderListener   m_debug_guifolder;
  DebugGuiTableListener    m_debug_guitable;
  DebugGuiTgrListener      m_debug_guitrigger;
  DebugGuiIconListener     m_debug_guiicon;
  DebugGuiMgrListener      m_debug_guimgr;
  DebugGuiWorkerListener   m_debug_guiworker;
  DebugXferListener        m_debug_xfer;
  DebugDBManagerListener   m_debug_dbmgr;
  DebugDBFilterListener    m_debug_dbfilter;
  DebugDBCuratorListener   m_debug_dbcurator;
  DebugDBTimestampListener m_debug_dbtimestamp;
  DebugOperatorListener    m_debug_operator;
  DebugStreamerListener    m_debug_streamer;
  DebugTargetListener      m_debug_targets;
  DebugJobListener         m_debug_job;
  DebugJobStartListener    m_debug_jobstart;
  DebugJobCodeListener     m_debug_jobcode;
  DebugJobStackListener    m_debug_jobstack;
  DebugPlotListener        m_debug_plot;
  DebugDataRefListener     m_debug_dataref;
  DebugDatapoolListener    m_debug_datapool;
  DebugUndoListener        m_debug_undo;
  DebugTTrailListener      m_debug_ttrail;
  DebugTTrailMemListener   m_debug_ttrail_mem;
  DebugPrintDataListener   m_debug_printdata;
  DebugPrintDictListener   m_debug_printdict;

  /** currently logged description function name */
  std::string              m_desFuncName;
  /** argument vector */
  typedef std::vector<std::string> argumentValues;
  std::map<std::string, argumentValues> m_desfileFuncArgs;
};

#endif
