#if HAVE_QGRAPHS
#include <QPrinter>
#include <QPrintDialog>
#include <QContextMenuEvent>
#include <QtGraphsWidgets/Q3DBarsWidgetItem>

#include "GuiQtBarGraph.h"
#include "GuiQt3dData.h"
#include "GuiQt3dPlot.h"

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiQtBarGraph::GuiQtBarGraph(GuiQt3dPlot* plot) :
  m_plot(plot){
  m_bars = new Q3DBarsWidgetItem();
  m_bars->setWidget(this);
  m_bars->widget()->setMinimumSize(QSize(256, 256));

  // config axis
  m_bars->rowAxis()->setTitleVisible(true);
  m_bars->columnAxis()->setTitleVisible(true);
  m_bars->valueAxis()->setTitleVisible(true);

  m_series = new QBar3DSeries;
  m_bars->addSeries(m_series);

  // Optional: Theme
  auto *theme = m_bars->activeTheme();
  theme->setLabelFont(QFont(QStringLiteral("Impact"), 18));
  m_bars->setActiveTheme(theme);
}

/* --------------------------------------------------------------------------- */
/* setData --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtBarGraph::setData(GuiQt3dData& data) {
  if (false) {
    QBarDataRow data;
    data << QBarDataItem(1.0f) << QBarDataItem(3.0f) << QBarDataItem(17.5f) << QBarDataItem(5.0f)
         << QBarDataItem(2.2f);
    m_series->dataProxy()->addRow(data);
  }
  QStringList row_labels;
  QStringList column_labels;
  const QBarDataArray& dataArray = data.getBarDataArray(row_labels, column_labels);
  if (dataArray.empty()) return;

  // NOTE we swapped Y <==> Z axis
  GuiQt3dData::Interval interval = data.getXInterval();
  if(interval.min == std::numeric_limits<double>::max() ||
     interval.max == std::numeric_limits<double>::min())
    return;

  m_bars->rowAxis()->setRange(0, row_labels.size());
  m_bars->columnAxis()->setRange(0, column_labels.size());
  m_series->dataProxy()->resetArray(dataArray, row_labels, column_labels);
  m_series->setDataArray(dataArray);
}

/* --------------------------------------------------------------------------- */
/* contextMenuEvent --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtBarGraph::contextMenuEvent ( QContextMenuEvent* event ){
  if (event->modifiers() == Qt::NoModifier) {
    m_plot->popupMenu(event);
    event->accept();
  } else {
    QQuickWidget::contextMenuEvent(event);
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtBarGraph::print(QPaintDevice& pd) {
}

/* --------------------------------------------------------------------------- */
/* printPlot --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtBarGraph::printPlot()
{
  QPrinter printer;
  printer.setPageOrientation(QPageLayout::Landscape);
  printer.setOutputFileName("image.pdf");
  QPrintDialog dialog(&printer);
  if ( dialog.exec()  == QDialog::Accepted) {
  }
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* ------------------------
   --------------------------------------------------- */
void GuiQtBarGraph::update(GuiQt3dData& data) {
  setData( data );
}

/* --------------------------------------------------------------------------- */
/* axisWidget --                                                               */
/* --------------------------------------------------------------------------- */
QAbstract3DAxis* GuiQtBarGraph::axisWidget(GuiElement::GuiAxisType axis) const {
  // NOTE we swapped Y <==> Z axis
  switch(axis){
  case GuiElement::yLeft:
  case GuiElement::yRight:
    return m_bars->columnAxis();
  case GuiElement::xBottom:
  case GuiElement::xTop:
    return m_bars->rowAxis();
  case GuiElement::zAxis:
    return m_bars->valueAxis();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* printLog --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtBarGraph::printLog(){
  auto p = m_bars->cameraTargetPosition();
  auto pn = p.normalized();
  //m_bars->setAspectRatio(10);
  std::cout << " cameraTargetPosition: " << p.x() << ", " << p.y()
            << ", " << p.z() << std::endl;
  std::cout << " camera rotation x: " << m_bars->cameraXRotation() << " y: " << m_bars->cameraYRotation() << ", zoom: " << m_bars->cameraZoomLevel() << std::endl;
  std::cout << " camera rotation x: " << m_bars->minCameraXRotation() << ", " << m_bars->maxCameraXRotation() << std::endl;
  std::cout << " camera rotation y: " << m_bars->minCameraYRotation() << ", " << m_bars->maxCameraYRotation() << std::endl;
  std::cout << " aspectRatio: " << m_bars->aspectRatio() << ", " << m_bars->horizontalAspectRatio()  << std::endl;
}

#endif
