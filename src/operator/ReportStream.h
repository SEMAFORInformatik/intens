
#ifndef REPORTSTREAM_H
#define REPORTSTREAM_H

#include <string>

#include "gui/FileSelectListener.h"
#include "gui/ConfirmationListener.h"
#include "gui/InformationListener.h"

#include "operator/Channel.h"
#include "operator/BatchProcess.h"

#include "job/JobAction.h"

#include "app/HardCopyListener.h"

class Stream;
class FileButton;
class InputFileStream;

class ReportStream: public HardCopyListener {
public:
  ReportStream( const std::string &name, Stream *repstr );
  virtual ~ReportStream();

/*=============================================================================*/
/* private Definitions for Save-Function                                       */
/*=============================================================================*/
  class SaveButtonListener : public GuiMenuButtonListener{
  public:
    SaveButtonListener(ReportStream *s ): m_stream( s ) {}
    virtual void ButtonPressed() { m_stream->saveFile( getButton() );
    }
    virtual JobAction *getAction(){ return 0; }
  private:
    ReportStream *m_stream;
  };

  class SaveListener : public FileSelectListener, public ConfirmationListener
  {
  public:
    SaveListener( ReportStream *s ): m_stream( s ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat format
			       , const std::string dir );
    virtual void canceled();
    virtual void confirmYesButtonPressed();
    virtual void confirmNoButtonPressed();
  private:
    std::string m_filename;
    HardCopyListener::FileFormat m_saveFormat;
    ReportStream *m_stream;
  };

  void setDirname( const std::string &dirname ) { m_dir = dirname; }
  virtual bool saveFile( GuiElement * );
  virtual HardCopyListener::FileFormat getFileFormat(){ return m_fileFormat; }
  virtual const std::string &getXSLFileName(){ return m_xslFileName; }
  virtual const std::string getTemplateFileName(){ return m_templateFileName; }
  virtual const std::string &getName();
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual bool isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat );
  virtual HardCopyListener::PrintType getPrintType( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel() {
    return m_title;
  }
  virtual bool write( InputChannelEvent &event );
  virtual bool write( const std::string &fileName ){ return false; }
  virtual bool write( std::ostream &os ); //{ assert( false );}
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &format,
				   double &lm, double &rm,
				   double &tm, double &bm );
  void setTitle( const std::string &title ) { m_title = title; }
  void setLatexFlag() { m_fileFormat = HardCopyListener::LaTeX; }
  void setXmlFileFormat() { m_fileFormat = HardCopyListener::XML; }
  void setXslFileName( const std::string &xslFile ) { m_xslFileName = xslFile; }
  void setTemplateFileName( const std::string &templateFile ) { m_templateFileName = templateFile; }
  bool install();
  void writeFile( const std::string &filename, HardCopyListener::FileFormat ff );

  /// set Hidden (No MenuButtons)
  void setHidden() { m_hidden = true; }
 private:
  std::string  m_dir;
  SaveListener m_saveListener;
  std::string  m_title;
  Stream *m_stream;
  HardCopyListener::FileFormat m_fileFormat;
  std::string  m_xslFileName;
  std::string  m_templateFileName;
  std::string  m_filename;
  GuiElement  *m_saveButton;
  bool         m_hidden;
};

#endif
