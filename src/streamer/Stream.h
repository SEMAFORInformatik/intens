
#ifndef STREAMER_STREAM_H
#define STREAMER_STREAM_H

#include "streamer/BasicStream.h"
#include "streamer/StreamParameter.h"
#include "streamer/StreamParameterGroup.h"
#include "xfer/XferConverter.h"
#include "app/DataPoolIntens.h"
#include "xml/Serializable.h"
#include "app/HardCopyListener.h"
#include "app/UserAttr.h"

#include "xml/XMLDocumentHandler.h"

/* forward declarations of local definitions */
class DataReference;
class GuiScrolledText;
class GuiForm;
class XferDataItem;
class StreamManager;
class InputChannelEvent;
class OutputChannelEvent;
class HardCopyListener;

class Stream_Rep;

class Stream : public BasicStream, public Serializable
{
  friend class StreamManager;
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  Stream( const std::string &name, bool check );
public:
  virtual ~Stream();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setLatexFlag(){ m_format = HardCopyListener::LaTeX; }
  void setXMLFlag(){ m_format = HardCopyListener::XML; }
  void setUrlFlag(){ m_format = HardCopyListener::URL; }
  void setJSONFlag(){ m_format = HardCopyListener::JSON; m_pargroup.setFormat(HardCopyListener::JSON); }
  bool hasJSONFlag(){ return m_format == HardCopyListener::JSON; }
  HardCopyListener::FileFormat getFormat() { return m_format; }

  /// set stream locale flag
  void setLocaleFlag(){ m_locale = true; }
  /// get stream locale flag
  char getLocaleFlag(){ return m_locale; }
  /// set stream file flag
  void setFileFlag(){ m_file = true; }
  /// get stream file flag
  bool getFileFlag(){ return m_file; }
  /// get file size of stream with file flag
  long getStreamFileSize() { return m_filesize; }
  /// write data to file, returns filename
  std::string writeFileData(void* data, int size);
  /// write data to file, returns filename
  std::string writeFileData(std::istream& is);
  /// read file content and write to ostream
  void readFileData(std::ostream& os);
  void readFileData(char *cp);

  // set stream delimiter
  void setDelimiter(char delim );
  // get stream delimiter
  char getDelimiter(){ return m_delimiter; }
  // set stream indentation
  void setIndentation(int i){ m_indentation = i; }
  // set option hidden to show hidden data
  void setHidden(bool yes) { m_show_hidden = yes; }
  // set option transient to show transient data
  void setTransient(bool yes) { m_show_transient = yes; }

  /// ignore hidden and/or dbTransient data items when writing (only used with JSON)
  SerializableMask getHideFlags() const;
  ///
  void setHideFlags( const SerializableMask flags );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

  /// open a new parameter group within the current group
/*   void beginGroup(); */
  StreamParameter *beginGroup();
  /// return current group
  StreamParameterGroup *getCurrentGroup();

  /// adds a parameter to the currently open group
  StreamParameter *addIntParameter();
  StreamParameter *addRealParameter();
  StreamParameter *addStringParameter();
  StreamParameter *addStructParameter();
  StreamParameter *addComplexParameter();
/*   StreamParameter *addParameter( XferDataItem *ditem */
/* 				 , int length=0, int prec=0, double scale=0 */
/* 				 , bool thousand_sep=false */
/* 				 , bool  mandatory=false */
/* 				 , bool isDatasetText=false ); */

  StreamParameter *addVarParameter();
  StreamParameter *addVarParameter( XferDataItem *varDataItem,  XferDataItem *dataItem );

  StreamParameter *addParameter( XferDataItem *ditem
				 , int length, int prec, Scale *scale
				 , bool thousand_sep
				 , bool  mandatory
				 , bool isDatasetText );

  StreamParameter *addXMLParameter( XferDataItem *ditem
                                    , const std::vector<std::string> &attrs
                                    , bool allCycles
                                    , const std::string &
                                    , const std::string &
                                    , const std::string &
                                    , const std::string & );

  StreamParameter *addJSONParameter( XferDataItem *ditem
                                     , bool allCycles );

  StreamParameter *addDatasetParameter();
  StreamParameter *addDatasetParameter( XferDataItem *ditem
				      , int length=0
				      , bool  mandatory=false );

  /// adds a matrix parameter to the currently open group
  StreamParameter *addNumericMatrixParameter();
  StreamParameter *addStringMatrixParameter();
  StreamParameter *addComplexMatrixParameter();
  StreamParameter *addMatrixParameter( XferDataItem *ditem
				       , int length, int prec, Scale *scale
				       , bool thousand_sep, char delimiter, bool mandatory );

  /// adds a plot or xml group parameter to the currently open group
  StreamParameter *addPlotGroupParameter( const std::string &name );
  StreamParameter *addXMLPlotGroupStreamParameter( HardCopyListener *hcl );

  /// adds an index parameter to the currently open group
  StreamParameter *addIndexParameter( const std::string &name
				      , int length );

  /// adds a constant string token to the currently open group
  StreamParameter *addToken( const std::string &token, int length, bool setFormat=false );

  /// adds a serializedFormParameter to the currently open group
  StreamParameter *addSerializedFormParameter( std::vector<std::string> &formIdList, const std::string &xslFile );

  StreamParameter *addStringDateParameter( XferDataItem *ditem, UserAttr::STRINGtype type
					   , int length=0
					   , bool  mandatory=false );

  StreamParameter *addGuiIndexParameter( GuiIndex *elem );

  /// adds a label token
  bool addLabel( DataReference * );

  /// adds a unit token
  bool addUnits( DataReference * );

  /// close the current group
  void endGroup();

  /// resets the stream
  void reset();

  /// returns the number of valid parameters
  int numberOfItems( bool count_list );

  /// reads from istream
  bool read( std::istream &istr );

#ifdef __MINGW32__
  /// reads from file named filename
  void read( const std::string &filename );
#endif

  /// ChannelListener Interface
  virtual bool read( OutputChannelEvent& event );
  virtual bool write( InputChannelEvent& event );
  virtual void resetContinuousUpdate();
  virtual bool setContinuousUpdate( bool flag );
#if defined HAVE_QT
  virtual QProcess* getQProcess() { assert(false); return NULL; }
#endif

  /// writes to ostream
  bool write( std::ostream &ostr );

  /// returns the number of parameters in the top most stream group
  size_t   size() const {return m_pargroup.size();}

  ///
  void putValues( StreamDestination *dest, bool transposed );
  ///
  void getValues( StreamSource *src );
  ///
  StreamParameterGroup *getParameterGroup(){ return &m_pargroup; }

  /// writes an identifier list of the included parameters
  virtual int getParameters( std::ostream &os ) const;
  /// returns a list of item names that must be valid
  virtual bool getInvalidItems(std::vector<std::string> &invaliditemnames);

  /** Diese Funktion löscht alle alle Daten, welche durch diesen Stream beim
      Ablauf eines Prozesses gefüllt werden.
  */
  virtual void clearRange();

  /** Die Funktion setzt bei allen verfügbaren, nicht transienten Items den
      DbTimestamp, damit die Werte 'not modified' sind. Diese Aktion ist nur
      nach dem Laden von Komponenten aus der Datenbank sinnvoll.
  */
  virtual void setDbItemsNotModified();

  /** Diese Funktion muss nach dem Parser laufen, damit das Löschen abhängiger
      Werte funktioniert (Targets)
  */
  virtual void fixupItemStreams();
  StreamParameterGroup *currentGroup(){ return m_current_group; }
  /** Hat der Stream gültige Daten
   */
  bool hasValidData();

  virtual std::string getErrorMessage(bool target=false);

 private:
  Stream( const Stream & );
  void operator=( const Stream & );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  std::string            *m_buffer;
  StreamParameterGroup    m_pargroup;
  StreamParameterGroup   *m_current_group;
  HardCopyListener::FileFormat m_format;
  char                    m_delimiter;
  int                     m_indentation;
  bool                    m_show_hidden;
  bool                    m_show_transient;
  bool                    m_locale;
  bool                    m_file;
  long                    m_filesize;
};

#endif
