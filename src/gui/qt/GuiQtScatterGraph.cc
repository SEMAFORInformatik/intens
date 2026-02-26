#if HAVE_QGRAPHS
#include <QPrinter>
#include <QPrintDialog>
#include <QContextMenuEvent>
#include <QtGraphsWidgets/Q3DScatterWidgetItem>
#include <QtGraphs/QScatter3DSeries>
#include <qwt_color_map.h>

#include "GuiQtScatterGraph.h"
#include "GuiQt3dData.h"
#include "GuiQt3dPlot.h"

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiQtScatterGraph::GuiQtScatterGraph(GuiQt3dPlot* plot) :
  m_plot(plot){

  m_scatter = new Q3DScatterWidgetItem();
  m_scatter->setWidget(this);
  m_scatter->widget()->setMinimumSize(QSize(256, 256));
  m_scatter->setHorizontalAspectRatio(1);  // same length x and y(z) axes

  // config axis
  m_scatter->setAxisX(new QValue3DAxis);
  m_scatter->setAxisY(new QValue3DAxis);
  m_scatter->setAxisZ(new QValue3DAxis);
  m_scatter->axisX()->setTitleVisible(true);
  m_scatter->axisY()->setTitleVisible(true);
  m_scatter->axisZ()->setTitleVisible(true);

  m_series = new QScatter3DSeries;
  m_scatter->addSeries(m_series);

  // Optional: Theme
  auto *theme = m_scatter->activeTheme();
  theme->setLabelFont(QFont(QStringLiteral("Impact"), 18));
  m_scatter->setActiveTheme(theme);

  setPlotStyle(plot->getPlotStyle());

  // Choose a nice gradient
  QLinearGradient gradient;
  gradient.setColorAt(0.0, Qt::blue);
  gradient.setColorAt(0.2, Qt::green);
  gradient.setColorAt(0.8, QColorConstants::Svg::orange);
  gradient.setColorAt(1.0, Qt::red);
  m_series->setBaseGradient(gradient);
  m_series->setColorStyle(QGraphsTheme::ColorStyle::RangeGradient);
}

/* --------------------------------------------------------------------------- */
/* setData --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::setData(GuiQt3dData& data) {
  const auto& dataArray = data.getScatterDataArray();
  if (dataArray.empty()) return;

  // NOTE we swapped Y <==> Z axis
  GuiQt3dData::Interval interval = data.getXInterval();
  if(interval.min == std::numeric_limits<double>::max() ||
     interval.max == std::numeric_limits<double>::min())
    return;
  m_scatter->axisX()->setRange(interval.min, interval.max);
  interval = data.getYInterval();
  m_scatter->axisZ()->setRange(interval.min, interval.max);
  interval = data.getZInterval();
  m_scatter->axisY()->setRange(interval.min, interval.max);

  m_series->dataProxy()->resetArray(dataArray);
  m_series->setDataArray(dataArray);
}

/* --------------------------------------------------------------------------- */
/* contextMenuEvent --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::contextMenuEvent ( QContextMenuEvent* event ){
  if (event->modifiers() == Qt::NoModifier) {
    m_plot->popupMenu(event);
    event->accept();
  } else {
    m_plot->hideConfigWidget();
    QQuickWidget::contextMenuEvent(event);
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::print(QPaintDevice& pd) {
  render(&pd, QPoint(), QRegion());
}

/* --------------------------------------------------------------------------- */
/* printPlot --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::printPlot()
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
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::update(GuiQt3dData& data) {
  setData( data );
}

/* --------------------------------------------------------------------------- */
/* axisWidget --                                                               */
/* --------------------------------------------------------------------------- */
QAbstract3DAxis* GuiQtScatterGraph::axisWidget(GuiElement::GuiAxisType axis) const {
  // NOTE we swapped Y <==> Z axis
  switch(axis){
  case GuiElement::yLeft:
  case GuiElement::yRight:
    return m_scatter->axisZ();
  case GuiElement::xBottom:
  case GuiElement::xTop:
    return m_scatter->axisX();
  case GuiElement::zAxis:
    return m_scatter->axisY();
  }
  return 0;
}


/* --------------------------------------------------------------------------- */
/* setPlotStyle --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::setPlotStyle(Gui3dPlot::Style plotStyle){
  switch(plotStyle){
  case Gui3dPlot::BAR:
    m_scatter->setOrthoProjection(false);
    m_scatter->setCameraZoomLevel(80.0f); // 85%
    m_scatter->setCameraXRotation(45.0f);
    m_scatter->setCameraYRotation(45.0f);
    break;
  case Gui3dPlot::SCATTER:
    m_scatter->setOrthoProjection(false);
    m_scatter->setCameraZoomLevel(75.0f); // 85%
    m_scatter->setCameraXRotation(45.0f);
    m_scatter->setCameraYRotation(45.0f);
    break;
  case Gui3dPlot::CONTOUR:
    m_scatter->setOrthoProjection(true);
    m_scatter->setCameraZoomLevel(100.0f);
    m_scatter->setCameraXRotation(0.0f);
    m_scatter->setCameraYRotation(90.0f);
    QQuickWidget::update();
    updateGeometry();
    break;
  }
}

/* --------------------------------------------------------------------------- */
/*getQwtColorMap --                                                            */
/* --------------------------------------------------------------------------- */
QwtColorMap* GuiQtScatterGraph::getQwtColorMap(){
  QLinearGradient gradient =  m_series->baseGradient();
  auto cm = gradient.stops();
  QwtLinearColorMap* colormap = new QwtLinearColorMap(/*cm.first().first, cm.back().first*/);
  int i(0);
  for (const auto &stop : gradient.stops()){
    //    if (stop == cm.first()) continue;
    //    if (stop == cm.back()) continue;
    colormap->addColorStop(stop.first, stop.second);
  }
  return colormap;
}

/* --------------------------------------------------------------------------- */
/* getConfigData --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::getConfigData(ConfigData& configData){
  configData.rotationX = m_scatter->cameraXRotation();
  configData.rotationY = m_scatter->cameraYRotation();
  configData.zoom = m_scatter->cameraZoomLevel();

  configData.orthoProjection = m_scatter->isOrthoProjection();
  configData.selectionMode = m_scatter->selectionMode();
  configData.showGrid = m_scatter->activeTheme()->isGridVisible();
  configData.showSmooth = m_series->isMeshSmooth();

}

/* --------------------------------------------------------------------------- */
/* updateConfigData --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::updateConfigData(ConfigData& configData){
  m_scatter->setCameraXRotation(configData.rotationX);
  m_scatter->setCameraYRotation(configData.rotationY);
  m_scatter->setCameraZoomLevel(configData.zoom);

  m_scatter->setOrthoProjection(configData.orthoProjection);
  QtGraphs3D::SelectionFlags sm = static_cast<QtGraphs3D::SelectionFlags>(configData.selectionMode);
  m_scatter->setSelectionMode(static_cast<QtGraphs3D::SelectionFlags>(configData.selectionMode));
  m_scatter->activeTheme()->setGridVisible(configData.showGrid);
  m_series->setMeshSmooth(configData.showSmooth);

  if (configData.rangeMinX != std::numeric_limits<double>::quiet_NaN() &&
      configData.rangeMaxX != std::numeric_limits<double>::quiet_NaN())  {
    GuiQt3dData::Interval interval = m_plot->getPlot3dData()->getXInterval();
    double delta = interval.max - interval.min;
    // m_scatter->axisX()->setRange(interval.min + (configData.rangeMinX/100.) * delta,
    //                              interval.max);
    m_scatter->axisX()->setRange(configData.rangeMinX, configData.rangeMaxX);
  }
  if (configData.rangeMinY != std::numeric_limits<double>::quiet_NaN() &&
      configData.rangeMaxY != std::numeric_limits<double>::quiet_NaN())  {
    GuiQt3dData::Interval interval = m_plot->getPlot3dData()->getYInterval();
    double delta = interval.max - interval.min;
    // m_scatter->axisZ()->setRange(interval.min + (configData.rangeMinY/100.) * delta,
    //                              interval.max);
    m_scatter->axisZ()->setRange(configData.rangeMinY, configData.rangeMaxY);
  }
}

/* --------------------------------------------------------------------------- */
/* printLog --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtScatterGraph::printLog(){
  auto p = m_scatter->cameraTargetPosition();
  auto pn = p.normalized();
  //m_scatter->setAspectRatio(10);
  std::cout << " cameraTargetPosition: " << p.x() << ", " << p.y() << ", " << p.z() << std::endl;
return;
  std::cout << " camera rotation x: " << m_scatter->cameraXRotation() << " y: " << m_scatter->cameraYRotation() << ", zoom: " << m_scatter->cameraZoomLevel() << std::endl;
  std::cout << " camera rotation x: " << m_scatter->minCameraXRotation() << ", " << m_scatter->maxCameraXRotation() << std::endl;
  std::cout << " camera rotation y: " << m_scatter->minCameraYRotation() << ", " << m_scatter->maxCameraYRotation() << std::endl;
  std::cout << " aspectRatio: " << m_scatter->aspectRatio() << ", " << m_scatter->horizontalAspectRatio()  << std::endl;
  std::cout << " selectionMode: " << m_scatter->selectionMode()  << std::endl;
}

#endif
