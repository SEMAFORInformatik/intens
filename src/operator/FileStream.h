
#if !defined(FILESTREAM_INCLUDED_H)
#define FILESTREAM_INCLUDED_H

#include <string>
#include <vector>

#include "operator/Channel.h"
#include "operator/BatchProcess.h"
#include "gui/DialogWorkClock.h"
#include "operator/Worker.h"
#include "gui/FileSelectListener.h"
#include "gui/InformationListener.h"
#include "gui/ConfirmationListener.h"
#include "gui/GuiElement.h"
#include "gui/GuiMenuButtonListener.h"
#include "job/JobAction.h"
#include "datapool/TransactionOwner.h"

class DataReference;
class InputChannelEvent;
class OutputChannelEvent;

class FileStream : public ChannelListener
                 , public TransactionOwner
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  FileStream(const std::string &title="", Stream *s=0);
public:
  virtual ~FileStream();

/*=============================================================================*/
/* private Definitions for Save-Function                                       */
/*=============================================================================*/
private:
  class FileStreamAction : public JobAction
  {
  public:
    FileStreamAction( FileStream *s, bool open )
      : m_stream( s )
      , m_open( open )
      , m_xfer( 0 ) {}
    virtual ~FileStreamAction(){}
    virtual void startJobAction();
    virtual void stopJobAction(){ assert( false );}

    virtual void printLogTitle( std::ostream &ostr ){
      ostr << "FILESTREAM ";
    }

    void endFileStream( JobResult rslt ){
      endJobAction( rslt );
    }

    virtual void serializeXML(std::ostream &os, bool recursive = false);
    void setFilename( const std::string& filename ) {
	  m_filename = filename;
	  m_stream->setFilename( filename );
    }
    void setBaseFilename( const std::string& filename ) {
      m_baseFilename = filename;
      m_stream->setBaseFilename( filename );
    }
    const std::string& getFilename() { return m_filename; }
    void setXfer( XferDataItem *xfer ){
      m_xfer = xfer;
    }
    XferDataItem * getXfer(){ return m_xfer; }
  protected:
    virtual void backFromJobController( JobResult rslt ){ assert( false ); }
  private:
    FileStream *m_stream;
    XferDataItem *m_xfer;
    std::string   m_filename;
    std::string   m_baseFilename;
    bool m_open;
  };

  class SaveButtonListener : public GuiMenuButtonListener
  {
  public:
    SaveButtonListener( FileStream *s ): m_stream( s ) {}
    virtual void ButtonPressed() { m_stream->startSaveAction(); }
    JobAction* getAction(){ return m_stream->getSaveAction(); }
  private:
    FileStream *m_stream;
  };

  class SaveListener : public FileSelectListener, public ConfirmationListener
  {
  public:
    SaveListener( FileStream *s ): m_stream( s ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat ff
			       , const std::string dir );
    virtual void canceled();
    virtual void confirmYesButtonPressed();
    virtual void confirmNoButtonPressed();
    virtual void writeStreamData(std::ostream& stream, HardCopyListener::FileFormat& format);
    /// get file flag of stream
    bool getStreamFileFlag(){
      if (m_stream != nullptr)
        return m_stream->getFileFlag();
      return false;
    }
    /// get file name of stream with file flag
    std::string getStreamFilename() { return m_stream->getStreamFilename(); }
    /// get stream
    Stream* getStream() {
      if (m_stream != nullptr)
        return m_stream->m_stream;
      return 0;
    }
  private:
    std::string m_filename;
    FileStream *m_stream;
  };

  class WriteListener : public WorkerController
			, public DialogWorkClockListener
  {
  public:
    WriteListener( FileStream *s ): m_stream( s ) {}
    virtual void startWorker() {}
    virtual JobAction::JobResult work() { return m_stream->workWriteFile(); }
    virtual void backFromWorker( JobAction::JobResult );
    virtual void cancelButtonPressed(){ m_stream->cancelFile(); }

  private:
    FileStream *m_stream;
  };

  class Trigger : public JobStarter
  {
  public:
    Trigger( JobAction *f )
      : JobStarter( f ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt ){}
  };


/*=============================================================================*/
/* private Definitions for Open-Function                                       */
/*=============================================================================*/
private:
  class OpenButtonListener : public GuiMenuButtonListener
  {
  public:
    OpenButtonListener( FileStream *s ): m_stream( s ) {}
    virtual void ButtonPressed() { m_stream->startOpenAction(); }
    JobAction* getAction(){ return m_stream->getOpenAction(); }
  private:
    FileStream *m_stream;
  };

  class OpenListener : public FileSelectListener, public ConfirmationListener
  {
  public:
    OpenListener( FileStream *s ): m_stream( s ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat ff
			       , const std::string dir );
    virtual void canceled();
    virtual void confirmYesButtonPressed();
    virtual void confirmNoButtonPressed();
    virtual void readStreamData(std::istream& stream);
  private:
    std::string m_filename;
    FileStream *m_stream;
  };

  class ReadListener : public WorkerController
    ,  public DialogWorkClockListener
  {
  public:
    ReadListener( FileStream *s ): m_stream( s ) {}
    virtual JobAction::JobResult work() { return m_stream->workReadFile(); }
    virtual void startWorker() {}
    virtual void backFromWorker( JobAction::JobResult rslt );
    virtual void cancelButtonPressed(){ m_stream->cancelFile(); }

  private:
    FileStream *m_stream;
  };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void startSaveAction();
  void startOpenAction();
  void endFileStream( JobAction::JobResult rslt );
  static FileStream *newFileStream( const std::string &name, Stream *s );

  /// extracts a registered filestream by name
  static FileStream *find( const std::string &name );

  /// extracts a registered filestream by stream
  static FileStream *find( const Stream *s );

  ///  returns true if there are readable filestreams registered
  static bool hasReadableFileStreams();

  ///  returns true if there are writable filestreams registered
  static bool hasWritableFileStreams();

  /// sets the current timestamp of all filestreams
  static void setTimestamp();

  /// decode base64 data
  static bool decodeBase64data(std::string& data, std::string& header);

  /// returns the name of the filestream
  const std::string &Name() { return m_name; }

  /// sets the timestamp of a specific filestream
  void setTimestamp( int trans );

  /// sets the pre process command
  bool setProcess( const std::string &cmd );

  /// sets the reset flag
  bool setReset();

  /// sets readonly flag
  void setReadOnly() { m_readonly = true; }
  /// is readonly
  bool ReadOnly() { return m_readonly; }

  /// sets writeonly flag
  void setWriteOnly() { m_writeonly = true; }
  /// is writeonly
  bool WriteOnly() { return m_writeonly; }

  /// defines the filter std::string in the fileselection box.
  void setFilter( const std::string &filter ) { m_filter = filter; getBaseFilename(); }

  /// defines the directory in the fileselection box.
  void setDirname( const std::string &dirname ) { m_dir = dirname; }

  /// defines the filestream menu button label
  void setLabel( const std::string &label ) { m_label = label; }

  /// standalone
  bool standalone() { return m_action == 0; }

  /// set Log-Output on
  void setLogOff() { m_log_on = false; }

  /// ask if log-output is on
  bool isLogOn() { return m_log_on; }

  /// set Hidden (No MenuButtons)
  void setHidden() { m_hidden = true; }

  /// set Binary Mode
  void setBinaryMode() { m_binary_mode = true; }

  /// set automatic file extension
  void setExtension( const std::string &extension ) { m_extension = extension; }

  /// returns the DataReference filename in the fileselection box.
  DataReference *getDataFilename() { return m_data_filename; }
  /// set the DataReference filename in the fileselection box.
  void setDataFilename(DataReference* dataRef) { m_data_filename = dataRef; }

  /// installs the file menu callback procedure
  bool install();

  /// Basisfunktion für den ChannelListener
  virtual void resetContinuousUpdate(){};
  virtual bool setContinuousUpdate( bool flag ){ return true; }
  bool read( OutputChannelEvent & );
  bool write( InputChannelEvent & );
#if defined HAVE_QT
    virtual QProcess* getQProcess() { assert(false); return NULL; }
#endif

  virtual void setTargetValid(){};
  virtual std::vector<std::string> getInvalidItems(){ return std::vector<std::string>(); }

  /// File save functions
  FileStreamAction *getSaveAction( XferDataItem * );
  /// File save functions with file name
  FileStreamAction *getSaveAction( const std::string& filename, const std::string& base_filename=std::string());
  /// Get Save FileStreamAction
  FileStreamAction *getSaveAction() { return &m_save_action; }
  void saveFile( FileStreamAction *action = 0 );
  void writeFile( const std::string &filename );
  virtual JobAction::JobResult workWriteFile();
  virtual void cancelFile();

  /// File read functions
  FileStreamAction *getOpenAction( XferDataItem * );
  /// File read functions with file name
  FileStreamAction *getOpenAction( const std::string& filename );
  /// Get Open FileStreamAction
  FileStreamAction *getOpenAction() { return &m_open_action; }
  ///
  void openFile( FileStreamAction *action = 0, FileSelectListener *listener=0 );
  void readFile( const std::string &filename );
  virtual JobAction::JobResult workReadFile();
  bool hasFailed()const{ return m_failed; }
  const std::string& getLabel() const { return m_label; }

  void workFailed( const char *msg );

  bool checkTargets();
  void clearTargets();

  // Function of TransactionOwner
  virtual std::string ownerId() const;
  /// get file flag of stream
  bool getFileFlag(){ return m_stream->getFileFlag(); }
  /// get file name of stream with file flag
  std::string getStreamFilename() { return m_stream->getStreamFilename(); }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void installData();
  void setFilename( const std::string &filename );
  void setBaseFilename( const std::string &filename );
  std::string getBaseFilename();

  FileStream( const FileStream & );
  void operator=(const FileStream & );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::vector<FileStream *> FileStreamList;

  static FileStreamList   s_filestreamlist;

  bool               m_readonly;
  bool               m_writeonly;
  std::string        m_filter;
  std::string        m_dir;
  DataReference     *m_data_filename;
  std::string        m_baseFilename;
  std::string        m_name;
  std::string        m_label;
  BatchProcess      *m_process;
  OutputChannel      m_outchannel;
  InputChannel       m_inchannel;
  Stream            *m_stream;
  int                m_cycle;
  bool               m_failed;
  bool               m_log_on;
  bool               m_hidden;
  bool               m_binary_mode;
  std::string        m_extension;
  FileStreamAction  *m_action;
  FileStreamAction   m_save_action;
  SaveListener       m_save_listener;
  OpenListener       m_open_listener;
  WriteListener      m_write_listener;
  FileStreamAction   m_open_action;
  ReadListener       m_read_listener;
  DialogWorkClock   *m_dialog;

  friend class HeadlessGuiFactory; // muss leider sein
};

#endif
