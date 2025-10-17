
#ifndef STREAMER_BASICSTREAM_H
#define STREAMER_BASICSTREAM_H

#include "datapool/DataPool.h"
#include "operator/Channel.h"
#include "operator/ChannelListener.h"
#include "operator/IntensServerSocket.h"
#include "operator/Worker.h"

/* forward declarations of local definitions */
class StreamSource;
class DataReference;
class GuiScrolledText;
class XferDataItem;
class BasicStream;
class BatchProcess;

typedef std::list<BasicStream *> TargetStreamList;
typedef std::map<BasicStream *, bool> TargetStreamMap;

class BasicStream : public ChannelListener, public IntensSocketListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  BasicStream( const std::string &name );
  virtual ~BasicStream();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool read( std::istream & ){ assert( false ); return false; } // from IntensSocketListener
  virtual bool write(std::ostream & ){ assert( false ); return false; } // from IntensSocketListener
  inline const std::string &Name() { return m_name; }
  void installData();
  virtual void fixupItemStreams() {}

  /// writes an identifier list of the included parameters
  virtual int getParameters( std::ostream &os ) const = 0;
  virtual void getValues( StreamSource *src ) = 0;

  void setTextWindow( GuiScrolledText *t ) { m_textwindow = t; }
  GuiScrolledText *getTextWindow() { return m_textwindow; }
  void clearTextWindow();

  virtual void clearRange() {}
  virtual bool checkRange( TransactionNumber timestamp ) { return false; }

  /** add target stream
      @param autoClear !!! if TRUE !!! clear target stream already if checkTargets methods are called
   */
  void addTargetStream( BasicStream *s, bool autoClear=false);
  void addItemStreams( TargetStreamList &streamlist );
  bool checkTargetStreams( TransactionNumber timestamp );
  void clearTargetStreams( TransactionNumber timestamp );
  bool checkAsTargetStream( TransactionNumber timestamp );
  void clearAsTargetStream( TransactionNumber timestamp );
  void printTargetStreams(std::ostream& os);
  void printSourceStreams(std::ostream& os);
  void printItemStreams(std::ostream& os);

  /// defines the pre process filter
  void setProcess( const std::string &cmd );
  bool hasProcess();
  bool runProcess(std::ostream &instr, std::ostream &outstr);
  /** Error Message
   */
  virtual std::string getErrorMessage(bool target=false);

  /** set stream append option
      if append option is true, Notice stream is not automaticly cleared
      it should be cleared self-controlled
   */
  void setAppendOption( bool appendOption );
  /** has stream append option
   */
  bool hasAppendOption() const;
  /** set stream no gz option
   */
  void setNoGzOption( bool noGzOption );
  /** has stream no gz option
   */
  bool hasNoGzOption() const;
  inline std::string DefinitionFilename() const { return m_definitionFile; }
  inline int Lineno() const { return m_lineno; }
  inline void setDefinitionFilename( std::string name ) { m_definitionFile = name; }
  inline void setLineno( int lineno ) { m_lineno = lineno; }
  /// get file name of stream with file flag
  std::string getStreamFilename();
  /** delete all temporary file. Only used with streams with file option */
  static void deleteFiles();


/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
 protected:
  bool writeBuffer( const std::string & s, int idx );
  bool readBuffer( std::string &s, int idx );
  void setDataItemsValid(bool target=true);
  bool isDataItemUpdated( TransactionNumber timestamp );
  void setDataItemsInvalid(bool target=true);
  /** has valid data
   */
  virtual bool hasValidData() { return true; }
  bool isTarget() { return m_sourcestreams.size() > 0; }
  /// set file name of stream with file flag
  std::string newStreamFilename();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void addItemStream( BasicStream *s );
  bool checkItemsAndTargets( TransactionNumber timestamp );
  void clearItemsAndTargets( TransactionNumber timestamp );
  bool checkTargets( TransactionNumber timestamp );
  void clearTargets( TransactionNumber timestamp );
  bool checkItemStreams( TransactionNumber timestamp );
  void clearItemStreams( TransactionNumber timestamp );
  bool DataItemsAreValid(bool target=true);
  int getLastTransactionNumber();
  bool getLastTransactionNumberFromSource(std::vector<int>& res);

/*=============================================================================*/
/* private classes                                                             */
/*=============================================================================*/
 private:
  class ProcessInputChannelListener : public ChannelListener {
  public:
    void setInStream(std::ostream &ostr) {
      m_ostr = &ostr;
    }
    virtual bool read( OutputChannelEvent & ) {
      BUG( BugTargets, "ProcessInputChannelListener::read" );
      return true;
    }
    /// writes to ostream
    virtual bool write( InputChannelEvent& event);
    /// terminates an updatable Stream
    virtual void resetContinuousUpdate() {
    }
    virtual bool setContinuousUpdate( bool flag ) {
      return false;
    }
  private:
    std::ostream* m_ostr;
  };

  class ProcessOutputChannelListener : public ChannelListener {
  public:
    virtual bool read( OutputChannelEvent & ) {
      BUG( BugTargets, "ProcessOutputChannelListener::read" );
      return true;
    }
    /// writes to ostream
    virtual bool write( InputChannelEvent& ) {
      BUG( BugTargets, "ProcessOutputChannelListener::write" );
      return true;
    }
    /// terminates an updatable Stream
    virtual void resetContinuousUpdate() {
    }
    virtual bool setContinuousUpdate( bool flag ) {
      return false;
    }
  };

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  std::string      m_name;
  std::string      m_processCmd; // cmd preprocess
  BatchProcess    *m_process;
  bool             m_appendOption;
  bool             m_noGzOption;
  int              m_lineno;
  std::string      m_definitionFile;
  ProcessInputChannelListener  m_preProcessInChannelListener;
  ProcessOutputChannelListener m_preProcessOutChannelListener;
  OutputChannel    m_preProcessOutChannel;
  InputChannel     m_preProcessInChannel;

  GuiScrolledText    *m_textwindow;

  // ----------------------------------------------------------------------------
  // Diese Liste enthält alle direkten Output-Streams, welche einem Input-Stream
  // in einem Process zugewiesen werden können. Ist der Stream nie Input, so
  // bleibt die Liste leer.
  // Neu als Map!!!
  // Im Falle von PROCESSGROUP Option DEPENDENCIES=CLEAR wird ungefragt
  // der targetstream geloescht
  // ----------------------------------------------------------------------------
  TargetStreamMap    m_targetstreams;
  TargetStreamList   m_sourcestreams;

  // ----------------------------------------------------------------------------
  // Diese Liste enthält alle Targetstreams aller Data-Items in diesem Stream.
  // Sie werden nur interessant, wenn der Stream irgendwo als Output-Stream in
  // einer Target-Liste auftaucht.
  // ----------------------------------------------------------------------------
  TargetStreamList    m_itemstreams;

  // ----------------------------------------------------------------------------
  // Im Datapool werden natürlich pro Cycle die letzten gültigen Output-Daten
  // abgelegt, sowie ein Flag, ob sie gültig sind.
  // ----------------------------------------------------------------------------
  DataReference      *m_data_buffer;
  DataReference      *m_data_valid; // wie m_data_buffer->GetItemValid() (alt)
  DataReference      *m_data_source_valid;
  DataReference      *m_data_filename;

  // ----------------------------------------------------------------------------
  // Der Timestamp wird benötigt um einen rekursiven Loop beim Aufruf von
  // checkTargetStreams() zu vermeiden.
  // ----------------------------------------------------------------------------
  TransactionNumber   m_timestamp;

  // ----------------------------------------------------------------------------
  // Diese Timestamps werden beim Aufruf von clearTargetStreams() benötigt,
  // damit jeder Stream nur einmal gelöscht wird.
  // ----------------------------------------------------------------------------
  TransactionNumber   m_items_are_cleared;
  TransactionNumber   m_targets_are_cleared;

  static std::set<std::string> s_filenames;
};


#endif
