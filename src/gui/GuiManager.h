
#if !defined(GUI_MANAGER_INCLUDED_H)
#define GUI_MANAGER_INCLUDED_H

#include <stack>

#include "datapool/DataPoolDefinition.h"

#include <string>
#include <sstream>
#include <set>
#include <assert.h>

#include "gui/GuiElement.h"

class GuiDialog;
class Gui3dPlot;
class GuiPulldownMenu;
class GuiInitWorker;
class GuiEventLoopListener;
class QuitApplication;
class Plugin;
class SmartPluginInterface;
/**
   Der GuiManager ist ein Singleton-Objekt. Er ist die Wurzel aller
   GuiElement-Bäume.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiManager.h,v 1.86 2008/01/07 16:11:32 amg Exp $
*/
class GuiManager
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
protected:
  GuiManager();
public:
  virtual ~GuiManager();

/*=============================================================================*/
/* public virtual member functions                                             */
/*=============================================================================*/
public:
  static GuiManager &Instance();

  void waitCursor( bool wait, void * );
  virtual void installWaitCursor( void * ){
    assert( false ); // for Qt use Dialog::installDialogsWaitCursor
  }
  virtual void removeWaitCursor( void * ){
    assert( false ); // for Qt use Dialog::installDialogsWaitCursor
  }
  static bool loadResourceFile( const std::string &resfilename );
  static bool writeResourceFile();
  virtual void setMainTitle( const std::string &title ) = 0;
  virtual void getDisplayName( std::string &title ) = 0;
  virtual void update( GuiElement::UpdateReason ) = 0;
  virtual void updateLater( GuiElement::UpdateReason ) = 0;
  virtual void omitNextAfterUpdateForms() { m_omitNextAfterUpdateForms = true; }

  virtual void initApplication( int *argc, char **argv, const std::string & ) = 0;
  virtual void installQuitApplication( QuitApplication *quit ) = 0;
  virtual void createApplication() = 0;
  virtual bool mapApplication() = 0;
  virtual bool runApplication() = 0;
  virtual bool closeApplication() = 0;
  virtual void showStartupBannerMessage(const std::string& message) {}
  virtual void writeSettings() = 0;

  virtual bool replace( GuiElement *old_el, GuiElement *new_el ) = 0;

  virtual TransactionNumber LastGuiUpdate() = 0;
  virtual TransactionNumber LastWebUpdate() = 0;
  virtual void setWebUpdateTimestamp() = 0;
  virtual void setUpdateAlways() = 0;
  virtual void overrideLastUpdate(TransactionNumber trans) = 0;
  virtual void resetOverrideLastUpdate() = 0;

  virtual Gui3dPlot *newGui3dPlot( GuiElement *, const std::string & ) { assert( false ); return 0; }

  virtual GuiButtonbar *getMainButtonbar();
  virtual Plugin *getPlugin( const std::string & ) { assert( false ); return 0; }
  virtual SmartPluginInterface *loadPlugin( const std::string & ) { assert( false ); return 0; }

  virtual void createFormMenu( GuiPulldownMenu *menu ) = 0;

  virtual bool isWaitingModalDialog() = 0;
  virtual void setWaitingModalDialog( bool activate ) = 0;

  /** get list of names of available stylesheets
      @param bAbsolutPath short (basename) or absolute path name.
   */
  virtual std::set<std::string> getAvailableStylesheets(bool bAbsolutPath=false) = 0;
  /** set stylesheet to application
   */
  virtual std::string setStylesheetName(const std::string& stylesheet) = 0;
  /** get actual stylesheet name
   */
  virtual const std::string& stylesheetName() = 0;

  virtual void initErrorBox() = 0;
  virtual std::ostream *getErrorBoxStream() = 0;
  virtual void showErrorBox( GuiElement *el ) = 0;
  virtual void dispatchPendingEvents() = 0;
  virtual GuiElement * getElement() = 0;
  virtual GuiDialog *  getDialog() = 0;
  void serializeXML(std::ostream &os, bool recursive = false);

/*=============================================================================*/
/* public NONE virtual member functions                                        */
/*=============================================================================*/
public:
  void attachEventLoopListener( GuiEventLoopListener *lsnr );
  void checkEventLoopListener();
  bool grabButton();


/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  static GuiManager    *s_instance;
  GuiButtonbar         *m_main_buttonbar;
  bool                  m_omitNextAfterUpdateForms;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::stack<GuiEventLoopListener *> GuiEventLoopStack;

  GuiEventLoopStack     m_eventloop_stack;
  GuiEventLoopListener *m_current_eventloop_lsnr;
};

#endif
