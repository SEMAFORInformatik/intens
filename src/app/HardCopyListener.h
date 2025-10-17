
#if !defined(HARDCOPY_LISTENER_INCLUDED_H)
#define HARDCOPY_LISTENER_INCLUDED_H

#include <vector>
#include <map>

#include "gui/GuiMenuButtonListener.h"
#include "operator/ChannelListener.h"
#include "operator/InputChannelEvent.h"

class HardCopyObject;
class OutputChannelEvent;
class GuiEventData;
class QPrinter;  //for Qt Printing

class HardCopyListener : public ChannelListener
                       , public GuiMenuButtonListener
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  HardCopyListener();
  virtual ~HardCopyListener(){
  }

/*******************************************************************************/
/* public definitions                                                          */
/*******************************************************************************/
public:
  enum PaperSize
  { A2
  , A3
  , A4
  , Unscaled
  };
  enum Orientation
  { Landscape
  , Portrait
  };
  enum FileFormat
  { Postscript
    , HPGL
    , XML
    , JSON
    , URL
    , LaTeX
    , PDF
    , JPEG
    , GIF
    , SVG
    , BMP
    , PPM
    , TIFF
    , XBM
    , XPM
    , Text
    , PNG
    , ASCII
    , GZ
    , HTML
    , NONE
  };
  enum PrintType
  { SCRIPT
  , OWN_CONTROL
  , FILE_EXPORT
  , NOT_SUPPORTED
  };

  typedef std::vector<FileFormat> FileFormats;
  typedef std::map<FileFormat, PrintType>  FileFormats2;

/*******************************************************************************/
/* public Interfaces                                                           */
/*******************************************************************************/
public:
  virtual JobAction *getAction(){ return 0; }
  bool showMenu(){ return m_showMenu; }
  void showMenu( bool show ){ m_showMenu = show; }
  virtual bool saveFile( GuiElement * ) = 0;
  virtual FileFormat getFileFormat() = 0;
  virtual const std::string &getXSLFileName(){ return getName(); }
  virtual const std::string getTemplateFileName(){ return ""; }
  virtual const std::string &getName() = 0;
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ) = 0;
  /** is export of file type available */
  virtual bool isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat )
  { return false; }
  /** who makes print processsing */
  virtual PrintType getPrintType( const HardCopyListener::FileFormat &fileFormat )
  { return HardCopyListener::NOT_SUPPORTED; }
  virtual void print(QPrinter* print=0) { assert(false); }
  virtual const std::string &PrintFilter();
  void setPrintFilter( const std::string &filter );
  virtual const std::string &MenuLabel() = 0;
  virtual bool useMargins() { return true; }
  virtual bool usePaperSize() { return true; }
  virtual bool useOrientation() { return true; }
  virtual bool isEmpty() { return false; }
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &fileFormat,
				   double &lm, double &rm,
				   double &tm, double &bm ) {
    return false;}

  // GuiButtonListener
  virtual void ButtonPressed();
  virtual void ButtonPressed( GuiEventData * );

  // ChannelListener
  virtual void resetContinuousUpdate(){}
  virtual bool setContinuousUpdate( bool flag ){ return false; }
  virtual bool read( OutputChannelEvent &event ) { return false; }
  virtual bool write( InputChannelEvent &event ) = 0;//{
  //(event.getOstream()) << "Bad HardCopy Object"; }
  virtual bool write( const std::string &fileName ) = 0;//{ assert( false ); }
  virtual bool write( std::ostream &os ){ assert( false );return false; }
  /// sets Plot XRange !!!remove as soon as possible !!!
  virtual void setXRange( const double start, const double end ){}
  // Damit wir den Ast im Baum finden
  void setHardCopyObject( HardCopyObject * obj );
  HardCopyObject *getHardCopyObject();
  void setIndex( int inx ) { m_data_index = inx; }
  int getIndex() { return m_data_index; }

/*******************************************************************************/
/* private Data                                                                */
/*******************************************************************************/
private:
  HardCopyObject   *m_object;
  std::string       m_filter;
  int               m_data_index;
  bool              m_showMenu; // save-print-preview entry
};

#endif
