
#if !defined(REPORT_GEN_INCLUDED_H)
#define REPORT_GEN_INCLUDED_H

#include <sstream>

#include <set>

#include "operator/BatchProcess.h"
#include "operator/Channel.h"
#include "operator/Worker.h"
#include "gui/DialogWorkClock.h"
#include "app/HardCopyListener.h"

class GuiElement;

class ReportGen : public WorkerController
		  , public DialogWorkClockListener{

  //---------------------------------------------------------------------------//
  // Constructor / Destructor                                                  //
  //---------------------------------------------------------------------------//
 private:
  ReportGen();
  ReportGen(const ReportGen &);
  ReportGen &operator=(const ReportGen&);
 public:
  virtual ~ReportGen();
  //---------------------------------------------------------------------------//
  // public definitions                                                        //
  //---------------------------------------------------------------------------//
 public:
  enum Mode{
      PRINT
    , SAVE
    , PREVIEW
  };

  //---------------------------------------------------------------------------//
  // private definitions                                                       //
  //---------------------------------------------------------------------------//
 private:
  class HardCopyMsgDisplay: public ChannelListener {
    // ChannelListener
    virtual void resetContinuousUpdate(){}
    virtual bool setContinuousUpdate( bool flag){ return false; }
    virtual bool read( OutputChannelEvent &event ){ return true; }
    virtual bool write( InputChannelEvent &event ) { return false; }
#if defined HAVE_QT
    virtual QProcess* getQProcess() { assert(false); return 0;}
#endif
  };

  //---------------------------------------------------------------------------//
  // public member functions of WorkerController                               //
  //---------------------------------------------------------------------------//
 public:
  virtual JobAction::JobResult work();
  virtual void startWorker(){}
  virtual void backFromWorker( JobAction::JobResult rslt );

  //---------------------------------------------------------------------------//
  // public member functions of DialogWorkClockListener                        //
  //---------------------------------------------------------------------------//
  virtual void cancelButtonPressed();

  //---------------------------------------------------------------------------//
  // public member functions                                                   //
  //---------------------------------------------------------------------------//
 public:
  int newTmpFile( const std::string &fileName );
  static ReportGen &Instance();
  void setExitMsg( const std::string &msg ){ m_exitMsg = msg; }
  bool printDocument( Mode mode
		      , HardCopyListener *hcl
		      , GuiElement *dialog
		      , std::string fileName
		      , const std::string &printer
		      , int paperSize
		      , HardCopyListener::Orientation orientation
		      , int quantity
		      , HardCopyListener::FileFormat fileFormat
		      , JobAction * jobAction=0);
  bool generateXMLReport();
  bool writeXML( HardCopyListener *hcl, std::string &fileName );
  bool writeXML( std::ostringstream &xmlStream, std::ostream &os, const std::string &xslFileName );
  void deleteFiles();
  std::string appendExtension( const std::string &name, const std::string &extension );
  static bool checkFormats( HardCopyListener *hcl, HardCopyListener::FileFormat ff );
  static void getFormats( HardCopyListener *hcl, std::vector<HardCopyListener::FileFormat> &formats );
  const std::string getSuffix( const HardCopyListener::FileFormat format );

  //---------------------------------------------------------------------------//
  // private member functions                                                  //
  //---------------------------------------------------------------------------//
 private:
  std::string locateXSLFile( const std::string &name,  const std::string &suffix = "" );
  std::string locateTemplateFile( const std::string &name );
  void end();
  void preview();
  void print();
  void save();
  bool runXalan( std::ostringstream &inputStream
		 , const std::string &xslFileName
		 , std::ostringstream &outputStream );
  const std::string getPrefix( const HardCopyListener::FileFormat format );

  void validatePreviewFile();

  //---------------------------------------------------------------------------//
  // private Data                                                              //
  //---------------------------------------------------------------------------//
 private:
  static ReportGen             *s_instance;
  BatchProcess                  m_repGenProcess;
  BatchProcess                  m_outputProcess[4];
  InputChannel                  m_repGenInputChannel;
  OutputChannel                 m_repGenOutputChannel;
  InputChannel                  m_outputInChannel;
  OutputChannel                 m_outputOutChannel;
  HardCopyMsgDisplay            m_msgDisplay;
  std::set<std::string>         m_tmpFiles;
  std::set<std::string>         m_files;
  std::string                   m_previewFile;
  std::string                   m_tmpFile;
  std::string                   m_currentFile;
  std::string                   m_currentPrinter;
  int                           m_currentPaperSize;
  std::ostream                 *m_currentStream;
  Mode                          m_currentMode;
  GuiElement                   *m_currentDialog;
  HardCopyListener             *m_currentListener;
  int                           m_currentQuantity;
  HardCopyListener::FileFormat  m_currentFileFormat;
  HardCopyListener::Orientation m_currentOrientation;
  std::string                   m_exitMsg;
  bool                          m_newVersion;
  bool                          m_checkExitStatus;

  HardCopyListener             *m_fileBufferListener;

  DialogWorkClock              *m_dialogWorkClock;
  JobAction                    *m_jobAction;
};

#endif
