
#include <typeinfo>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <limits.h>
#ifdef HAVE_QT
#include <QTemporaryFile>
#include <QDir>
#endif
#ifdef __MINGW32__
//#define SSIZE_MAX INT_MAX
#endif

#include "gui/GuiFactory.h"
#include "gui/GuiPrinterDialog.h"
#ifdef HAVE_QT
#include "gui/qt/GuiQtElement.h"
#endif
#include "utils/Debugger.h"
#include "datapool/DataReference.h"
#include "streamer/PlotGroupStreamParameter.h"
#include "app/HardCopyListener.h"
#include "app/ReportGen.h"
#include "app/AppData.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferScale.h"
#include "streamer/Stream.h"
#include "streamer/DatasetStreamParameter.h"
#include "streamer/RealStreamParameter.h"
#include "streamer/IntStreamParameter.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

PlotGroupStreamParameter::PlotGroupStreamParameter(const std::string &name)
  : StreamParameter()
  , m_name(name)
  , m_transparentBg( true )
  , m_range( 0 ){}

PlotGroupStreamParameter::~PlotGroupStreamParameter(){
  delete m_range;
}

/* --------------------------------------------------------------------------- */
/* setXRange --                                                                */
/* --------------------------------------------------------------------------- */

bool PlotGroupStreamParameter::setXRange( const double start, const double end ){
  if( m_range != 0 )
    delete m_range;
  m_range = new Range( start, end );
  return true;
}
/* --------------------------------------------------------------------------- */
/* setXRange --                                                                */
/* --------------------------------------------------------------------------- */

bool PlotGroupStreamParameter::setXRange( XferDataItem *start
					  , XferDataItem *end
					  , Scale *startScale
					  , Scale *endScale
					  , StreamParameterGroup *s
					  , char delimiter ){
  if( m_range != 0 )
    delete m_range;

  if( start == 0 || end == 0 )
    return false;

  m_range = new Range( start, end, startScale, endScale );

  DataStreamParameter *p=0;
  int level = start ->getNumberOfLevels()-1;

  switch( start->Data()->getDataType() ){
  case DataDictionary::type_Integer:
    p = new IntStreamParameter( start, level, 0, 0, false, delimiter );

    break;
  case DataDictionary::type_Real:
    p = new RealStreamParameter( start, level, 0, -1, 0, false, false, delimiter, false );
    break;

  default:
    return false;
  }
  if( s != 0 ){
    s->linkWithIndex( p );
  }

  p=0;
  level = end ->getNumberOfLevels()-1;
  switch( end->Data()->getDataType() ){
  case DataDictionary::type_Integer:
    p = new IntStreamParameter( end, level, 0, 0, false, delimiter );

    break;
  case DataDictionary::type_Real:
    p = new RealStreamParameter( end, level, 0, -1, 0, false, false, delimiter, false );
    break;

  default:
    return false;
  }
  if( s!= 0 ){
    s->linkWithIndex( p );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setTransparentBackground --                                                 */
/* --------------------------------------------------------------------------- */

void PlotGroupStreamParameter::setTransparentBackground( bool transparent ) {
  m_transparentBg = transparent;
}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

size_t PlotGroupStreamParameter::getDimensionSize(int ndim) const {
  return SSIZE_MAX;
}

/* --------------------------------------------------------------------------- */
/* read --                                                                     */
/* --------------------------------------------------------------------------- */

bool PlotGroupStreamParameter::read( std::istream &is ){
  //
  std::cerr << "PlotGroupStreamParameter::read() " << m_name << std::endl;
  // FIX IT: does it make sense being able to read a plotgroup?
  abort();

  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool PlotGroupStreamParameter::write( std::ostream &os ){
#ifdef HAVE_QT
  BUG(BugStreamer, "PlotGroupStreamParameter::write(ostream &)" );
  HardCopyListener *hcl = 0;
#ifdef __MINGW32__
  const char * ext = "png";
#else
  const char * ext = AppData::Instance().disableFeatureSVG() ? "png" : "pdf";
#endif
  std::string tmpFileName;
  // muss im einem block sein, weil der File sonst am Ende der Funktion
  // gelöscht wird.
  {
    QTemporaryFile tmp_pdf(QString::fromStdString(compose("%1%2%3XXXXXX.%4", QDir::tempPath().toStdString(),QDir::separator().toLatin1(), m_name, ext)));
    if(tmp_pdf.open()){
      ///    tmp_pdf.setAutoRemove( true );
      tmpFileName = tmp_pdf.fileName().toStdString();
      tmp_pdf.close();
    }
  }
  /// ReportGen::Instance().newTmpFile( tmpFileName );
  std::string repstr("_"); //_.
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
  ///  ReportGen::Instance().newTmpFile( tmpFileName );

  hcl = GuiFactory::Instance()->createPrinterDialog()->getHardCopy( m_name );
  if( hcl == 0 ) {
    GuiElement* elem = GuiElement::findElement( m_name );
    if (!elem) {
      std::cerr << compose(_("no GuiElement named %1 found"), m_name) << std::flush;
      return false;
    }
    if (elem->getQtElement()) {
      elem->getQtElement()->writeFile(0, tmpFileName, m_transparentBg );  // background transparent
      os << tmpFileName;
      return true;
    }
    return false;
  }
  if( m_range != 0 ){
    double start, end;
    if( m_range->getRange( start, end ) )
      hcl -> setXRange( start, end );
  }
  if( !(hcl->write( tmpFileName ) ) )
    return false;
  os << tmpFileName;
  return true;
  #else
  return false; // TODO
  #endif
}

/* --------------------------------------------------------------------------- */
/* putValues --                                                                */
/* --------------------------------------------------------------------------- */

void PlotGroupStreamParameter::putValues( StreamDestination *dest, bool transposed ){
  if( dest != 0 )
    dest -> putValue(*this, m_name );
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */
void PlotGroupStreamParameter::marshal( std::ostream &os ){
  os << "<PlotGroupStreamParameter";
  os << " name=\"" << m_name << "\"";
  os << ">\n";
  if( m_range != 0 )
    m_range -> marshal( os );
  os << "</PlotGroupStreamParameter>\n";
}
void PlotGroupStreamParameter::Range::marshal( std::ostream &os ){
  os << "<Range";
  os << " start=\""    << m_start       << "\"";
  os << " end=\""      << m_end         << "\"";
  os << ">\n";
  if( m_startScale != 0 ){
    os << "<startScale";
    if( typeid(*m_startScale) == typeid(XferScale) )
      os << " XferScale=\"true\">\n";
    else
      os << ">\n";
    m_startScale -> marshal( os );
    os << "</startScale>\n";
  }
  if( m_endScale != 0 ){
    os << "<endScale";
    if( typeid(*m_endScale) == typeid(XferScale) )
      os << " XferScale=\"true\">\n";
    else
      os << ">\n";
    m_endScale -> marshal( os );
    os << "</endScale>\n";
  }
  if( m_xferStart != 0 ){
    os << "<xferStart>\n";
    m_xferStart -> marshal( os );
    os << "</xferStart>\n";
  }
  if( m_xferEnd != 0 ){
    os << "<xferEnd>\n";
    m_xferEnd -> marshal( os );
    os << "</xferEnd>\n";
  }
  os << "</Range>\n";
}
/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */
Serializable *PlotGroupStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "PlotGroupStreamParameter" ){
    m_name = attributeList["name"];
    return this;
  }
  else if( element == "Range" ){
    m_range = new Range();
    return m_range -> unmarshal( element, attributeList );
  }
  return this;
}
Serializable *PlotGroupStreamParameter::Range::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  if( element == "Range" ){
    std::string s;
    s=attributeList["start"];
    if( s.empty() ){
      m_start=0;
    }
    else {
      std::istringstream is( s );
      is >> m_start;
    }
    s=attributeList["end"];
    if( s.empty() ){
      m_end=0;
    }
    else {
      std::istringstream is( s );
      is >> m_end;
    }
    return this;
  }
  else if( element == "startScale" ){
    std::string s=attributeList["XferScale"];
    if( s.empty() ){
      m_startScale = new Scale();
    }
    else {
      m_startScale = new XferScale();
    }
    m_startScale -> unmarshal( element, attributeList );
    return m_startScale;
  }
  else if( element == "endScale" ){
    std::string s=attributeList["XferScale"];
    if( s.empty() ){
      m_endScale = new Scale();
    }
    else {
      m_endScale = new XferScale();
    }
    m_endScale -> unmarshal( element, attributeList );
    return m_endScale;
  }
  else if( element == "xferStart" ){
    m_xferStart = new XferDataItem();
    return m_xferStart -> unmarshal( element, attributeList );
  }
  else if( element == "xferEnd" ){
    m_xferEnd = new XferDataItem();
    return m_xferEnd -> unmarshal( element, attributeList );
  }
  return this;
}
/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */
void PlotGroupStreamParameter::setText( const std::string &text ){
}

void PlotGroupStreamParameter::Range::setText( const std::string &text ){
}


PlotGroupStreamParameter::Range::~Range(){
  delete m_xferStart;
  delete m_xferEnd;
}

bool PlotGroupStreamParameter::Range::getRange( double &start, double &end ){
  if( m_xferStart != 0 && m_xferEnd != 0 ){
    double s, e;
    m_xferStart->getValue( s );
    m_xferEnd->getValue( e );
    if( e > s){
      start = s * ( m_startScale != 0 ? m_startScale->getValue() : 1 );
      end = e * ( m_endScale != 0 ? m_endScale->getValue() : 1 );
      return true;
    }
    return false;
  }
  if( m_end > m_start ){
    start = m_start;
    end = m_end;
    return true;
  }
  return false;
}

PlotGroupStreamParameter::Range::Range( XferDataItem *start, XferDataItem *end
					, Scale *startScale, Scale *endScale ):
  m_xferStart(start)
  , m_xferEnd(end){
  m_startScale = startScale != 0 ? startScale->copy() : 0;
  m_endScale = endScale != 0 ? endScale->copy() : 0;
}
