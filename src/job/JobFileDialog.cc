
#include "job/JobIncludes.h"

#include "utils/gettext.h"
#include "job/JobFileDialog.h"
#include "job/JobStackDataString.h"
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "xfer/XferDataItem.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

JobFileDialog::JobFileDialog( JobEngine *eng )
  : m_element( 0 )
  , m_eng( eng )
  , m_dirname( "" )
  , m_filter( "" )
  , m_dironly(true)
  , m_dirnamexfer( 0 )
  , m_dialog( 0 ){
  setLogOff();
}

JobFileDialog::~JobFileDialog(){
  delete m_dirnamexfer;
}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void JobFileDialog::startJobAction(){
  if ( m_dirnamexfer ) {
    m_dirnamexfer->getValue( m_dirname );
  }
  BUG_DEBUG("show FileSelection-Dialog with Directory " << m_dirname << ")");

  GuiFactory::Instance()->showDialogFileSelection
    ( 0
      , m_dironly?"Choose a directory":"Choose a file"
      , m_filter
      , m_dirname
      , this
      , m_open ? DialogFileSelection::Open : DialogFileSelection::Save
      , 0
      , m_dironly?DialogFileSelection::DirectoryOnly:DialogFileSelection::ExistingFiles );
}

/* --------------------------------------------------------------------------- */
/* stopJobAction --                                                            */
/* --------------------------------------------------------------------------- */

void JobFileDialog::stopJobAction(){
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* FileSelected --                                                             */
/* --------------------------------------------------------------------------- */

void JobFileDialog::FileSelected( const std::string &filename
				  , const HardCopyListener::FileFormat ff
				  , const std::string dir ){
  BUG_DEBUG("FileSelected(" << filename << ", " << dir << ")");
  JobStackDataString *item = new JobStackDataString( filename );
  m_eng->push( item );
  if( m_dialog ){
    BUG_DEBUG("set Directory to '" << dir << "'");
    m_dialog->setDirname( dir );
  }
  endJobAction( JobAction::job_Ok_ignore );
}

/* --------------------------------------------------------------------------- */
/* canceled --                                                                 */
/* --------------------------------------------------------------------------- */

void JobFileDialog::canceled(){
  BUG_DEBUG("canceled");
  m_eng->pushInvalid();
  endJobAction( JobAction::job_Ok_ignore );
}
