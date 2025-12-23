
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include <errno.h>

#ifdef HAVE_QT
#include <qmessagebox.h>
#include <qpushbutton.h>
#include "gui/qt/QtDialogFileSelection.h"
#endif

#include "app/UiManager.h"
#include "app/DataPoolIntens.h"
#include "datapool/DataReference.h"
#include "datapool/DataDictionary.h"
#include "streamer/DataStreamParameter.h"
#include "operator/FileStream.h"
#include "operator/InputChannelEvent.h"
#include "operator/OutputChannelEvent.h"
#include "operator/MessageQueueThreads.h"
#include "operator/Worker.h"
#include "utils/Date.h"
#include "utils/gettext.h"
#if 1
#include "utils/gzstream.h"
#else
#include "utils/fdiostream.h"
#endif

#include "utils/StringUtils.h"
#include "utils/FileUtilities.h"
#include "utils/base64.h"
#include "parser/errorhandler.h"
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/UnitManager.h"

#include "streamer/Stream.h"
#include "gui/GuiScrolledText.h"

FileStream::FileStreamList FileStream::s_filestreamlist;

INIT_LOGGER();
INIT_DESLOGGER();
INIT_PYTHONLOGGER();

#ifdef HAVE_QT
class MyQMessageBox : public QMessageBox{
public:
  MyQMessageBox(const QString & caption,
                const QString & text,
                Icon icon,
                StandardButtons buttons)
    : QMessageBox(icon, caption, text, buttons)
  {}

  virtual ~MyQMessageBox(){}
protected:
  virtual void closeEvent( QCloseEvent*){}
};
#endif

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

FileStream::FileStream( const std::string &name, Stream *s )
  : m_readonly( false )
  , m_writeonly( false )
  , m_data_filename(0)
  , m_failed(false)
  , m_stream( s )
  , m_cycle( 0 )
  , m_name( name )
  , m_label( name )
  , m_process( 0 )
  , m_log_on( true )
  , m_hidden( false )
  , m_binary_mode( false )
  , m_action( 0 )
  , m_outchannel( this )
  , m_inchannel( this, "", 0 )
  , m_save_action( this, false )
  , m_save_listener( this )
  , m_write_listener( this )
  , m_open_action( this, true )
  , m_open_listener( this )
  , m_read_listener( this )
  , m_dialog(0){
  installData();
  m_save_action.setSilent();
  m_open_action.setSilent();
}

FileStream::~FileStream( ){
  FileStreamList::iterator I=std::find( s_filestreamlist.begin()
				      , s_filestreamlist.end(), this );
  if( I != s_filestreamlist.end() ){
    s_filestreamlist.erase( I );
  }
  delete m_process;
  delete m_dialog;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* newFileStream --                                                            */
/* --------------------------------------------------------------------------- */

FileStream *FileStream::newFileStream( const std::string &name, Stream *s ){
  FileStream *fs = new FileStream( name, s );
  s_filestreamlist.push_back( fs );
  return fs;
}

/* --------------------------------------------------------------------------- */
/* find --                                                                     */
/* --------------------------------------------------------------------------- */

FileStream *FileStream::find( const std::string &name ){
  FileStreamList::iterator I;
  for( I=s_filestreamlist.begin(); I!=s_filestreamlist.end(); ++I ){
    if( (*I)->Name() == name ){
      return *I;
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* find --                                                                     */
/* --------------------------------------------------------------------------- */

FileStream *FileStream::find( const Stream *s ){
  FileStreamList::iterator I;
  for( I = s_filestreamlist.begin(); I != s_filestreamlist.end(); ++I ){
    if( (*I)->m_stream == s ){
      return *I;
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* hasReadableFileStreams --                                                   */
/* --------------------------------------------------------------------------- */

bool FileStream::hasReadableFileStreams(){
  FileStreamList::iterator I;
  for( I=s_filestreamlist.begin(); I!=s_filestreamlist.end(); ++I ){
    if( !(*I)->m_writeonly ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* hasWritableFileStreams --                                                   */
/* --------------------------------------------------------------------------- */

bool FileStream::hasWritableFileStreams(){
  FileStreamList::iterator I;
  for( I=s_filestreamlist.begin(); I!=s_filestreamlist.end(); ++I ){
    if( !(*I)->m_readonly ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* installData --                                                              */
/* --------------------------------------------------------------------------- */

void FileStream::installData(){
  std::string varname = "__filestream_filename_"+m_name;
  if( !DataPoolIntens::getDataReference( varname ) )
    DataPoolIntens::getDataPool().AddToDictionary( "", varname
						   , DataDictionary::type_String
						   );

  m_data_filename = DataPoolIntens::getDataReference( varname );
  assert( m_data_filename != 0 );
}

/* --------------------------------------------------------------------------- */
/* setTimestamp --                                                             */
/* --------------------------------------------------------------------------- */

void FileStream::setTimestamp(){
  TransactionNumber trans = DataPoolIntens::NewTransaction();
  FileStreamList::iterator I;
  for( I=s_filestreamlist.begin(); I!=s_filestreamlist.end(); ++I ){
    (*I)->setTimestamp( (int)trans );
  }
}

/* --------------------------------------------------------------------------- */
/* setTimestamp --                                                             */
/* --------------------------------------------------------------------------- */

void FileStream::setTimestamp( int trans ){
  BUG( BugOperator, "FileStream::setTimestamp" );
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool FileStream::write( InputChannelEvent &event ){
  BUG( BugOperator, "FileStream::write" );
  if( m_stream != 0 ){
    return m_stream->write(event);
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool FileStream::read( OutputChannelEvent &event ){
  BUG( BugOperator, "FileStream::read" );
  if( m_stream != 0 ){
    return m_stream->read(event);
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* install --                                                                  */
/* --------------------------------------------------------------------------- */

bool FileStream::install(){
  if( m_hidden || m_name.empty() ){
    return true;
  }

  UImanager &ui = UImanager::Instance();
  GuiMenuButtonListener *lsnr = 0;

  if( !m_writeonly ){
    lsnr = new OpenButtonListener( this );
    if( ui.addOpenButton( m_name, lsnr ) ){
      lsnr->setDialogLabel( m_label );
    }
    else{
      delete lsnr;
      return false;
    }
  }

  if( !m_readonly ){
    lsnr = new SaveButtonListener( this );
    if( ui.addSaveButton( m_name, lsnr ) ){
      lsnr->setDialogLabel( m_label );
    }
    else{
      delete lsnr;
      return false;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* SetProcess --                                                               */
/* --------------------------------------------------------------------------- */

bool FileStream::setProcess( const std::string &cmd  ){
  BUG_PARA( BugOperator, "FileStream::setProcess",cmd );
  m_process = new BatchProcess();
#ifdef HAVE_QT
  m_inchannel.setQProcess(  m_process->getQProcess() );
#endif
  m_process->addChannel( &m_inchannel );
  m_process->addChannel( &m_outchannel );
  m_process->setExecCmd( cmd );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setReset --                                                                 */
/* --------------------------------------------------------------------------- */

bool FileStream::setReset(){
  OutputChannel *out;

  return true;
}

/* --------------------------------------------------------------------------- */
/* setFilename --                                                              */
/* --------------------------------------------------------------------------- */

void FileStream::setFilename( const std::string &filename ){
  if( m_data_filename ) m_data_filename->SetValue( filename );
}

/* --------------------------------------------------------------------------- */
/* setBaeFilename --                                                           */
/* --------------------------------------------------------------------------- */

void FileStream::setBaseFilename( const std::string &filename ){
  m_baseFilename = filename;
}

/* --------------------------------------------------------------------------- */
/* getBaeFilename --                                                           */
/* --------------------------------------------------------------------------- */

std::string FileStream::getBaseFilename() {
  std::string def(m_stream->Name());
  if (m_baseFilename.size())
    return m_baseFilename;
  if (m_filter.size()) {
    std::smatch match;
    std::string m_filterr = split(m_filter, ";;").back();  // last
    std::regex re("\\(.*\\)");
    if (std::regex_search(m_filterr, match, re) && match.size() > 0) {
      std::string s = match.str(0).substr(1, match.str(0).size()-2);
      std::vector<std::string> res = split(s, " ");
      replaceAll(res.back(), std::string("*"), def);
      return res.back();
    }
    return def;
  }

  return "";
}

/* --------------------------------------------------------------------------- */
/* getSaveAction --                                                            */
/* --------------------------------------------------------------------------- */

FileStream::FileStreamAction *FileStream::getSaveAction( XferDataItem *xfer ){
  if( !m_readonly ){
    if( xfer ){
      if( xfer->getDataType() != DataDictionary::type_String ){
	return 0;
      }
    }
    FileStreamAction *action = new FileStreamAction( this, false );
    action->setXfer( xfer );
    action->setSilent();
    return action;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getSaveAction --                                                            */
/* --------------------------------------------------------------------------- */

FileStream::FileStreamAction *FileStream::getSaveAction(const std::string& filename,
                                                        const std::string& base_filename){
  if( !m_readonly ){
    FileStreamAction *action = new FileStreamAction( this, false );
    action->setFilename( filename );
    action->setBaseFilename( base_filename );
    action->setSilent();
    return action;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* startSaveAction --                                                          */
/* --------------------------------------------------------------------------- */

void FileStream::startSaveAction(){
  m_save_action.setName("@FileStream@"); // DES_INFO logger called detectly by FileStream
  DES_DEBUG(compose("  //startSaveAction: Name: %1, ActionName: %2, CallingActionName: %3",
					Name(), m_open_action.Name(), m_open_action.CallingActionName()));
  Trigger *trigger = new Trigger( &m_save_action );
  trigger->startJob();
}

/* --------------------------------------------------------------------------- */
/* startOpenAction --                                                          */
/* --------------------------------------------------------------------------- */

void FileStream::startOpenAction(){
  m_open_action.setName("@FileStream@"); // DES_INFO logger called directly by FileStream
  DES_DEBUG(compose("  //startOpenAction: Name: %1, ActionName: %2, CallingActionName: %3",
					Name(), m_open_action.Name(), m_open_action.CallingActionName()));
  Trigger *trigger = new Trigger( &m_open_action );
  trigger->startJob();
}

/* --------------------------------------------------------------------------- */
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

void FileStream::saveFile( FileStreamAction *action ){
  BUG( BugOperator, "FileStream::saveFile" );
  m_action = action;

  if( isLogOn() ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": BEGIN : SAVE FileStream " << getLabel() << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }

  XferDataItem *xfer = 0;
  if( action ){
    std::string s;
    xfer = action->getXfer();
    if( xfer ){
      xfer->getValue( s );
    } else {
      s = action->getFilename();
    }
    if( !s.empty() ){
      m_save_listener.FileSelected( s, HardCopyListener::NONE, "" );
      return;
    }
  }
  if (m_dir.empty()) m_dir = ".";
  GuiFactory::Instance()->showDialogFileSelection
    ( 0
      , compose(_("Save %1"),m_label)
      , m_filter
      , m_dir
      , &m_save_listener
      , DialogFileSelection::Save
      , 0
      , DialogFileSelection::AnyFile
      , getBaseFilename()
      );

}

/* --------------------------------------------------------------------------- */
/* FileSelected --                                                             */
/*   called by DialogFileSelection                                             */
/* --------------------------------------------------------------------------- */

void FileStream::SaveListener::FileSelected( const std::string &fn
					     , const HardCopyListener::FileFormat ff
					     , const std::string dir ){
  BUG( BugOperator, "FileStream::SaveListener::FileSelected" );
  if (fn.empty()) {  // WebApi
    m_stream->endFileStream( JobAction::job_Ok );
    return;
  }
  m_stream->setDirname( dir );
  std::string filename = fn;

#ifndef CONFIRM_TO_CLEAR_TARGETS
//   struct stat sbuf;
//   if( stat( filename.c_str(), &sbuf ) == 0 ){
//     m_filename = filename;
//     GuiFactory::Instance()->showDialogConfirmation
//       ( 0
// 	, "Save anyway"
// 	, compose(_("File '%1' already exists.\nSave it anyway?"),filename)
// 	, this );

//     return;
//   }
#endif
  m_stream->writeFile( filename );
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void FileStream::SaveListener::confirmYesButtonPressed(){
  BUG( BugOperator, "FileStream::confirmYesButtonPressed" );
  m_stream->writeFile( m_filename );
  m_stream->endFileStream( JobAction::job_Ok );
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void FileStream::SaveListener::confirmNoButtonPressed(){
  BUG( BugOperator, "FileStream::confirmNoButtonPressed" );
  m_filename="";
  canceled();
}

/* --------------------------------------------------------------------------- */
/* canceled --                                                                 */
/* --------------------------------------------------------------------------- */

void FileStream::SaveListener::canceled(){
  BUG( BugOperator, "FileStream::SaveListener::canceled" );
  m_stream->endFileStream( JobAction::job_Canceled );
}

/* --------------------------------------------------------------------------- */
/* writeStreamData --                                                          */
/* --------------------------------------------------------------------------- */

void FileStream::SaveListener::writeStreamData(std::ostream& stream, HardCopyListener::FileFormat& format){
  BUG( BugOperator, "FileStream::SaveListener::writeStreamData" );
  if( m_stream->m_process != 0 ){
    m_stream->m_outchannel.setOstream(stream);  // set output stream
    m_stream->m_process->start();
    do {
      m_stream->m_process->waitForFinished();
    } while (!m_stream->m_process->hasTerminated());
  } else {
    m_stream->m_stream->write(stream);
  }
  format = m_stream->m_stream->getFormat();
}

/* --------------------------------------------------------------------------- */
/* writeFile --                                                                */
/* --------------------------------------------------------------------------- */

void FileStream::writeFile( const std::string &filename ){
  BUG_PARA( BugOperator, "FileStream::writeFile","File '" << filename << "'" );
  InputChannel *in;

  if( m_process != 0 ){
    if( !m_outchannel.open( filename ) ){
      IPrintMessage( compose("%1 %2",strerror(errno),filename) );
      endFileStream( JobAction::job_Aborted );
      BUG_EXIT("open of outputchannel failed");
      return;
    }
    DataPoolIntens::Instance().BeginDataPoolTransaction( this );
    setFilename( filename );
    if (!m_hidden) {
      m_dialog = GuiFactory::Instance()->createDialogWorkClock( 0
							      , &m_write_listener
							      , ""
							      , "" );
      m_dialog->manage();
    }
    Worker *w = new Worker( &m_write_listener );
    w->start();
  }
  else {
    std::ostream *ostr=0;
    if( filename.find(".gz", filename.size()-3) != std::string::npos ){
#if 1
      ostr = new ogzstream( filename.c_str() );
#else
      ostr = new gzofstream( filename.c_str() );
#endif
    }
    else{
      if (m_binary_mode ||
          FileUtilities::isBinaryFormat(filename)) {
        ostr = new std::ofstream( filename.c_str(), std::ofstream::out|std::ofstream::binary);
      } else
        ostr = new std::ofstream( filename.c_str() );
    }
    if( !(ostr->good()) ){
      IPrintMessage( compose("%1 %2",strerror(errno),filename) );
      if( m_action ){
        endFileStream( JobAction::job_Aborted );
      }
      else{
	if( isLogOn() ){
	  std::stringstream logmsg;
	  logmsg << DateAndTime() << ": ABORT : SAVE FileStream " << getLabel() << std::endl;
	  GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
	}
      }
      BUG_EXIT("open of filestream failed");
      return;
    }
    setFilename( filename );
    m_stream->write( *ostr );
    if( filename.find(".gz", filename.size()-3) != std::string::npos ){
#if 1
      ((ogzstream*)ostr)->close();
#else
      ((gzofstream*)ostr)->close();
#endif
    }
    else{
      ((std::ofstream*)ostr)->close();
    }
    delete ostr;
    // ----------------------------------------------------------------------
    // Unter Umstaenden befindet sich auf der Oberflaeche irgendwo der Filename
    // unseres Filestreams. Dieser muss ein update() erhalten.
    // ----------------------------------------------------------------------
    GuiManager::Instance().update( GuiElement::reason_FieldInput );
    if( m_action ) {
      endFileStream( JobAction::job_Ok );
    } else{
      if( isLogOn() ){
        std::stringstream logmsg;
        logmsg << DateAndTime() << ": END : SAVE FileStream " << getLabel() << std::endl;
        GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
      }
    }
    BUG_EXIT("write ok without process");
  }

  // update unit manager
  if (AppData::Instance().hasUnitManagerFeature() &&
      UnitManager::Instance().getFilename() == filename) {
    UnitManager::Instance().reset();
    GuiManager::Instance().update(GuiElement::reason_Unit);
  }
}

/* --------------------------------------------------------------------------- */
/* workWriteFile --                                                            */
/* --------------------------------------------------------------------------- */

JobAction::JobResult FileStream::workWriteFile(){
  //  BUG_DEBUG("FileStream::workWriteFile");

  if( !m_process->isRunning() ){
    m_failed = false;
    m_process->start();
    return JobAction::job_Wait; /* continue */
  }
  if( !m_process->hasTerminated() ){
    return JobAction::job_Wait; /* continue */
  }
  if( m_process->getExitStatus() ){
    return JobAction::job_Aborted;
  }
  return JobAction::job_Ok; /* stop */
}

/* --------------------------------------------------------------------------- */
/* cancelFile --                                                               */
/* --------------------------------------------------------------------------- */

void FileStream::cancelFile(){
  BUG( BugOperator, "FileStream::cancelFile" );

  if( m_process == 0 ){
    return;
  }
  if( !m_process->stop() ){
    perror( "FileStream::cancelFile" );
    DataPoolIntens::Instance().RollbackDataPoolTransaction( this );
    return;
  }
  while( !m_process->hasTerminated() );
  DataPoolIntens::Instance().RollbackDataPoolTransaction( this );

  GuiFactory::Instance()->showDialogWarning
    ( 0
      , _("Error")
      , _("Input error.\n(See Log Window)")
      , 0 );
}

/* --------------------------------------------------------------------------- */
/* getOpenAction --                                                            */
/* --------------------------------------------------------------------------- */

FileStream::FileStreamAction *FileStream::getOpenAction( XferDataItem *xfer ){
  if( !m_writeonly ){
    if( xfer ){
      if( xfer->getDataType() != DataDictionary::type_String ){
	return 0;
      }
    }
    FileStreamAction *action = new FileStreamAction( this, true );
    action->setXfer( xfer );
    action->setSilent();
    return action;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getOpenAction --                                                            */
/* --------------------------------------------------------------------------- */

FileStream::FileStreamAction *FileStream::getOpenAction( const std::string& filename ){
  if( !m_writeonly ){
    FileStreamAction *action = new FileStreamAction( this, true );
    action->setFilename( filename );
    action->setSilent();
    return action;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* openFile --                                                                 */
/* --------------------------------------------------------------------------- */

void FileStream::openFile( FileStreamAction *action, FileSelectListener *listener ){
// #endif

  BUG( BugOperator, "FileStream::openFile" );
  std::string title( compose(_("Open %1"),m_label) );
  m_action = action;
  MessageQueueReplyThread *mq_reply = m_action->getMessageQueueReplyThread();

  if( isLogOn() ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": BEGIN.: OPEN FileStream " << getLabel() << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }

  // Ist der Filename vorgegeben?
  XferDataItem *xfer = 0;
  if( action ){
	std::string s;
    xfer = action->getXfer();
    if( xfer ){
      xfer->getValue( s );
    } else {
	  s = action->getFilename();
    if (!s.empty()) action->setFilename("");
	}
	if( s.empty() ){
	  // gibt es ein RESOURCE File-Argument?
	  double d;
	  int i;
	  AppData::Instance().getResource(compose("Arg@%1",Name()), s, d, i);
	}

  // WebReply Aufruf, do WebQuery
  std::string requestDataType = "file_handle";
  if (s.empty() && mq_reply) {
    Json::Value jsonObj = Json::Value(Json::objectValue);
    jsonObj["command"] = "file_open";
    jsonObj["message"] = "Please upload a file!";
    jsonObj["directory"] = m_dir;
    jsonObj["filter"] = m_filter;
    jsonObj["title"] = title;
    std::vector<std::string> expected;
    expected.push_back("data");
    expected.push_back("type");
    expected.push_back("filename");
    Json::Value retObj = mq_reply->doQuery(jsonObj);
    BUG_DEBUG("FileOpenDialog mq reply return["<<ch_semafor_intens::JsonUtils::value2string(retObj)<<"]");
    if (!retObj.isNull() &&  retObj.isMember("data")) {  // json data
      s = retObj["data"].asString();
    } else if (mq_reply->getQueryData().size() == 1) {   // kein json, Dateiinhalt
      s = mq_reply->getQueryData()[0];
    } else {
      BUG_WARN("Warning, no single answer data received." << mq_reply->getQueryData().size());
    }
    requestDataType = retObj.isMember("type") ? retObj["type"].asString() : "stream";
    if (retObj.isMember("filename")) {
      action->setFilename(retObj["filename"].asString());
      setFilename(retObj["filename"].asString());
    }
  }
	if( !s.empty() ){
    if(requestDataType == "file_handle") {
	    m_open_listener.FileSelected( s, HardCopyListener::NONE, "" );

    } else if(requestDataType == "stream") {
      BUG_DEBUG("Reading from stream");
      // filename check
      std::string filename = FileUtilities::getDataFilename(s);
      if (filename.size())  {
        BUG_INFO("Got data filename: " <<filename);
        action->setFilename(filename);
        setFilename(filename);
      }

      std::string header;
      decodeBase64data(s, header);

      // gzip check
      std::string mimeTypeData = FileUtilities::getDataMimeType(s);
      BUG_DEBUG("Got data mime type: " << mimeTypeData);
      if (mimeTypeData == "application/gzip") {
        header = "data:application/gzip;base64";
      }
      char fn[200];
      if( m_process != 0 || header == "data:application/gzip;base64") {
        strcpy(fn, header == "data:application/gzip;base64" ?
               "/tmp/streamXXXXXX.gz" : "/tmp/streamXXXXXX" );
        int fpP = mkstemp(fn);
        remove(fn);
        close(fpP);
        std::ofstream ofs(fn);
        ofs << s;
        ofs.close();
        if(m_process != 0){
          readFile(fn);
        } else {
          BUG_INFO("igzstream, file: " << fn);
          std::istream *is = new igzstream(fn);
          if(!is->good()){
            BUG_INFO("igz failed: " << fn << ", good:" << is->good()) ;
            endFileStream( JobAction::job_Nok );
          } else {
            JobAction::JobResult rslt = JobAction::job_Ok;
            BUG_INFO("read " << fn << ", good:" << is->good());
            if(!m_stream->read(*is)){
              BUG_INFO("read Aborted.");
              rslt = JobAction::job_Aborted;
            }
            delete is;
            remove(fn);
            endFileStream(rslt);
          }
        }
        remove(fn);
      } else {
        std::istringstream iStringStream(s);
        m_stream->read(iStringStream);
        endFileStream( JobAction::job_Ok );
      }
    } else if(requestDataType == "abort") {
      endFileStream( JobAction::job_Canceled );
    }
	  return;
	}
  }

#if defined HAVE_QT && !defined HAVE_HEADLESS
  QString ret =
    QtDialogFileSelection::getOpenFileName( m_dir.c_str(), m_filter.c_str(), NULL, title.c_str() );
  if (ret.length()) {
    m_dir=QFileInfo(ret).absolutePath().toStdString();
    BUG_MSG( "QT FileOpen File " << ret.toStdString() );
    if( checkTargets() ) {
#ifndef CONFIRM_TO_CLEAR_TARGETS
      BUG_MSG("FileStream consistent check failed on stream named '"<<m_stream->Name()<<"'");
      MyQMessageBox mb(_("OK"),
                       _("The results are not consistent anymore.\nDo you want to delete them?"),
                       QMessageBox::Warning,
                       QMessageBox::NoButton
                       );
      QAbstractButton *yesButton = mb.addButton(_("&Yes"), QMessageBox::YesRole);
      mb.addButton(_("&No"), QMessageBox::NoRole);
      mb.setDefaultButton(QMessageBox::Yes);
      mb.setEscapeButton(QMessageBox::No);

      bool grabButton( false );
      GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( grabButton );
      GuiManager::Instance().attachEventLoopListener( loopcontrol );
      mb.exec();
      bool result =  mb.clickedButton() == yesButton;
      loopcontrol->goAway();
      if (result) {
        clearTargets();
      } else {
        endFileStream( JobAction::job_Canceled );
        return;
      }
#else
      // Target-Streams von eingelesenen Daten werden stillschweigend geloescht.
      clearTargets();
#endif
    }
    if(action->Name() != "@FileStream@") {
      std::string s(ret.toStdString());
      std::string key(Name());
      s = UImanager::Instance().addDescriptionFunctionArgument(key, s);
      DES_DEBUG(compose("  //SET_RESOURCE(\"Arg@%1\", \"%2\"); // -- Name: %3, actionName: %4",
                        key, ret.toStdString(), Name(), action->Name()));
    } else {
      DES_INFO(compose("  OPEN(%1, \"%2\"); // FileStream",
                       Name(), ret.toStdString()));
      PYLOG_INFO(compose(PYLOG_FILEOPEN, Name(), ret.toStdString()));
    }
    readFile( ret.toStdString() );
  } else{
    endFileStream( JobAction::job_Nok );
  }
#else
  if( listener == 0 )
    listener = &m_open_listener;

  //  GuiFactory::Instance()->showDialogFileSelection( 0, title, m_filter, m_dir, listener ); // TODO
#endif
}

/* --------------------------------------------------------------------------- */
/* checkTargets --                                                             */
/* --------------------------------------------------------------------------- */

bool FileStream::checkTargets(){
  TransactionNumber timestamp = DataPoolIntens::NewTransaction();
  return m_stream->checkAsTargetStream( timestamp );
}

/* --------------------------------------------------------------------------- */
/* clearTargets --                                                             */
/* --------------------------------------------------------------------------- */

void FileStream::clearTargets(){
  TransactionNumber timestamp = DataPoolIntens::NewTransaction();
  m_stream->clearAsTargetStream( timestamp );
}

/* --------------------------------------------------------------------------- */
/* FileSelected --                                                             */
/* --------------------------------------------------------------------------- */

void FileStream::OpenListener::FileSelected( const std::string &filename
					     , const HardCopyListener::FileFormat ff
					     , const std::string dir ) {
  BUG_PARA( BugOperator, "FileStream::OpenListener::FileSelected"
	    , "File " << filename );
  m_stream->setDirname( dir );
  if( m_stream->checkTargets() ){
#if defined(CONFIRM_TO_CLEAR_TARGETS)
    m_filename = filename;
    BUG_MSG("FileStream consistent check failed on stream named '"<<m_stream->Name()<<"'");
    DialogConfirmation::showDialog
      ( 0
	, _("OK")
	, _("The results are not consistent anymore.\nDo you want to delete them?"),
	, this
	);
    return;
#else
    // Target-Streams von eingelesenen Daten werden stillschweigend geloescht.
    m_stream->clearTargets();
#endif
  }
  DES_DEBUG(compose("  //OPEN(%1, \"%2\"); // FileSelected with filename", m_stream->Name(), filename));
  m_stream->readFile( filename );
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */

void FileStream::OpenListener::confirmYesButtonPressed(){
  BUG( BugOperator, "FileStream::OpenListener::confirmYesButtonPressed" );
  m_stream->clearTargets();
  DES_INFO(compose("  OPEN(%1, \"%2\"); // confirmYesButtonPressed", m_stream->Name(), m_filename));
  PYLOG_INFO(compose(PYLOG_FILEOPEN, m_stream->Name(), m_filename));
  m_stream->readFile( m_filename );
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */

void FileStream::OpenListener::confirmNoButtonPressed(){
  BUG( BugOperator, "FileStream::OpenListener::confirmNoButtonPressed" );
  canceled();
}

/* --------------------------------------------------------------------------- */
/* canceled --                                                                 */
/* --------------------------------------------------------------------------- */

void FileStream::OpenListener::canceled(){
  BUG( BugOperator, "FileStream::OpenListener::canceled" );
  m_stream->endFileStream( JobAction::job_Canceled );
}

/* --------------------------------------------------------------------------- */
/* readStreamData --                                                           */
/* --------------------------------------------------------------------------- */

void FileStream::OpenListener::readStreamData(std::istream& stream){
  BUG( BugOperator, "FileStream::OpenListener::readStreamData" );
  m_stream->m_stream->read(stream);
  m_stream->endFileStream( JobAction::job_Ok );
}

/* --------------------------------------------------------------------------- */
/* readFile --                                                                 */
/* --------------------------------------------------------------------------- */

void FileStream::readFile( const std::string &filename ){
  BUG_DEBUG("FileStream::readFile, filename: " << filename);

  if( m_process != 0 ){
    BUG_DEBUG( "process available" );
    m_inchannel.open( filename );
    m_process->clearTextWindows();

    DataPoolIntens::Instance().BeginDataPoolTransaction( this );
    setFilename( filename );
    if (!m_hidden) {
      m_dialog = GuiFactory::Instance()->createDialogWorkClock( 0
							      , &m_read_listener
							      , ""
							      , "" );
      m_dialog->manage();
    }
    Worker *w = new Worker( &m_read_listener );
    w->start();
    BUG_DEBUG("process started");
    return;
  }

  if( m_stream == 0 ){
    endFileStream( JobAction::job_Aborted );
    BUG_DEBUG( "stream missing" );
    return;
  }

  BUG_DEBUG( "simple read" );

  bool isgz_stream(true);
  std::istream *is = m_stream->hasNoGzOption() ||
    filename.find(".gz", filename.size()-3) == std::string::npos ?
    0 : new igzstream(filename.c_str());
  if( !is || !(*is) ){
    isgz_stream = false;
    is = FileUtilities::ReadFile(filename);
    // special case!
    // if unitManager feature && we use default units
    // convert default units to json
    // from outside, it looks like we open a valid json file
    if ((!is || !(*is)) &&
        AppData::Instance().hasUnitManagerFeature()){
      std::ostringstream os;
      UnitManager::Instance().writeUnitData(os);
      is = new std::istringstream(os.str());
    }
    if( !is || !(*is) ){
      delete is;
      endFileStream( JobAction::job_Nok );
      BUG_DEBUG( "no file " << filename );
      return;
    }
#ifdef __MINGW32__
    // !!! der ifstream kann nicht mit Zeilenenden "\n\r" umgehen
    // => daher wird der ifstream zu einem istringstream (2007-06-05; gcc 3.4.2)
    if (!isgz_stream ){
      std::ostringstream ofs;
      char c;
      while( (c=is->get()) != EOF ) ofs << c;
      delete is;
      is = new std::istringstream( ofs.str().c_str() );
    }
#endif
  }
  JobAction::JobResult rslt = JobAction::job_Ok;

#ifdef __MINGW32__
  if (!isgz_stream && filename.substr(filename.size()-3) == "xml") {  // find_*_of() hat brain death
    // da Windows Probleme mit groesseren Dateinen > 350M hat
    // folgende Loesung anstatt stream den Dateiname uebergeben
    delete is;
    is = 0;
    m_stream->read( filename );
  } else
#endif

    if( !m_stream->read( *is ) ){
      rslt = JobAction::job_Aborted;
    }

  if (is) delete is;

  BUG_DEBUG("File '" << filename << "' is open");
  setFilename( filename );
  if( rslt == JobAction::job_Ok ){
    GuiManager::Instance().update( GuiElement::reason_Process );
    BUG_DEBUG("ok");
  }
  else{
    BUG_DEBUG("aborted");
  }
  endFileStream( rslt );
}

/* --------------------------------------------------------------------------- */
/* endFileStream --                                                            */
/* --------------------------------------------------------------------------- */

void FileStream::endFileStream( JobAction::JobResult rslt ){
  if( m_dialog ){
    m_dialog->unmanage();
    delete m_dialog;
    m_dialog=0;
  }
  if( m_action )
    m_action->endFileStream( rslt );
  if( isLogOn() ){
    std::ostringstream logmsg;
    logmsg << DateAndTime() << ": ";
    switch( rslt ){
    case JobAction::job_Ok :
    case JobAction::job_Nok :
      logmsg << "END : ";
      break;
    case JobAction::job_Aborted :
      logmsg << "ABORT : ";
      break;
    case JobAction::job_Canceled :
      logmsg << "CANCEL : ";
      break;
    case JobAction::job_FatalError :
      logmsg << "FATAL ERROR : ";
      break;
    default :
      break;
    }
    logmsg << "FileStream " << getLabel() << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }
}

/* --------------------------------------------------------------------------- */
/* workReadFile --                                                             */
/* --------------------------------------------------------------------------- */

JobAction::JobResult FileStream::workReadFile(){
  BUG( BugOperator, "FileStream::workReadFile" );

  if( !m_process->isRunning() ){
    m_failed = false;
    m_process->start();
    return JobAction::job_Wait; /* continue */
  }
  if( !m_process->hasTerminated() )
    return JobAction::job_Wait; /* continue */

  if( m_process->getExitStatus() ){
    return JobAction::job_Aborted;

  }
  return JobAction::job_Ok; /* stop */
}

/* --------------------------------------------------------------------------- */
/* FileStream::ReadListener::backFromWorker --                                 */
/* --------------------------------------------------------------------------- */

void FileStream::ReadListener::backFromWorker( JobAction::JobResult rslt ){
  m_stream->endFileStream( rslt );
}

/* --------------------------------------------------------------------------- */
/* FileStream::ReadListener::backFromWorker --                                 */
/* --------------------------------------------------------------------------- */

void FileStream::WriteListener::backFromWorker( JobAction::JobResult rslt ){
  m_stream->endFileStream( rslt );
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */

void FileStream::FileStreamAction::startJobAction(){
  if( m_open ) {
	m_stream->openFile(this);
  } else {
	m_stream->saveFile(this);
  }
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void FileStream::FileStreamAction::serializeXML(std::ostream &os, bool recursive){
  os << "<filestream name=\"" << Name() << "\">" << std::endl;
  os << "</filestream>" <<  std::endl;
}

/* --------------------------------------------------------------------------- */
/* ownerId --                                                                  */
/* --------------------------------------------------------------------------- */

std::string FileStream::ownerId() const{
  return m_name;
}

/* --------------------------------------------------------------------------- */
/* decodeBase64data --                                                         */
/* --------------------------------------------------------------------------- */

bool FileStream::decodeBase64data(std::string& data, std::string& header) {

  // decode data if necessary
  size_t pos = data.find_first_of(',');
  header = data.substr(0, pos);
  if (header.rfind("data:application/") == 0 ||
      header.rfind("data:text/") == 0 ||
      header.rfind("data:image/") == 0) {
    BUG_INFO("Reading from stream  header["<<header<<"] dataSize["<<data.size() - pos<<"]");
    std::string encoded = data.substr(pos + 1);
    size_t outsize = data.size() - pos;
    char *output = new char[data.size()];

    // decode
    if( base64decode(encoded, reinterpret_cast< unsigned char *>( output ), outsize ) ){
      data = std::string(output, outsize);
      BUG_INFO("base64decode header["<<header<<"] newSize["<<outsize<<"]");
    }
    delete[] output;
    return true;
  }
#if 0
  else {
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": BEGIN : data type unkown, data: " << header.substr(0, 20) << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    GuiFactory::Instance()->showDialogWarning
      ( 0
        , _("Error")
        , _("Unknown file format. (See Log Window)")
        , 0 );
  }
#endif
  return false;
}
