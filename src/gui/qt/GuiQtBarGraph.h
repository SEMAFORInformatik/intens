#ifndef GUIQTBARGRAPH_H_
#define GUIQTBARGRAPH_H_
#if HAVE_QGRAPHS

#include <QtQuickWidgets/QQuickWidget>
#include "gui/GuiElement.h"
#include "gui/Gui3dPlot.h"
#include "gui/qt/GuiQt3dBasePlot.h"

class GuiQt3dData;
class GuiQt3dPlot;
class Q3DBarsWidgetItem;
class QBar3DSeries;
class QAbstract3DAxis;

class GuiQtBarGraph : public QQuickWidget, public GuiQt3dBasePlot
  {
  public:
    GuiQtBarGraph(GuiQt3dPlot* plot);

    /** update data
        @param data plot data
     */
    void update(GuiQt3dData& data);

    /** returns axis
        @param atype axis type
        @return pointer to axis
    */
    QAbstract3DAxis *axisWidget(GuiElement::GuiAxisType atype) const;

    /**  print to paint device (eg. Printer)
        @param paintdevice
     */
    void print(QPaintDevice& paintdevice);
    void printLog();
    QWidget* myWidget() {return this;}

  private:
    /** set new data */
    void setData(GuiQt3dData& data);

    /** contextMenuEvent */
    void contextMenuEvent ( QContextMenuEvent* event );


#ifndef QT_NO_PRINTER
    void printPlot();
#endif

  private:
    GuiQtBarGraph();
    GuiQtBarGraph(const GuiQtBarGraph& );

  private:
    QBar3DSeries*      m_series;
    Q3DBarsWidgetItem* m_bars;
    GuiQt3dPlot       *m_plot;
  };
#endif // HAVE_QGRAPHS
#endif // !defined(GUIQTBARGRAPH_H_)
