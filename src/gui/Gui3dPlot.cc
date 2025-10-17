
#include "streamer/StreamManager.h"
#include "streamer/Stream.h"

#include "gui/GuiElement.h"
#include "gui/Gui3dPlot.h"
#include "gui/GuiPlotDataItem.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiFactory.h"
#include "utils/gettext.h"
#include "utils/Date.h"

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */

Gui3dPlot::Gui3dPlot()
  : m_width(400)
  , m_height(400)
  , m_isInitialSize(true)
  , m_isCloned( false )
  , m_pheaderText( 0 )
  , m_pheaderStream( 0 )
  , m_pfooterText( 0 )
  , m_pfooterStream( 0 )
{
}

Gui3dPlot::Gui3dPlot( const Gui3dPlot &plot)
  : m_plotStyle( plot.m_plotStyle )
  , m_width(plot.m_width)
  , m_height(plot.m_height)
  , m_isInitialSize(plot.m_isInitialSize)
  , m_isCloned( true )
  , m_pheaderText( plot.m_pheaderText )
  , m_pheaderStream( plot.m_pheaderStream )
  , m_pfooterText( plot.m_pfooterText )
  , m_pfooterStream( plot.m_pfooterStream )
{
}

Gui3dPlot::~Gui3dPlot() {
  if (m_isCloned) return;
  m_pheaderText.clear();
  m_pfooterText.clear();
}

/* --------------------------------------------------------------------------- */
/* setHeaderText --                                                            */
/* --------------------------------------------------------------------------- */

bool Gui3dPlot::setHeaderText( const std::string &headerText ) {
  Gui3dPlot::separateLines(m_pheaderText, headerText);
  return true;
}

/* --------------------------------------------------------------------------- */
/* setHeaderStream --                                                          */
/* --------------------------------------------------------------------------- */

bool Gui3dPlot::setHeaderStream( const std::string &headerStream ) {
  Stream *s = StreamManager::Instance().getStream( headerStream );
  if( s != 0 ) {
    m_pheaderStream = s;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setFooterText --                                                            */
/* --------------------------------------------------------------------------- */

bool Gui3dPlot::setFooterText( const std::string &footerText ) {
  Gui3dPlot::separateLines(m_pfooterText, footerText);
  return true;
}

/* --------------------------------------------------------------------------- */
/* setFooterStream --                                                          */
/* --------------------------------------------------------------------------- */

bool Gui3dPlot::setFooterStream( const std::string &footerStream ) {
  Stream *s = StreamManager::Instance().getStream( footerStream );
  if( s != 0 ) {
    m_pfooterStream = s;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* drawHeaderText --                                                           */
/* --------------------------------------------------------------------------- */

void Gui3dPlot::drawHeaderText() {
  if( m_pheaderStream != 0 ){
    Gui3dPlot::separateStreamLines(m_pheaderText, convertToString(m_pheaderStream));
  }
  if( m_pheaderText.size() != 0 ) {
    setTitle(m_pheaderText);
#if 0
    m_widget->setTitle( QString::fromStdString( m_pheaderText[0] ) );
#if HAVE_QWTPLOT3D
    if ( m_widget3d )
      get3dPlot()->setTitle( QString::fromStdString( m_pheaderText[0] ) );
#endif
#endif
    if ( m_pheaderText.size() > 1)
      GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + ": " +
                                                         _("3dPlot: Only single title line is supported.") );
  }
}

/* --------------------------------------------------------------------------- */
/* drawFooterText --                                                           */
/* --------------------------------------------------------------------------- */

void Gui3dPlot::drawFooterText() {
  if( m_pfooterStream != 0 ){
    Gui3dPlot::separateStreamLines(m_pfooterText, convertToString(m_pfooterStream));
  }
}

/* --------------------------------------------------------------------------- */
/* convertToString --                                                          */
/* --------------------------------------------------------------------------- */

std::string Gui3dPlot::convertToString( Stream *s ) {
  if ( s != 0 ) {
    std::ostringstream ostr;
    s->write( ostr );
    return ostr.str();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* separateLines --                                                            */
/*    allocates and fills an array of char-pointers which holds the text lines */
/* --------------------------------------------------------------------------- */

void Gui3dPlot::separateLines(std::vector<std::string>& result, const std::string &str) {
  std::string::size_type beg = 0;
  std::string::size_type end = 0;
  std::list<std::string> text;
  char** tmp;
  result.clear();


  // create list of text lines terminated by "\n"
  beg = str.find_first_not_of("\\");
  while( beg != std::string::npos ) {
    end = str.find_first_of("\\", beg);
    if( end != std::string::npos ) {
      if( str.at( end+1 ) == 'n' ) {
        result.push_back(str.substr(beg,end-beg));
      }
      beg = str.find_first_not_of("\\", end+2);
    }
    else {
      result.push_back(str.substr(beg,str.length()-beg));
      break;
    }
  }
#if 0
  //   cerr << "Size: " << text.size() << std::endl;
  tmp = new char*[text.size()+1];
  std::list<std::string>::iterator iter;
  int i=0;

  for( iter=text.begin(); iter != text.end(); ++iter, i++ ) {
    char* pzeile = new char[(*iter).size()+1];
    //     cerr << i << " COPY: " << (*iter).c_str() << " (SIZE: " << (*iter).size() << ")\n";
    strcpy( pzeile, (*iter).c_str() );
    tmp[i] = pzeile;
  }

  tmp[text.size()] = 0;
  return tmp;
#endif
}

/* --------------------------------------------------------------------------- */
/* separateStreamLines --                                                      */
/* --------------------------------------------------------------------------- */

void Gui3dPlot::separateStreamLines(std::vector<std::string>& result, const std::string &str ) {
  std::string::size_type beg = 0;
  std::string::size_type end = 0;
  std::list<std::string> text;
  char **tmp;
  result.clear();

  beg = str.find_first_not_of("\n");
  while( beg != std::string::npos ) {
    end = str.find_first_of("\n", beg);
    if( end != std::string::npos ) {
      result.push_back(str.substr(beg,end-beg));
      beg = str.find_first_not_of("\n", end+1);
    }
    else {
      result.push_back(str.substr(beg, str.length()-beg));
      break;
    }
  }

#if 0
  tmp = new char*[text.size()+1];
  std::list<std::string>::iterator iter;
  int i=0;

  for( iter=text.begin(); iter != text.end(); ++iter, i++ ) {
    char* pzeile = new char[(*iter).size()+1];
    strcpy( pzeile, (*iter).c_str() );
    tmp[i] = pzeile;
  }

  tmp[text.size()] = 0;
  return tmp;
#endif
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void Gui3dPlot::serializeXML(std::ostream &os, bool recursive){
  os << "<Gui3dPlot" << std::endl;
  os << " name=\""<<getName()<<"\">" << std::endl;
  os << "</Gui3dPlot>"<< std::endl;
}

/* --------------------------------------------------------------------------- */
/* getData --                                                                  */
/* --------------------------------------------------------------------------- */
#include <limits>
double Gui3dPlot::getZValue(int ix, int iy,
                            std::vector<double>& xvalues, std::vector<double>& yvalues) {
  GuiPlotDataItem *xaxis = getPlotDataItem("XAXIS");
  GuiPlotDataItem *yaxis = getPlotDataItem("YAXIS");
  GuiPlotDataItem *zaxis = getPlotDataItem("ZAXIS");
  XferDataItemIndex *ixrows = zaxis->getDataItemIndexWildcard( 1 );
  XferDataItemIndex *ixcols = zaxis->getDataItemIndexWildcard( 2 );

  BUG(BugGui, "GuiQwtContourPlotData::getZValue");
  double x, y;
  double z = std::numeric_limits<double>::quiet_NaN();

  // z-Wert ermitteln
  ixrows->setIndex( zaxis->Data(), ix );
  ixcols->setIndex( zaxis->Data(), iy );
  if (zaxis->getValue(z)) {
    z *= zaxis->getScaleFactor();
  } else
    z = std::numeric_limits<double>::max();

  // y-Wert ermitteln
  if( yaxis ) {
    if( yaxis->XferData() ) {
      XferDataItemIndex *dix = yaxis->getDataItemIndexWildcard( 1 );
      dix->setIndex( yaxis->Data(), iy );
      if (yaxis->getValue(y)) {
	y *= yaxis->getScaleFactor();
      } else
	y = std::numeric_limits<double>::max();
    }
  }

  // x-Wert ermitteln
  if( xaxis ) {
    if( xaxis->XferData() ) {
      XferDataItemIndex *dix = xaxis->getDataItemIndexWildcard( 1 );
      dix->setIndex( xaxis->Data(), ix );
      if (xaxis->getValue(x)) {
	x *= xaxis->getScaleFactor();
      } else
	x = std::numeric_limits<double>::max();
    }
  }

  // cache data
  if (xvalues[ix] == std::numeric_limits<double>::max() ) {
    xvalues[ix] = x;
  }
  if (yvalues[iy] == std::numeric_limits<double>::max() ) {
    yvalues[iy] = y;
  }
  return z;
}

void Gui3dPlot::getData(std::vector<double>& xvalues,
                        std::vector<double>& yvalues,
                        std::vector< std::vector<double> >&  zmatrix){
  GuiPlotDataItem *xaxis = getPlotDataItem("XAXIS");
  GuiPlotDataItem *yaxis = getPlotDataItem("YAXIS");
  GuiPlotDataItem *zaxis = getPlotDataItem("ZAXIS");
  XferDataItemIndex *ixrows = zaxis->getDataItemIndexWildcard( 1 );
  XferDataItemIndex *ixcols = zaxis->getDataItemIndexWildcard( 2 );

  // get Dimension
  int rows = ixrows->getDimensionSize( zaxis->Data() );
  int columns = 0;
  for( int i=0; i < rows; i++ ) {
    ixrows->setIndex( zaxis->Data(), i );
    int tmp = ixcols->getDimensionSize(zaxis->Data());
    if( tmp > columns ) columns = tmp;
  }

  // clear old data
  zmatrix.resize( rows );
  xvalues.clear();
  xvalues.insert( xvalues.begin(), rows, std::numeric_limits<double>::max());
  yvalues.clear();
  yvalues.insert( yvalues.begin(), columns, std::numeric_limits<double>::max());

  // get new z values
  for (long x = 0; x < rows; ++x) {
    zmatrix[x].resize(columns);
    for (long y = 0; y < columns; ++y) {
      zmatrix[x][y] = this->getZValue(x, y, xvalues, yvalues);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool Gui3dPlot::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  std::string label;
  std::vector< std::vector<double> > zmatrix;
  std::vector<double> xvalues, yvalues;
///TODO amg2020-10-12  if( !isDataItemUpdated(GuiManager::Instance().LastUpdate()))
///    return false;

  drawHeaderText(); // update headertextx
  getData(xvalues, yvalues, zmatrix);

  getElement()->writeJsonProperties(jsonObj);
  jsonObj["style"] = m_plotStyle.getStyle() == CONTOUR ? "Contour" :
    m_plotStyle.getStyle() == SURFACE ? "Surface" : "Bar";
  jsonObj["width"] = m_width;
  jsonObj["height"] = m_height;
  if (!m_pheaderText.empty())
    jsonObj["header_text"] = m_pheaderText[0];
  if (m_plotStyle.getStyle() == CONTOUR) {
    jsonObj["draw_contour"] = m_plotStyle.getFlagDrawContours();
    jsonObj["draw_interpolated"] = m_plotStyle.getFlagDrawZones();
  }

  // xaxis
  GuiPlotDataItem *xaxis = getPlotDataItem("XAXIS");
  if (xaxis) {
    Json::Value jsonElemAxis = Json::Value(Json::objectValue);
    xaxis->getLabelAndUnit( label );
    jsonElemAxis["label"] = label;
    jsonElemAxis["numwc"] = (int) xaxis->getNumOfWildcards();
    jsonElemAxis["numdims"] = (int) xaxis->getNumDimensions();
    jsonElemAxis["min_range"] = xaxis->getMinRange();
    jsonElemAxis["max_range"] = xaxis->getMaxRange();
    jsonElemAxis["scale"] = xaxis->getScaleFactor();
    jsonElemAxis["updated"] = xaxis->isDataItemUpdated(GuiManager::Instance().LastWebUpdate());
    Json::Value dataAry = Json::Value(Json::arrayValue);
    for (std::vector<double>::iterator it = xvalues.begin(); it != xvalues.end(); ++it) {
      dataAry.append((*it));
    }
    jsonElemAxis["data"] = dataAry;
    jsonObj["xaxis"] = jsonElemAxis;
  }

  // yaxis
  GuiPlotDataItem *yaxis = getPlotDataItem("YAXIS");
  if (yaxis) {
    Json::Value jsonElemAxis = Json::Value(Json::objectValue);
    yaxis->getLabelAndUnit( label );
    jsonElemAxis["label"] = label;
    jsonElemAxis["numwc"] = (int) yaxis->getNumOfWildcards();
    jsonElemAxis["numdims"] = (int) yaxis->getNumDimensions();
    jsonElemAxis["min_range"] = yaxis->getMinRange();
    jsonElemAxis["max_range"] = yaxis->getMaxRange();
    jsonElemAxis["scale"] = yaxis->getScaleFactor();
    jsonElemAxis["updated"] = yaxis->isDataItemUpdated(GuiManager::Instance().LastWebUpdate());
    Json::Value dataAry = Json::Value(Json::arrayValue);
    for (std::vector<double>::iterator it = yvalues.begin(); it != yvalues.end(); ++it) {
      dataAry.append((*it));
    }
    jsonElemAxis["data"] = dataAry;
    jsonObj["yaxis"] = jsonElemAxis;
  }

  // zaxis
  GuiPlotDataItem *zaxis = getPlotDataItem("ZAXIS");
  if (zaxis) {
    Json::Value jsonElemAxis = Json::Value(Json::objectValue);
    zaxis->getLabelAndUnit( label );
    jsonElemAxis["label"] = label;
    jsonElemAxis["numwc"] = (int) zaxis->getNumOfWildcards();
    jsonElemAxis["numdims"] = (int) zaxis->getNumDimensions();
    jsonElemAxis["min_range"] = zaxis->getMinRange();
    jsonElemAxis["max_range"] = zaxis->getMaxRange();
    jsonElemAxis["scale"] = zaxis->getScaleFactor();
    jsonElemAxis["updated"] = zaxis->isDataItemUpdated(GuiManager::Instance().LastWebUpdate());
    Json::Value matrixAry = Json::Value(Json::arrayValue);
    std::vector< std::vector<double> >::iterator itx;
    for(itx = zmatrix.begin(); itx != zmatrix.end(); ++itx) {
      Json::Value dataAry = Json::Value(Json::arrayValue);
      for (std::vector<double>::iterator it = (*itx).begin(); it != (*itx).end(); ++it) {
        dataAry.append((*it));
      }
      matrixAry.append(dataAry);
    }
    jsonElemAxis["matrix"] = matrixAry;
    jsonObj["zaxis"] = jsonElemAxis;
  }
  return true;
}
/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool Gui3dPlot::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  std::string label;
  std::vector< std::vector<double> > zmatrix;
  std::vector<double> xvalues, yvalues;
///TODO amg2020-10-12  if(!isDataItemUpdated(GuiManager::Instance().LastUpdate()))
///    return false;

  drawHeaderText(); // update headertextx
  getData(xvalues, yvalues, zmatrix);
  auto element = eles->add_plot_3ds();
  element->set_allocated_base(getElement()->writeProtobufProperties());

  element->set_style(m_plotStyle.getStyle() == CONTOUR ?
                      in_proto::Plot3D::Plot3DStyle::Plot3D_Plot3DStyle_Contour :
                      m_plotStyle.getStyle() == SURFACE ?
                      in_proto::Plot3D::Plot3DStyle::Plot3D_Plot3DStyle_Surface :
                      in_proto::Plot3D::Plot3DStyle::Plot3D_Plot3DStyle_Bar);
  element->mutable_base()->set_width(m_width);
  element->mutable_base()->set_height(m_height);
  if (!m_pheaderText.empty())
    element->set_header_text(m_pheaderText[0]);
  if (m_plotStyle.getStyle() == CONTOUR) {
    element->set_draw_contour(m_plotStyle.getFlagDrawContours());
    element->set_draw_interpolated(m_plotStyle.getFlagDrawZones());
  }

  // xaxis
  GuiPlotDataItem *xaxis = getPlotDataItem("XAXIS");
  if (xaxis) {
    auto axis = element->mutable_x_axis();
    xaxis->getLabelAndUnit(label );
    axis->set_label(label);
    axis->set_numwc(xaxis->getNumOfWildcards());
    axis->set_numdims(xaxis->getNumDimensions());
    axis->set_min_range(xaxis->getMinRange());
    axis->set_max_range(xaxis->getMaxRange());
    axis->set_scale(xaxis->getScaleFactor());
    Json::Value dataAry = Json::Value(Json::arrayValue);
    for (std::vector<double>::iterator it = xvalues.begin(); it != xvalues.end(); ++it) {
      dataAry.append((*it));
    }
    axis->mutable_data()->Assign(xvalues.begin(), xvalues.end());
  }

  // yaxis
  GuiPlotDataItem *yaxis = getPlotDataItem("YAXIS");
  if (yaxis) {
    auto axis = element->mutable_y_axis();
    yaxis->getLabelAndUnit(label );
    axis->set_label(label);
    axis->set_numwc((int) yaxis->getNumOfWildcards());
    axis->set_numdims((int) yaxis->getNumDimensions());
    axis->set_min_range(yaxis->getMinRange());
    axis->set_max_range(yaxis->getMaxRange());
    axis->set_scale(yaxis->getScaleFactor());
    Json::Value dataAry = Json::Value(Json::arrayValue);
    axis->mutable_data()->Assign(xvalues.begin(), xvalues.end());
  }

  // zaxis
  GuiPlotDataItem *zaxis = getPlotDataItem("ZAXIS");
  if (zaxis) {
    auto axis = element->mutable_z_axis();
    zaxis->getLabelAndUnit(label );
    axis->set_label(label);
    axis->set_numwc((int) zaxis->getNumOfWildcards());
    axis->set_numdims((int) zaxis->getNumDimensions());
    axis->set_min_range(zaxis->getMinRange());
    axis->set_max_range(zaxis->getMaxRange());
    axis->set_scale(zaxis->getScaleFactor());
    std::vector< std::vector<double> >::iterator itx;
    for(itx = zmatrix.begin(); itx != zmatrix.end(); ++itx) {
      auto line = axis->add_matrix();
      for (std::vector<double>::iterator it = (*itx).begin(); it != (*itx).end(); ++it) {
        line->add_a(*it);
      }
    }
  }
  return true;
}
#endif
