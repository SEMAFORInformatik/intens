
#include <string>
#include <assert.h>

#include "parser/errorhandler.h"
#include "utils/gettext.h"
#include "gui/GuiFactory.h"
#include "app/UiManager.h"
#include "app/ReportGen.h"
#include "operator/ReportStream.h"
#include "streamer/Stream.h"

ReportStream::ReportStream( const std::string &name, Stream *repstr )
  : m_stream(repstr)
    , m_saveButton( 0 )
    , m_title(name)
    , m_saveListener( this )
    , m_hidden( false )
    , m_fileFormat( HardCopyListener::Text ){
  m_xslFileName = "Gui2LaTeX.xsl";
  UImanager::Instance().addHardCopy(m_title, this);
}

ReportStream::~ReportStream(){
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */
const std::string &ReportStream::getName() {
  assert( m_stream != 0 );
  return m_stream->Name();
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool ReportStream::write( InputChannelEvent &event ) {
  if( m_stream != 0 ){
#if defined HAVE_QT
    std::ostringstream os;
    bool ret = m_stream->write( os );
    event.write( os );
    return ret;
#else
    return m_stream->write( event.getOstream() );
#endif
  }
  return false;
}

bool ReportStream::write( std::ostream &os ) {
  if( m_stream != 0 ){
    return m_stream->write( os );
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool ReportStream::isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){
  return (  fileFormat == m_fileFormat ||
	   (fileFormat == HardCopyListener::Postscript && m_fileFormat == HardCopyListener::XML) );
}


/* --------------------------------------------------------------------------- */
/* isExportPrintFormat --                                                      */
/* --------------------------------------------------------------------------- */
bool ReportStream::isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat ) {
  if (fileFormat == HardCopyListener::LaTeX)
    return true;
  if (fileFormat == HardCopyListener::PDF)
    return true;
  if (fileFormat == HardCopyListener::Postscript)
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getPrintType --                                                             */
/* --------------------------------------------------------------------------- */
HardCopyListener::PrintType ReportStream::getPrintType( const HardCopyListener::FileFormat &fileFormat ) {
// DEBUG  std::cout <<    "  m_fileFormat["<<m_fileFormat<<"] in ff["<<fileFormat<<"]\n";
  if (fileFormat == HardCopyListener::ASCII ||
      fileFormat == HardCopyListener::Postscript ||
      fileFormat == HardCopyListener::PDF )
    return HardCopyListener::SCRIPT;
  return isFileFormatSupported(fileFormat)? HardCopyListener::SCRIPT : HardCopyListener::NOT_SUPPORTED;
}

/* --------------------------------------------------------------------------- */
/* getDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */
bool ReportStream::getDefaultSettings( HardCopyListener::PaperSize &size,
				       HardCopyListener::Orientation &orient,
				       int &quantity,
				       HardCopyListener::FileFormat &format,
				       double &lm, double &rm,
				       double &tm, double &bm ) {
  size = HardCopyListener::A4;
  orient = HardCopyListener::Portrait;
  quantity = 1;
  format = HardCopyListener::Postscript;
  lm = 10;
  rm = 10;
  tm = 10;
  bm = 10;

  return true;
}

/* --------------------------------------------------------------------------- */
/* install --                                                                  */
/* --------------------------------------------------------------------------- */

bool ReportStream::install(){
  if( m_hidden ){
    setMenuInstalled();
    return true;
  }
  UImanager &ui = UImanager::Instance();
  GuiMenuButtonListener *lsnr = 0;

  lsnr = new SaveButtonListener( this );
  if( ui.addSaveButton( m_title, lsnr ) ){
    lsnr->setDialogLabel( m_title );
  }
  else{
    delete lsnr;
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

bool ReportStream::saveFile( GuiElement *e ){
  BUG( BugOperator, "ReportStream::saveFile" );
  m_saveButton = e;
  std::vector<HardCopyListener::FileFormat> formats;
  ReportGen::getFormats( this, formats );
  GuiFactory::Instance()->showDialogFileSelection
    ( 0
      , compose(_("Save %1"),m_title)
      , "PDF (*.pdf)", m_dir
      , &m_saveListener
      , DialogFileSelection::Save
      , &formats
      , DialogFileSelection::AnyFile
      , getName());
  return true;
}

/* --------------------------------------------------------------------------- */
/* FileSelected --                                                             */
/*   called by DialogFileSelection                                             */
/* --------------------------------------------------------------------------- */

void ReportStream::SaveListener::FileSelected( const std::string &fn
					       , const HardCopyListener::FileFormat format
					       , const std::string dir ){
  BUG( BugOperator, "ReportStream::SaveListener::FileSelected" );
  m_stream->setDirname( dir );
  std::string filename = fn;
//   struct stat sbuf;
//   if( stat( filename.c_str(), &sbuf ) == 0 ){
//     m_filename = filename;
//     m_saveFormat = format;
//     GuiFactory::Instance()->showDialogConfirmation
//       ( 0
// 	, "Save anyway"
// 	, compose(_("File '%1' already exists.\nSave it anyway?"),filename)
// 	, this );

//     return;
//   }
  m_stream->writeFile( filename, format );
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void ReportStream::SaveListener::confirmYesButtonPressed(){
  BUG( BugOperator, "ReportStream::confirmYesButtonPressed" );
  m_stream->writeFile( m_filename, m_saveFormat );
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void ReportStream::SaveListener::confirmNoButtonPressed(){
  BUG( BugOperator, "ReportStream::confirmNoButtonPressed" );
  //  m_stream->endJobAction( JobAction::job_Canceled );
}

/* --------------------------------------------------------------------------- */
/* canceled --                                                                 */
/* --------------------------------------------------------------------------- */

void ReportStream::SaveListener::canceled(){
  BUG( BugOperator, "ReportStream::SaveListener::canceled" );
  //  m_stream->endJobAction( JobAction::job_Canceled );
}

/* --------------------------------------------------------------------------- */
/* writeFile --                                                                 */
/* --------------------------------------------------------------------------- */

void ReportStream::writeFile( const std::string &filename, HardCopyListener::FileFormat ff ){
  BUG( BugOperator, "ReportStream::writeFile" );
  assert( m_saveButton != 0 );
  ReportGen::Instance().printDocument( ReportGen::SAVE
				       , this
				       , m_saveButton
				       , filename
				       , ""
				       , 0
				       , HardCopyListener::Portrait
				       , 1
				       , ff );

}
