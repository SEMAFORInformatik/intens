
#include <string>
#include <limits.h>
#include <sys/stat.h>
#ifdef __MINGW32__
//#define SSIZE_MAX INT_MAX
#endif

#include "utils/Debugger.h"
#include "datapool/DataReference.h"
#include "streamer/XMLPlotGroupStreamParameter.h"
#include "app/HardCopyListener.h"
#include "app/ReportGen.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

XMLPlotGroupStreamParameter::XMLPlotGroupStreamParameter( HardCopyListener *hcl)
  : StreamParameter()
  , m_listener( hcl ){
}

XMLPlotGroupStreamParameter::~XMLPlotGroupStreamParameter(){}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */
size_t XMLPlotGroupStreamParameter::getDimensionSize(int ndim) const {
  return SSIZE_MAX;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */
bool XMLPlotGroupStreamParameter::read( std::istream &is ){
  //
  std::cerr << "XMLPlotGroupStreamParameter::read() " << m_listener->getName() << std::endl;
  // FIX IT: does it make sense being able to read a plotgroup?
  abort();

  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool XMLPlotGroupStreamParameter::write( std::ostream &os ){
  BUG(BugStreamer, "XMLPlotGroupStreamParameter::write(ostream &)" );
#ifdef __MINGW32__
  std::string tmpFileName( tempnam( "/tmp" , m_listener->getName().c_str() ) );
#else
  char tfile[200];
  strcpy(tfile, std::string("/tmp/"+ m_listener->getName() + "-XXXXXX").c_str());
  int fd = mkstemp( tfile );
  if(fd == -1) {
    std::cerr << "Could not create temporary file " << tfile << std::endl;
  }
  std::string tmpFileName( tfile );
  close(fd);
  remove(tfile);
#endif
  std::string repstr("_.");
  char repchar='Z';
  struct stat filestat;
  // we must replace all underscores (LaTeX will love us) and make
  // sure that the file does not already exist
  do {
    for( std::string::size_type p=tmpFileName.find_first_of( repstr, 0 );
	 p!=std::string::npos;
	 p=tmpFileName.find_first_of( repstr, ++p ) ){
      tmpFileName[p]=repchar;
    }
    --repchar;
  } while( repchar > 'A' && stat( tmpFileName.c_str(), &filestat ) != -1 );
  os << tmpFileName;
  return ReportGen::Instance().writeXML( m_listener, tmpFileName );
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */
std::string XMLPlotGroupStreamParameter::getName()const{
  return m_listener->getName();
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */
void XMLPlotGroupStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 )
    dest -> putValue(*this, m_listener->getName() );
}

/* --------------------------------------------------------------------------- */
/* getFrameBoxSize --                                                          */
/* --------------------------------------------------------------------------- */
bool XMLPlotGroupStreamParameter::getFrameBoxSize( int &x, int &y ){
  x = m_frameBoxSize_x;
  y = m_frameBoxSize_y;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setFrameBoxSize --                                                          */
/* --------------------------------------------------------------------------- */
void XMLPlotGroupStreamParameter::setFrameBoxSize( const int x, const int y ){
  m_frameBoxSize_x = x;
  m_frameBoxSize_y = y;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void XMLPlotGroupStreamParameter::marshal( std::ostream &os ){
  os << "<XMLPlotGroupStreamParameter";
  os << " name=\"" << m_listener->getName() <<"\"";
  os << ">";
  os << "</XMLPlotGroupStreamParameter>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *XMLPlotGroupStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  std::cerr<<"PlotGroupStreamParameter::unmarshal: <"<<element<<">    NOT IMPLEMENTED YET !!!"<<std::endl;
//   cout<<"PlotGroupStreamParameter::unmarshal: <"<<element<<">"<<endl;
//   if( element == "XMLPlotGroupStreamParameter" ){
//     m_listener = GuiManager::Instance().getPlugin( attributeList["name"] );
//   }
  return 0;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void XMLPlotGroupStreamParameter::setText( const std::string &text ){
}
