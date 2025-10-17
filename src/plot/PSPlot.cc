
#include "plot/PSPlot.h"
#include "utils/Debugger.h"
#include "app/ReportGen.h"
#include "app/UiManager.h"

//***************************
//********* PSPlot **********
//***************************
/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
PSPlot::PSPlot( PSStream *stream )
  : m_stream( stream ){
  assert( stream != 0 );
  stream->installData();
  bool result = UImanager::Instance().addHardCopy( stream->Name(), this );
  assert(result );
  m_supportedFileFormats.push_back( HardCopyListener::Postscript );
  m_supportedFileFormats.push_back( HardCopyListener::PDF );
}

PSPlot::~PSPlot(){
  delete m_stream;
}

/* --------------------------------------------------------------------------- */
/* write                                                                       */
/* --------------------------------------------------------------------------- */
bool PSPlot::write( InputChannelEvent &event ){
  return m_stream->write( event );
}

bool PSPlot::write( const std::string &fileName ){
  return m_stream->write( fileName );
}

/* --------------------------------------------------------------------------- */
/* getFileFormat --                                                            */
/* --------------------------------------------------------------------------- */
HardCopyListener::FileFormat PSPlot::getFileFormat(){
  return HardCopyListener::Postscript;
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */
const std::string &PSPlot::getName(){
  return m_stream->Name();
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported                                                       */
/* --------------------------------------------------------------------------- */
bool PSPlot::isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::FileFormats::iterator it;
  bool result = false;
  for( it = m_supportedFileFormats.begin(); it != m_supportedFileFormats.end() && !result; ++it ){
    if( (*it) == fileFormat )
      result = true;
  }
  return result;
}

/* --------------------------------------------------------------------------- */
/* MenuLabel                                                                   */
/* --------------------------------------------------------------------------- */
const std::string &PSPlot::MenuLabel(){
  return m_title.empty() ? m_stream->Name() : m_title;
}

/* --------------------------------------------------------------------------- */
/* getStream                                                                   */
/* --------------------------------------------------------------------------- */
BasicStream* PSPlot::getStream(){
  return m_stream;
}

/* --------------------------------------------------------------------------- */
/* getDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */
bool PSPlot::getDefaultSettings( HardCopyListener::PaperSize &size,
				      HardCopyListener::Orientation &orient,
				      int &quantity,
				      HardCopyListener::FileFormat &fileFormat,
				      double &lm, double &rm,
				      double &tm, double &bm ) {
  size = HardCopyListener::A4;
  orient = HardCopyListener::Landscape;
  quantity = 1;
  fileFormat = HardCopyListener::Postscript;
  lm = 10;
  rm = 10;
  tm = 10;
  bm = 10;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setTitle                                                                    */
/* --------------------------------------------------------------------------- */
void PSPlot::setTitle( const std::string &title ){
  m_title = title;
}

//***************************
//**** PSPlot::PSStream *****
//***************************
/* --------------------------------------------------------------------------- */
/* getParameters --                                                            */
/* --------------------------------------------------------------------------- */
int PSPlot::PSStream::getParameters( std::ostream &os ) const{
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */
void PSPlot::PSStream::getValues( StreamSource *src ){
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */
bool PSPlot::PSStream::read( OutputChannelEvent &event ){
  BUG(BugPlot, "PSStream::read" );

  int idx = 0;
  writeBuffer( event.getStreamBuffer(), idx );
  setDataItemsValid();
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool PSPlot::PSStream::write( InputChannelEvent& event ){
  std::string buffer;
  if( !readBuffer( buffer, 0 ) )
    return false;
#ifdef HAVE_QT
  assert( false );
#else
  event.getOstream() << buffer;
#endif
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool PSPlot::PSStream::write( const std::string &fileName ){
  std::string myFileName = fileName + ".ps";
  std::string buffer;
  ReportGen::Instance().newTmpFile( myFileName );
  std::ofstream ofs( myFileName.c_str() );
  std::ostream &reportStream = ofs;
  if( !readBuffer( buffer, 0 ) ){
    reportStream << std::endl;
    return false;
  }
  reportStream << buffer;
  return true;
}
