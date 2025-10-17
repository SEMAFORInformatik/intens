
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <wchar.h>

#define Arg XArg
#include "app/ReportGen.h"
#include "app/HardCopyListener.h"
#include "gui/GuiElement.h"
#ifdef HAVE_QT
#include <qdir.h>
#include <qprocess.h>
#endif

#include "utils/Debugger.h"

#include "gui/DialogWorkClock.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiFactory.h"
#include "operator/BatchProcess.h"
#include "operator/Channel.h"
#include "operator/ReportStreamFileBuffer.h"
#include "utils/gettext.h"
#include "xml/XMLFactory.h"
#include "app/HardCopyListener.h"
#include "gui/HardCopyTree.h"
#include "app/AppData.h"
#include "utils/StringUtils.h"
#undef Arg

#define PRINTER_CMD "bin/ReportConv.py -P"
#define LPNUM_COPIES " -#"

static const std::string XSL_SUFFIX  = "xsl";
static const std::string EPS_SUFFIX  = "eps";
static const std::string PS_SUFFIX   = "ps";
static const std::string PDF_SUFFIX  = "pdf";
static const std::string HPGL_SUFFIX = "hpgl";
static const std::string PNG_SUFFIX  = "png";
static const std::string SVG_SUFFIX  = "svg";
static const std::string TXT_SUFFIX  = "txt";

static const std::string EPS_PREFIX  = "PS";
static const std::string PDF_PREFIX  = "PDF";
static const std::string HPGL_PREFIX = "HPGL";
static const std::string PNG_PREFIX  = "PNG";
static const std::string TXT_PREFIX  = "TXT";

static const std::string TEMPLATE_DIR  = "templates";

static std::string s_xsdFileLocation;

// initialisation
ReportGen *ReportGen::s_instance = 0;

/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/

ReportGen::ReportGen()
  : m_repGenInputChannel( 0, "", 0 )
  , m_repGenOutputChannel( 0 )
  , m_outputInChannel( 0, "", 0 )
  , m_outputOutChannel( 0 )
#ifdef __MINGW32__
  , m_previewFile( tempnam( "/tmp", "preview" ) )
  , m_currentFile( tmpnam(0) )
#else
  , m_currentFile( "")
#endif
  , m_currentDialog( 0 )
  , m_currentPaperSize( HardCopyListener::A4 )
  , m_currentListener( 0 )
  , m_currentStream( 0 )
  , m_fileBufferListener( 0 )
  , m_dialogWorkClock( 0 ) {
#ifndef __MINGW32__
  char tfile[200] = "/tmp/previewXXXXXX";
int fpP=
  mkstemp( tfile );
  m_previewFile = tfile;
  strcpy(tfile, "/tmp/currentXXXXXX");
int fpC=
  mkstemp( tfile );
  m_currentFile = tfile;
  remove(m_previewFile.c_str());
  remove(m_currentFile.c_str());
#endif

#ifdef HAVE_QT
  m_repGenInputChannel.setQProcess(  m_repGenProcess.getQProcess() );
  m_outputInChannel.setQProcess(  m_outputProcess[3].getQProcess() );
#endif
  m_repGenProcess.addChannel( &m_repGenInputChannel );
  m_repGenProcess.addChannel( &m_repGenOutputChannel );
  m_outputProcess[3].addChannel( &m_outputOutChannel );
  m_outputProcess[3].addChannel( &m_outputInChannel );
}

ReportGen::~ReportGen(){
  if( m_dialogWorkClock )
    delete m_dialogWorkClock;
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */
ReportGen &ReportGen::Instance(){
  if( s_instance == 0 ){
    s_instance = new ReportGen();
  }
  return *s_instance;
}

/* --------------------------------------------------------------------------- */
/* validatePreviewFile --                                                      */
/* --------------------------------------------------------------------------- */

void ReportGen::validatePreviewFile() {
#ifdef HAVE_QT
  // replace '\' to '/' that makes reportconv happy
  for( std::string::size_type p=m_previewFile.find_first_of( '\\', 0 );
       p!=std::string::npos;
       p=m_previewFile.find_first_of( '\\', ++p ) ){
    m_previewFile[p]='/';
  }
#endif

  std::string extension( getSuffix( m_currentFileFormat ) );
  // we must replace all underscores (LaTeX will love us) and make
  // sure that the file does not already exist
  std::string repstr("_.");
  char repchar='Z';

#ifndef HAVE_QT
  struct stat filestat;
#endif
  do {
#ifndef _WIN32
      // 2009-10-05 warum will man einen bestehenden Verzeichnisnamen (basename) des Files umbenennen???
      // man muesste ja dann auch das Verzeichnis im Filesystem umbenennen
      // fuer linux ist das allerdings in Ordnung da verzeichnis immer '/tmp' ist
    for( std::string::size_type p=m_previewFile.find_first_of( repstr, 0 );
	 p!=std::string::npos;
	 p=m_previewFile.find_first_of( repstr, ++p ) ){
      m_previewFile[p]=repchar;
    }
#endif
    // generate a new unique filename
#if HAVE_QT
    while ( QFile::exists( appendExtension( m_previewFile, extension ).c_str()) ) {
#else
    while (stat( appendExtension( m_previewFile, extension ).c_str(), &filestat ) != -1) {
#endif
      wchar_t c =  m_previewFile[m_previewFile.size()-1];
      if ( (c>= '0' && c < '9') || (c>= 'a' && c < 'z') || (c>= 'A' && c < 'Z') ) {
	m_previewFile[m_previewFile.size()-1] = ++c;
      } else
	m_previewFile.push_back( 'a' );
    }
    --repchar;
#if HAVE_QT
    } while( repchar > 'A' && QFile::exists(appendExtension( m_previewFile, extension ).c_str()) );
#else
  } while( repchar > 'A' && stat( appendExtension( m_previewFile, extension ).c_str(), &filestat ) != -1 );
#endif
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */
bool ReportGen::writeXML( std::ostringstream &xmlStream, std::ostream &os, const std::string &xslFileName ){
  std::ostringstream xalanOutputStream;
  runXalan( xmlStream, xslFileName, xalanOutputStream );
  std::string str( xalanOutputStream.str() );
  os << str;
  return true;
}
bool ReportGen::writeXML( HardCopyListener *hcl, std::string &fileName ){
  m_fileBufferListener = 0;
  std::string name;
  std::string extension;
  switch( m_currentListener->getFileFormat() ){
  case HardCopyListener::XML :
    // doent put fileName into m_fileNames !!
    extension = getSuffix( m_currentFileFormat );
    name = getPrefix( m_currentFileFormat );
    break;
  case HardCopyListener::Postscript :
    extension = getSuffix( m_currentFileFormat );
    name = getPrefix( m_currentFileFormat );
    newTmpFile( appendExtension( fileName, extension ) );
    break;
  case HardCopyListener::PDF :
    name = PNG_PREFIX;
    extension = PNG_SUFFIX;
    newTmpFile( appendExtension( fileName, extension ) );
    break;
  case HardCopyListener::LaTeX :
    switch( m_currentFileFormat ){
    case HardCopyListener::Postscript :
    case HardCopyListener::PDF :
      name = EPS_PREFIX;
      extension = PS_SUFFIX;
      newTmpFile( appendExtension( fileName, extension ) );
      break;
    default :
      break;
    }
    break;
  default :
    break;
  }
  std::ofstream ofs( appendExtension( fileName, extension ).c_str() );
  std::ostream &reportStream = ofs;
  if( name.empty() ){
    reportStream << std::endl;
    return true;
  }
  std::string xslFileName;
  name += hcl->getXSLFileName();
  if( name.find("<XML>") != std::string::npos ){
    hcl->write( reportStream );
  }
  else {
    xslFileName = locateXSLFile( name, XSL_SUFFIX );
    if( xslFileName.empty() ){
      std::string msg;
      msg = name + " (" + XSL_SUFFIX + ") : " + _("Stylesheet not found.");
      setExitMsg( msg );
      return false;
    }
    std::ostringstream xmlStream;
    std::ostringstream xalanOutputStream;
    bool ret = hcl->write( xmlStream );
    if( !ret || !runXalan( xmlStream, xslFileName, xalanOutputStream ) ){
      std::string msg;
      msg = _("XML Process failed ( see log window ).");
      setExitMsg( msg );
      return false;
    }
    std::string str( xalanOutputStream.str() );
    // Leerzeilen entfernen
    std::string::size_type endPos = str.find_first_not_of( "\n" );
    if( endPos != std::string::npos )
      str.erase( 0, endPos );
    reportStream << str;

    // latex oder ps im xalanOutputStream ??
    if( str.find("documentclass") != std::string::npos ){
      m_fileBufferListener = new ReportStreamFileBuffer( str );
    } else {
      m_fileBufferListener = 0;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* generateXMLReport --                                                        */
/* --------------------------------------------------------------------------- */
bool ReportGen::generateXMLReport(){
  m_checkExitStatus = false;
  std::string fileName;
  if( m_currentMode == PREVIEW )
    fileName = m_previewFile;
  else
    fileName = m_tmpFile;
  if( !( writeXML( m_currentListener, fileName ) ) ){
    if( m_exitMsg.empty() )
      m_exitMsg = _("Printing aborted.");
    m_currentDialog->printMessage( m_exitMsg, GuiElement::msg_Error );
    m_exitMsg="";
    return false;
  }
  backFromWorker( JobAction::job_Ok );
  return true;
}

/* --------------------------------------------------------------------------- */
/* work --                                                                     */
/* --------------------------------------------------------------------------- */
JobAction::JobResult ReportGen::work(){
  if( m_repGenProcess.hasTerminated() ){
    return JobAction::job_Ok;
  }
  return JobAction::job_Wait;
}

/* --------------------------------------------------------------------------- */
/* backFromWorker --                                                           */
/* --------------------------------------------------------------------------- */
void ReportGen::backFromWorker( JobAction::JobResult rslt ){

 if( m_dialogWorkClock ){
    m_dialogWorkClock->unmanage();
    delete m_dialogWorkClock;
    m_dialogWorkClock=0;
  }

  if( rslt != JobAction::job_Ok )
    return;
  if( m_currentListener->getFileFormat() == HardCopyListener::XML ){
    switch( m_currentMode ){
    case PREVIEW :
      preview();
      break;
    case SAVE :
      save();
      break;
    case PRINT :
      print();
      break;
    }
  }
  else{
    if( m_currentMode == PREVIEW ){
      preview();
    }
    if( m_currentMode == SAVE ){
      save();
    }
    else{
      end();
    }
  }
  std::set<std::string>::iterator it;
  for( it = m_tmpFiles.begin(); it != m_tmpFiles.end(); ++it )
    unlink( (*it).c_str() );
}

/* --------------------------------------------------------------------------- */
/* cancelButtonPressed --                                                      */
/* --------------------------------------------------------------------------- */
void ReportGen::cancelButtonPressed(){
  m_repGenProcess.stop();
  m_currentDialog->printMessage( _("Printing canceled."), GuiElement::msg_Warning );
}

/* --------------------------------------------------------------------------- */
/* end --                                                                      */
/* --------------------------------------------------------------------------- */
void ReportGen::end(){
  if( m_repGenProcess.getExitStatus() == 0 ){
    m_currentDialog->printMessage( _("Printing done."), GuiElement::msg_Information );
    if (m_jobAction) {
      m_jobAction->endJobAction( JobAction::job_Ok_ignore  );
      m_jobAction = 0;
    }
  }
  else{
    if( m_exitMsg.empty() )
      m_exitMsg = _("Printing aborted.");
    m_currentDialog->printMessage( m_exitMsg, GuiElement::msg_Error );
    m_exitMsg="";
    if (m_jobAction) {
      m_jobAction->endJobAction( JobAction::job_Aborted  );
      m_jobAction = 0;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */
void ReportGen::print(){
  if( m_fileBufferListener == 0 ){
    if( m_checkExitStatus ){
      if( m_repGenProcess.getExitStatus() != 0 ){
	if( m_exitMsg.empty() )
	  m_exitMsg = _("Printing aborted.");
	m_currentDialog->printMessage( m_exitMsg, GuiElement::msg_Error );
	m_exitMsg="";
	return;
      }
    }
    std::string extension( getSuffix( m_currentFileFormat ) );
    m_currentDialog->printMessage( _("Printing started."), GuiElement::msg_Information );
    std::ostringstream printCommand;
    printCommand << "lpr -P " << m_currentPrinter << " " << appendExtension( m_tmpFile, extension );
    m_outputProcess[3].setExecCmd( printCommand.str() );
    m_outputProcess[3].start();
  }
}

/* --------------------------------------------------------------------------- */
/* save --                                                                     */
/* --------------------------------------------------------------------------- */
#include "utils/FileUtilities.h"

void ReportGen::save(){
  if( m_checkExitStatus ){
    if( m_repGenProcess.getExitStatus() != 0 ){
      if( m_exitMsg.empty() )
	m_exitMsg = _("Save file aborted.");
      m_currentDialog->printMessage( m_exitMsg, GuiElement::msg_Error );
      m_exitMsg="";
      return;
    }
  }

  m_currentDialog->printMessage( _("Save file."), GuiElement::msg_Information );
  if ( FileUtilities::NoFile == FileUtilities::FileInfo(appendExtension( m_tmpFile, getSuffix( m_currentFileFormat)))  )
    return;
  std::ostringstream saveCommand;
  saveCommand << "cp "
	      << appendExtension( m_tmpFile, getSuffix( m_currentFileFormat ) )
	      << " "  << m_currentFile;

  m_outputProcess[3].setExecCmd( saveCommand.str() );
  m_outputProcess[3].start();

}

/* --------------------------------------------------------------------------- */
/* appendExtension --                                                          */
/* --------------------------------------------------------------------------- */

std::string ReportGen::appendExtension( const std::string &name, const std::string &extension ){
  BUG( BugMisc, "ReportGen::appendExtension" );
  std::string::size_type n = 0;

  std::string filename = name;
  if( extension.empty() ){
    BUG_EXIT( "no extension available" );
    return filename;
  }
  if( extension.find("*") != std::string::npos ){
    BUG_EXIT( "extension has asterisk " );
    return filename;
  }

  n = filename.find_last_of( '.' );
  if( n != std::string::npos ){
    if( n < filename.size() ){
      std::string ext = filename.substr( n+1 );
      if( !ext.empty() ){
	if( ext == extension || lower(ext) == extension){
	  BUG_EXIT("already extended");
	  return filename;
	}
	if( ext.size() < 6){
	  BUG_EXIT("has a other extension: "<< ext);
	  return filename;
	}
	filename.append( "." );
      }
    }
  }
  else{
    filename.append( "." );
  }
  filename.append( extension );
  BUG_EXIT( "extended filename '" << filename << "'" );
  return filename;
}

// --------------------------------------------------------------------------- //
// getSuffix --                                                                //
// --------------------------------------------------------------------------- //

const std::string ReportGen::getSuffix( const HardCopyListener::FileFormat format ){
  switch( format ){
  case HardCopyListener::Postscript :
    return EPS_SUFFIX;
    break;
  case HardCopyListener::PDF :
    return PDF_SUFFIX;
    break;
  case HardCopyListener::HPGL :
    return HPGL_SUFFIX;
    break;
  case HardCopyListener::PNG :
    return PNG_SUFFIX;
    break;
  case HardCopyListener::SVG :
    return SVG_SUFFIX;
    break;
  case HardCopyListener::ASCII :
    return TXT_SUFFIX;
    break;
  default :
    std::cerr << " Unknown Suffix format["<<format<<"]\n"<<std::flush;
//     assert( false );
  }
  return "";
}

// --------------------------------------------------------------------------- //
// getPrefix --
// --------------------------------------------------------------------------- //
const std::string ReportGen::getPrefix( const HardCopyListener::FileFormat format ){
  switch( format ){
  case HardCopyListener::Postscript :
    return EPS_PREFIX;
    break;
  case HardCopyListener::PDF :
    return PDF_PREFIX;
    break;
  case HardCopyListener::HPGL :
    return HPGL_PREFIX;
    break;
  case HardCopyListener::PNG :
    return PNG_PREFIX;
    break;
  case HardCopyListener::ASCII :
    return TXT_PREFIX;
    break;
  default :
    assert( false );
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* preview --                                                                  */
/* --------------------------------------------------------------------------- */
void ReportGen::preview(){
  if( m_checkExitStatus ){
    if( m_repGenProcess.getExitStatus() != 0 ){
      if( m_exitMsg.empty() )
	m_exitMsg = _("Preview aborted.");
      m_currentDialog->printMessage( m_exitMsg, GuiElement::msg_Error );
      m_exitMsg="";
      return;
    }
  }
  std::ostringstream previewCommand;
  int i = 0;
  std::string extension;
  switch( m_currentFileFormat ){
  case HardCopyListener::Postscript :
#ifdef __MINGW32__
    if (AppData::Instance().PsPreviewer().find("gv") != 0 &&
	AppData::Instance().PsPreviewer().find("gsviewer32") != 0) {
      extension = PDF_SUFFIX;
      break;
    }
#else
    if( m_currentListener->getFileFormat() == HardCopyListener::XML ){
      i=0;
      if( m_fileBufferListener == 0 )
	extension = EPS_SUFFIX;
      else
	extension = PS_SUFFIX;

    }
    else{
      i=1;
      if (AppData::Instance().PsPreviewer().find("acroread") == 0) {
	extension = PDF_SUFFIX;
	break;
      }
    }
#endif
    extension = PS_SUFFIX;
    break;
  case HardCopyListener::PDF :
    extension = PDF_SUFFIX;
    i=2;
    break;
  default :
    m_currentDialog->printMessage( _("Preview of selected file format is not supported."), GuiElement::msg_Error );
    return;
  }
  m_currentDialog->printMessage( _("Preview started."), GuiElement::msg_Information );

  if( !m_outputProcess[i].hasTerminated() ) {
    if( m_outputProcess[i].hangup() ){
      return;
    }
  }
#ifdef __MINGW32__
  std::string fn( appendExtension( m_previewFile, extension ) );
  int pos = fn.find_last_of('/');
  std::string dir = fn.substr(0, pos);
  fn = fn.substr(pos+1);
  std::cerr << "TODO: how to handle platform: " << QSysInfo::productVersion().toStdString() << std::endl;

  if (QSysInfo::productVersion() == "QSysInfo::WV_2003") {
    previewCommand << "start %TEMP%/" << fn;  // win64bit
  } else {
    previewCommand << "start " << fn;  // "/bin/start"

    m_outputProcess[i].getQProcess()->setWorkingDirectory( QString::fromStdString( dir ) );
    if ( m_outputProcess[i].getQProcess()->workingDirectory().isEmpty() ) {
      m_outputProcess[i].getQProcess()->setWorkingDirectory( getenv("TMP") );
    }
    m_files.insert( appendExtension( m_previewFile, extension ) );
    m_outputProcess[i].setShell( "sh.exe" );
  }
#else
  QString program(QString::fromStdString(AppData::Instance().PsPreviewer()));
  QStringList arguments({QString::fromStdString(appendExtension( m_previewFile, extension))});
  m_files.insert( appendExtension( m_previewFile, extension ) );
  m_outputProcess[i].setShell( "" );
#endif

#ifdef __MINGW32__
std::cout << " try to start cmd["<<previewCommand.str()<<"] fn["<<fn<<"] dir["<<dir<<"]\n"<<std::flush;
  m_outputOutChannel.setListener( m_currentListener );
  m_outputProcess[i].setExecCmd( previewCommand.str() );
  m_outputProcess[i].start();
#elif defined(HAVE_QT)
  QProcess::startDetached(program, arguments);
#else
  // TODO:
#endif
}

/* --------------------------------------------------------------------------- */
/* runXalan --                                                                 */
/* --------------------------------------------------------------------------- */
bool ReportGen::runXalan( std::ostringstream &xmlStream
			  , const std::string &xslFileName
			  , std::ostringstream &outputStream ){
  XMLFactory &xmlFactory = XMLFactory::Instance();
  xmlFactory.reset();
  if( m_currentFileFormat == HardCopyListener::ASCII )
    xmlFactory.setXalanFormatterToXML();
  bool rslt = true;

  if( !xmlFactory.process( xmlStream, xslFileName, outputStream ) ){
    outputStream << '\0';
    rslt = false;
  }
  xmlFactory.reset();
  return rslt;
}

/* --------------------------------------------------------------------------- */
/* checkFormats --                                                             */
/* --------------------------------------------------------------------------- */
bool ReportGen::checkFormats( HardCopyListener *hcl, HardCopyListener::FileFormat ff ){
  bool result = false;
  assert( hcl != 0 );
  switch( hcl->getFileFormat() ){
  case HardCopyListener::LaTeX :
    switch( ff ){
    case HardCopyListener::Postscript :
    case HardCopyListener::PDF :
    case HardCopyListener::ASCII :
      result = true;
      break;
    default :
      result = false;
      break;
    }
    break;
  case HardCopyListener::Text :
    switch( ff ){
    case HardCopyListener::Postscript :
    case HardCopyListener::ASCII :
    case HardCopyListener::PDF :
      result = true;
      break;
    default :
      result = false;
      break;
    }
    break;
  default :
    result = hcl->isFileFormatSupported( ff );
    break;
  }
  return result;
}

void ReportGen::getFormats( HardCopyListener *hcl, std::vector<HardCopyListener::FileFormat> &formats ){
  if( checkFormats( hcl, HardCopyListener::PDF ) )
    formats.push_back( HardCopyListener::PDF );
  if( checkFormats( hcl, HardCopyListener::JPEG ) )
    formats.push_back( HardCopyListener::JPEG );
  if( checkFormats( hcl, HardCopyListener::PNG ) )
    formats.push_back( HardCopyListener::PNG );
  if( checkFormats( hcl, HardCopyListener::Postscript ) )
    formats.push_back( HardCopyListener::Postscript );
  if( checkFormats( hcl, HardCopyListener::HPGL ) )
    formats.push_back( HardCopyListener::HPGL );
  if( checkFormats( hcl, HardCopyListener::ASCII ) )
    formats.push_back( HardCopyListener::ASCII );
}

/* --------------------------------------------------------------------------- */
/* printDocument --                                                            */
/* --------------------------------------------------------------------------- */
bool ReportGen::printDocument( Mode mode
			       , HardCopyListener *hcl
			       , GuiElement *dialog
			       , std::string fileName
			       , const std::string &printer
			       , int paperSize
			       , HardCopyListener::Orientation orientation
			       , int quantity
			       , HardCopyListener::FileFormat fileFormat
			       , JobAction * jobAction ){
  m_currentDialog = dialog;
  m_currentMode = mode;
  m_currentListener = hcl;
  m_currentFile = fileName;
  m_currentPrinter = printer;
  m_currentPaperSize = paperSize;
  m_currentQuantity = quantity;
  m_currentFileFormat = fileFormat;
  m_currentOrientation = orientation;
  m_jobAction = jobAction;

#if _WIN32
  std::string tmp(m_currentPrinter);
  std::string::size_type pos = 0;
  while (  (pos = tmp.find_first_of ( "\\", pos ) ) != std::string::npos ) {
    tmp.replace(pos, 1, "\\\\");
    pos +=2;
  }
  m_currentPrinter = tmp;
#endif

  if( m_fileBufferListener != 0 ){
    delete m_fileBufferListener;
    m_fileBufferListener = 0;
  }

  if( m_currentListener == 0 ){
    m_currentDialog->printMessage( _("No such conversion available."), GuiElement::msg_Warning );
    return false;
  }
  if( m_currentListener->isEmpty() ){
    m_currentDialog->printMessage( _("Empty document."), GuiElement::msg_Warning );
    return false;
  }

  if( !checkFormats( m_currentListener, m_currentFileFormat) ){
    m_currentDialog->printMessage( _("Selected file format is not supported."), GuiElement::msg_Error );
    return false;
  }
  if( !m_tmpFile.empty() )
    unlink( m_tmpFile.c_str() );
#ifdef __MINGW32__
  m_tmpFile = ( tempnam( "/tmp", m_currentListener->getName().c_str() ) );
  m_previewFile = tempnam("/tmp", "preview");
#else
  char tfile[200]="";
  if (getenv("TMP"))
    strcpy(tfile, getenv("TMP"));
  if (strlen(tfile)==0) strcpy(tfile, "/tmp/");
  strcpy(tfile+strlen(tfile), m_currentListener->getName().c_str());
  strcpy(tfile+strlen(tfile), "XXXXXX");
int fpC=
  mkstemp( tfile );
  m_tmpFile = tfile;
  newTmpFile( m_tmpFile ); // to remove this file
  strcpy(tfile, "/tmp/previewXXXXXX");
int fpP=
  mkstemp( tfile );
  m_previewFile = tfile;
  remove(m_previewFile.c_str());
  remove(m_currentFile.c_str());
  close(fpP);
#endif
#ifdef __MINGW32__
   // special case on windows becourse of bad tempname function
   pos = m_previewFile.find("preview");
   if (pos!=std::string::npos)
;//     m_previewFile = m_previewFile.substr(0, pos-1) + tempnam("/tmp", "preview");
#endif
  validatePreviewFile();

  std::string repstr("_.");
  char repchar='Z';
  // we must replace all underscores (LaTeX will love us) and make
  // sure that the file does not already exist
#ifndef HAVE_QT
  struct stat filestat;
#endif
  do {
    for( std::string::size_type p=m_tmpFile.find_first_of( repstr, 0 );
	 p!=std::string::npos;
	 p=m_tmpFile.find_first_of( repstr, ++p ) ){
      m_tmpFile[p]=repchar;
    }
    --repchar;
#ifdef HAVE_QT
  } while( repchar > 'A' && QFile::exists(m_tmpFile.c_str()) );
#else
  } while( repchar > 'A' && stat( m_tmpFile.c_str(), &filestat ) != -1 );
#endif
  if( m_currentListener->getFileFormat() == HardCopyListener::XML ){
    if( hcl->getXSLFileName() != "<XML>" ){
      bool retval = generateXMLReport();
      if( m_fileBufferListener == 0 ) return retval;
    }
  }
  m_checkExitStatus = true;

  std::string print_command;

  bool printer_command_installed = false;

  // check if there is a special filter requested
  if( !m_currentListener->PrintFilter().empty() ){
    print_command =m_currentListener->PrintFilter() + " | ";
  }

  std::string formatOption;
  std::string previewSuffix;
  switch( m_currentFileFormat ){
  case HardCopyListener::Postscript :
    formatOption = "-Fpostscript";
    break;
  case HardCopyListener::PDF :
    formatOption = "-Fpdf";
    break;
  case HardCopyListener::ASCII :
    formatOption = "-Fascii";
    break;
  case HardCopyListener::JPEG :
    formatOption = "-Fjpeg";
    break;
  case HardCopyListener::PNG :
    formatOption = "-Fpng";
    break;
  case HardCopyListener::GIF :
    formatOption = "-Fgif";
    break;
  default :
    break;
  }
  if( orientation == HardCopyListener::Landscape ){
    formatOption += " -Olandscape";
  }
  else {
    formatOption += " -Oportrait";
  }
  std::string size;
  switch( m_currentPaperSize ){
  case HardCopyListener::A2 :
    size = "a2";
    break;
  case HardCopyListener::A3 :
    size = "a3";
    break;
  case HardCopyListener::A4 :
    size = "a4";
    break;
  default :
    size = "unscaled";
  }
  formatOption += " -S" + size;
#ifdef _WIN32
  print_command +=  "\"";
#endif
  print_command +=  AppData::Instance().IntensHome()
    + "bin/ReportConv.py"; // + formatOption;
#ifdef _WIN32
  print_command +=  "\"";
#endif
  print_command +=  " " + formatOption;

  // has template filename
  if( hcl->getTemplateFileName().size() ){
    std::string templateFileNameAbs = locateTemplateFile( hcl->getTemplateFileName() );

    if( templateFileNameAbs.size() > 0 ){
      print_command += " -t "  + templateFileNameAbs;
    }
  }

  switch( m_currentListener->getFileFormat() ){
  case HardCopyListener::XML:
  case HardCopyListener::Text:
  case HardCopyListener::Postscript :
  case HardCopyListener::LaTeX:
  case HardCopyListener::JPEG:
  case HardCopyListener::PNG:
  case HardCopyListener::GIF:{
    if( m_currentMode == SAVE ){
      // replace all '\' with '/' and ' ' with '\ '
      std::string::size_type fs = 0;
      while ((fs = m_currentFile.find_first_of("\\", fs)) != std::string::npos) {
	m_currentFile.replace(fs, 1, "/");
	fs++;
      }
      fs = 0;
      while ((fs = m_currentFile.find_first_of(" ", fs)) != std::string::npos) {
	m_currentFile.replace(fs, 1, "\\ ");
	fs +=2;
      }
      print_command += std::string(" ") + m_currentFile;
    }
    else
    if( m_currentMode == PREVIEW ){
      if( m_currentFileFormat == HardCopyListener::ASCII
	  || m_currentFileFormat == HardCopyListener::JPEG
	  || m_currentFileFormat == HardCopyListener::PNG
	  || m_currentFileFormat == HardCopyListener::GIF ){
	m_currentDialog->printMessage( _("Preview of selected file format is not supported."), GuiElement::msg_Warning );
	return false;
      }
      print_command += std::string(" ") + m_previewFile;
    }
    else{
      if( m_currentFileFormat == HardCopyListener::ASCII
	  || m_currentFileFormat == HardCopyListener::JPEG
	  || m_currentFileFormat == HardCopyListener::PNG
	  || m_currentFileFormat == HardCopyListener::GIF ){
	m_currentDialog->printMessage( _("Printing of selected file format is not supported"), GuiElement::msg_Warning );
	return false;
      }
      print_command += std::string(" -P") + m_currentPrinter;
    }
    break;
  }
  case HardCopyListener::HPGL :{
    if( !printer_command_installed ){
      if( !m_currentListener->isFileFormatSupported( m_currentFileFormat ) ){
	m_currentDialog->printMessage( _("Selected file format is not supported."), GuiElement::msg_Warning );
	return false;
      }
      if( m_currentMode == PREVIEW ){
	m_currentDialog->printMessage( _("Preview of selected file format is not supported."), GuiElement::msg_Warning );
	return false;
      }
      else if( m_currentMode == SAVE ){
	print_command += std::string("cat > ") + m_currentFile + ".hpgl";
      }
      else{
	std::string size;
	switch( m_currentPaperSize ){
	case HardCopyListener::A2 :
	  size = "a2";
	  break;
	case HardCopyListener::A3 :
	  size = "a3";
	  break;
	case HardCopyListener::A4 :
	  size = "a4";
	  break;
	default :
	  size = "unscaled";
	}
	print_command += std::string(PRINTER_CMD) + m_currentPrinter + " -S" + size;
      }
    }
  }
  default :
    assert(false);
    break;
  }
  // Anzahl Kopien
  if( ( m_currentMode != PREVIEW ) && ( m_currentMode != SAVE ) ){
    int copies = m_currentQuantity;
    if( copies > 1 ){
      std::ostringstream ostr;
      ostr << copies;
      print_command += LPNUM_COPIES + ostr.str();
    }
  }

  m_currentDialog->printMessage( print_command, GuiElement::msg_Information );
  m_repGenProcess.setExecCmd( print_command );
  if( m_fileBufferListener == 0 )
    m_repGenInputChannel.setListener( m_currentListener );
  else
    m_repGenInputChannel.setListener( m_fileBufferListener );

  m_repGenOutputChannel.setListener( &m_msgDisplay );
  m_repGenProcess.start();
  if (AppData::Instance().HeadlessWebMode()) {
    // webapi: wait for finishing
    m_repGenProcess.waitForFinished();
    return true;
  }


  std::string where( "Printer: " );
  if( m_currentMode == PREVIEW ){
    where += "Preview";
  }
  else{
    where += m_currentPrinter;
  }

  m_dialogWorkClock = GuiFactory::Instance()->createDialogWorkClock( 0
								     , this
								     , ""
								     , "" );
  Worker *w = new Worker( this );
  w->start();
  m_dialogWorkClock->manage();
  return true;
}

/* --------------------------------------------------------------------------- */
/* newTmpFile --                                                               */
/* --------------------------------------------------------------------------- */
int ReportGen::newTmpFile( const std::string &fileName ) {
  m_tmpFiles.insert( fileName );
  return creat( fileName.c_str(), 0644 );
}

/* --------------------------------------------------------------------------- */
/* locateXSLFile --                                                            */
/* --------------------------------------------------------------------------- */
std::string ReportGen::locateXSLFile( const std::string &name, const std::string &_suffix ) {
  std::string suffix;
  if( ! _suffix.empty() )
    suffix = ( '.' + _suffix );
  const char *envVarValue;

  std::vector<std::string> envDirs;
  // --- APP HOME | INTENS_HOME -----------------------------------------
  envDirs.push_back("APPHOME");
  envDirs.push_back("INTENS_HOME");
  std::vector<std::string>::iterator it = envDirs.begin();
  for (; it !=  envDirs.end(); ++it) {
    if( (envVarValue=getenv((*it).c_str())) != NULL ){
      std::string filename = "/" + std::string( "xml" ) + "/" + name + suffix;
      struct stat result;
      if( stat(filename.c_str(),&result) == -1 )
        continue;
      if ( result.st_mode & S_IFREG )
	return filename;
    }
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* locateTemplateFile --                                                       */
/* --------------------------------------------------------------------------- */
std::string ReportGen::locateTemplateFile( const std::string &name ) {
#ifdef HAVE_QT
  std::string filename;
  std::vector<std::string> dirs;

  // --- APP HOME | INTENS_HOME -----------------------------------------
  if (AppData::Instance().AppHome().size())
    dirs.push_back(AppData::Instance().AppHome());
  dirs.push_back(AppData::Instance().IntensHome());
  for (std::vector<std::string>::iterator it = dirs.begin(); it !=  dirs.end(); ++it) {
    filename = (*it);
    if (filename[filename.size()-1] != QDir::separator().toLatin1())
      filename += QDir::separator().toLatin1();
    filename += std::string( TEMPLATE_DIR ) + QDir::separator().toLatin1()+ name;
    QFile file( filename.c_str() );
    if ( file.exists() )
      return filename;
  }
#endif
  // not found
  return "";
}

/* --------------------------------------------------------------------------- */
/* deleteFiles --                                                            */
/* --------------------------------------------------------------------------- */
void ReportGen::deleteFiles(){
  std::set<std::string>::iterator iter;
  for( iter = m_tmpFiles.begin(); iter != m_tmpFiles.end(); ++iter )
    unlink( (*iter).c_str() );
  for( iter = m_files.begin(); iter != m_files.end(); ++iter )
    unlink( (*iter).c_str() );
}
