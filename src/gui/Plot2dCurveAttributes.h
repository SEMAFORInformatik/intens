
#ifndef GUI_2DPLOT_CURVE_ATTRIBUTES_INCLUDED_H
#define GUI_2DPLOT_CURVE_ATTRIBUTES_INCLUDED_H

#include <map>
#include <QList>

class Gui2dPlot;
class ColorSet;

//------------------------------------
// Plot2dCurvePalette Class
//------------------------------------
struct Plot2dCurvePalette {
  Plot2dCurvePalette(int idx, std::string& lineColor, std::string& symColor,
                     std::string& symPenColor,
                     int lineWidth, int lineStyle, int symStyle, int symSize)
    : index(idx)
    , lineColor(lineColor)
    , symbolColor(symColor)
    , symbolPenColor(symPenColor)
    , lineWidth(lineWidth)
    , lineStyle(lineStyle)
    , symbolStyle(symStyle)
    , symbolSize(symSize)
  {}
  int index;
  std::string lineColor;
  std::string symbolColor;
  std::string symbolPenColor;
  int lineWidth;
  int lineStyle;
  int symbolStyle;
  int symbolSize;
  std::string getLineStyleString(int lineStyle);
  std::string getSymbolStyleString(int symbolStyle);
};

//------------------------------------
// Plot2dCurveAttributes Class
//------------------------------------
class Plot2dCurveAttributes {
 public:
	Plot2dCurveAttributes(Gui2dPlot* gui2dPlot);

	/** get Curve Palette */
 	Plot2dCurvePalette* getPalette(const std::string& curveName);
	/** reset Curve Palette, reread settings */
	void reset();
private:
	Plot2dCurveAttributes();

protected:
  std::string getNewColor(const std::string& curveName, int index);
  std::string getNewSymbolColor(const std::string& curveName, int index);
  std::string getNewSymbolPenColor(const std::string& curveName, int index);
  int getNewLineWidth(const std::string& curveName, int index);
  int getNewSymbol(const std::string& curveName, int index);
  int getNewSymbolSize(const std::string& curveName, int index);
  int getNewPenStyle(const std::string& curveName, int index);
  std::string getColor(const std::string& curveName, ColorSet *colorset,
                       bool colorsetBg, std::vector<std::string>& colors,
                       int index);
  std::string getPenColor(const std::string& curveName,
                          std::vector<std::string>& colors,
                          int index);

 private:
	Gui2dPlot* m_2dPlot;
	std::map<std::string, Plot2dCurvePalette*> m_curvePaletteMap;

 public:
  std::vector<std::string> m_lineColors;
  std::vector<std::string> m_symbolColors;
  std::vector<std::string> m_symbolPenColors;
  std::vector<int>   m_curveSymbols; // QwtSymbol::Style
  std::vector<int>   m_curveSymbolsSize;
  std::vector<int>   m_lineWidths;
  std::vector<int>   m_lineStyles;   // Qt::PenStyle

  static std::vector<std::string> s_default_colors;
  static std::vector<std::string> s_default_pen_colors;
  static std::vector<int>  s_default_lineWidths;
  static std::vector<int>  s_default_curveSymbol;
  static std::vector<int>  s_default_curveSymbolSize;
  static std::vector<int>  s_default_penStyles;
};

#endif
