#if HAVE_PROTOBUF
#include <google/protobuf/repeated_ptr_field.h>
#endif
#ifdef HAVE_QT
#include <QRectF>
#endif
#include <limits>

#include "utils/Debugger.h"
#include "utils/StringUtils.h"
#include "utils/NumLim.h"
#include "streamer/StreamManager.h"
#include "streamer/Stream.h"
#include "xfer/XferDataItem.h"
#include "app/ColorSet.h"
#include "datapool/DataReference.h"
#include "datapool/DataVector.h"
#include "plot/ConfigDialog.h"
#include "plot/ScaleDialog.h"
#include "plot/CyclesDialog.h"
#include "gui/GuiPlotDataItem.h"
#include "gui/GuiForm.h"
#include "gui/GuiIndex.h"
#include "gui/Gui2dPlot.h"
#include "gui/Plot2dCurveAttributes.h"
#include "gui/UnitManager.h"


INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */

Gui2dPlot::Gui2dPlot( const std::string &name )
  : m_pheaderText(0)
  , m_pheaderStream(0)
  , m_pfooterText(0)
  , m_pfooterStream(0)
  , m_name( name )
  , m_function( 0 )
  , m_drefStruct( 0 )
  , m_configDialog( 0 )
  , m_scaleDialog( 0 )
  , m_cyclesDialog( 0 )
  , m_initial_size( false )
  , m_barStyleOption_plotItemGrouped( false )
  , m_initial_height( 300 )
  , m_initial_width( 600 )
  , m_colorset( 0 )
  , m_uiMode( 0 )
  , m_overrideSymbolSize(0)
  , m_aspectRatioMode(false)
  , m_aspectRatioReferenceAxis(XAXIS)
  , m_isCloned( false )
  , m_showMINMAX(true)
  , m_showAVG(true)
  , m_showRMS(true)
  , m_precMINMAX(2)
  , m_precAVG(2)
  , m_precRMS(2)
  , m_allCycles( false )
  , m_maxCurveLen( 0 )
{
  for( int axis = 0; axis < 2; ++axis ){
    m_style[axis] = PLOT;
  }
  for( int axis = 0; axis < AXIS_TYPE_COUNT; ++axis ){
    m_titleStream[axis] = 0;
  }
  if (m_name.size()) // with popupMenu
    createLocalData();

  DataReference *ref = DataPoolIntens::Instance().getDataReference( "PLOT2D_UIMODE" );
  if (ref) {
    m_uiMode = new XferDataItem(ref);
  }
  ref = DataPoolIntens::Instance().getDataReference( "PLOT2D_SYMBOLSIZE" );
  if (ref) {
    m_overrideSymbolSize = new XferDataItem(ref);
  }

  // headless mode, minimize data
  double maxCurveLenFac = 0;
  if (AppData::Instance().HeadlessWebMode()) {
    const char *a = getenv("PLOT2D_MAX_CURVE_LENGTH");
    if(a){
      m_maxCurveLen = atoi(a);
    }
    if (m_maxCurveLen == 0) { m_maxCurveLen = 5000; }
  }
}

Gui2dPlot::Gui2dPlot( const Gui2dPlot& plot)
  : //m_plotItems(plot.m_plotItems)
  m_dummyItems() //plot.m_dummyItems)

  , m_colorset( 0 )
  , m_colorsetName( plot.m_colorsetName )
  , m_name( plot.m_name )
  , m_function( plot.m_function )
  , m_drefStruct( plot.m_drefStruct )
  , m_configDialog( 0 ) //plot.m_configDialog )
  , m_scaleDialog( plot.m_scaleDialog )
  , m_cyclesDialog( plot.m_cyclesDialog )

  , m_pheaderText(copyCharArray(plot.m_pheaderText))
  , m_pheaderStream(plot.m_pheaderStream)
  , m_pfooterText(copyCharArray(plot.m_pfooterText))
  , m_pfooterStream(plot.m_pfooterStream)
  , m_menuText( plot.m_menuText )


  , m_initial_height( plot.m_initial_height )
  , m_initial_width( plot.m_initial_width )
  , m_initial_size( plot.m_initial_size )
  , m_barStyleOption_plotItemGrouped( plot.m_barStyleOption_plotItemGrouped )
  , m_isCloned( true )
  , m_axisHiddenSet( plot.m_axisHiddenSet )
  , m_aspectRatioMode(plot.m_aspectRatioMode)
  , m_aspectRatioReferenceAxis(plot.m_aspectRatioReferenceAxis)
  , m_allCycles(plot.m_allCycles)
  , m_maxCurveLen(plot.m_maxCurveLen)
  , m_showMINMAX(plot.m_showMINMAX)
  , m_showAVG(plot.m_showAVG)
  , m_showRMS(plot.m_showRMS)
  , m_precMINMAX(plot.m_precMINMAX)
  , m_precAVG(plot.m_precAVG)
  , m_precRMS(plot.m_precRMS)
{
  for( int axis = 0; axis < 2; ++axis ){
    m_style[axis] = plot.m_style[axis];
  }
  for( int axis = 0; axis < AXIS_TYPE_COUNT; ++axis ){
    m_titleStream[axis] = plot.m_titleStream[axis];
    m_title[axis] = plot.m_title[axis];
    m_axis[axis] = plot.m_axis[axis];
  }

  // initialize m_plotitems
  if (m_plotItems.size() == 0) {
    tPlotItemVector::const_iterator it;
    for( it = plot.m_plotItems.begin(); it != plot.m_plotItems.end(); ++it ){
      m_plotItems.push_back( new PlotItem( *(*it)) );
    }
  }

  // initialize m_dummyItems
  if (m_dummyItems.size() == 0) {
    tPlotItemMap::const_iterator it;
    for( it = plot.m_dummyItems.begin(); it != plot.m_dummyItems.end(); ++it ){
      m_dummyItems[it->first] =  new PlotItem( *(it->second) );
    }
  }

  // colorset
  if (plot.m_colorsetName.size()) {
    m_colorset = DataPoolIntens::Instance().getColorSet(plot.m_colorsetName);
  }

  // uiMode
  if (plot.m_uiMode)
    m_uiMode = new XferDataItem( *(plot.m_uiMode) );

  // overrideSymbolSize
  if (plot.m_overrideSymbolSize)
    m_overrideSymbolSize = new XferDataItem(*(plot.m_overrideSymbolSize));
}

Gui2dPlot::~Gui2dPlot(){
  if (m_isCloned)  // cloned object =>do nothing
    return;
  if( m_pheaderText != 0 ){
    deleteMultiLineString( m_pheaderText );
  }
  if( m_pheaderStream != 0 ){
    StreamManager::Instance().deleteStream( m_pheaderStream );
  }
  if( m_pfooterText != 0 ) {
    deleteMultiLineString( m_pfooterText );
  }
  if( m_pfooterStream != 0 ){
    StreamManager::Instance().deleteStream( m_pfooterStream );
  }
  for( int y = 0; y < AXIS_TYPE_COUNT; ++y ){
    if( m_titleStream[y] != 0 && !m_isCloned)
      StreamManager::Instance().deleteStream( m_titleStream[y] );
  }
  delete m_configDialog;
  delete m_scaleDialog;
  delete m_cyclesDialog;
  delete m_drefStruct;
  delete m_uiMode;
  delete m_overrideSymbolSize;
}

/* --------------------------------------------------------------------------- */
/* deleteMultiLineString --                                                    */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::deleteMultiLineString( char **multiline ){
  assert( multiline != 0 );
  for( size_t i = 0; multiline[i] != 0; i++ ){
    delete [] multiline[i];
  }
  delete [] multiline;
}

/* --------------------------------------------------------------------------- */
/* drawHeaderText --                                                           */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::drawHeaderText() {
  if( m_pheaderStream != 0 ){
    if( m_pheaderText != 0 ) {
      deleteMultiLineString( m_pheaderText );
    }
    m_pheaderText = separateStreamLines( convertToString(m_pheaderStream) );
  }
}

/* --------------------------------------------------------------------------- */
/* drawFooterText --                                                           */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::drawFooterText() {
  if( m_pfooterStream != 0 ){
    if( m_pfooterText != 0 ) {
      deleteMultiLineString( m_pfooterText );
    }
    m_pfooterText = separateStreamLines( convertToString(m_pfooterStream) );
  }
}

/* --------------------------------------------------------------------------- */
/* setHeaderText --                                                            */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::setHeaderText( const std::string &headerText ) {
  if( m_pheaderText != 0 ){
    deleteMultiLineString( m_pheaderText );
  }
  m_pheaderText = separateLines( headerText );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setHeaderStream --                                                          */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::setHeaderStream( const std::string &headerStream ) {
  Stream *s = StreamManager::Instance().getStream( headerStream );
  if( s != 0 ) {
    m_pheaderStream = s;
    return true;
  }
  return false;
}
/* --------------------------------------------------------------------------- */
/* setLabelStream --                                                           */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setLabelStream( const std::string &stream, eAxisType axis ) {
  Stream *s = StreamManager::Instance().getStream( stream );
  if( s != 0 ) {
    m_titleStream[axis] = s;
  }
}

void Gui2dPlot::setLabel( const std::string &title, eAxisType axis ){
  m_title[axis] = title;
}
/* --------------------------------------------------------------------------- */
/* setFooterText --                                                            */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::setFooterText( const std::string &footerText ) {
  if( m_pfooterText != 0 ){
    deleteMultiLineString( m_pfooterText );
  }
  m_pfooterText = separateLines( footerText );
  return true;
}

/* --------------------------------------------------------------------------- */
/* setFooterStream --                                                          */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::setFooterStream( const std::string &footerStream ) {
  Stream *s = StreamManager::Instance().getStream( footerStream );
  if( s != 0 ) {
    m_pfooterStream = s;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setMenuText --                                                              */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::setMenuText( const std::string &menuText ) {
  m_menuText = menuText;
  return true;
}

/* --------------------------------------------------------------------------- */
/* separateLines --                                                            */
/* --------------------------------------------------------------------------- */
char** Gui2dPlot::separateLines( const std::string &str ) {
  std::string::size_type beg = 0;
  std::string::size_type end = 0;
  std::list<std::string> text;
  char** tmp;

  beg = str.find_first_not_of("\\");
  while( beg != std::string::npos ) {
    end = str.find_first_of("\\", beg);
    if( end != std::string::npos ) {
      if( str.at( end+1 ) == 'n' ) {
	text.push_back(str.substr(beg, end-beg));
      }
      beg = str.find_first_not_of("\\", end+2);
    }
    else {
      text.push_back(str.substr(beg));
      break;
    }
  }

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
}

/* --------------------------------------------------------------------------- */
/* separateStreamLines --                                                      */
/* --------------------------------------------------------------------------- */
char** Gui2dPlot::separateStreamLines( const std::string &str ) {
  std::string::size_type beg = 0;
  std::string::size_type end = 0;
  std::list<std::string> text;
  char **tmp;

  beg = str.find_first_not_of("\n");
  while( beg != std::string::npos ) {
    end = str.find_first_of("\n", beg);
    if( end != std::string::npos ) {
	text.push_back(str.substr(beg,end-beg));
      beg = str.find_first_not_of("\n", end+1);
    }
    else {
      text.push_back(str.substr(beg));
      break;
    }
  }

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
}

/* --------------------------------------------------------------------------- */
/* convertToString --                                                          */
/* --------------------------------------------------------------------------- */
std::string Gui2dPlot::convertToString( Stream *s ) {
  if ( s != 0 ) {
    std::ostringstream ostr;
    s->write( ostr );
    return ostr.str();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* copyCharArray --                                                            */
/* --------------------------------------------------------------------------- */
char** Gui2dPlot::copyCharArray( char** text ) {
  char **res = 0;
  if (text) {
    int size = 0;
    while(text[size] != 0) size++;
    if (!size) return res;
    res = new char*[size+1];
    res[size] = 0;
    for (int i=0; i < size; ++i) {
      char* pzeile = new char[strlen(text[i])+1];
      strcpy( pzeile, text[i] );
      res[i] = pzeile;
    }
  }
  return res;
}

/* --------------------------------------------------------------------------- */
/* MenuLabel --                                                                */
/* --------------------------------------------------------------------------- */
const std::string &Gui2dPlot::MenuLabel() {
  return m_menuText.empty() ? m_name : m_menuText;
}

/* --------------------------------------------------------------------------- */
/* PlotItemLabel --                                                            */
/* --------------------------------------------------------------------------- */
const std::string Gui2dPlot::PlotItemLabel(GuiPlotDataItem* yItem) {
  // get label
  std::string label, name;
  yItem->getLabelAndUnit( label );
  if(  getCyclesDialog() && ( m_plots.size() > 1 ) ){
    dpi().getCycleName(  dpi().currentCycle(), name );
    std::ostringstream os;
    if (name.size() )
      os << name << ": " << label;
    else
      os << "#" << dpi().currentCycle() + 1<< ": " << label;
    label = os.str();
  }
  return label;
}

/* --------------------------------------------------------------------------- */
/* addDataItem --                                                              */
/* --------------------------------------------------------------------------- */
Gui2dPlot::PlotItem *Gui2dPlot::addDataItem( GuiPlotDataItem *dataitem, PlotItem *xPlotItem ) {
  BUG(BugGui, "Gui2dPlot::addDataItem");
  PlotItem *item = new PlotItem( dataitem );
  item->setXPlotItem( xPlotItem );
  if( dataitem != 0 ) {
    BUG_MSG("Name = '" << dataitem->getName() << "'");
    m_plotItems.push_back( item );

    if (m_name.size()) {// with popupMenu
      int idx = m_plotItems.size();
      configureMenu( item, idx );
    }
  }
  else{
    BUG_MSG("Dataitem is null");
    if (m_dummyItems.find(m_plotItems.size()) == m_dummyItems.end())
      m_dummyItems[m_plotItems.size()] = item;
    item->setAxisType( XAXIS );
  }
  return item;
}

/* --------------------------------------------------------------------------- */
/* setFunction --                                                              */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setFunction( JobFunction *func ) {
  assert( func != 0 );
  func->setUsed();
  m_function = func;
}

/* --------------------------------------------------------------------------- */
/* setColorsetName --                                                          */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setColorsetName( const std::string &name ) {
  m_colorsetName = name;
  m_colorset = DataPoolIntens::Instance().getColorSet(name);
}

//**********************************//
//PlotItem functions                //
//**********************************//

/* --------------------------------------------------------------------------- */
/* Constructor/Destructor --                                                   */
/* --------------------------------------------------------------------------- */
Gui2dPlot::PlotItem::PlotItem( GuiPlotDataItem *item )
  : m_plotDataItem( item )
  , m_xPlotItem( 0 )
  , m_xferXAxisType( 0 )
  , m_xferY1AxisType( 0 )
  , m_xferY2AxisType( 0 )
  , m_xferLineColor( 0 )
  , m_xferSymbolColor( 0 )
  , m_xferLineStyle( 0 )
  , m_xferSymbolStyle( 0 )
  , m_xferSymbolSize( 0 )
  , m_xferUnit( 0 )
  , m_axisType( HIDDEN )
  , m_axisWildcardIndex( -1 )
  , m_marker( 0 )
  , m_isMarker( false )
  , m_colorset( 0 )
  , m_legendHide( DEFAULT ){
}

Gui2dPlot::PlotItem::PlotItem( const PlotItem& plotitem )
  : m_plotDataItem( plotitem.plotDataItem() ? new GuiPlotDataItem( * (plotitem.plotDataItem()) ) : 0)
  , m_xPlotItem( 0 )
  , m_xferXAxisType( plotitem.m_xferXAxisType ? new XferDataItem( *(plotitem.m_xferXAxisType))   : 0)
  , m_xferY1AxisType( plotitem.m_xferY1AxisType ? new XferDataItem( *(plotitem.m_xferY1AxisType)) : 0)
  , m_xferY2AxisType( plotitem.m_xferY2AxisType ?new XferDataItem( *(plotitem.m_xferY2AxisType)) : 0)
  , m_xferLineColor( plotitem.m_xferLineColor ? new XferDataItem( *(plotitem.m_xferLineColor)) : 0)
  , m_xferSymbolColor( plotitem.m_xferSymbolColor ? new XferDataItem( *(plotitem.m_xferSymbolColor)) : 0)
  , m_xferLineStyle( plotitem.m_xferLineStyle ? new XferDataItem( *(plotitem.m_xferLineStyle)) : 0)
  , m_xferSymbolStyle( plotitem.m_xferSymbolStyle ? new XferDataItem( *(plotitem.m_xferSymbolStyle)) : 0)
  , m_xferSymbolSize( plotitem.m_xferSymbolSize ? new XferDataItem( *(plotitem.m_xferSymbolSize)) : 0)
  , m_xferUnit( plotitem.m_xferUnit ? new XferDataItem( *(plotitem.m_xferUnit)) : 0)
  , m_axisType( plotitem.m_axisType )
  , m_axisWildcardIndex( plotitem.m_axisWildcardIndex )
  , m_colorset( plotitem.m_colorset )
  , m_marker( 0 )
  , m_isMarker( plotitem.m_isMarker )
  , m_colorsetName( plotitem.m_colorsetName )
  , m_legendHide( plotitem.m_legendHide ){
  if (plotitem.m_xPlotItem)
    m_xPlotItem = new PlotItem( (plotitem.xPlotDataItem()) );
  if (plotitem.m_marker)
    m_marker = new XferDataItem( *(plotitem.m_marker) );
  // colorset
  if (plotitem.m_colorsetName.size()) {
    m_colorset = DataPoolIntens::Instance().getColorSet(plotitem.m_colorsetName);
  }
}

Gui2dPlot::PlotItem::~PlotItem(){
  delete m_plotDataItem;
  delete m_xferXAxisType;
  delete m_xferY1AxisType;
  delete m_xferY2AxisType;
  delete m_xferLineColor;
  delete m_xferSymbolColor;
  delete m_xferLineStyle;
  delete m_xferSymbolStyle;
  delete m_xferSymbolSize;
  delete m_xferUnit;
  delete m_marker;
}

/* --------------------------------------------------------------------------- */
/* setColorsetName --                                                          */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setColorsetName(const std::string &name) {
  m_colorsetName = name;
  m_colorset = DataPoolIntens::Instance().getColorSet(name);
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::PlotItem::isDataItemUpdated( TransactionNumber trans ) {
  if( m_plotDataItem ){
    if( m_plotDataItem->isDataItemUpdated(trans) ){
      return true;
    }
  }
  if( m_isMarker && m_marker ){
    if( m_marker->isDataItemUpdated( trans ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isXAxisDefined --                                                           */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::PlotItem::isXAxisDefined(){
  if( m_xPlotItem != 0 )
    if( m_xPlotItem->plotDataItem() != 0 )
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/*  getNPoints --                                                              */
/* --------------------------------------------------------------------------- */
int Gui2dPlot::PlotItem::getNPoints(){
  int pointsCount = 0;
  if( m_plotDataItem && m_plotDataItem->XferData() != 0 ) {
    XferDataItemIndex *index = m_plotDataItem->getDataItemIndexWildcard( getDataPlotItemIndex() );
    pointsCount = index->getDimensionSize( m_plotDataItem->Data() );
    if( pointsCount == 1 ){
      index->setIndex( m_plotDataItem->Data(), 0 );
      if( !m_plotDataItem->isValid() )
        pointsCount = 0;
    }
  }
  return pointsCount;
}

/* --------------------------------------------------------------------------- */
/*  getPlotCurve --                                                            */
/* --------------------------------------------------------------------------- */
void* Gui2dPlot::PlotItem::getPlotCurve(int cycle, int xIndex, int yIndex){
  if (m_plotCurve.size() > cycle)
    if ( m_plotCurve[cycle].size() > xIndex)
      if ( m_plotCurve[cycle][xIndex].size() > yIndex)
        return m_plotCurve[cycle][xIndex][yIndex];
  return 0;
}

/* --------------------------------------------------------------------------- */
/*  setPlotCurve --                                                            */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setPlotCurve(int cycle, int xIndex, int yIndex, void* pc) {

  // PlotItem cycle data
  while(m_plotCurve.size() <= cycle) {
    m_plotCurve.push_back(QwtPlotItemMatrix());
  }

  // PlotItem x vector data
  while (m_plotCurve[cycle].size() <= xIndex) {
    m_plotCurve[cycle].push_back(QwtPlotItemVector());
  }

  // PlotItem y vector data
  while (m_plotCurve[cycle][xIndex].size() <= yIndex) {
    m_plotCurve[cycle][xIndex].push_back(0);
  }

  m_plotCurve[cycle][xIndex][yIndex] = pc;
}
/* --------------------------------------------------------------------------- */
/*  -- setMarker                                                               */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setMarker( XferDataItem *xfer ){
  m_marker = xfer;
  int num = m_marker->getNumOfWildcards();
  assert( num < 2 );
  if( num == 0 ){
    XferDataItemIndex *index = m_marker->newDataItemIndex();
    index->setWildcard();
  }
  m_marker->setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/*  --  xPlotDataItem                                                          */
/* --------------------------------------------------------------------------- */
GuiPlotDataItem *Gui2dPlot::PlotItem::xPlotDataItem() const {
  if( m_xPlotItem != 0 )
    return m_xPlotItem->plotDataItem();
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getAllXfers --                                                              */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::getAllXfers(XferDataItem *&xXfer, XferDataItem *&y1Xfer,
                                      XferDataItem *&y2Xfer, XferDataItem *&colorXfer,
                                      XferDataItem *&symbolColorXfer,
                                      XferDataItem *&lineStyleXfer, XferDataItem *&symbolStyleXfer,
                                      XferDataItem *&symbolSizeXfer, XferDataItem *&unitXfer) {
  xXfer = m_xferXAxisType;
  y1Xfer = m_xferY1AxisType;
  y2Xfer = m_xferY2AxisType;
  colorXfer = m_xferLineColor;
  symbolColorXfer = m_xferSymbolColor;
  lineStyleXfer = m_xferLineStyle;
  symbolStyleXfer = m_xferSymbolStyle;
  symbolSizeXfer = m_xferSymbolSize;
  unitXfer = m_xferUnit;
}

/* --------------------------------------------------------------------------- */
/* updateAxisTypes --                                                          */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::updateAxisTypes( bool plot2d, PlotItem *xPlotItem ){
  double value = 0;
  eAxisType type;
  type = xPlotItem ? m_axisType : HIDDEN;
  // Diese Abfrage ist nötig, weil sonst bei einem Cycle-Switch vor dem ersten
  // ploten alle Axis Types auf HIDDEN gesetzt werden
  if(  m_xferXAxisType && !( m_xferXAxisType->getValue( value )) ){
    value = m_y1ResetValue;
    m_xferY1AxisType->setValue( value );
    value = m_y2ResetValue;
    m_xferY2AxisType->setValue( value );
    value = m_xResetValue;
    m_xferXAxisType->setValue( value );
  }
  if( plot2d )
    m_xPlotItem = xPlotItem;
  else if( value ==1 )
     type = XAXIS;
  if (m_xferY1AxisType)
    m_xferY1AxisType->getValue( value );
  if( value == 1 )
    type = Y1AXIS;
  if (m_xferY2AxisType)
    m_xferY2AxisType->getValue( value );
  if( value == 1 )
    type = Y2AXIS;

  m_axisType = type;
}

/* --------------------------------------------------------------------------- */
/* setAllXfers --                                                              */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setAllXfers(XferDataItem *xXfer
                                      , XferDataItem *y1Xfer
                                      , XferDataItem *y2Xfer
                                      , XferDataItem *colorXfer
                                      , XferDataItem *symbolColorXfer
                                      , XferDataItem *lineStyleXfer
                                      , XferDataItem *symbolStyleXfer
                                      , XferDataItem *symbolSizeXfer
                                      , XferDataItem *unitXfer) {
  m_xferXAxisType = xXfer;
  m_xferY1AxisType = y1Xfer;
  m_xferY2AxisType = y2Xfer;
  m_xferLineColor = colorXfer;
  m_xferSymbolColor = symbolColorXfer;
  m_xferLineStyle = lineStyleXfer;
  m_xferSymbolStyle = symbolStyleXfer;
  m_xferSymbolSize = symbolSizeXfer;
  m_xferUnit = unitXfer;
}

/* --------------------------------------------------------------------------- */
/* setLineColorXfer --                                                         */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setLineColorXfer(XferDataItem *colorXfer ){
  m_xferLineColor = colorXfer;
}

/* --------------------------------------------------------------------------- */
/* setSymbolColorXfer --                                                       */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setSymbolColorXfer(XferDataItem *symbolColorXfer ){
  m_xferSymbolColor = symbolColorXfer;
}

/* --------------------------------------------------------------------------- */
/* setLineStyleXfer -  -                                                       */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setLineStyleXfer(XferDataItem *lineStyleXfer ){
  m_xferLineStyle = lineStyleXfer;
}

/* --------------------------------------------------------------------------- */
/* setSymbolStyleXfer --                                                       */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setSymbolStyleXfer(XferDataItem *symbolStyleXfer ){
  m_xferSymbolStyle = symbolStyleXfer;
}

/* --------------------------------------------------------------------------- */
/* setSymbolSizeXfer --                                                       */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setSymbolSizeXfer(XferDataItem *symbolSizeXfer ){
  m_xferSymbolSize = symbolSizeXfer;
}

/* --------------------------------------------------------------------------- */
/* setUnitXfer --                                                              */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setUnitXfer(XferDataItem *unitXfer ){
  m_xferUnit = unitXfer;
}

/* --------------------------------------------------------------------------- */
/* getLineColorXfer --                                                         */
/* --------------------------------------------------------------------------- */
XferDataItem* Gui2dPlot::PlotItem::getLineColorXfer() {
  return m_xferLineColor;
}

/* --------------------------------------------------------------------------- */
/* PlotItem::initAxisTypeValues --                                             */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::initAxisTypeValues( bool plot2d, int posOfX ){
  if( m_xferXAxisType != 0 &&
      m_xferY1AxisType != 0 &&
      m_xferY2AxisType != 0 ){
    switch( m_axisType ){
    case XAXIS  :
      m_xferXAxisType->setValue( plot2d ? posOfX : 1 );
      m_xferY1AxisType->setValue( 0 );
      m_xferY2AxisType->setValue( 0 );
      break;
    case Y1AXIS  :
      m_xferXAxisType->setValue( plot2d ? posOfX : 0 );
      m_xferY1AxisType->setValue( 1 );
      m_xferY2AxisType->setValue( 0 );
      break;
    case Y2AXIS  :
      m_xferXAxisType->setValue( plot2d ? posOfX : 0 );
      m_xferY1AxisType->setValue( 0 );
      m_xferY2AxisType->setValue( 1 );
      break;
    case HIDDEN  :
    default      :
      m_xferXAxisType->setValue( plot2d ? posOfX : 0 );
      m_xferY1AxisType->setValue( 0 );
      m_xferY2AxisType->setValue( 0 );
      break;
    }
    int value;
    m_xferXAxisType->getValue( value );
    m_xResetValue = value;
    m_xferY1AxisType->getValue( value );
    m_y1ResetValue = value;
    m_xferY2AxisType->getValue( value );
    m_y2ResetValue = value;
  }
}

/* --------------------------------------------------------------------------- */
/* PlotItem::resetAxisTypeValues --                                            */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::resetAxisTypeValues(){
  m_xferXAxisType->setValue( m_xResetValue );
  m_xferY1AxisType->setValue( m_y1ResetValue );
  m_xferY2AxisType->setValue( m_y2ResetValue );
}

/* --------------------------------------------------------------------------- */
/* PlotItem::getXPos --                                                        */
/* --------------------------------------------------------------------------- */
int Gui2dPlot::PlotItem::getXPos(){
  int value = 0;
  if( m_xferXAxisType != 0 )
    m_xferXAxisType->getValue( value );
  return value;
}

/* --------------------------------------------------------------------------- */
/* PlotItem::setAxisWildcardIndex --                                           */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setAxisWildcardIndex(int wildcardIdx) {
  assert(wildcardIdx);
  m_axisWildcardIndex = wildcardIdx;
}

/* --------------------------------------------------------------------------- */
/* PlotItem::hasAxisWildcardIndex --                                           */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::PlotItem::hasAxisWildcardIndex() {
  return m_axisWildcardIndex != -1;
}

/* --------------------------------------------------------------------------- */
/* PlotItem::getAxisWildcardIndexDimension --                                  */
/* --------------------------------------------------------------------------- */
int Gui2dPlot::PlotItem::getAxisWildcardIndexDimension() {
  if ( m_axisWildcardIndex < 0 )
    return 0;
  XferDataItemIndex* index =  m_plotDataItem->getDataItemIndexWildcard( m_axisWildcardIndex );
  if (!index)
    return 0;
  return index->getDimensionSize( plotDataItem()->Data() );
}

/* --------------------------------------------------------------------------- */
/* PlotItem::setAxisWildcardIndexValue --                                      */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setAxisWildcardIndexValue(int idxValue) {
  XferDataItemIndex* index =  m_plotDataItem->getDataItemIndexWildcard(m_axisWildcardIndex);
  if ( !index )
    return;
  index->setIndex(plotDataItem()->Data(), idxValue);

  // label title
  m_plotDataItem->setValueLabelXferIndex( idxValue );
}

/* --------------------------------------------------------------------------- */
/* PlotItem::getAxisWildcardIndexValue --                                      */
/* --------------------------------------------------------------------------- */
int Gui2dPlot::PlotItem::getAxisWildcardIndexValue() {
  XferDataItemIndex* index =  m_plotDataItem->getDataItemIndexWildcard( m_axisWildcardIndex );
  if ( !index )
    return 0; //-1;
  return index->getValue();
}

/* --------------------------------------------------------------------------- */
/* PlotItem::getDataPlotItemIndex --                                           */
/* --------------------------------------------------------------------------- */
int Gui2dPlot::PlotItem::getDataPlotItemIndex() {
  return m_axisWildcardIndex == 1 ? 2 : 1;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::PlotItem::acceptIndex( const std::string &name, int inx) {
  if (!plotDataItem()->acceptIndex( name, inx ))
    return false;
  if (m_marker && !m_marker->acceptIndex( name, inx ))
    return false;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::PlotItem::setIndex( const std::string &name, int inx ) {
  plotDataItem()->setIndex( name, inx );
  if(m_marker != 0)
    m_marker->setIndex( name, inx );
}


//---------------------------------------------------------------------------
// serializeXML
//---------------------------------------------------------------------------

void Gui2dPlot::PlotItem::serializeXML(std::ostream &os, const std::string &label, const std::string &axis,
				       tPointVector& pts, tMarkerDataVector& markerLabels) {
  os << "<intens:PlotItem";
  if (m_plotDataItem) {
    os << " npoints=\""<< getNPoints() << "\"";
    os << " label=\""<< label << "\"";
    os << " axis=\""<< axis << "\"";
    os << " min_range=\""<< m_plotDataItem->getMinRange() << "\"";
    os << " max_range=\""<< m_plotDataItem->getMaxRange() << "\"";
  }
  os << " marker=\""<<isMarker() << "\""
     << " legend=\""<<!isLegendHide() << "\"";
  os << ">"<< std::endl;

  // data points
  if (m_plotDataItem) {
    os << "<x>";
    for (tPointVector::iterator it=pts.begin();
         it!=pts.end(); ++it) {
      os << (*it).x << " ";
    }
    os << "</x>";
    os << "<y>";
    for (tPointVector::iterator it=pts.begin();
         it!=pts.end(); ++it) {
      os << (*it).y << " ";
    }
    os << "</y>";
  }

  os << "</intens:PlotItem>"<< std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::PlotItem::serializeJson(Json::Value& jsonObj, const std::string &label, const std::string &axis,
				       tPointVector& pts, tMarkerDataVector& markerLabels) {
  double xmin(NumLim::MAX_DOUBLE), ymin(NumLim::MAX_DOUBLE);
  double xmax(NumLim::MIN_DOUBLE), ymax(NumLim::MIN_DOUBLE);

  jsonObj["npoints"] = getNPoints();
  jsonObj["label"] = label;
  jsonObj["axis"] = axis;
  jsonObj["is_marker"] = isMarker();
  jsonObj["legend"] = !isLegendHide();
  jsonObj["min_range"] = m_plotDataItem->getMinRange();
  jsonObj["max_range"] = m_plotDataItem->getMaxRange();
  Json::Value xjsonAry = Json::Value(Json::arrayValue);
  Json::Value yjsonAry = Json::Value(Json::arrayValue);
  for (tPointVector::iterator it=pts.begin();
       it!=pts.end(); ++it) {
    xjsonAry.append(std::isnan((*it).x) ? Json::Value(Json::nullValue) : (*it).x);
    xmin = std::min(xmin, (*it).x);
    xmax = std::max(xmax, (*it).x);

    yjsonAry.append(std::isnan((*it).y) ? Json::Value(Json::nullValue) : (*it).y);
    ymin = std::min(ymin, (*it).y);
    ymax = std::max(ymax, (*it).y);
  }
  if (ymax == NumLim::MIN_DOUBLE) {
    return false;
  }
  if (isMarker()) {
    Json::Value mjsonAry = Json::Value(Json::arrayValue);
    for (tMarkerDataVector::iterator it=markerLabels.begin();
         it!=markerLabels.end(); ++it) {
      Json::Value jsonObj = Json::Value(Json::objectValue);
      jsonObj["label"] = it->label;
      jsonObj["background"] = it->background;
      jsonObj["foreground"] = it->foreground;
      mjsonAry.append(jsonObj);
    }
    jsonObj["marker"] = mjsonAry;
  } else {
    jsonObj["marker"] = Json::Value(Json::arrayValue);
  }
  jsonObj["x"] = xjsonAry;;
  jsonObj["y"] = yjsonAry;

  // min, max
  jsonObj["min_x"] = xmin;
  jsonObj["max_x"] = xmax;
  jsonObj["min_y"] = ymin;
  jsonObj["max_y"] = ymax;

  BUG_DEBUG("PlotItem::serializeJson Return: " << ch_semafor_intens::JsonUtils::value2string(jsonObj));
  return true;
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool Gui2dPlot::PlotItem::serializeProtobuf(in_proto::Plot2D::PlotItem* plotItem, const std::string &label, const std::string &axis,
				       tPointVector& pts, tMarkerDataVector& markerLabels) {
  double xmin(NumLim::MAX_DOUBLE), ymin(NumLim::MAX_DOUBLE);
  double xmax(NumLim::MIN_DOUBLE), ymax(NumLim::MIN_DOUBLE);

  plotItem->set_label(label);
  plotItem->set_axis(axis);
  plotItem->set_is_marker(isMarker());
  plotItem->set_legend(!isLegendHide());
  for (tPointVector::iterator it=pts.begin();
       it!=pts.end(); ++it) {
    plotItem->add_x((*it).x);
    xmin = std::min(xmin, (*it).x);
    xmax = std::max(xmax, (*it).x);

    plotItem->add_y((*it).y);
    ymin = std::min(ymin, (*it).y);
    ymax = std::max(ymax, (*it).y);
  }
  if (ymax == NumLim::MIN_DOUBLE) {
    return false;
  }
  if (isMarker()) {
    Json::Value mjsonAry = Json::Value(Json::arrayValue);
    for (tMarkerDataVector::iterator it=markerLabels.begin();
         it!=markerLabels.end(); ++it) {
      auto marker = plotItem->add_marker_labels();
      marker->set_label(it->label);
      marker->set_background(it->background);
      marker->set_foreground(it->foreground);
    }
  }
  // min, max
  plotItem->set_min_x(xmin);
  plotItem->set_max_x(xmax);
  plotItem->set_min_y(ymin);
  plotItem->set_max_y(ymax);

  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* clearDataCache --                                                           */
/* --------------------------------------------------------------------------- */

void Gui2dPlot::PlotItem::clearDataCache() {
  if(m_cachePts.size())
    BUG_DEBUG("Clear Data cache, size:Points, counts total: " << m_cachePts.size());
  m_cachePts.clear();
  m_cacheMarkerLabels.clear();
}

//---------------------------------------------------------------------------
// readData
//---------------------------------------------------------------------------

int Gui2dPlot::readData(PlotItem* item, int xAxisWildcardIndex, int yAxisWildcardIndex,
                        tPointVector& pts, tMarkerDataVector& markerLabels, QRectF* bRect) {
  GuiPlotDataItem *yItem = item->plotDataItem();
  GuiPlotDataItem *xItem = item->xPlotDataItem();
  PlotItem *xPlotItem = item->xPlotItem();
  int validCnt=0, firstInvalidIdx=-1;
  int npoints = item->getNPoints();
  double y, x;
  XferDataItemIndex *yIndex = yItem->getDataItemIndexWildcard( item->getDataPlotItemIndex() );
  XferDataItemIndex *xIndex = 0;
  XferDataItemIndex *markerIndex = 0;

  pts.clear();
  pts.reserve(npoints);

  // y axis wildcard
  if (item->hasAxisWildcardIndex()) {
    item->setAxisWildcardIndexValue(yAxisWildcardIndex);
  }

  // x axis wildcard
  if (item->xPlotItem() &&
      item->xPlotItem()->hasAxisWildcardIndex()) {
    item->xPlotItem()->setAxisWildcardIndexValue(xAxisWildcardIndex);
  }

  // markerIndex
  if( item->getMarker() ){
    markerIndex = item->getMarker()->getDataItemIndexWildcard( item->getDataPlotItemIndex() );
  }

  if( xItem ){
    xIndex = xItem->getDataItemIndexWildcard(item->xPlotItem()->getDataPlotItemIndex());
  }

  // headless mode, minimize data
  double maxCurveLenFac = 0;
  if (AppData::Instance().HeadlessWebMode() && !markerIndex && npoints) {
    if (m_maxCurveLen < npoints) {
      maxCurveLenFac = 1.0 * m_maxCurveLen / npoints;
    }
  }

  int nextJ = 1;
  for( int j = 0; j < npoints; ++j ) {
    // omit this point?
    // no boundingbox (not webtens zoom mode)
    // First and Last Point will not omit
    if (maxCurveLenFac > 0. && !bRect &&
        j != 0 && j != (npoints-1)) {
      if ((j * maxCurveLenFac) < (nextJ - std::numeric_limits<double>::epsilon()) ||
          nextJ >= (m_maxCurveLen -1))
        continue;
      nextJ += 1;
    }

    x = 0., y = 0.;
    yIndex->setIndex( item->plotDataItem()->Data(), j );
    if( xItem ){
      xIndex->setIndex( xItem->Data(), j );
      if (!xItem->getValue( x ) ||
          (getAxis(2).isLogarithmic() && x <= 0) // x-Axis Logarithmic: ignore values <= 0
          ) {
        BUG_DEBUG("CONTINUE "<< xItem->Data()->fullName(true) << " val: " << x
                  << " isLog: " << getAxis(2).isLogarithmic());
        continue;
      }
      x*=xItem->getScaleFactor();
    }
    else
      x=j;

    yItem->getValue( y );
    if ( !yItem->XferData()->isValid() ||
         (getAxis(0).isLogarithmic() && y <= 0) // y-Axis Logarithmic: ignore values <= 0
         ) {
      y = std::numeric_limits<double>::quiet_NaN();
      if (firstInvalidIdx<0)
        firstInvalidIdx = j;
    } else {
      y*=yItem->getScaleFactor();
      ++validCnt;
    }
    pts.push_back(Point(x,y));

    // get marker label
    std::string label;
    double dvalue = std::numeric_limits<double>::quiet_NaN();
    std::string background, foreground;
    if( markerIndex ){
      markerIndex->setIndex( item->getMarker()->Data(), j );
      item->getMarker()->getValue( label );
      // get colorset colors
      if (item->colorset()) {
        item->getMarker()->getValue( dvalue );
        if (item->colorset()->getInterpolatedColors(dvalue, background, foreground)) {
          BUG_DEBUG("Got Colors form coloset, fg["<<foreground<<"] bg["<<background<<"]\n");
        }
      }
    }
    else{
      yItem->getLabelAndUnit( label );
    }
    markerLabels.push_back(MarkerData(label, background, foreground));
  }

  return validCnt;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool Gui2dPlot::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  bool updated = isDataItemUpdated(GuiManager::Instance().LastWebUpdate()) ||
                 isAxisUpdated(GuiManager::Instance().LastWebUpdate());
  // axis label stream update check
  eAxisType axisType[4] = { XAXIS, XAXIS2, Y1AXIS, Y2AXIS };
  for( int axis = 0; axis < 4; ++axis ) {
    if (!updated && getTitleStream(axisType[axis]) &&
        convertToString(getTitleStream(axisType[axis])) != getTitle(axisType[axis]) ) {
      updated=true;
    }
  }
  if (onlyUpdated && !updated) {
    return false;
  }
  ///  showAnnotationLabels( true );
  drawHeaderText();  // set header text
  drawFooterText();  // set footer text
  getElement()->writeJsonProperties(jsonObj);
  if (m_initial_size) {
    jsonObj["fixed_width"] = m_initial_width;
    jsonObj["fixed_height"] = m_initial_height;
  }

  if (pheaderText() && pheaderText()[0])
    jsonObj["header_text"] = pheaderText()[0];
  if (pfooterText() && pfooterText()[0])
    jsonObj["footer_text"] = pfooterText()[0];
  if (MenuLabel().size())
    jsonObj["menu_label"] = MenuLabel();
  jsonObj["updated"] = updated;

  // X Axis
  std::string s;
  if( getTitleStream(XAXIS) != 0  ) {
    s = convertToString( getTitleStream(XAXIS) );
    setLabel(s, XAXIS);  // save last title
  } else if( !getTitle(XAXIS).empty() ){
    s = getTitle(XAXIS);
  }
  if (s.empty()){
    s = getXAxisPlotItemTitles();
  }
  Json::Value jsonElem = Json::Value(Json::objectValue);
  getAxis(2).serializeJson(jsonElem, onlyUpdated);
  jsonElem["title"] = s;
  jsonObj["XAxis"] = jsonElem;

  // Y1 Axis
  s.clear();
  if( getTitleStream(Y1AXIS) != 0  )
    s = convertToString( getTitleStream(Y1AXIS) );
  else if( !getTitle(Y1AXIS).empty() ){
    s = getTitle(Y1AXIS);
  }
  jsonElem = Json::Value(Json::objectValue);
  getAxis(0).serializeJson(jsonElem, onlyUpdated);
  jsonElem["style"] = getAxisStyle(m_style[0]);
  jsonElem["title"] = s;
  jsonObj["Y1Axis"] = jsonElem;

  // Y2 Axis
  s.clear();
  if( getTitleStream(Y2AXIS) != 0  ) {
    s = convertToString( getTitleStream(Y2AXIS) );
    setLabel(s, Y2AXIS);  // save last title
  } else if( !getTitle(Y2AXIS).empty() ){
    s = getTitle(Y2AXIS);
  }
  jsonElem = Json::Value(Json::objectValue);
  getAxis(1).serializeJson(jsonElem, onlyUpdated);
  jsonElem["style"] = getAxisStyle(m_style[1]);
  jsonElem["title"] = s;
  jsonObj["Y2Axis"] = jsonElem;

  // plotitems
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  tPlotIterator plotIter;
  tPointVector pts;
  bool replot( false );
  TransactionNumber trans = GuiManager::Instance().LastWebUpdate();
  trans = 0;  // xml export immer alles

  updateAxisTypes();

  // update axis scale
  updateAxisScaleValues();

  doPlot(true);

  double xmin(NumLim::MAX_DOUBLE), ymin(NumLim::MAX_DOUBLE);
  double xmax(NumLim::MIN_DOUBLE), ymax(NumLim::MIN_DOUBLE);
  int currentCycle = dpi().currentCycle();
  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    if ( !(*plotIter) || !dpi().goCycle( (*plotIter)->cycle(), false )) continue;
    dpi().goCycle( (*plotIter)->cycle(), false );  // do not clear undo stack
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      std::string axisStr(axis == 0 ? "Y1Axis" : "Y2Axis");
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        if( (*it)->getAxisType() !=XAXIS && (*it)->getAxisType() !=HIDDEN ){
          if( (*it) && (*it)->isDataItemUpdated(trans) ||
              (*it)->xPlotDataItem() && (*it)->xPlotDataItem()->isDataItemUpdated(trans) ){
            if (m_showCycleVector[(*plotIter)->cycle() ]) {
              std::string label = PlotItemLabel((*it)->plotDataItem());
              tMarkerDataVector markerLabels;
              int validCnt = readData((*it), (*plotIter)->xIndex(), (*plotIter)->yIndex(), pts, markerLabels);

              Json::Value jsonElem = Json::Value(Json::objectValue);
              (*it)->clearDataCache();  //  cache for webtens zoom mode
              if (!(*it)->serializeJson(jsonElem, label, axisStr, pts, markerLabels)) {
                ++it;
                continue;
              }
              // add color
              GuiPlotDataItem *yItem = (*it)->plotDataItem();
              std::string unique = yItem->getUniqueName((*plotIter)->xIndex(),
                                                        (*plotIter)->yIndex(),
                                                        getAllCycles());
              Plot2dCurvePalette* curvePalette = getPalette(unique);
              jsonElem["lineColor"] = curvePalette->lineColor;
              jsonElem["symbolColor"] = curvePalette->symbolColor;
              jsonElem["lineStyle"] = curvePalette->lineStyle;
              jsonElem["lineStyleString"] = curvePalette->getLineStyleString(curvePalette->lineStyle);
              jsonElem["symbolStyle"] = curvePalette->symbolStyle;
              jsonElem["symbolStyleString"] = curvePalette->getSymbolStyleString(curvePalette->symbolStyle);
              jsonElem["symbolSize"] = curvePalette->symbolSize;

              jsonAry.append(jsonElem);

              // min, max
              xmin = std::min(xmin, jsonElem["min_x"].asDouble());
              xmax = std::max(xmax, jsonElem["max_x"].asDouble());
              ymin = std::min(ymin, jsonElem["min_y"].asDouble());
              ymax = std::max(ymax, jsonElem["max_y"].asDouble());
           }
          }
        }
        ++it;
      }
    }

    // x annotation
    double xdelta = 0.1 * xmax - xmin;
    GuiPlotDataItem::AnnotationLabelsMap annoMap;
    std::set<GuiPlotDataItem *> items;
    (*plotIter)->getXPlotDataItems( items );
    std::set<GuiPlotDataItem *>::iterator itemIter;
    for( itemIter = items.begin(); itemIter != items.end(); ++itemIter ) {
      if( (*itemIter) != 0 ){
        if (hasAnnotationOption()) {
          Json::Value jsonAnnoAry = Json::Value(Json::arrayValue);
          if((*itemIter)->getAnnotationLabelsMap(annoMap)) {
            GuiPlotDataItem::AnnotationLabelsMap::iterator it;
            for( it = annoMap.begin(); it != annoMap.end(); ++it ) {
              if ((*it).first > (xmin-xdelta) && (*it).first < (xmax+xdelta)) {
                Json::Value elemAry = Json::Value(Json::objectValue);
                elemAry["pos"] = (*it).first;
                elemAry["label"] = (*it).second;
                jsonAnnoAry.append(elemAry);
              }
            }
          }
          jsonObj["xAnnotation"] = jsonAnnoAry;
        }
      }
    }
  }

  dpi().goCycle( currentCycle, false ); // do not clear undo stack
  jsonObj["plotitems"] = jsonAry;
  // min, max
  jsonObj["min_x"] = xmin;
  jsonObj["max_x"] = xmax;
  jsonObj["min_y"] = ymin;
  jsonObj["max_y"] = ymax;

  // action
  if( m_function != 0 ){
    jsonObj["action"] = m_function->Name();
  }

  return true;
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool Gui2dPlot::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  bool updated = isDataItemUpdated(GuiManager::Instance().LastWebUpdate()) ||
                 isAxisUpdated(GuiManager::Instance().LastWebUpdate());
  // axis label stream update check
  eAxisType axisType[4] = { XAXIS, XAXIS2, Y1AXIS, Y2AXIS };
  for( int axis = 0; axis < 4; ++axis ) {
    if (!updated && getTitleStream(axisType[axis]) &&
        convertToString(getTitleStream(axisType[axis])) != getTitle(axisType[axis]) ) {
      updated=true;
    }
  }
  if (onlyUpdated && !updated) {
    return false;
  }
  auto element = eles->add_plot_2ds();
  ///  showAnnotationLabels( true );
  drawHeaderText();  // set header text
  drawFooterText();  // set footer text
  element->set_allocated_base(getElement()->writeProtobufProperties());
  if (m_initial_size) {
    element->set_fixed_width(m_initial_width);
    element->set_fixed_height(m_initial_height);
  }

  if (pheaderText() && pheaderText()[0])
    element->set_header_text(pheaderText()[0]);
  if (pfooterText() && pfooterText()[0])
    element->set_footer_text(pfooterText()[0]);
  if (MenuLabel().size())
    element->set_menu_label(MenuLabel());

  // X Axis
  std::string s;
  if( getTitleStream(XAXIS) != 0  ) {
    s = convertToString( getTitleStream(XAXIS) );
    setLabel(s, XAXIS);  // save last title
  } else if( !getTitle(XAXIS).empty() ){
    s = getTitle(XAXIS);
  }
  if (s.empty()){
    s = getXAxisPlotItemTitles();
  }
  getAxis(2).serializeProtobuf(element->mutable_axis_x(), onlyUpdated);
  element->mutable_axis_x()->set_title(s);

  // Y1 Axis
  s.clear();
  if( getTitleStream(Y1AXIS) != 0  )
    s = convertToString( getTitleStream(Y1AXIS) );
  else if( !getTitle(Y1AXIS).empty() ){
    s = getTitle(Y1AXIS);
  }
  getAxis(0).serializeProtobuf(element->mutable_axis_y1(), onlyUpdated);
  element->mutable_axis_y1()->set_style(getAxisStyle(m_style[0]));
  element->mutable_axis_y1()->set_title(s);

  // Y2 Axis
  s.clear();
  if( getTitleStream(Y2AXIS) != 0  ) {
    s = convertToString( getTitleStream(Y2AXIS) );
    setLabel(s, Y2AXIS);  // save last title
  } else if( !getTitle(Y2AXIS).empty() ){
    s = getTitle(Y2AXIS);
  }
  getAxis(1).serializeProtobuf(element->mutable_axis_y2(), onlyUpdated);
  element->mutable_axis_y2()->set_style(getAxisStyle(m_style[1]));
  element->mutable_axis_y2()->set_title(s);

  // plotitems
  tPlotIterator plotIter;
  tPointVector pts;
  bool replot( false );
  TransactionNumber trans = GuiManager::Instance().LastWebUpdate();
  trans = 0;  // xml export immer alles

  updateAxisTypes();

  // update axis scale
  updateAxisScaleValues();

  doPlot(true);

  double xmin(NumLim::MAX_DOUBLE), ymin(NumLim::MAX_DOUBLE);
  double xmax(NumLim::MIN_DOUBLE), ymax(NumLim::MIN_DOUBLE);
  int currentCycle = dpi().currentCycle();
  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    if ( !(*plotIter) || !dpi().goCycle( (*plotIter)->cycle(), false )) continue;
    dpi().goCycle( (*plotIter)->cycle(), false );  // do not clear undo stack
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      std::string axisStr(axis == 0 ? "Y1Axis" : "Y2Axis");
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        if( (*it)->getAxisType() !=XAXIS && (*it)->getAxisType() !=HIDDEN ){
          if( (*it) && (*it)->isDataItemUpdated(trans) ||
              (*it)->xPlotDataItem() && (*it)->xPlotDataItem()->isDataItemUpdated(trans) ){
            if (m_showCycleVector[(*plotIter)->cycle() ]) {
              std::string label = PlotItemLabel((*it)->plotDataItem());
              tMarkerDataVector markerLabels;
              int validCnt = readData((*it), (*plotIter)->xIndex(), (*plotIter)->yIndex(), pts, markerLabels);

              auto plotItem = element->add_plot_items();
              (*it)->clearDataCache();  //  cache for webtens zoom mode
              if (!(*it)->serializeProtobuf(plotItem, label, axisStr, pts, markerLabels)) {
                ++it;
                continue;
              }
              // add color
              GuiPlotDataItem *yItem = (*it)->plotDataItem();
              std::string unique = yItem->getUniqueName((*plotIter)->xIndex(),
                                                        (*plotIter)->yIndex(),
                                                        getAllCycles());
              Plot2dCurvePalette* curvePalette = getPalette(unique);
              plotItem->set_line_color(curvePalette->lineColor);
              plotItem->set_symbol_color(curvePalette->symbolColor);
              plotItem->set_line_style(curvePalette->getLineStyleString(curvePalette->lineStyle));
              plotItem->set_symbol_style(curvePalette->getSymbolStyleString(curvePalette->symbolStyle));
              plotItem->set_symbol_size(curvePalette->symbolSize);

              // min, max
              xmin = std::min(xmin, plotItem->min_x());
              xmax = std::max(xmax, plotItem->max_x());
              ymin = std::min(ymin, plotItem->min_y());
              ymax = std::max(ymax, plotItem->max_y());
           }
          }
        }
        ++it;
      }
    }

    // x annotation
    double xdelta = 0.1 * xmax - xmin;
    GuiPlotDataItem::AnnotationLabelsMap annoMap;
    std::set<GuiPlotDataItem *> items;
    (*plotIter)->getXPlotDataItems( items );
    std::set<GuiPlotDataItem *>::iterator itemIter;
    for( itemIter = items.begin(); itemIter != items.end(); ++itemIter ) {
      if( (*itemIter) != 0 ){
        if (hasAnnotationOption()) {
          Json::Value jsonAnnoAry = Json::Value(Json::arrayValue);
          if((*itemIter)->getAnnotationLabelsMap(annoMap)) {
            GuiPlotDataItem::AnnotationLabelsMap::iterator it;
            for( it = annoMap.begin(); it != annoMap.end(); ++it ) {
              if ((*it).first > (xmin-xdelta) && (*it).first < (xmax+xdelta)) {
                Json::Value elemAry = Json::Value(Json::objectValue);
                auto elem = element->add_x_annotation();
                elem->set_pos((*it).first);
                elem->set_label((*it).second);
                jsonAnnoAry.append(elemAry);
              }
            }
          }
        }
      }
    }
  }

  dpi().goCycle( currentCycle, false ); // do not clear undo stack
  // min, max
  element->set_min_x(xmin);
  element->set_max_x(xmax);
  element->set_min_y(ymin);
  element->set_max_y(ymax);

  // action
  if( m_function != 0 ){
    element->set_action(m_function->Name());
  }

  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* schrinkPlotCurveData --                                                     */
/* --------------------------------------------------------------------------- */

bool Gui2dPlot::schrinkPlotCurveData(tPointVector& pts, tMarkerDataVector& markerLabels, QRectF* bRect, PlotItem* item) {
  // webtens zoom mode
  if (bRect) {
    tPointVector pts2;
    pts2.reserve(pts.size());
    tMarkerDataVector markerLabels2;
    markerLabels2.reserve(markerLabels.size());
    tPointVector::iterator itNext=pts.begin();
    ++itNext;
    tMarkerDataVector::iterator itML=markerLabels.begin();
    tMarkerDataVector::iterator itMLNext=markerLabels.begin();
    ++itMLNext;
    for (tPointVector::iterator it=pts.begin();
         it!=pts.end() && itNext!=pts.end() ; ++it, ++itNext, ++itML, ++itMLNext) {
      bool nextInside(bRect->contains((*itNext).x, (*itNext).y));
      if (nextInside || bRect->contains((*it).x, (*it).y)) {
        // insert this point
        pts2.push_back(*it);
        markerLabels2.push_back(*itML);
      }
      if (nextInside) {
        // insert next point
        pts2.push_back(*itNext);
        markerLabels2.push_back(*itMLNext);
        ++it, ++itNext, ++itML, ++itMLNext;
        // insert next one point
        if (itNext!=pts.end()) {
          pts2.push_back(*itNext);
          markerLabels2.push_back(*itMLNext);
          ++it, ++itNext, ++itML, ++itMLNext;
        }
      }
    }

    int npoints = pts.size();
    pts.swap(pts2);
    markerLabels.swap(markerLabels2);

    // cache all Data
    if (item->DataCache_PointVector().empty()) {
      item->DataCache_PointVector().swap(pts2);
      item->DataCache_MarkerLabelsVector().swap(markerLabels2);
      BUG_INFO("Add Data cache, size:Points: " << item->DataCache_PointVector().size());
    }

    BUG_INFO("Schrink Points, counts total: " << npoints << ", points: " << pts.size() << ", max: " << m_maxCurveLen);
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializePlotCurveData --                                                   */
/* --------------------------------------------------------------------------- */

bool Gui2dPlot::serializePlotCurveData(Json::Value& jsonObj, QRectF* bRect) {
  tPlotIterator plotIter;
  TransactionNumber trans = 0;
  Json::Value jsonAry = Json::Value(Json::arrayValue);

  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    if ( !(*plotIter) || !dpi().goCycle( (*plotIter)->cycle(), false )) continue;
    dpi().goCycle( (*plotIter)->cycle(), false );  // do not clear undo stack
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      std::string axisStr(axis == 0 ? "Y1Axis" : "Y2Axis");
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        if( (*it)->getAxisType() !=XAXIS && (*it)->getAxisType() !=HIDDEN ){
          if( (*it) && (*it)->isDataItemUpdated(trans) ||
              (*it)->xPlotDataItem() && (*it)->xPlotDataItem()->isDataItemUpdated(trans) ){
            if (m_showCycleVector[(*plotIter)->cycle() ]) {
              std::string label = PlotItemLabel((*it)->plotDataItem());
              tMarkerDataVector markerLabels;
              tPointVector pts;
              if ((*it)->DataCache_PointVector().empty()) {
                readData((*it), (*plotIter)->xIndex(), (*plotIter)->yIndex(), pts, markerLabels, bRect);
              } else {
                copy((*it)->DataCache_PointVector().begin(), (*it)->DataCache_PointVector().end(), back_inserter(pts));
                copy((*it)->DataCache_MarkerLabelsVector().begin(), (*it)->DataCache_MarkerLabelsVector().end(), back_inserter(markerLabels));
              }
              schrinkPlotCurveData(pts, markerLabels, bRect, (*it));
              Json::Value jsonElem = Json::Value(Json::objectValue);
              if ((*it)->serializeJson(jsonElem, label, axisStr, pts, markerLabels))
                jsonAry.append(jsonElem);
            }
          }
        }
        ++it;
      }
    }
  }
  BUG_DEBUG("serializePlotCurveData Data Return "
            << ch_semafor_intens::JsonUtils::value2string(jsonAry));
  jsonObj["plotitems"] = jsonAry;
  return true;
}

/* --------------------------------------------------------------------------- */
/* serializePlotCurveData --                                                   */
/* --------------------------------------------------------------------------- */

#if HAVE_PROTOBUF
bool Gui2dPlot::serializePlotCurveData(google::protobuf::RepeatedPtrField<in_proto::Plot2D::PlotItem>* items, QRectF* bRect) {
  tPlotIterator plotIter;
  TransactionNumber trans = 0;

  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    if ( !(*plotIter) || !dpi().goCycle( (*plotIter)->cycle(), false )) continue;
    dpi().goCycle( (*plotIter)->cycle(), false );  // do not clear undo stack
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      std::string axisStr(axis == 0 ? "Y1Axis" : "Y2Axis");
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        if( (*it)->getAxisType() !=XAXIS && (*it)->getAxisType() !=HIDDEN ){
          if( (*it) && (*it)->isDataItemUpdated(trans) ||
              (*it)->xPlotDataItem() && (*it)->xPlotDataItem()->isDataItemUpdated(trans) ){
            if (m_showCycleVector[(*plotIter)->cycle() ]) {
              std::string label = PlotItemLabel((*it)->plotDataItem());
              tMarkerDataVector markerLabels;
              tPointVector pts;
              if ((*it)->DataCache_PointVector().empty()) {
                readData((*it), (*plotIter)->xIndex(), (*plotIter)->yIndex(), pts, markerLabels, bRect);
              } else {
                copy((*it)->DataCache_PointVector().begin(), (*it)->DataCache_PointVector().end(), back_inserter(pts));
                copy((*it)->DataCache_MarkerLabelsVector().begin(), (*it)->DataCache_MarkerLabelsVector().end(), back_inserter(markerLabels));
              }
              schrinkPlotCurveData(pts, markerLabels, bRect, (*it));
              Json::Value jsonElem = Json::Value(Json::objectValue);
              (*it)->serializeProtobuf(items->Add(), label, axisStr, pts, markerLabels);
            }
          }
        }
        ++it;
      }
    }
  }
  return true;
}
#endif

/* --------------------------------------------------------------------------- */
/* serializePlotCurveInfo --                                                   */
/* --------------------------------------------------------------------------- */

bool Gui2dPlot::serializePlotCurveInfo(Json::Value& jsonObj, int curveIndex) {
  tPlotIterator plotIter;
  TransactionNumber trans = 0;
  Json::Value jsonAry = Json::Value(Json::arrayValue);
  int cIdx(0);

  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    if ( !(*plotIter) || !dpi().goCycle( (*plotIter)->cycle(), false )) continue;
    dpi().goCycle( (*plotIter)->cycle(), false );  // do not clear undo stack
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      std::string axisStr(axis == 0 ? "Y1Axis" : "Y2Axis");
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        if( (*it)->getAxisType() !=XAXIS && (*it)->getAxisType() !=HIDDEN ){
          if (m_showCycleVector[(*plotIter)->cycle() ]) {
            if (curveIndex >= 0 && curveIndex != (cIdx)) {
              continue;
            }
            ++cIdx;
            // get curve info
            std::string label = PlotItemLabel((*it)->plotDataItem());
            tMarkerDataVector markerLabels;
            tPointVector pts;
            readData((*it), (*plotIter)->xIndex(), (*plotIter)->yIndex(), pts, markerLabels);
            std::string curveCharData = getCurveInfo(pts);
            BUG_DEBUG("curveInfo(" << cIdx << "): " << curveCharData);
            jsonAry.append(curveCharData);
          }
        }
        ++it;
      }
    }
  }
  BUG_DEBUG("serializePlotCurveInfoData Return "
            << ch_semafor_intens::JsonUtils::value2string(jsonAry));
  jsonObj["plotitemsInfo"] = jsonAry;
  return true;
}

//---------------------------------------------------------------------------
// serializeXML
//---------------------------------------------------------------------------

void Gui2dPlot::serializeXML(std::ostream &os, bool recursive) {
  drawHeaderText();  // set header text
  drawFooterText();  // set footer text
  os << "<intens:Gui2dPlot"
     << " name=\""<<getName() << "\"";
  if (getTitleStream(XAXIS))
    os << " x_title=\""<<getTitleStream(XAXIS) << "\"";
  if (getTitleStream(XAXIS2))
    os << " xtop_title=\""<<getTitleStream(XAXIS2) << "\"";
  if (getTitleStream(Y1AXIS))
    os << " yleft_title=\""<<getTitleStream(Y1AXIS) << "\"";
  if (getTitleStream(Y2AXIS))
    os << " yright_title=\""<<getTitleStream(Y2AXIS) << "\"";
  if (pheaderText() && pheaderText()[0])
    os << " header_text=\""<<pheaderText()[0] << "\"";
  if (pfooterText() && pfooterText()[0])
    os << " footer_text=\""<<pfooterText()[0] << "\"";
  if (MenuLabel().size())
    os << " menu_label=\""<<MenuLabel() << "\"";
  os << " id=\""<<getElement()->getElementId()<<"\">" << std::endl;
  getAxis(2).serializeXML(os, "XAxis");
  getAxis(0).serializeXML(os, "Y1Axis");
  getAxis(1).serializeXML(os, "Y2Axis");

  // plotitems
  tPlotIterator plotIter;
  tPointVector pts;
  bool replot( false );
  TransactionNumber trans = GuiManager::Instance().LastGuiUpdate();
  trans = 0;  // xml export immer alles
  int currentCycle = dpi().currentCycle();
  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    if(!(dpi().goCycle((*plotIter)->cycle(), false)))  // do not clear undo stack
      assert(false);
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      std::string axisStr(axis == 0 ? "Y1Axis" : "Y2Axis");
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        if( (*it)->getAxisType() !=XAXIS && (*it)->getAxisType() !=HIDDEN ){
          if( (*it) && (*it)->isDataItemUpdated(trans) ||
              (*it)->xPlotDataItem() && (*it)->xPlotDataItem()->isDataItemUpdated(trans) ){
            if (m_showCycleVector[(*plotIter)->cycle() ]) {
	      std::string label = PlotItemLabel((*it)->plotDataItem());
        tMarkerDataVector markerLabels;
 	      int validCnt = readData((*it), (*plotIter)->xIndex(), (*plotIter)->yIndex(), pts, markerLabels);
	      (*it)->serializeXML(os, label, axisStr, pts, markerLabels);
            }
          }
	}
        ++it;
      }
    }
  }
  dpi().goCycle( currentCycle, false ); // do not clear undo stack

  os << "</intens:Gui2dPlot>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string Gui2dPlot::variantMethod(const std::string& method,
                                     const Json::Value& jsonArgs,
                                     JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);

  // GetActiveCycles
  if (lower(method) == "getactivecycles") {
    Json::Value jsonAry = Json::Value(Json::arrayValue);
    if ( getCyclesDialog()) {
      std::vector<int> showCycleVector;
      getCyclesDialog()->getValues(showCycleVector, 0);
      showCycleVector.resize( dpi().numCycles() -1, 0 );
      if(dpi().currentCycle() == showCycleVector.size())
        showCycleVector.push_back( 1 );
      else{
        std::vector<int>::iterator intIterator = showCycleVector.begin() + dpi().currentCycle();
        showCycleVector.insert( intIterator, 1 );
      }
      for (std::vector<int>::iterator it=showCycleVector.begin();
           it!=showCycleVector.end(); ++it) {
        if ((*it) == 1)
          jsonAry.append((int)(it - showCycleVector.begin()));
      }
      jsonElem["status"] = "OK";
    } else {
      jsonAry.append(dpi().currentCycle());
      jsonElem["active_cycles_id"] = jsonAry;
      jsonElem["status"] = "OK";
      BUG_DEBUG("GetActiveCycles Return: " << ch_semafor_intens::JsonUtils::value2string(jsonElem));
    }
  }
  // SetActiveCycles
  else if (lower(method) == "setactivecycles") {
    Json::Value::Members members = jsonArgs.getMemberNames();
    BUG_DEBUG("SetActiveCycles Args: " << ch_semafor_intens::JsonUtils::value2string(jsonArgs));
    if (jsonArgs.isMember("id") && jsonArgs["id"].isArray()) {
      BUG_DEBUG("SetActiveCycles Args: " << members[0] << ", isArray: " << jsonArgs[members[0]].isArray());

      if(!getCyclesDialog()) buildCyclesDialog();
      std::vector<int> intVector(dpi().numCycles(), 0);
      for (int i=0; i< jsonArgs["id"].size(); ++i) {
        int idx = jsonArgs["id"].get(i, 0).asInt()-1;
        if (idx == dpi().currentCycle() ||
            idx > (dpi().numCycles()-1) || idx < 0) continue;
        if (idx >= dpi().currentCycle()) --idx;
        intVector[idx] = 1;
      }
      getCyclesDialog()->setValues(intVector, 0);
      jsonElem["status"] = "OK";
    } else {
      jsonElem["message"] = "Wrong Parameter";
      jsonElem["status"] = "Error";
    }
  }
  // fullscreen
  else if (lower(method) == "fullscreen") {
    Json::Value::Members members = jsonArgs.getMemberNames();
    BUG_DEBUG("fullscreen Args: " << ch_semafor_intens::JsonUtils::value2string(jsonArgs));
    fullscreen();
  }
  // get curve data
  else if (lower(method) == "getcurvedata") {
    Json::Value::Members members = jsonArgs.getMemberNames();
    if (jsonArgs.isMember("boundingbox") &&
        jsonArgs["boundingbox"].isArray() &&
        jsonArgs["boundingbox"].size() == 4) {
      QRectF bb(jsonArgs["boundingbox"].get((Json::ArrayIndex) 0, std::numeric_limits<double>::quiet_NaN()).asDouble(),
               jsonArgs["boundingbox"].get((Json::ArrayIndex) 1, std::numeric_limits<double>::quiet_NaN()).asDouble(),
               jsonArgs["boundingbox"].get((Json::ArrayIndex) 2, std::numeric_limits<double>::quiet_NaN()).asDouble(),
               jsonArgs["boundingbox"].get((Json::ArrayIndex) 3, std::numeric_limits<double>::quiet_NaN()).asDouble());
      serializePlotCurveData(jsonElem, &bb);
      jsonElem["status"] = "OK";
    }
    BUG_DEBUG("getCurveData Args: " << ch_semafor_intens::JsonUtils::value2string(jsonArgs));
  }
  // get curve data
  else if (lower(method) == "getcurveinfo") {
    int id(-1);
    if (jsonArgs.isMember("id") && ! jsonArgs["id"].isArray()) {
      id = jsonArgs["id"].asInt();
    }
    serializePlotCurveInfo(jsonElem, id);
    jsonElem["status"] = "OK";
  }
  // get curve data
  else if (lower(method) == "configdialog_reset") {
    configDialogResetEvent();
    jsonElem["message"] = "configDialog_reset done";
    jsonElem["status"] = "OK";
  }
  // set xaxis toggle
  else if (lower(method) == "configdialog_setxaxis") {
    // vector variant
    if (jsonArgs.isMember("id") && jsonArgs["id"].isArray() &&
        jsonArgs.isMember("value") && jsonArgs["value"].isArray() &&
        jsonArgs["id"].size() == jsonArgs["value"].size()) {
      for(int i=0; i<jsonArgs["id"].size(); ++i){
        int id = jsonArgs["id"].get((Json::ArrayIndex) i, -1).asInt();
        int value = jsonArgs["value"].get((Json::ArrayIndex) i, -1).asInt();
        // set value
        PlotItem * xPlotItem = 0;
        if(id <  m_plotItems.size())
          xPlotItem = m_plotItems[id];
        XferDataItem *xXfer = 0, *y1Xfer = 0, *y2Xfer = 0, *colorXfer = 0, *symbolColorXfer = 0, *unitXfer = 0;
        XferDataItem *lineStyleXfer = 0, *symbolStyleXfer = 0, *symbolSizeXfer = 0;
        xPlotItem->getAllXfers(xXfer, y1Xfer, y2Xfer, colorXfer, symbolColorXfer,
                               lineStyleXfer, symbolStyleXfer, symbolSizeXfer, unitXfer);
        if (xXfer) {
          xXfer->setValue(value);
        }
      }
    }
    else {
      BUG_INFO("Invalid Parameter for method: " << method);
    }
    jsonElem["message"] = "configDialog_setxaxis done";
    jsonElem["status"] = "OK";
  }
  if (!jsonElem.isNull()) {
    BUG_DEBUG("variantMethod Method[" << method << "], Args["
              << ch_semafor_intens::JsonUtils::value2string(jsonArgs) << "]  Return: "
              << ch_semafor_intens::JsonUtils::value2string(jsonElem));
    return ch_semafor_intens::JsonUtils::value2string(jsonElem);
  }
  return getElement()->defaultVariantMethod(method, jsonArgs, eng);
}

/* --------------------------------------------------------------------------- */
/* getTitle --                                                                 */
/* --------------------------------------------------------------------------- */
const std::string Gui2dPlot::getTitle(eAxisType axis) {
  if (axis < AXIS_TYPE_COUNT)
    return UnitManager::extractValue(m_title[axis]);
  return "";
}

/* --------------------------------------------------------------------------- */
/* getTitleStream --                                                           */
/* --------------------------------------------------------------------------- */
Stream* Gui2dPlot::getTitleStream(eAxisType axis) {
  if (axis < AXIS_TYPE_COUNT)
    return m_titleStream[axis];
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* getXferUiMode --                                                            */
/* --------------------------------------------------------------------------- */
std::string Gui2dPlot::getXferUiMode() {
  std::string dpMode;
  if (m_uiMode)
	m_uiMode->getValue(dpMode);
  return dpMode;
}

/* --------------------------------------------------------------------------- */
/* setXferUiMode --                                                            */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setXferUiMode(std::string mode) {
  if (m_uiMode)
	m_uiMode->setValue(mode);
}

/* --------------------------------------------------------------------------- */
/* getOverrideSymbolSize --                                                    */
/* --------------------------------------------------------------------------- */
int Gui2dPlot::getOverrideSymbolSize() {
  int dpSize(0);
  if (m_overrideSymbolSize)
    m_overrideSymbolSize->getValue(dpSize);
  return dpSize;
}

/* --------------------------------------------------------------------------- */
/* AspectRatioMode --                                                          */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::AspectRatioMode() {
  return m_aspectRatioMode;
}

/* --------------------------------------------------------------------------- */
/* AspectRatioReferenceAxis --                                                 */
/* --------------------------------------------------------------------------- */
Gui2dPlot::eAxisType Gui2dPlot::AspectRatioReferenceAxis() {
  return m_aspectRatioReferenceAxis;
}
/* --------------------------------------------------------------------------- */
/* createLocalData --                                                          */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::createLocalData() {
  BUG(BugGui,"Gui2dPlot::createDataReference");
  DataPool &datapool = dpi().getDataPool();
  std::string varname;
  varname = "@Gui2dPlotVar" + getName();

  DataDictionary *dict_struct;
  dict_struct = datapool.AddToDictionary( ""
					, varname
				        , DataDictionary::type_StructVariable
					, INTERNAL_XRTGRAPH_STRUCT );
  assert( dict_struct != 0 );

  // Wir schützen die Variable vor dem Cycle-Clear. Sonst gehen immer
  // alle Einstellungen verloren.
  dict_struct->setItemProtected();
  static_cast<UserAttr*>(dict_struct->GetAttr())->SetEditable();

  m_drefStruct = dpi().getDataReference( varname );
  assert( m_drefStruct != 0 );
  static_cast<UserAttr*>(m_drefStruct->getUserAttr())->SetEditable();
}

/* --------------------------------------------------------------------------- */
/* getAxisStyle --                                                             */
/* --------------------------------------------------------------------------- */
std::string Gui2dPlot::getAxisStyle(eStyle style) {
  switch (style)
    {
    case PLOT:   return "Plot";
    case BAR:    return "Bar";
    case AREA:   return "Area";
    case POLAR:  return "Polar";
    case STACKING_BAR: return "StackingBar";
    case STEP:   return "Step";
    case DOTS:   return "Dots";
    default:     return "Unknown";
    }
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::isDataItemUpdated( TransactionNumber trans ) {
  BUG(BugGui,"Gui2dPlot::isDataItemUpdated");

  tPlotItemIterator it;
  bool ret(false);
  for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it ){

    if ((*it)->hasAxisWildcardIndex()) {
      for (int i=0; i < (*it)->getAxisWildcardIndexDimension(); ++i) {
        (*it)->setAxisWildcardIndexValue(i);
        if( (*it)->plotDataItem()->isDataItemUpdated(trans) ) {
          BUG_MSG( "dataitem '" << (*it)->plotDataItem()->getName() <<"' is updated" );
          ret = true;
        }
      }
    } else
      if( (*it)->plotDataItem()->isDataItemUpdated(trans) ) {
        BUG_MSG( "dataitem '" << (*it)->plotDataItem()->getName() <<"' is updated" );
        ret = true;
      }
  }
  if (ret) return true;

  if( m_drefStruct ) {
    if( m_drefStruct->isDataItemUpdated( DataReference::ValueUpdated, trans ) ) {
      BUG_MSG( "struct '" << m_drefStruct->fullName( true ) << "' is updated" );
      return true;
    }
  }

  if( m_configDialog ){
    if( m_configDialog->isUpdated( trans ) ){
      BUG_MSG( "ConfigDialog is updated" );
      return true;
    }
  }

  if( m_uiMode ){
    if( m_uiMode->isDataItemUpdated( trans ) ){
      return true;
    }
  }

  if( m_overrideSymbolSize ){
    if( m_overrideSymbolSize->isDataItemUpdated( trans ) ){
      return true;
    }
  }

  BUG_MSG( "No item are updated" );
  return false;
}

/* --------------------------------------------------------------------------- */
/* initXfer --                                                                 */
/* --------------------------------------------------------------------------- */
XferDataItem *Gui2dPlot::initXfer( const std::string &item, int idx ){
  DataReference *dref = dpi().getDataReference( m_drefStruct, item );

  if( dref != 0 ){
    XferDataItem *xfer = new XferDataItem( dref );
    XferDataItemIndex *index = xfer->newDataItemIndex( 1 );
    index->setLowerbound( idx );
    xfer->setDimensionIndizes();
    return xfer;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* closeEvent --                                                               */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::closeEvent( GuiEventData *event ) {
  if( event == 0 )
    return;
  GuiElement *element = event->m_element;
  if( element != 0 )
    event->m_element->unmanage();
}

/* --------------------------------------------------------------------------- */
/* resetEvent --                                                               */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::resetEvent( GuiEventData *event ) {
  if( event == 0 )
    return;
  GuiElement *element = event->m_element;
  if( element == 0 )
    return;
  if( getConfigDialog() ){
    if( element == getConfigDialog()->getForm()->getElement() ){
      configDialogResetEvent();
      return;
    }
  }
  if( getScaleDialog() ){
    if( element == getScaleDialog()->getElement() ){
      scaleDialogResetEvent();
      return;
    }
  }
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* scaleDialogResetEvent --                                                    */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::scaleDialogResetEvent() {
  for( int y = 0; y < AXIS_TYPE_COUNT; ++y )
    getAxis(y).resetScaleValues();
  if( m_scaleDialog )
    m_scaleDialog->getElement()->update( GuiElement::reason_Always );
  getElement()->update( GuiElement::reason_FieldInput );
}

//----------------------------------------------------
// facility class PointPlotCurve
//----------------------------------------------------
class PointPlotCurve {
public:
  PointPlotCurve( double x, double y ): m_x(x), m_y(y){}
  // berechnet die Fläche des mit p aufgespannten Rechtecks:
  double area( const PointPlotCurve &p ) const{
    return (m_y + p.m_y)/2.0 * fabs( m_x - p.m_x );
  }
  // berechnet den Amplitudenmittelwert:
  double x()const{ return m_x; }
  double y()const{ return m_y; }
  double yavg( const PointPlotCurve &p ) const{
    return (m_y + p.m_y)/2.0;
  }
  operator double()const{
    return m_y;
  }
private:
  double m_x;
  double m_y;
};

//----------------------------------------------------
// functor class MonotonicIncreasing
//----------------------------------------------------
class MonotonicIncreasing {
public:
  bool operator()( const PointPlotCurve &p1, const PointPlotCurve &p2 )const{
    if( p1.x() == std::numeric_limits<double>::max() ||
	p2.x() == std::numeric_limits<double>::max() )
      return false;
    return p2.x() >= p1.x();
  }
};

//----------------------------------------------------
// functor class MonotonicDecreasing
//----------------------------------------------------
class MonotonicDecreasing {
public:
  bool operator()( const PointPlotCurve &p1, const PointPlotCurve &p2 )const{
    if( p1.x() == std::numeric_limits<double>::max() ||
	p2.x() == std::numeric_limits<double>::max())
      return false;
    return p2.x() <= p1.x();
  }
};

//----------------------------------------------------
// functor class Integral
//----------------------------------------------------
class Integral {
public:
  double operator()( const PointPlotCurve &p1, const PointPlotCurve &p2 ){
    return p1.area(p2);
  }
};

//----------------------------------------------------
// functor class Rms2
//----------------------------------------------------
// Funktor zur Bildung des Effektivwertquadrats
class Rms2 {
public:
  double operator()( const PointPlotCurve &p1, const PointPlotCurve &p2 ){
    return p1.area(p2)*p1.yavg(p2);
  }
};

//----------------------------------------------------
// functor class MonotonicIncreasing
//----------------------------------------------------
// Algorithmus zum Testen von Eigenschaften eines Containers:
template <class InputIterator, class Predicate>
bool check_adjacent_if (InputIterator first, InputIterator last, Predicate pred){
  while (first != last-1 ){
    if( !pred(*first, *(first+1) ) )
      return false;
    ++first;
  }
  return true;
}

//----------------------------------------------------
// getCurveInfo
//----------------------------------------------------

std::string Gui2dPlot::getCurveInfo(tPointVector& pts) {
  const double EPSILON = 10 * std::numeric_limits<double>::min();
  double xmin(NumLim::MAX_DOUBLE), ymin(NumLim::MAX_DOUBLE);
  double xmax(NumLim::MIN_DOUBLE), ymax(NumLim::MIN_DOUBLE);

  std::ostringstream ostr;
  std::vector<PointPlotCurve> curvePts;
  curvePts.reserve(pts.size());
  if (!pts.size()) return std::string();

  for (tPointVector::iterator pit=pts.begin(); pit!=pts.end(); ++pit) {
       if (isnan((*pit).x) || isnan((*pit).y)) return std::string();
       curvePts.push_back( PointPlotCurve((*pit).x, (*pit).y) );
       xmin = std::min(xmin, (*pit).x);
       xmax = std::max(xmax, (*pit).x);
       ymin = std::min(ymin, (*pit).y);
       ymax = std::max(ymax, (*pit).y);
  }

  double dx = xmax - xmin;
  bool undefinedData = ymax == std::numeric_limits<double>::max();
  double avg = 0;
  double rms = 0;
  double init = 0;
  std::vector<double> f( curvePts.size() );
  std::adjacent_difference( curvePts.begin(), curvePts.end(), f.begin(), Integral() );
  avg=std::accumulate( f.begin()+1, f.end(), init )/dx;
  std::adjacent_difference( curvePts.begin(), curvePts.end(), f.begin(), Rms2() );
  rms=sqrt(std::accumulate( f.begin()+1, f.end(), init )/dx);

  RealConverter conv( 0, 2, 0, false, false );
  if( m_showMINMAX ){
    ostr << "min=";
    conv.setPrecision( m_precMINMAX );
    conv.write( double(ymin), ostr );
    ostr << ", max=";
    conv.write( double(ymax), ostr );
    if( ( m_showAVG || m_showRMS ) && !undefinedData )
      ostr << ", ";
    if( undefinedData )
      ostr << " (undefined data)";
  }
  if( m_showAVG || m_showRMS ){
    if( ( !( check_adjacent_if( curvePts.begin(), curvePts.end(), MonotonicIncreasing() ) ||
             check_adjacent_if( curvePts.begin(), curvePts.end(), MonotonicDecreasing() ) ) ) || (dx < 2.*EPSILON) ){
      if( !( m_showMINMAX && undefinedData ) )
        ostr << "non monotonic curve";
    }
    else if( undefinedData && !m_showMINMAX )
      ostr << "undefined data";
    else if( !undefinedData ){
      if( m_showAVG ){
        ostr << "avg=";
        conv.setPrecision( m_precAVG );
        conv.write( double(avg), ostr );
        if( m_showRMS )
          ostr << ", ";
      }
      if( m_showRMS ){
        ostr << "rms=";
        conv.setPrecision( m_precRMS );
        conv.write( double(rms), ostr );
      }
    }
  }
  return ostr.str();
}

/* --------------------------------------------------------------------------- */
/* updateAxisTypes --                                                          */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::updateAxisTypes(){
  double value = 0;
  eAxisType type;
  tPlotItemIterator it;
  if (getConfigDialog() &&
      getConfigDialog()->getStyle() == ConfigDialog::LIST) {
    std::vector<PlotItem*> _tmpVec;
    int x=0, pre_x=0;
    PlotItem* lastXPlotItem = 0;
    for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it, ++x ){
      if (x && m_dummyItems.find(x) != m_dummyItems.end()) {
        lastXPlotItem = 0;
        pre_x = x;
      }
      if ((*it)->getXPos() ) {
        lastXPlotItem = (*it);
        while(pre_x < x)
          _tmpVec[pre_x++] = lastXPlotItem;
      }
      _tmpVec.push_back(lastXPlotItem);
    }
    for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it ){
      if (getName().size()) // mit popupMenu
        (*it)->updateAxisTypes( true, _tmpVec[it-m_plotItems.begin()] );
    }
  } else {
    for( it = m_plotItems.begin(); it != m_plotItems.end(); ++it ){
      PlotItem * xPlotItem = 0;
      int pos = (*it)->getXPos();
      if( pos <  m_plotItems.size() )
        xPlotItem = m_plotItems[ pos ];
      if (xPlotItem)
        (*it)->updateAxisTypes( true, xPlotItem );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* updateAxisScaleValues --                                                    */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::updateAxisScaleValues(){
  double value = 0;

  for( int y = 0; y < AXIS_TYPE_COUNT; ++y ){
    m_axis[y].setMin();
    m_axis[y].setMax();
    m_axis[y].setScaleEnable();
  }
}

/* --------------------------------------------------------------------------- */
/* setPlotAxisScale --                                                         */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setPlotAxisScale( double min, double max, eAxisType axistype ){
  switch( axistype ){
  case XAXIS :
    m_axis[2].setMin( min );
    m_axis[2].setMax( max );
    m_axis[2].setScaleEnable( true );
    break;
  case XAXIS2 :
    m_axis[3].setMin( min );
    m_axis[3].setMax( max );
    m_axis[3].setScaleEnable( true );
    break;
  case Y1AXIS :
    m_axis[0].setMin( min );
    m_axis[0].setMax( max );
    m_axis[0].setScaleEnable( true );
    break;
  case Y2AXIS :
    m_axis[1].setMin( min );
    m_axis[1].setMax( max );
    m_axis[1].setScaleEnable( true );
    break;
  default:
    assert( false );
  }
}

void Gui2dPlot::setPlotAxisScale( XferDataItem * min, Scale *min_scale, XferDataItem * max, Scale *max_scale, eAxisType axistype ){
  switch( axistype ){
  case XAXIS :
    m_axis[2].setAllXferScale( min, min_scale,  max, max_scale, 0, 0 );
    m_axis[2].setScaleEnable( true );
    break;
  case XAXIS2 :
    m_axis[3].setAllXferScale( min, min_scale,  max, max_scale, 0, 0 );
    m_axis[3].setScaleEnable( true );
    break;
  case Y1AXIS :
    m_axis[0].setAllXferScale( min, min_scale,  max, max_scale , 0, 0 );
    m_axis[0].setScaleEnable( true );
    break;
  case Y2AXIS :
    m_axis[1].setAllXferScale( min, min_scale,  max, max_scale , 0, 0 );
    m_axis[1].setScaleEnable( true );
    break;
  default:
    assert( false );
  }
}

/* --------------------------------------------------------------------------- */
/* setPlotAxisFormat --                                                        */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setPlotAxisFormat( eAxisType axistype, const std::string& format ){
  int prec = 0;
  int fwidth = 0;
  char fmt = 'g';
  std::istringstream is( format );
  is >> fwidth;
  is >> fmt;
  if (fmt == ':') {
    fmt = 'g';
    is >> prec >> fmt;
  }
  else prec = -1;
  switch( axistype ){
  case XAXIS :
    m_axis[2].setFormat( fmt, fwidth, prec );
    break;
  case XAXIS2 :
    m_axis[3].setFormat( fmt, fwidth, prec );
    break;
  case Y1AXIS :
    m_axis[0].setFormat( fmt, fwidth, prec );
    break;
  case Y2AXIS :
    m_axis[1].setFormat( fmt, fwidth, prec );
    break;
  default:
    assert( false );
  }
}

/* --------------------------------------------------------------------------- */
/* isAxisUpdated --                                                            */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::isAxisUpdated( TransactionNumber trans ){
  for (int i=0; i < AXIS_TYPE_COUNT; ++i) {
    if( m_axis[i].isUpdated( trans ) ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setAxisAspectRatio --                                                       */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setAxisAspectRatio(Gui2dPlot::eAxisType axistype, double value) {
  BUG_DEBUG("setAxisAspectRatio, axis: " << axistype << ", value: " << value);
  switch(axistype) {
  case Y1AXIS:
    getAxis(0).setAspectRatio(value);
    break;
  case Y2AXIS:
    getAxis(1).setAspectRatio(value);
    break;
  case XAXIS:
    getAxis(2).setAspectRatio(value);
    break;
  case XAXIS2:
    getAxis(3).setAspectRatio(value);
    break;
  default:
    // ignore
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* setAxisAspectRatioXfer --                                                   */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setAxisAspectRatioXfer(Gui2dPlot::eAxisType axistype, XferDataItem* xfer) {
  BUG_DEBUG("setAxisAspectRatioXfer, axis: " << axistype);
  switch(axistype) {
  case Y1AXIS:
    getAxis(0).setXferAspectRatio(xfer);
    break;
  case Y2AXIS:
    getAxis(1).setXferAspectRatio(xfer);
    break;
  case XAXIS:
    getAxis(2).setXferAspectRatio(xfer);
    break;
  case XAXIS2:
    getAxis(3).setXferAspectRatio(xfer);
    break;
  default:
    // ignore
    ;
  }
}

/* --------------------------------------------------------------------------- */
/* setAxisAspectRatioReferenceAxis --                                          */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::setAxisAspectRatioReferenceAxis(Gui2dPlot::eAxisType axistype) {
  m_aspectRatioReferenceAxis = axistype;
  m_aspectRatioMode = true;
}

/* --------------------------------------------------------------------------- */
/* class Plot methods --                                                       */
/* --------------------------------------------------------------------------- */

bool Gui2dPlot::Plot::hasAxisWildcardIndex() {
  for( int y = 0; y < 2; ++y ){
    for(tPlotItemIterator it = m_yPlotItems[y].begin(); it != m_yPlotItems[y].end(); ++it ){
      if ((*it)->hasAxisWildcardIndex())
        return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/*  addPlotItem --                                                             */
/* --------------------------------------------------------------------------- */
Gui2dPlot::eAxisType Gui2dPlot::Plot::addPlotItem( PlotItem *plotItem ){
  eAxisType type;
  type = plotItem->getAxisType();
  switch( type ){
  case Y1AXIS   :
    m_yPlotItems[0].push_back( plotItem );
    break;
  case Y2AXIS   :
    m_yPlotItems[1].push_back( plotItem );
    break;
  case HIDDEN   :
    m_yPlotItems[0].push_back( plotItem );
    break;
  case XAXIS    :
  default       :
    break;
  }
  return type;
}

/* --------------------------------------------------------------------------- */
/*  setAllXPlotItems --                                                        */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::Plot::setAllXPlotItems( PlotItem *item ){
  tPlotItemIterator it;
  for( int y = 0; y < 2; ++y ){
    for( it = m_yPlotItems[y].begin(); it != m_yPlotItems[y].end(); ++it )
      (*it)->setXPlotItem( item );
  }
}

/* --------------------------------------------------------------------------- */
/*  plotItems --                                                               */
/* --------------------------------------------------------------------------- */
Gui2dPlot::tPlotItemVector &Gui2dPlot::Plot::plotItems( eAxisType type ){
  switch( type ){
  case Y1AXIS :
    return m_yPlotItems[0];
  case Y2AXIS :
    return m_yPlotItems[1];
  case XAXIS :
  case HIDDEN :
  default :
    assert( false );
    // the following dummy code is never executed (because of above assert)
    // but it avoids the compiler warning: no return statement
    return m_yPlotItems[0];
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* getXPlotDataItems --                                                        */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::Plot::getXPlotDataItems( std::set<GuiPlotDataItem *> &plotDataItems ){
  for( int y = 0; y < 2; ++y ){
    tPlotItemIterator iter;
    for( iter = m_yPlotItems[y].begin(); iter != m_yPlotItems[y].end(); ++iter ){
      plotDataItems.insert( (*iter)->xPlotDataItem() );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* isAnnotationLabelsUpdated --                                                */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::Plot::isAnnotationLabelsUpdated( TransactionNumber trans ){
  bool result = false;
  std::set<GuiPlotDataItem *> items;
  getXPlotDataItems( items );
  std::set<GuiPlotDataItem *>::iterator itemIter;
  for( itemIter = items.begin(); itemIter != items.end() && !result; ++itemIter ){
    if( (*itemIter) != 0 )
      result = (*itemIter)->isAnnotationLabelsUpdated( trans );
  }
  return result;
}

/* --------------------------------------------------------------------------- */
/* refreshAllDataItemIndexedList --                                            */
/* --------------------------------------------------------------------------- */
void Gui2dPlot::refreshAllDataItemIndexedList() {
  tPlotIterator plotIter;
  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        if ((*it)->plotDataItem() && (*it)->plotDataItem()->XferData()) {
          (*it)->plotDataItem()->XferData()->refreshDataItemIndexedList();
        }
        if ((*it)->xPlotDataItem() && (*it)->xPlotDataItem()->XferData()) {
          (*it)->xPlotDataItem()->XferData()->refreshDataItemIndexedList();
        }
        ++it;
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* isScaleUnitUpdated --                                                       */
/* --------------------------------------------------------------------------- */
bool Gui2dPlot::isScaleUnitUpdated(){
  UnitManager::Unit* xUnit, *yUnit;
  bool ret(false);
  double yFactorOld, xFactorOld, yFactor, yShift, xFactor, xShift;
  GuiPlotDataItem *yItem, *xItem;
  bool use_divide;
  tPlotIterator plotIter;
  for( plotIter = m_plots.begin(); plotIter != m_plots.end(); ++plotIter ) {
    eAxisType axisType[2] = { Y1AXIS, Y2AXIS };
    for( int axis = 0; axis < 2; ++axis ) {
      tPlotItemIterator it = (*plotIter)->plotItems( axisType[axis] ).begin();
      while( it != (*plotIter)->plotItems( axisType[axis] ).end() ) {
        yItem = (*it)->plotDataItem();
        xItem = (*it)->xPlotDataItem();
        yFactorOld = yItem ? yItem->getScaleFactor() : 1;
        xFactorOld = xItem ? xItem->getScaleFactor() : 1;
        yUnit = UnitManager::Instance().getUnitData(yItem->Attr()->Unit(false));
        xUnit = UnitManager::Instance().getUnitData(xItem->Attr()->Unit(false));
        yFactor = yUnit ? yUnit->factor : 1.;
        yShift = yUnit ? yUnit->shift : 0;
        xFactor = xUnit ? xUnit->factor : 1.;
        xShift = yUnit ? xUnit->shift : 0;
        if (yFactor != yFactorOld || yShift != yShift) {
          use_divide = yUnit ? yUnit->use_divide : false;
          yItem->setScale(new Scale(yFactor, (use_divide ? '/' : '*'), yShift));
          ret = true;
        }
        if (xFactor != xFactorOld || xShift != xShift) {
          use_divide = xUnit ? xUnit->use_divide : false;
          xItem->setScale(new Scale(xFactor, (use_divide ? '/' : '*'), xShift));
          ret = true;
        }
        ++it;
      }
    }
  }
  return ret;
}
