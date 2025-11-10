#ifdef HAVE_QT
#include <qglobal.h>

#include <qstylefactory.h>
#include <qstringlist.h>
#include <qapplication.h>
// #include <qpopupmenu.h>
#include <qstyle.h>
#include <qfontdialog.h>
#include <qcolordialog.h>
#include <qsettings.h>
#if !defined HAVE_HEADLESS
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtPulldownMenu.h"
#include "gui/qt/stylesheeteditor.h"
#endif
#endif

#include <string>

#include <stdio.h>
#include <assert.h>

#include "app/UiManager.h"
#include "app/AppData.h"
#include "app/App.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

#include "gui/GuiCycleDialog.h"
#include "job/JobManager.h"
#include "job/JobTask.h"
#include "job/JobController.h"
#include "job/InitialWorker.h"
#include "job/JobActionSimple.h"
#include "operator/ProcessGroup.h"
#include "datapool/DataPoolDefinition.h"

#include "gui/GuiFactory.h"
#include "gui/GuiForm.h"
#include "gui/GuiApplHeader.h"
#include "gui/GuiMenubar.h"
#include "gui/GuiPulldownMenu.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiMenuButtonListener.h"
#include "gui/GuiMenuToggle.h"
#include "gui/GuiButtonbar.h"
#include "gui/GuiSeparator.h"
#include "gui/GuiButton.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiMessageLine.h"
#include "gui/DialogCopyright.h"
#include "gui/ConfirmationListener.h"
#include "gui/GuiPrinterDialog.h"

#include "app/QuitApplication.h"
#include "app/HelpManager.h"
#include "streamer/Stream.h"
#include "streamer/XMLStreamParameter.h"

/* initialize */
UImanager *UImanager::s_instance = 0;

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

UImanager::UImanager()
  : m_main_form( 0 )
  , m_main_titlebar( 0 )
  , m_main_menubar( 0 )
  , m_file_menu( 0 )
  , m_open_menu( 0 )
  , m_save_menu( 0 )
  , m_print_menu( 0 )
  , m_preview_menu( 0 )
  , m_edit_menu( 0 )
  , m_form_menu( 0 )
  , m_process_menu( 0 )
  , m_option_menu( 0 )
  , m_help_menu( 0 )
  , m_helpmsg_none_listener(AppData::NoneType)
  , m_helpmsg_menubar_listener(AppData::StatusBarType)
  , m_helpmsg_tooltip_listener(AppData::ToolTipType)
  , m_main_buttonbar( 0 )
  , m_open_menu_with_remaining_entries( true )
  , m_save_menu_with_remaining_entries( true )
  , m_data_undo_listener( true )
  , m_data_redo_listener( false )
  , m_desFuncName("intens_script_func") {
  createBasicElements();
}

UImanager::~UImanager(){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

UImanager &UImanager::Instance(){
  if( s_instance == 0 ){
    s_instance = new UImanager;
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* setApplicationTitle --                                                      */
/* --------------------------------------------------------------------------- */

void UImanager::setApplicationTitle( const std::string & title ){
  AppData::Instance().setTitle( title );
}

/* --------------------------------------------------------------------------- */
/* createBasicElements --                                                      */
/* --------------------------------------------------------------------------- */

void UImanager::createBasicElements(){
  m_main_menubar = GuiFactory::Instance()->createMenubar( 0 );

  createFileMenu( m_main_menubar );
  //  createEditMenu( m_main_menubar );
  createFormMenu( m_main_menubar );
  createProcessMenu( m_main_menubar );
  createOptionMenu();
  createEditMenu( m_main_menubar );
  createHelpMenu();

  m_main_buttonbar = GuiManager::Instance().getMainButtonbar();
}

/* --------------------------------------------------------------------------- */
/* createFileMenu --                                                           */
/* --------------------------------------------------------------------------- */

void UImanager::createFileMenu( GuiMenubar *menubar ){
  GuiFactory *factory = GuiFactory::Instance();

  m_file_menu = factory->createPulldownMenu( menubar->getElement(), "File" );
  m_file_menu->setLabel( _("&File") );
  menubar->attach( m_file_menu->getElement() );

  // open menu Button of file menu
  m_open_menu = factory->createPulldownMenu( m_file_menu->getElement(), "Open" );
  m_open_menu->setLabel( _("&Open") );
  m_file_menu->attach( m_open_menu->getElement() );
  m_open_menu->setAlways();

  // save menu Button of file menu
  m_save_menu = factory->createPulldownMenu( m_file_menu->getElement(), "Save" );
  m_save_menu->setLabel( _("&Save") );
  m_file_menu->attach( m_save_menu->getElement() );
  m_save_menu->setAlways();

  // print menu Button of File menu
  m_print_menu = factory->createPulldownMenu( m_file_menu->getElement(), "Print" );
  ///m_print_menu->getElement()->setShownWebApi(false);  // not visible for webapi
  m_print_menu->setLabel( _("&Print") );
  m_file_menu->attach( m_print_menu->getElement() );
  m_print_menu->setAlways();

}

/* --------------------------------------------------------------------------- */
/* createEditMenu --                                                           */
/* --------------------------------------------------------------------------- */

void UImanager::createEditMenu( GuiMenubar *menubar ){
  GuiFactory *factory = GuiFactory::Instance();

  if (AppData::Instance().Undo()) {
    m_edit_menu = factory->createPulldownMenu( menubar->getElement(), "Edit" );
    m_edit_menu->getElement()->setShownWebApi(false);  // not visible for webapi
    m_edit_menu->setLabel( _("&Edit") );
    menubar->attach( m_edit_menu->getElement() );
    m_edit_menu->setAlways();

    // Buttons für Undo-Feature
    GuiMenuButton *button = 0;
    DataPoolIntens::getDataPool().attachUndoListener( &m_data_undo_listener );
    button = factory->createMenuButton( m_edit_menu->getElement(), &m_data_undo_listener );
    m_edit_menu->attach( button->getElement() );
    button->setLabel( _("Undo") );
    button->setAccelerator( "Ctrl<Key>Z", "Ctrl-Z" );

    DataPoolIntens::getDataPool().attachRedoListener( &m_data_redo_listener );
    button = factory->createMenuButton( m_edit_menu->getElement(), &m_data_redo_listener );
    m_edit_menu->attach( button->getElement() );
    button->setLabel( _("Redo") );

#if 1 // Windows like
    button->setAccelerator( "Ctrl<Key>Y", "Ctrl-Y" );
#else // Linux like
    button->setAccelerator( "Ctrl<Key>Shift<Key>Z", "Ctrl-Shift-Z" );
#endif
  }

}

/* --------------------------------------------------------------------------- */
/* createFormMenu --                                                           */
/* --------------------------------------------------------------------------- */

void UImanager::createFormMenu( GuiMenubar *menubar ){
  GuiFactory *factory = GuiFactory::Instance();

  m_form_menu = factory->createPulldownMenu( menubar->getElement(), "Form" );
  m_form_menu->setLabel( _("For&ms") );
  menubar->attach( m_form_menu->getElement() );
}

/* --------------------------------------------------------------------------- */
/* createProcessMenu --                                                        */
/* --------------------------------------------------------------------------- */

void UImanager::createProcessMenu( GuiMenubar *menubar ){
  GuiFactory *factory = GuiFactory::Instance();

  m_process_menu = factory->createPulldownMenu( menubar->getElement(), "Process" );
  m_process_menu->setLabel( _("&Process") );
  m_process_menu->getElement()->setShownWebApi(false);  // not visible for webapi
  menubar->attach( m_process_menu->getElement() );
}

/* --------------------------------------------------------------------------- */
/* createOptionMenu --                                                         */
/* --------------------------------------------------------------------------- */

void UImanager::createOptionMenu(){
  GuiFactory *factory = GuiFactory::Instance();

  m_option_menu = factory->createPulldownMenu( 0, "Option" );
  m_option_menu->setLabel( _("&Options") );
  m_option_menu->setAlways();
  // Der attach erfolgt erst am Ende der Initialisierung des GUI.
}

/* --------------------------------------------------------------------------- */
/* createHelpMenu --                                                           */
/* --------------------------------------------------------------------------- */

void UImanager::createHelpMenu(){
  GuiFactory *factory = GuiFactory::Instance();

  m_help_menu = factory->createPulldownMenu( 0, "Help" );
  m_help_menu->setLabel( _("&Help") );
  m_help_menu->setAlways();
  // Der attach erfolgt erst am Ende der Initialisierung des GUI.
}

/* --------------------------------------------------------------------------- */
/* createNewMenu --                                                            */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *UImanager::createNewMenu( const std::string &title ){
  GuiFactory *factory = GuiFactory::Instance();

  std::string name(title);
  removeString(name, "&");
  if (name == "Help") {
    return m_help_menu;
  }
  if (name == "File") {
    return m_file_menu;
  }
  if (name == "Open") {
    return m_open_menu;
  }
  if (name == "Save") {
    return m_save_menu;
  }
  if (name == "Print") {
    return m_print_menu;
  }
  if (name == "Preview") {
    return m_preview_menu;
  }
  if (name == "Edit") {
    return m_edit_menu;
  }
  if (name == "Form") {
    return m_form_menu;
  }
  if (name == "Process") {
    return m_process_menu;
  }
  if (name == "Option") {
    return m_option_menu;
  }
  GuiPulldownMenu *menu = factory->createPulldownMenu( m_main_menubar->getElement(), name );
  m_main_menubar->attach( menu->getElement() );
  menu->setLabel( title );
  return menu;
}

/* --------------------------------------------------------------------------- */
/* createNewMenu --                                                            */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *UImanager::createNewMenu( const std::string &name,
					   const std::string &title ){
  GuiFactory *factory = GuiFactory::Instance();

  GuiPulldownMenu *menu = factory->createPulldownMenu( 0, name );
  if( title.empty() )
    menu->setLabel( _( name.c_str() ) );
  else
    menu->setLabel( title );
  return menu;
}

/* --------------------------------------------------------------------------- */
/* getExistingMenu --                                                          */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *UImanager::getExistingMenu( const std::string &name ){
  GuiElement *el = GuiElement::findElement( name );
  if( el == 0 ) return 0;
  if( el->Type() != GuiElement::type_PulldownMenu ) return 0;

  return (GuiPulldownMenu *)el;
}

/* --------------------------------------------------------------------------- */
/* createNewSubMenu --                                                         */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *UImanager::createNewSubMenu( GuiElement *parent
                                            , const std::string &title ){
  GuiFactory *factory = GuiFactory::Instance();

  std::string name(title);
  removeString(name, "&");
  GuiPulldownMenu *menu = factory->createPulldownMenu( parent, name );
  parent->attach( menu->getElement() );
  menu->setLabel( title );
  return menu;
}

/* --------------------------------------------------------------------------- */
/* createNewSubMenu --                                                         */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *UImanager::createNewSubMenu( GuiElement *parent
                                            , const std::string &name,
					      const std::string &title ){
  GuiFactory *factory = GuiFactory::Instance();

  GuiPulldownMenu *menu = factory->createPulldownMenu( parent, name );
  parent->attach( menu->getElement() );
  if( title.empty() )
    menu->setLabel( _(name.c_str()) );
  else
    menu->setLabel( title );
  return menu;
}

/* --------------------------------------------------------------------------- */
/* attachMenu --                                                               */
/* --------------------------------------------------------------------------- */

void UImanager::attachMenu( GuiElement *menu ){
  if( menu->installed() ) return; /* hängt bereits irgendwo */

  menu->setParent( m_main_menubar->getElement() );
  m_main_menubar->attach( menu );
}

/* --------------------------------------------------------------------------- */
/* addFormMenuButton --                                                        */
/* --------------------------------------------------------------------------- */

bool UImanager::addFormMenuButton( GuiElement *parent
                                 , const std::string &name, const std::string &title
                                 , bool toggle ){
  GuiElement *el = GuiElement::findElement( name );
  if( el == 0 ){
    return false;
  }
  if( el->Type() != GuiElement::type_Form ){
    return false;
  }

  GuiForm *form = el->getForm();

  std::string label;
  if( !title.empty() )
    label = title;
  else
  if( !form->getTitle().empty() )
    label = form->getTitle();
  else
    label = name;

  std::string text;
  GuiFactory *factory = GuiFactory::Instance();

  if( toggle ){
    GuiToggleListener *lsnr = form->getFormToggleListener();
    GuiMenuToggle *togglebutton = factory->createMenuToggle( parent, lsnr );
    parent->attach( togglebutton->getElement() );
    togglebutton->setDialogLabel( label );
    form->getElement()->getHelptext( text );
    togglebutton->getElement()->setHelptext( text );
  }
  else{
    GuiButtonListener *lsnr = form->getFormButtonListener();
    GuiMenuButton *pushbutton = factory->createMenuButton( parent, lsnr );
    parent->attach( pushbutton->getElement() );
    pushbutton->setDialogLabel( label );
    form->getElement()->getHelptext( text );
    pushbutton->getElement()->setHelptext( text );
  }
  form->setFormMenuInstalled();
  return true;
}

/* --------------------------------------------------------------------------- */
/* addFunctionMenuButton --                                                    */
/* --------------------------------------------------------------------------- */

bool UImanager::addFunctionMenuButton( GuiElement *parent
                                     , const std::string &name, const std::string &title ){
  JobFunction *func = JobManager::Instance().getFunction( name, true );
  if( func == 0 ){
    return false;
  }
  GuiButtonListener *lsnr = func->getButtonListener();
  GuiFactory *factory = GuiFactory::Instance();
  std::string label = title.empty() ? name : title;

  GuiMenuButton *pushbutton = factory->createMenuButton( parent, lsnr,
                                                         0, false, label );
  parent->attach( pushbutton->getElement() );
  pushbutton->setLabel( label );

  //  pushbutton->setHelptext( func->getHelptext() );
  func->setMenuInstalled();
  return true;
}

/* --------------------------------------------------------------------------- */
/* addFunctionNavigatorMenuButton --                                           */
/* --------------------------------------------------------------------------- */

bool UImanager::addFunctionNavigatorMenuButton( GuiElement *parent
                                     , const std::string &name, const std::string &title ){
  assert( parent != 0 );
  JobFunction *func = JobManager::Instance().getFunction( name, true );
  if( func == 0 ){
    return false;
  }
  GuiButtonListener *lsnr = func->getNavigatorButtonListener( parent );
  GuiFactory *factory = GuiFactory::Instance();
  std::string label = title.empty() ? name : title;

  GuiMenuButton *pushbutton = factory->createMenuButton( parent, lsnr,
                                                         0, false, label );
  parent->attach( pushbutton->getElement() );
  pushbutton->setLabel( label );

  //  pushbutton->setHelptext( func->getHelptext() );
  func->setMenuInstalled();
  return true;
}

/* --------------------------------------------------------------------------- */
/* addTaskMenuButton --                                                        */
/* --------------------------------------------------------------------------- */

bool UImanager::addTaskMenuButton( GuiElement *parent
                                 , const std::string &name, const std::string &title ){
  JobTask *task = JobManager::Instance().getTask( name );
  if( task == 0 ){
    return false;
  }
  GuiButtonListener *lsnr = task->getButtonListener();
  GuiFactory *factory = GuiFactory::Instance();
  std::string label = title.empty() ? name : title;

  GuiMenuButton *pushbutton = factory->createMenuButton( parent, lsnr,
                                                         0, false, label );
  parent->attach( pushbutton->getElement() );
  pushbutton->setLabel( label );

  pushbutton->getElement()->setHelptext( task->getHelptext());
  task->setMenuInstalled();
  return true;
}

/* --------------------------------------------------------------------------- */
/* addTaskActionButton --                                                      */
/* --------------------------------------------------------------------------- */

void UImanager::addTaskActionButton( JobTask *task ){
  assert( task != 0 );

  GuiButtonbar *bar = task->getButtonbar();
  if( bar == 0 ) return;

  GuiButtonListener *lsnr = task->getButtonListener();
  GuiButton *button = GuiFactory::Instance() -> createButton( bar->getElement(), lsnr );
  button->setLabel( task->getLabel() );
  button->getElement()->setHelptext( task->getHelptext() );
}

/* --------------------------------------------------------------------------- */
/* addProcessMenuButton --                                                     */
/* --------------------------------------------------------------------------- */

bool UImanager::addProcessMenuButton( GuiElement *parent
                                    , const std::string &name, const std::string &title ){
  ProcessGroup *proc = ProcessGroup::find( name );
  if( proc == 0 ){
    return false;
  }
  GuiButtonListener *lsnr = proc->getButtonListener();
  GuiFactory *factory = GuiFactory::Instance();
  std::string label = title.empty() ?
    (proc->getLabel().empty() ? name : proc->getLabel()) : title;

  GuiMenuButton *pushbutton = factory->createMenuButton( parent, lsnr,
                                                         0, false, label );
  parent->attach( pushbutton->getElement() );
  pushbutton->setLabel( label );

  pushbutton->getElement()->setHelptext( proc->getHelptext() );
  proc->setMenuInstalled();
  return true;
}

/* --------------------------------------------------------------------------- */
/* addProcessActionButton --                                                   */
/* --------------------------------------------------------------------------- */

void UImanager::addProcessActionButton( ProcessGroup *group ){
  assert( group != 0 );

  GuiButtonbar *bar = group->getButtonbar();
  if( bar == 0 ) return;

  GuiButtonListener *lsnr = group->getButtonListener();
  GuiButton *button = GuiFactory::Instance() -> createButton( bar->getElement(), lsnr );
  button->setLabel( group->getLabel() );
  button->getElement()->setHelptext( group->getHelptext() );
}


/* --------------------------------------------------------------------------- */
/* addOpenButton --                                                            */
/* --------------------------------------------------------------------------- */

bool UImanager::addOpenButton( const std::string &name, GuiMenuButtonListener *listener ){
  if( m_openbutton_list.find( name ) != m_openbutton_list.end() ){
    return false;
  }
  m_openbutton_list.insert( FileButtonList::value_type( name, listener ) );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getOpenButtonList --                                                        */
/* --------------------------------------------------------------------------- */

FileButtonList *UImanager::getOpenButtonList(){
  return &m_openbutton_list;
}

/* --------------------------------------------------------------------------- */
/* getOpenMenu --                                                              */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *UImanager::getOpenMenu( bool with_remaining ){
  m_open_menu_with_remaining_entries = with_remaining;
  return m_open_menu;
}

/* --------------------------------------------------------------------------- */
/* createOpenMenuButtons --                                                    */
/* --------------------------------------------------------------------------- */

void UImanager::createOpenMenuButtons(){
  if( !m_open_menu_with_remaining_entries ){
    return;
  }

  FileButtonList::iterator fi;
  for( fi = m_openbutton_list.begin(); fi != m_openbutton_list.end(); ++fi ){
    if( !(*fi).second->isMenuInstalled() ){
      (*fi).second->attach( m_open_menu->getElement() );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* addSaveButton --                                                            */
/* --------------------------------------------------------------------------- */

bool UImanager::addSaveButton( const std::string &name, GuiMenuButtonListener *listener ){
  if( m_savebutton_list.find( name ) != m_savebutton_list.end() ){
    return false;
  }
  m_savebutton_list.insert( FileButtonList::value_type( name, listener ) );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSaveButtonList --                                                        */
/* --------------------------------------------------------------------------- */

FileButtonList *UImanager::getSaveButtonList(){
  return &m_savebutton_list;
}

/* --------------------------------------------------------------------------- */
/* getSaveMenu --                                                              */
/* --------------------------------------------------------------------------- */

GuiPulldownMenu *UImanager::getSaveMenu( bool with_remaining ) {
  m_save_menu_with_remaining_entries = with_remaining;
  return m_save_menu;
}

/* --------------------------------------------------------------------------- */
/* createSaveMenuButtons --                                                    */
/* --------------------------------------------------------------------------- */

void UImanager::createSaveMenuButtons(){
  if( !m_save_menu_with_remaining_entries ){
    return;
  }

  FileButtonList::iterator fi;
  for( fi = m_savebutton_list.begin(); fi != m_savebutton_list.end(); ++fi ){
    if( !(*fi).second->isMenuInstalled() ){
      (*fi).second->attach( m_save_menu->getElement() );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* createFileButton --                                                         */
/* --------------------------------------------------------------------------- */

bool UImanager::createFileButton( GuiElement *menu
                                , const std::string &name
				, const std::string &label
				, FileButtonList *buttonlist ){
  FileButtonList::iterator fi = buttonlist->find( name );
  if( fi == buttonlist->end() ){
    return false;
  }
  if( !label.empty() ){
    (*fi).second->setLabel( label );
  }
  (*fi).second->attach( menu );
  return true;
}

/* --------------------------------------------------------------------------- */
/* addHardCopy --                                                              */
/* --------------------------------------------------------------------------- */

bool UImanager::addHardCopy( const std::string &name
                           , HardCopyListener *hdc ){
  GuiFactory::Instance()->createPrinterDialog()->insertHardcopy( name, hdc );
  return true;
}

/* --------------------------------------------------------------------------- */
/* createPrintMenuButtons --                                                   */
/* --------------------------------------------------------------------------- */

void UImanager::createPrintMenuButtons(){
  GuiFactory::Instance()->createPrinterDialog()->createPrintMenu( m_print_menu );
}

/* --------------------------------------------------------------------------- */
/* createPreviewMenuButtons --                                                   */
/* --------------------------------------------------------------------------- */

void UImanager::createPreviewMenuButtons(){
  bool preview(true);
  GuiFactory::Instance()->createPrinterDialog()->createPrintMenu( m_preview_menu, preview );
}

/* --------------------------------------------------------------------------- */
/* addHelpCallback --                                                          */
/* --------------------------------------------------------------------------- */

void UImanager::addHelp( const std::string &name, GuiMenuButtonListener *lsnr ){
  GuiFactory *factory = GuiFactory::Instance();

  GuiMenuButton *button = factory->createMenuButton( m_help_menu->getElement(), lsnr,
                                                     0, false, name );
  button->setLabel( name );
  m_help_menu->attach( button->getElement() );
}

/* --------------------------------------------------------------------------- */
/* addHelpCallback --                                                          */
/* --------------------------------------------------------------------------- */

// ActionButton *UImanager::addHelpCallback( const std::string &name
// 	                                , ActionButtonProc proc, XtPointer data ){
//   ActionButton *action = new ActionButton;
//   action->addActionCallback( name, proc, data );
//   action->attach( (GuiMotifPulldownMenu*)m_help_menu );
//   return action;
// }

/* --------------------------------------------------------------------------- */
/* addCopyrightDialog --                                                       */
/* --------------------------------------------------------------------------- */

void UImanager::addCopyrightDialog(){
  GuiFactory *factory = GuiFactory::Instance();

  DialogCopyright *copyright = factory->createDialogCopyright();
  std::string name("Copyright");
  GuiMenuButton *button = factory->createMenuButton( m_help_menu->getElement(), copyright->getElement(), 0, false, name );
  button->setLabel( _("Copyright") );
  m_help_menu->attach( button->getElement() );
}

/* --------------------------------------------------------------------------- */
/* addOptions --                                                               */
/* --------------------------------------------------------------------------- */

void UImanager::addOptions(){
  GuiFactory *factory = GuiFactory::Instance();

  GuiMenuToggle *toggle = 0;
  GuiMenuButton *button = 0;
  GuiPulldownMenu *menu = 0;

  if( GuiElement::getMainForm()->hasCyclebutton() ){
    button = factory->createMenuButton( m_option_menu->getElement(),
                                        factory->createCycleDialog() );
    m_option_menu->attach( button->getElement() );
    button->setDialogLabel( _("Case") );
    button->setAccelerator( "Ctrl<Key>L", "Ctrl-L" );
  }

  if( m_main_titlebar != 0 && !AppData::Instance().HeadlessWebMode()){
    toggle = factory->createMenuToggle( m_option_menu->getElement(), m_main_titlebar );
    toggle->setLabel(_("Titlebar") );
    m_option_menu->attach( toggle->getElement() );
    m_main_titlebar->setToggleStatus( AppData::Instance().AppTitlebar() );
  }

  AppData::HelpMessageType helpMsgType = AppData::Instance().Helpmessages();
  // help messages
#if defined HAVE_QT && !defined HAVE_HEADLESS
  if( GuiQtManager::Instance().Settings() && !AppData::Instance().HeadlessWebMode()) {
    helpMsgType = (AppData::HelpMessageType) GuiQtManager::Instance().Settings()->value
      ( "Intens/HelpMessageType", helpMsgType).toInt();
    AppData::Instance().setHelpmessages( helpMsgType );
  }
#endif

  if (AppData::Instance().UnitManagerFeature()) {
    GuiMenuButton *button = factory->createMenuButton( m_option_menu->getElement(), &m_unit_manager_listener);
    button->setLabel( _("Unit Manager") );
    m_option_menu->attach( button->getElement() );
  }
  if (!AppData::Instance().HeadlessWebMode()) {
#if defined(_DEBUG)
    toggle = factory->createMenuToggle( m_option_menu->getElement(), &m_debug_desfile );
    toggle->setLabel(_("Write Script file"));
    m_option_menu->getElement()->attach( toggle->getElement() );
    m_debug_desfile.setToggleStatus( Debugger::DebugFlagEnabled( BugDescriptionFile ) );
#endif

    toggle = factory->createMenuToggle( m_option_menu->getElement(), &m_pylogger_file );
    toggle->setLabel(_("Write Python Script file"));
    m_option_menu->getElement()->attach( toggle->getElement() );
    m_pylogger_file.setToggleStatus( Debugger::DebugFlagEnabled( BugPythonLogFile ) );
  }

  menu = factory->createPulldownMenu( m_option_menu->getElement(), _("Help messages") );
  m_option_menu->attach( menu->getElement() );
  menu->getElement()->setShownWebApi(false);  // not visible for webapi
  menu->setRadio( true );

  toggle = factory->createMenuToggle( menu->getElement(), &m_helpmsg_menubar_listener );
  toggle->setLabel( _("Statusbar Help messages") );
  menu->attach( toggle->getElement() );
  m_helpmsg_menubar_listener.setToggleStatus( helpMsgType==AppData::StatusBarType );

  toggle = factory->createMenuToggle( menu->getElement(), &m_helpmsg_tooltip_listener );
  toggle->setLabel( _("Tooltip Help messages") );
  menu->attach( toggle->getElement() );
  m_helpmsg_tooltip_listener.setToggleStatus( helpMsgType==AppData::ToolTipType );

  toggle = factory->createMenuToggle( menu->getElement(), &m_helpmsg_none_listener );
  toggle->setLabel( _("Disable Help messages") );
  menu->attach( toggle->getElement() );
  m_helpmsg_none_listener.setToggleStatus( helpMsgType==AppData::NoneType );

#if defined(_DEBUG)
  if (AppData::Instance().HeadlessWebMode()) {
    return; // no more, return
  }
  GuiSeparator *sep = factory->createSeparator(  m_option_menu->getElement() );
  m_option_menu->attach( sep->getElement() );

  // ========== Begin of Gui Menu ==========
  menu = factory->createPulldownMenu( m_option_menu->getElement(), "Trace Gui" );
  m_option_menu->attach( menu->getElement() );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_gui );
  toggle->setLabel( "Trace Gui" );
  menu->attach( toggle->getElement() );
  m_debug_gui.setToggleStatus( Debugger::DebugFlagEnabled( BugGui ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_guifld );
  toggle->setLabel( "Trace DataFields" );
  menu->attach( toggle->getElement() );
  m_debug_guifld.setToggleStatus( Debugger::DebugFlagEnabled( BugGuiFld ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_guifolder );
  toggle->setLabel( "Trace Folders" );
  menu->attach( toggle->getElement() );
  m_debug_guifolder.setToggleStatus( Debugger::DebugFlagEnabled( BugGuiFolder ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_guitable );
  toggle->setLabel( "Trace GuiTable" );
  menu->attach( toggle->getElement() );
  m_debug_guitable.setToggleStatus( Debugger::DebugFlagEnabled( BugGuiTable ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_guitrigger );
  toggle->setLabel( "Trace Triggers" );
  menu->attach( toggle->getElement() );
  m_debug_guitrigger.setToggleStatus( Debugger::DebugFlagEnabled( BugGuiTrigger ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_guiicon );
  toggle->setLabel( "Trace IconManager" );
  menu->attach( toggle->getElement() );
  m_debug_guiicon.setToggleStatus( Debugger::DebugFlagEnabled( BugGuiIcon ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_guimgr );
  toggle->setLabel( "Trace GuiManager" );
  menu->attach( toggle->getElement() );
  m_debug_guimgr.setToggleStatus( Debugger::DebugFlagEnabled( BugGuiMgr ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_guiworker );
  toggle->setLabel( "Trace Worker" );
  menu->attach( toggle->getElement() );
  m_debug_guiworker.setToggleStatus( Debugger::DebugFlagEnabled( BugGuiWorker ) );
  // ========== End of Gui Menu ==========

  toggle = factory->createMenuToggle( m_option_menu->getElement(), &m_debug_xfer );
  toggle->setLabel( "Trace Xfer" );
  m_option_menu->attach( toggle->getElement() );
  m_debug_xfer.setToggleStatus( Debugger::DebugFlagEnabled( BugXfer ) );

  // ========== Begin of Datapool Menu ==========
  menu = factory->createPulldownMenu( m_option_menu->getElement(), "Trace Datapool" );
  m_option_menu->attach( menu->getElement() );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_dataref );
  toggle->setLabel( "Trace DataReference" );
  menu->attach( toggle->getElement() );
  m_debug_dataref.setToggleStatus( Debugger::DebugFlagEnabled( BugRef ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_datapool );
  toggle->setLabel( "Trace Datapool" );
  menu->attach( toggle->getElement() );
  m_debug_datapool.setToggleStatus( Debugger::DebugFlagEnabled( BugData ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_undo );
  toggle->setLabel( "Trace Undo/Redo" );
  menu->attach( toggle->getElement() );
  m_debug_undo.setToggleStatus( Debugger::DebugFlagEnabled( BugUndo ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_ttrail );
  toggle->setLabel( "Trace TTrail" );
  menu->attach( toggle->getElement() );
  m_debug_ttrail.setToggleStatus( Debugger::DebugFlagEnabled( BugTTrail ) );

  button = factory->createMenuButton( menu->getElement(), &m_debug_ttrail_mem );
  menu->attach( button->getElement() );
  button->setLabel( "Print TTrail Memory" );

  button = factory->createMenuButton( menu->getElement(), &m_debug_printdata );
  menu->attach( button->getElement() );
  button->setLabel( "Print Datapool" );

  button = factory->createMenuButton( menu->getElement(), &m_debug_printdict );
  menu->attach( button->getElement() );
  button->setLabel( "Print Dictionary" );
  // ========== End of Datapool Menu ==========

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_dbtimestamp );
  toggle->setLabel( "Trace Timestamp" );
  menu->attach( toggle->getElement() );
  m_debug_dbtimestamp.setToggleStatus( Debugger::DebugFlagEnabled( BugDBTimestamp ) );

  // ========== End of Database Menu ==========

  toggle = factory->createMenuToggle( m_option_menu->getElement(), &m_debug_operator );
  toggle->setLabel( "Trace Operator" );
  m_option_menu->attach( toggle->getElement() );
  m_debug_operator.setToggleStatus( Debugger::DebugFlagEnabled( BugOperator ) );

  toggle = factory->createMenuToggle( m_option_menu->getElement(), &m_debug_streamer );
  toggle->setLabel( "Trace Streamer" );
  m_option_menu->attach( toggle->getElement() );
  m_debug_streamer.setToggleStatus( Debugger::DebugFlagEnabled( BugStreamer ) );

  toggle = factory->createMenuToggle( m_option_menu->getElement(), &m_debug_targets );
  toggle->setLabel( "Trace Stream Targets" );
  m_option_menu->attach( toggle->getElement() );
  m_debug_targets.setToggleStatus( Debugger::DebugFlagEnabled( BugTargets ) );

  // ========== Begin of Job Menu ===============
  menu = factory->createPulldownMenu( m_option_menu->getElement(), "Trace Tasks and Functions" );
  m_option_menu->attach( menu->getElement() );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_job );
  toggle->setLabel( "Trace Job Manager" );
  menu->attach( toggle->getElement() );
  m_debug_job.setToggleStatus( Debugger::DebugFlagEnabled( BugJob ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_jobstart );
  toggle->setLabel( "Trace Job Control" );
  menu->attach( toggle->getElement() );
  m_debug_jobstart.setToggleStatus( Debugger::DebugFlagEnabled( BugJobStart ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_jobcode );
  toggle->setLabel( "Trace Job Code" );
  menu->attach( toggle->getElement() );
  m_debug_jobcode.setToggleStatus( Debugger::DebugFlagEnabled( BugJobCode ) );

  toggle = factory->createMenuToggle( menu->getElement(), &m_debug_jobstack );
  toggle->setLabel( "Trace Job Stack Items" );
  menu->attach( toggle->getElement() );
  m_debug_jobstack.setToggleStatus( Debugger::DebugFlagEnabled( BugJobStack ) );
  // ========== End of Job Menu ===============

  toggle = factory->createMenuToggle( m_option_menu->getElement(), &m_debug_plot );
  toggle->setLabel( "Trace Plot3" );
  m_option_menu->attach( toggle->getElement() );
  m_debug_plot.setToggleStatus( Debugger::DebugFlagEnabled( BugPlot ) );

#endif // _DEBUG

#ifdef HAVE_QT
#if defined(_DEBUG)
  // ========== Begin of Style Gui Menu ==========
  GuiSeparator* sepStyle = factory->createSeparator(  m_option_menu->getElement() );
  m_option_menu->attach( sepStyle->getElement() );

  menu = factory->createPulldownMenu( m_option_menu->getElement(), _("Style") );
  m_option_menu->attach( menu->getElement() );
  menu->setRadio( true );

  QStringList styles = QStyleFactory::keys();
  styles.sort();
  for ( QStringList::Iterator it = styles.begin(); it != styles.end(); ++it ) {
    QtStyleListener* listener = new QtStyleListener( (*it).toStdString() );
    toggle = factory->createMenuToggle( menu->getElement(), listener );
    toggle->setLabel( (*it).toStdString() );
    menu->attach( toggle->getElement() );
    if (AppData::Instance().QtGuiStyle().size() ) {
      if (AppData::Instance().QtGuiStyle() == (*it).toStdString())
	listener->setToggleStatus( true );
    } else
      if (QApplication::style()->objectName().compare((*it), Qt::CaseInsensitive) == 0) {
	listener->setToggleStatus( true );
      }
  }
  // ========== End of Style Gui Menu ==========

  // ========== Begin of StyleSheet Gui Menu ==========
  sepStyle = factory->createSeparator(  m_option_menu->getElement() );
  m_option_menu->attach( sepStyle->getElement() );

  menu = factory->createPulldownMenu( m_option_menu->getElement(), _("StyleSheet") );
  m_option_menu->attach( menu->getElement() );
  menu->setRadio( true );

  std::set<std::string> list = GuiManager::Instance().getAvailableStylesheets();
  std::set<std::string>::const_iterator it;

  for (it = list.begin(); it != list.end(); ++it) {
    QtStyleSheetListener* listener = new QtStyleSheetListener( (*it) );
    toggle = factory->createMenuToggle( menu->getElement(), listener );
    toggle->setLabel( (*it) );
    menu->attach( toggle->getElement() );
    if (GuiManager::Instance().stylesheetName() == (*it)) {
      listener->setToggleStatus( true );
    }
  }
  // ========== End of StyleSheet Gui Menu ==========

  sepStyle = factory->createSeparator(   m_option_menu->getElement() );
  m_option_menu->attach( sepStyle->getElement() );

  QtDialogListener* listener = new QtDialogListener( "QFontDialog" );
  button = factory->createMenuButton( m_option_menu->getElement(), listener);
  m_option_menu->attach( button->getElement() );
  button->setLabel( "QFontDialog" );

  listener = new QtDialogListener( "QColorDialog" );
  button = factory->createMenuButton( m_option_menu->getElement(), listener);
  m_option_menu->attach( button->getElement() );
  button->setLabel( "QColorDialog" );

  listener = new QtDialogListener( "QStylesheet" );
  button = factory->createMenuButton( m_option_menu->getElement(), listener);
  m_option_menu->attach( button->getElement() );
  button->setLabel( "QStylesheetEditor" );
#endif

#endif
}

/* --------------------------------------------------------------------------- */
/* setMainTitle --                                                             */
/* --------------------------------------------------------------------------- */

void UImanager::setMainTitle(){
  AppData &appdata = AppData::Instance();
  std::ostringstream title;
  std::string version(appdata.Version());
#if defined Q_OS_WIN
  // some customer want it
  replaceAll(version, "dev", "");
#endif

  title << appdata.Title();
  if ( ! appdata.AppShortMainTitle() ) {
    title << " - " << appdata.ProgName() << "@" << appdata.HostName()
          << "(" << version << ") ";
  }
  GuiManager::Instance().setMainTitle( title.str() );
}

/* --------------------------------------------------------------------------- */
/* finalAttachments --                                                         */
/* --------------------------------------------------------------------------- */

void UImanager::finalAttachments(){
  m_option_menu->getElement()->setParent( m_main_menubar->getElement() );
  m_main_menubar->attach( m_option_menu->getElement() );

  m_help_menu->getElement()->setParent( m_main_menubar->getElement() );
  m_main_menubar->attachHelp(  m_help_menu );
}

/* --------------------------------------------------------------------------- */
/* initApplication --                                                          */
/* --------------------------------------------------------------------------- */

void UImanager::initApplication( int *argc, char **argv,
				 const std::string &classname ){
  GuiManager::Instance().initApplication( argc, argv, classname );

  GuiScrolledText *text = 0;
  HardCopyListener *lsnr = 0;

  text = GuiFactory::Instance()->getStandardWindow();
  lsnr = text->getElement()->getHardCopyListener();
  assert( lsnr != 0 );
  addHardCopy( "STD_WINDOW", lsnr );
  addSaveButton( "STD_WINDOW", text->getSaveListener() );

  text = GuiFactory::Instance()->getLogWindow();
  lsnr = text->getElement()->getHardCopyListener();
  assert( lsnr != 0 );
  addHardCopy( "LOG_WINDOW", lsnr );
  addSaveButton( "LOG_WINDOW", text->getSaveListener() );

}

/* --------------------------------------------------------------------------- */
/* createApplication --                                                        */
/* --------------------------------------------------------------------------- */

void UImanager::createApplication(){
  AppData &appdata    = AppData::Instance();
  GuiManager &gui     = GuiManager::Instance();

  GuiFactory *factory = GuiFactory::Instance();

  std::string displayname;
  gui.getDisplayName( displayname );
  appdata.setDisplayName( displayname );

  gui.dispatchPendingEvents();

  if( appdata.Undo() ){
    DataPoolIntens::Instance().getDataPool().activateUndo();
  }

  // Falls noch kein Hauptfenster (MAIN) existiert, muss nun hier
  // eine erstellt werden. Die Main Form erhaelt zusaetzliche Elemente
  // wie Title-Bar, Menu-Bar und Tool-Bar.
  // -----------------------------------------------------------------
  m_main_form = GuiElement::getMainForm();
  if( m_main_form == 0 ){
    m_main_form = factory->createForm( "@MAIN@" );
    m_main_form->setMain();
    assert( m_main_form != 0 );
  }
  if (!AppData::Instance().HeadlessWebMode() || getenv("WEBTENS_HIDE_MENUBAR") == NULL) {
    m_main_menubar->getElement()->setParent( m_main_form->getElement() );
    m_main_form->getElement()->front( m_main_menubar->getElement() );
  }

  m_main_titlebar = factory->createApplHeader( m_main_form->getElement() );
  m_main_form->getElement()->front( m_main_titlebar->getElement() );

  // Die textwindows Standard- und Log-window sollen immer vorhanden sein
  // ------------------------------------------------------------------
  GuiScrolledText *text = 0;
  text = GuiFactory::Instance()->getStandardWindow();
  if( !text->getElement()->installed() ){
    text->getElement()->setParent( m_main_form->getElement() );
    m_main_form->getElement()->attach( text->getElement() );
  }
   HardCopyListener *lsnr = 0;
   lsnr = text->getElement()->getHardCopyListener();
   assert( lsnr != 0 );
   addHardCopy( "STD_WINDOW", lsnr );
   if( lsnr->showMenu() )
     addSaveButton( "STD_WINDOW", text->getSaveListener() );

  text = GuiFactory::Instance()->getLogWindow();
  if( !text->getElement()->installed() ){
    text->getElement()->setParent( m_main_form->getElement() );
    m_main_form->getElement()->attach( text->getElement() );
  }
  lsnr = text->getElement()->getHardCopyListener();
  assert( lsnr != 0 );
  addHardCopy( "LOG_WINDOW", lsnr );
  if( lsnr->showMenu() )
    addSaveButton( "LOG_WINDOW", text->getSaveListener() );


  // Die Main Buttonbar kann erst am Schluss installiert werden
  // ----------------------------------------------------------
  GuiMessageLine *line = factory->createMessageLine( m_main_form->getElement() );
  m_main_form->getElement()->attach( line->getElement() );
  m_main_buttonbar->getElement()->setParent( m_main_form->getElement() );
  m_main_form->getElement()->attach( m_main_buttonbar->getElement() );

  // Alle Options werden im Menu installiert
  // ---------------------------------------
  addOptions();

  createOpenMenuButtons();
  createSaveMenuButtons();
  createPrintMenuButtons();
  if (!AppData::Instance().HeadlessWebMode() || getenv("WEBTENS_SHOW_FORM_MENU") != NULL) {
    GuiManager::Instance().createFormMenu( m_form_menu );
  }
  JobManager::Instance().createTaskMenu( m_process_menu );
  ProcessGroup::createProcessMenu( m_process_menu );

  // Der Printer-Dialog wird initialisiert.
  // Es werden DataDictionary-Eintraege gemacht
  // ------------------------------------------

  GuiFactory::Instance()->createPrinterDialog()->initPrinterDialog();

  // ############################################################### //
  // Create DataPool                                                 //
  // Alle Items sollten nun deklariert sein !!                       //
  // ############################################################### //
  DataPoolIntens::Instance().createDataPool();

  addCopyrightDialog();
  installQuitApplication();

  finalAttachments();

  GuiManager::Instance().createApplication();
  setMainTitle();

  GuiManager::Instance().mapApplication();
}

/* --------------------------------------------------------------------------- */
/* startApplication --                                                         */
/* --------------------------------------------------------------------------- */

void UImanager::startApplication(){
  DES_INFO(compose("FUNC %1 ", m_desFuncName));
  JobAction *action = 0;
  m_pylogger_file.setToggleStatus( false );  // Disable PyLog is default

  BUG_INFO("startApplication, headlessMode:" << AppData::Instance().HeadlessWebMode()
           << " NoInitFunc:" << AppData::Instance().NoInitFunc());
  if (AppData::Instance().HeadlessWebMode() ||
      AppData::Instance().NoInitFunc()) {
    BUG_INFO("startApplication with HeadlessWebMode");
  } else {
    if (AppData::Instance().ReplyPort() > 0) {
      BUG_WARN("startApplication with DesktopMode, replyPort: " << AppData::Instance().ReplyPort());
    } else {
      BUG_INFO("startApplication with DesktopMode");
    }
    action = JobManager::Instance().getInitialAction( "INIT" );
    if( action ){
      action->setSilent();
      InitialWorker::Instance().appendAction( action );
    }

    if (AppData::Instance().hasTestModeFunc()) {
      // test func
      action = JobManager::Instance().getInitialAction( AppData::Instance().TestModeFunc() );
      if( action ){
        action->setSilent();
        InitialWorker::Instance().appendAction( action );
      } else  {
        std::cerr << "ABORT in TestMode func '" << AppData::Instance().TestModeFunc() << "' is undefined.\n"<<std::flush;
        exit(1);
      }
    }

    // is app_init function defined?
    std::string appInitPostLoad;
    double d;
    int i;
    AppData::Instance().getResource("APP_INIT_POSTLOAD", appInitPostLoad, d, i);
    if (!appInitPostLoad.empty()) {
      if (!App::Instance().parseFile(appInitPostLoad))
        std::cout << "ERROR post load, file: '"<< appInitPostLoad << "'.\n";
    }

    // is app_init function defined?
    std::string appInitFuncName;
    AppData::Instance().getResource("APP_INIT_FUNC", appInitFuncName, d, i);
    if (!appInitFuncName.empty()) {
      // pre test func
      action = JobManager::Instance().getInitialAction( compose("pre_%1", appInitFuncName) );
      if( action ){
        action->setSilent();
        InitialWorker::Instance().appendAction( action );
      }

      // test func
      action = JobManager::Instance().getInitialAction( appInitFuncName );
      if( action ){
        action->setSilent();
        InitialWorker::Instance().appendAction( action );
      }

      // post test func
      action = JobManager::Instance().getInitialAction( compose("post_%1", appInitFuncName) );
      if( action ){
        action->setSilent();
        InitialWorker::Instance().appendAction( action );
      }
    }
  } // End !WebMode

  // ActionEnableUndo
  action = new ActionEnableUndo();
  InitialWorker::Instance().appendAction( action );

  // webapi create HelpText
  HelpManager::Instance().createAllHelpText();

  GuiManager::Instance().runApplication();
}

/* --------------------------------------------------------------------------- */
/* closeDescriptionFunction --                                                 */
/* --------------------------------------------------------------------------- */

void UImanager::closeDescriptionFunction(){
  // close function
  DES_INFO(compose("}; // End of %1", m_desFuncName));
  if (!m_desfileFuncArgs.empty()) {

	// define pre_<funcName>
	DES_INFO(compose("FUNC pre_%1 {", m_desFuncName));
	std::map<std::string, argumentValues>::iterator it = m_desfileFuncArgs.begin();
	for(; it != m_desfileFuncArgs.end(); ++it) {
	  DES_INFO(compose("  SET_RESOURCE(\"Arg@%1\", \"%2\");",
			   it->first, it->second[0]));
	}
	DES_INFO("};");  // close function

	// define post_<funcName>
	DES_INFO(compose("FUNC post_%1 {", m_desFuncName));
	it = m_desfileFuncArgs.begin();
	for(; it != m_desfileFuncArgs.end(); ++it) {
	  DES_INFO(compose("  SET_RESOURCE(\"Arg@%1\", \"\");", it->first));
	}
	DES_INFO("};");  // close function
  }
}

/* --------------------------------------------------------------------------- */
/* addDescriptionFunctionArgument --                                           */
/* --------------------------------------------------------------------------- */

std::string UImanager::addDescriptionFunctionArgument(std::string& key, std::string& value) {
  std::map<std::string, argumentValues>::iterator it = m_desfileFuncArgs.find(key);

  if (it == m_desfileFuncArgs.end() ) {
	m_desfileFuncArgs.insert (m_desfileFuncArgs.end(), std::pair<std::string,argumentValues>(key,argumentValues()));
	m_desfileFuncArgs[key].push_back(value);
} else
	m_desfileFuncArgs[key].push_back(value);
  return compose("%1_Arg_%2",m_desFuncName, m_desfileFuncArgs.size());
}

/* --------------------------------------------------------------------------- */
/* installQuitApplication --                                                   */
/* --------------------------------------------------------------------------- */

void UImanager::installQuitApplication(){
  GuiManager &gui = GuiManager::Instance();

  GuiFactory *factory = GuiFactory::Instance();

  QuitApplication *quit = QuitApplication::Instance( m_main_form->getDialog() );
  gui.installQuitApplication( quit );

  GuiSeparator *sep = factory->createSeparator(  m_file_menu->getElement() );
  m_file_menu->attach( sep->getElement() );

  GuiMenuButton *button = factory->createMenuButton( m_file_menu->getElement(), quit,
                                                     0, false, "Quit"  );
   m_file_menu->attach( button->getElement() );
#ifndef __MINGW32__
   button->setAccelerator( "Ctrl<Key>X", "Ctrl-X" );
#endif
   button->setLabel( _("Quit") );
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void UImanager::UnitManagerListener::ButtonPressed(){
  if (!m_element)
    m_element = GuiElement::findElement("intens_namespace_unit_manager_form");
  if (m_element)
    m_element->manage();
}

/* --------------------------------------------------------------------------- */
/* ToggleStatusChanged --                                                      */
/* --------------------------------------------------------------------------- */

void UImanager::HelpmsgListener::ToggleStatusChanged( bool state ){
  if (!state) {
    AppData::Instance().setHelpmessages( AppData::NoneType );
  } else if (m_helpMessageType == AppData::StatusBarType) {
    AppData::Instance().setHelpmessages( AppData::StatusBarType );
  } else if (m_helpMessageType == AppData::ToolTipType) {
    AppData::Instance().setHelpmessages( AppData::ToolTipType );
  } else if (m_helpMessageType == AppData::NoneType) {
    AppData::Instance().setHelpmessages( AppData::NoneType );
  }
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void UImanager::DebugPrintDataListener::ButtonPressed(){
  std::ofstream f;
  f.open( "datapool.xml" );
  std::vector<std::string> attrs;
  int level = 0;
  bool debug = false;
  // Das Result der Funktion write() ist als Debug-Information nicht geeignet.
  // Die Funktion writeDP() schreibt auch Timestamps und der Output ist etwas kompakter.
  // DataPoolIntens::Instance().getDataPool().write( f, attrs, level, debug );
  DataPoolIntens::Instance().getDataPool().writeDP( f, 0 );
  f.close();

  GuiElement *el = getButton();
  if( el ){
    el->printMessage( "File datapool.xml written", GuiElement::msg_Information );
  }
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void UImanager::DebugPrintDictListener::ButtonPressed(){
  std::ofstream f;
  f.open( "dictionary.xml" );
  std::vector<std::string> attrs;
  int level = 0;
  bool debug = false;

  DataPoolIntens::Instance().getDataPool().writeDD( f, 0 );
  f.close();

  GuiElement *el = getButton();
  if( el ){
    el->printMessage( "File dictionary.xml written", GuiElement::msg_Information );
  }
}

/* --------------------------------------------------------------------------- */
/* QtStyleListener::ToggleStatusChanged --                                     */
/* --------------------------------------------------------------------------- */
#include <qfontdialog.h>
void UImanager::QtStyleListener::ToggleStatusChanged( bool state ) {
  QApplication::setStyle( m_styleString.c_str() );

  QStyle* style = QApplication::style();

  QWidgetList  list = QApplication::allWidgets();
  QWidgetList::Iterator it = list.begin();         // iterate over the widgets
  for ( ;it !=list.end(); ++it ) {  // for each widget...
    style->unpolish( *it );
   }

  GuiManager::Instance().update( GuiElement::reason_Always );
}

/* --------------------------------------------------------------------------- */
/* QtStyleSheetListener::ToggleStatusChanged --                                */
/* --------------------------------------------------------------------------- */
void UImanager::QtStyleSheetListener::ToggleStatusChanged( bool state ) {
  GuiManager::Instance().setStylesheetName(  m_styleSheetString );
  GuiManager::Instance().update( GuiElement::reason_Always );
}

/* --------------------------------------------------------------------------- */
/* DebugDescriptionFileListener::ToggleStatusChanged --                        */
/* --------------------------------------------------------------------------- */

void UImanager::DebugDescriptionFileListener::ToggleStatusChanged(bool state) {

  if (!state) {
	// close function
	UImanager::Instance().closeDescriptionFunction();

	// set state(false)
	Debugger::ModifyDebugFlag( "DescriptionFile", state );
  } else {
	// set state(true)
	Debugger::ModifyDebugFlag( "DescriptionFile", state );

	// open new function
	bool ok;
	std::string funcName;
	bool oldValue =  GuiManager::Instance().isWaitingModalDialog();
	GuiManager::Instance().setWaitingModalDialog( true );
	do {
	  funcName = GuiFactory::Instance()->showDialogTextInput(NULL,
															_("Description file"),
															"",
															_("New function name:"),
															ok,
															NULL);
	} while ( (ok && !funcName.size()) );
	GuiManager::Instance().setWaitingModalDialog( oldValue );

	UImanager::Instance().setDesFuncName(funcName);
	DES_INFO(compose("FUNC %1 {", funcName));
  }

}

/* --------------------------------------------------------------------------- */
/* DebugPythonLogFileListener::ToggleStatusChanged --                          */
/* --------------------------------------------------------------------------- */

void UImanager::DebugPythonLogFileListener::ToggleStatusChanged(bool state) {
  QColor bg = state ? GuiQtManager::Instance().alarmForegroundColor() : GuiQtManager::Instance().backgroundColor();
  QPalette pal=  QApplication::palette();
  pal.setColor(QPalette::Window, bg);
  QApplication::setPalette(pal);

  if (!state) {
    // set state(false)
    AppData::Instance().setPyLogMode(false);
    Debugger::ModifyDebugFlag( "PythonLogFile", state );
   } else {
    // set state(true)
    AppData::Instance().setPyLogMode(true);
    Debugger::ModifyDebugFlag( "PythonLogFile", state );
  }

}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void UImanager::DebugTTrailMemListener::ButtonPressed(){
#if defined _DEBUG
  DataTTrail::showMemoryUsage( std::cerr );
  DataPool::showMemoryUsage( std::cerr );
#endif
}

/* --------------------------------------------------------------------------- */
/* ButtonPressed --                                                            */
/* --------------------------------------------------------------------------- */

void UImanager::QtDialogListener::ButtonPressed() {
#if defined  HAVE_QT && defined _DEBUG && !defined HAVE_HEADLESS

  if (m_dialog == "QFontDialog") {
    bool ok=false;
    QFontDialog::getFont(&ok);
  } else
  if (m_dialog == "QColorDialog") {
    bool ok=false;
    QPalette pal=  QApplication::palette();
    QColor bg = QColorDialog::getColor( pal.color(QPalette::Window));
    pal.setColor(QPalette::Window, bg );
    QApplication::setPalette(pal);
  } else
  if (m_dialog == "QStylesheet") {
    static StyleSheetEditor* styleSheetEditor = new StyleSheetEditor();
    styleSheetEditor->show();
    styleSheetEditor->activateWindow();
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* executeSimpleAction --                                                      */
/* --------------------------------------------------------------------------- */

void UImanager::ActionEnableUndo::executeSimpleAction(){
  DataPoolIntens::getDataPool().enableUndo();
}
