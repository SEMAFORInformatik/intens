#ifndef GUIQTSURFACEGRAPH_H_
#define GUIQTSURFACEGRAPH_H_
#if HAVE_QGRAPHS

#include <QtQuickWidgets/QQuickWidget>
#include "gui/GuiElement.h"
#include "gui/Gui3dPlot.h"
#include "gui/qt/GuiQt3dBasePlot.h"

class GuiQt3dPlot;
class GuiQt3dData;
class Q3DSurfaceWidgetItem;
class QSurface3DSeries;
class QValue3DAxis;
class QwtColorMap;

class GuiQtSurfaceGraph : public QQuickWidget, public GuiQt3dBasePlot
  {
  public:
    GuiQtSurfaceGraph(GuiQt3dPlot* plot);

    void update(GuiQt3dData& data);
    QAbstract3DAxis *axisWidget(GuiElement::GuiAxisType atype) const;

    /**  print to paint device (eg. Printer)
        @param paintdevice
     */
    void print(QPaintDevice& paintdevice);

    /**  set surface or contour plot style
        @param plotStyle
     */
    void setPlotStyle(Gui3dPlot::Style plotStyle);
    /**  get qwt colormap
     */
    QwtColorMap* getQwtColorMap();

    void getConfigData(ConfigData& configData);
    void updateConfigData(ConfigData& configData);

    void printLog();
    ///    QWidget* myWidget() {return this;}

  private:
    /** set new data */
    void setData(GuiQt3dData& data);

    /** contextMenuEvent */
    void contextMenuEvent ( QContextMenuEvent* event );


#ifndef QT_NO_PRINTER
    void printPlot();
#endif

  private:
    GuiQtSurfaceGraph();
    GuiQtSurfaceGraph(const GuiQtSurfaceGraph& );

  private:
    QSurface3DSeries*     m_series;
    Q3DSurfaceWidgetItem* m_surface;
    GuiQt3dPlot          *m_plot;
  };
#endif // HAVE_QGRAPHS
#endif // !defined(GUIQTCONTOURPLOT_H__INCLUDED_)
