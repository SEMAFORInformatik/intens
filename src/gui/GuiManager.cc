
#include "utils/utils.h"
#include "gui/GuiFolderGroup.h"
#include "gui/GuiManager.h"
#include "gui/GuiFactory.h"
#include "gui/GuiForm.h"

#include "job/JobManager.h"
#include "job/JobFunction.h"

#include "datapool/DataPoolDefinition.h"
#include "app/DataPoolIntens.h"

#if !defined(GUI_STANDALONE_TEST)
#include "app/AppData.h"
#include "app/log.h"
#endif

#if !defined(INTENSHOME)
#define INTENSHOME "/usr/local/intens"
#endif

#ifdef HAVE_QT
#include "gui/qt/GuiQtManager.h"
#endif

/* initialize */
GuiManager *GuiManager::s_instance = 0;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiManager::GuiManager() :
  m_current_eventloop_lsnr( 0 )
  , m_main_buttonbar( 0 )
  , m_omitNextAfterUpdateForms(false)
{}

GuiManager::~GuiManager(){
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */

GuiManager &GuiManager::Instance(){
  if( s_instance == 0 ){
    s_instance = GuiFactory::Instance()->createManager();
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* waitCursor --                                                               */
/* --------------------------------------------------------------------------- */

void GuiManager::waitCursor( bool wait, void *installer ){
  if( wait ){
    installWaitCursor( installer );
  }
  else{
    removeWaitCursor( installer );
  }
}

/* --------------------------------------------------------------------------- */
/* attachEventLoopListener --                                                  */
/* --------------------------------------------------------------------------- */

void GuiManager::attachEventLoopListener( GuiEventLoopListener *lsnr ){
  BUG_PARA(BugGuiMgr,"GuiManager::attachEventLoopListener", "Listener = " << lsnr );
  assert( lsnr != 0 );
  if( m_current_eventloop_lsnr != 0 ){
    BUG_MSG("Push " << m_current_eventloop_lsnr << " on the stack" );
    m_eventloop_stack.push( m_current_eventloop_lsnr );
  }
  m_current_eventloop_lsnr = lsnr;
}

/* --------------------------------------------------------------------------- */
/* checkEventLoopListener --                                                   */
/* --------------------------------------------------------------------------- */

void GuiManager::checkEventLoopListener(){
  if( m_current_eventloop_lsnr != 0 ){
    if( !m_current_eventloop_lsnr->stayHere() ){
      if( m_eventloop_stack.empty() ){
	m_current_eventloop_lsnr = 0;
      }
      else{
	m_current_eventloop_lsnr = m_eventloop_stack.top();
	m_eventloop_stack.pop();
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* grabButton --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiManager::grabButton(){
  // BUG_PARA(BugGuiMgr,"GuiManager::grabButton", "Listener = " << m_current_eventloop_lsnr );

  if( m_current_eventloop_lsnr != 0 ){
    return m_current_eventloop_lsnr->grabButton();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiManager::serializeXML(std::ostream &os, bool recursive){
}

/* --------------------------------------------------------------------------- */
/* loadResourceFile --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiManager::loadResourceFile( const std::string &resfilename ) {
#if defined HAVE_QT && !defined HAVE_HEADLESS
  return GuiQtManager::loadResourceFile( AppData::Instance().ResourceFile() );
#endif
  return false;
}

/* --------------------------------------------------------------------------- */
/* writeResourceFile --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiManager::writeResourceFile() {
#if defined HAVE_QT && !defined HAVE_HEADLESS
  return GuiQtManager::writeResourceFile();
#endif
  return false;
}

/* --------------------------------------------------------------------------- */
/* getMainButtonbar --                                                         */
/* --------------------------------------------------------------------------- */

GuiButtonbar *GuiManager::getMainButtonbar(){
  if( m_main_buttonbar == 0 ){
    m_main_buttonbar = GuiFactory::Instance() -> createButtonbar( 0 );
  }
  return m_main_buttonbar;
}
