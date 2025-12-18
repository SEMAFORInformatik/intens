#ifndef GUIQTGRAPHSPLOT_H_
#define GUIQTGRAPHSPLOT_H_
#if HAVE_QGRAPHS

#include <QtQuickWidgets/QQuickWidget>
#include <QtGraphsWidgets/q3dsurfacewidgetitem.h>
#include "gui/GuiElement.h"

class GuiQtGraphsPlotData;
class GuiQt3dPlot;
class QSurface3DSeries;

class GuiQtGraphsPlot : public QQuickWidget
  {
  public:
    GuiQtGraphsPlot(GuiQt3dPlot* plot, GuiQtGraphsPlotData* data);

    /** update data
        @param data plot data
     */
    void update(GuiQtGraphsPlotData& data);

    /** returns axis
        @param atype axis type
        @return pointer to axis
    */
    QValue3DAxis *axisWidget(GuiElement::GuiAxisType atype) const;

    /**  print to paint device (eg. Printer)
        @param paintdevice
     */
    void print(QPaintDevice& paintdevice);

  private:
    /** set new data */
    void setData(GuiQtGraphsPlotData* data);
    /** contextMenuEvent */
    void contextMenuEvent ( QContextMenuEvent* event );


#ifndef QT_NO_PRINTER
    void printPlot();
#endif

  private:
    GuiQtGraphsPlot();
    GuiQtGraphsPlot(const GuiQtGraphsPlot& );

  private:
    QSurface3DSeries*   m_series;
    Q3DSurfaceWidgetItem* m_surface;
    int                 currentEditedAxisLabelId;

    GuiQtGraphsPlotData *m_data;
    GuiQt3dPlot          *m_plot;
  };
#endif // HAVE_QGRAPHS
#endif // !defined(GUIQTCONTOURPLOT_H__INCLUDED_)
