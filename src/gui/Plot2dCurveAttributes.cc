
#include <string>
#ifdef HAVE_QT
#include <QColor>
#include <qwt_symbol.h>
#endif

#include "gui/Gui2dPlot.h"
#include "gui/Plot2dCurveAttributes.h"
///#include "app/ColorSet.h"
#include "app/ColorSet.h"
#include "utils/Debugger.h"

INIT_LOGGER();

std::vector<std::string> Plot2dCurveAttributes::s_default_colors;
std::vector<std::string> Plot2dCurveAttributes::s_default_pen_colors;
std::vector<int>         Plot2dCurveAttributes::s_default_lineWidths;
std::vector<int>         Plot2dCurveAttributes::s_default_curveSymbol;
std::vector<int>         Plot2dCurveAttributes::s_default_curveSymbolSize;
std::vector<int>         Plot2dCurveAttributes::s_default_penStyles;

//----------------------------------------------------
// CurvePaintAttributes Class
//----------------------------------------------------
Plot2dCurveAttributes::Plot2dCurveAttributes(Gui2dPlot* plot) :
  m_2dPlot(plot) {
  // default plot symbol color settings
  m_lineColors = Plot2dCurveAttributes::s_default_colors;

  m_lineWidths = Plot2dCurveAttributes::s_default_lineWidths;
  m_curveSymbols = Plot2dCurveAttributes::s_default_curveSymbol;
  m_curveSymbolsSize = Plot2dCurveAttributes::s_default_curveSymbolSize;
  m_lineStyles   = Plot2dCurveAttributes::s_default_penStyles;
  m_symbolColors = Plot2dCurveAttributes::s_default_colors;
  m_symbolPenColors = Plot2dCurveAttributes::s_default_pen_colors;
}

//----------------------------------------------------
// getPalette
//----------------------------------------------------
Plot2dCurvePalette*
Plot2dCurveAttributes::getPalette(const std::string& curveName) {

  // existing curveName
  if (m_curvePaletteMap.count(curveName)) {
	if (m_2dPlot->colorset()) {  // dynamic color
	  Plot2dCurvePalette* pal = m_curvePaletteMap[curveName];
	  pal->lineColor = getNewColor(curveName, pal->index);
	  BUG_DEBUG("Dynamic lineColor CurveName[" << curveName << "] lineColor["
				<< m_curvePaletteMap[curveName]->lineColor << "]");
	  return pal;
	}
    return m_curvePaletteMap[curveName];
  }
  int newIndex = m_curvePaletteMap.size();

  std::string lineColor( getNewColor(curveName, newIndex) );
  std::string symbolColor( getNewSymbolColor(curveName, newIndex) );
  std::string symbolPenColor( getNewSymbolPenColor(curveName, newIndex) );
  int symbol = getNewSymbol(curveName, newIndex);
  int symSize = getNewSymbolSize(curveName, newIndex);
  int penWidth = getNewLineWidth(curveName, newIndex);
  int penStyle = getNewPenStyle(curveName, newIndex);
  m_curvePaletteMap[curveName] = new Plot2dCurvePalette(newIndex, lineColor, symbolColor, symbolPenColor, penWidth, penStyle, symbol, symSize);
  BUG_DEBUG("New Plot2dCurvePalette curveName["<<curveName<<"]  lineColor["<<lineColor
            <<"] symbol["<< symbol <<"] penStyle["<< penStyle <<"]");
  return m_curvePaletteMap[curveName];
}

//----------------------------------------------------
// reset
//----------------------------------------------------
void Plot2dCurveAttributes::reset() {
  for( std::map<std::string, Plot2dCurvePalette*>::iterator it = m_curvePaletteMap.begin();
       it != m_curvePaletteMap.end(); ++it) {
    Plot2dCurvePalette *palette = it->second;
    std::string curveName(it->first);
    int index = palette->index;
    palette->lineColor   = getNewColor(curveName, index);
    palette->symbolColor = getNewSymbolColor(curveName, index);
    palette->symbolPenColor = getNewSymbolPenColor(curveName, index);
    palette->lineStyle   = getNewPenStyle(curveName, index);
    palette->symbolStyle = getNewSymbol(curveName, index);
    palette->symbolSize = getNewSymbolSize(curveName, index);
  }
}

//----------------------------------------------------
// getNewColor
//----------------------------------------------------
std::string Plot2dCurveAttributes::getNewColor(const std::string& curveName, int index){
  return getColor(curveName, m_2dPlot->colorset(), true, m_lineColors, index);
}

//----------------------------------------------------
// getNewSymbolColor
//----------------------------------------------------
std::string Plot2dCurveAttributes::getNewSymbolColor(const std::string& curveName, int index){
   return getColor(curveName, m_2dPlot->colorset(), false, m_symbolColors, index);
}

//----------------------------------------------------
// getNewSymbolPenColor
//----------------------------------------------------
std::string Plot2dCurveAttributes::getNewSymbolPenColor(const std::string& curveName, int index){
   return getPenColor(curveName, m_symbolPenColors, index);
}

//----------------------------------------------------
// getNewLineWidth
//----------------------------------------------------
int Plot2dCurveAttributes::getNewLineWidth(const std::string& curveName, int index){
  if (m_lineWidths.size() == 0) {
    m_lineWidths = s_default_lineWidths;
  }
  int line_index = index % m_lineWidths.size();
  return m_lineWidths[line_index];
}

//----------------------------------------------------
// getNewSymbol
//----------------------------------------------------
int Plot2dCurveAttributes::getNewSymbol( const std::string& curveName, int index ){
  if (m_curveSymbols.size() == 0) {
    m_curveSymbols = s_default_curveSymbol;
  }
  int curveSymbolIndex = index %  m_curveSymbols.size();
  return m_curveSymbols[curveSymbolIndex];
}

//----------------------------------------------------
// getNewSymbolSize
//----------------------------------------------------
int Plot2dCurveAttributes::getNewSymbolSize(const std::string& curveName, int index){
  if (m_curveSymbolsSize.size() == 0) {
    m_curveSymbolsSize = s_default_curveSymbolSize;
  }
  int curveSymbolIndex = index % m_curveSymbolsSize.size();
  return m_curveSymbolsSize[curveSymbolIndex];
}

//----------------------------------------------------
// getNewPenStyle
//----------------------------------------------------
int Plot2dCurveAttributes::getNewPenStyle(const std::string& curveName,
												 int index){
  if (m_lineStyles.size() == 0) {
    m_lineStyles = s_default_penStyles;
  }
  int lineStyleIndex = index %  m_lineStyles.size();
  return m_lineStyles[lineStyleIndex];
}

//----------------------------------------------------
// getColor
//----------------------------------------------------
std::string Plot2dCurveAttributes::getColor(const std::string& curveName, ColorSet *colorset,
                                            bool colorsetBg, std::vector<std::string>& colors,
                                            int index){

  // colorset, we look for a color inside colorset
  if (colorset) {
    ColorSetItem *csitem = m_2dPlot->colorset()->getColorItem( index );
    if (csitem) {
      std::string retString = colorsetBg ? csitem->background() : csitem->foreground();
      if (QColor(QString::fromStdString(retString)).isValid()) {
        BUG_DEBUG("New ColorSet color: " << retString <<  ", curvename: " << curveName );
        return retString;
      }
    }
  }

  if (colors.size() == 0) {
    colors = s_default_colors;
  }

  int colorIndex = index < colors.size() ? index : index % colors.size();
  BUG_DEBUG("New color: " << colors[index] <<  ", curvename: " << curveName << ", index: " << index);
  return colors[colorIndex];
}

//----------------------------------------------------
// getPenColor
//----------------------------------------------------
std::string Plot2dCurveAttributes::getPenColor(const std::string& curveName,
                                               std::vector<std::string>& colors,
                                               int index){
  if (colors.size() == 0) {
    colors = s_default_pen_colors;
  }

  int colorIndex = index < colors.size() ? index : index % colors.size();
  BUG_DEBUG("New pen color: " << colors[index] <<  ", curvename: " << curveName << ", index: " << index);
  return colors[colorIndex];
}

//----------------------------------------------------
// getLineStyleString
//----------------------------------------------------
std::string Plot2dCurvePalette::getLineStyleString(int lineStyle) {
#ifdef HAVE_QT
  switch(lineStyle) {
  case Qt::NoPen:
    return "NoPen";
  case Qt::SolidLine:
    return "SolidLine";
  case Qt::DashLine:
    return "DashLine";
  case Qt::DotLine:
    return "DotLine";
  case Qt::DashDotLine:
    return "DashDotLine";
  case Qt::DashDotDotLine:
    return "DashDotDotLine";
  case Qt::CustomDashLine:
    return "CustomDashLine";
  default:
    return "NoPen";
  }
#endif
  return "";
}
//----------------------------------------------------
// getSymbolStyleString
//----------------------------------------------------
std::string Plot2dCurvePalette::getSymbolStyleString(int symbolStyle) {
#ifdef HAVE_QT
  switch(symbolStyle) {
  case QwtSymbol::NoSymbol:
    return "NoSymbol";
  case QwtSymbol::Ellipse:
    return "Ellipse";
  case QwtSymbol::Rect:
    return "Rect";
  case QwtSymbol::Diamond:
    return "Diamond";
  case QwtSymbol::Triangle:
    return "Triangle";
  case QwtSymbol::DTriangle:
    return "DTriangle";
  case QwtSymbol::UTriangle:
    return "UTriangle";
  case QwtSymbol::LTriangle:
    return "LTriangle";
  case QwtSymbol::RTriangle:
    return "RTriangle";
  case QwtSymbol::Cross:
    return "Cross";
  case QwtSymbol::XCross:
    return "XCross";
  case QwtSymbol::HLine:
    return "HLine";
  case QwtSymbol::VLine:
    return "VLine";
  case QwtSymbol::Star1:
    return "Star1";
  case QwtSymbol::Star2:
    return "Star2";
  case QwtSymbol::Hexagon:
    return "Hexagon";
  case QwtSymbol::Path:
    return "Path";
  case QwtSymbol::Pixmap:
    return "Pixmap";
  case QwtSymbol::Graphic:
    return "Graphic";
  case QwtSymbol::SvgDocument:
    return "SvgDocument";
  case QwtSymbol::UserStyle:
    return "UserStyle";
  default:
    return "NoSymbol";
  }
#endif
  return "";
}
