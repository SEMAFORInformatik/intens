#if HAVE_QGRAPHS
#include <QPrinter>
#include <QPrintDialog>
#include <QContextMenuEvent>
#include <QtGraphs/QSurface3DSeries>
#include <qwt_color_map.h>

#include "GuiQtSurfaceGraph.h"
#include "GuiQt3dData.h"
#include "GuiQt3dPlot.h"

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiQtSurfaceGraph::GuiQtSurfaceGraph(GuiQt3dPlot* plot) :
  m_plot(plot){

  m_surface = new Q3DSurfaceWidgetItem();
  m_surface->setWidget(this);
  m_surface->widget()->setMinimumSize(QSize(256, 256));
  m_surface->setHorizontalAspectRatio(1);  // same length x and y(z) axes

  // config axis
  m_surface->setAxisX(new QValue3DAxis);
  m_surface->setAxisY(new QValue3DAxis);
  m_surface->setAxisZ(new QValue3DAxis);
  m_surface->axisX()->setTitleVisible(true);
  m_surface->axisY()->setTitleVisible(true);
  m_surface->axisZ()->setTitleVisible(true);

  m_series = new QSurface3DSeries;
  m_surface->addSeries(m_series);

  // Optional: Theme
  auto *theme = m_surface->activeTheme();
  theme->setLabelFont(QFont(QStringLiteral("Impact"), 18));
  m_surface->setActiveTheme(theme);

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
void GuiQtSurfaceGraph::setData(GuiQt3dData& data) {
  const auto& dataArray = data.getSurfaceDataArray();
  if (dataArray.empty()) return;

  // NOTE we swapped Y <==> Z axis
  GuiQt3dData::Interval interval = data.getXInterval();
  if(interval.min == std::numeric_limits<double>::max() ||
     interval.max == std::numeric_limits<double>::min())
    return;
  m_surface->axisX()->setRange(interval.min, interval.max);
  interval = data.getYInterval();
  m_surface->axisZ()->setRange(interval.min, interval.max);
  interval = data.getZInterval();
  m_surface->axisY()->setRange(interval.min, interval.max);

  m_series->dataProxy()->resetArray(dataArray);
  m_series->setDataArray(dataArray);
}

/* --------------------------------------------------------------------------- */
/* contextMenuEvent --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtSurfaceGraph::contextMenuEvent ( QContextMenuEvent* event ){
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
void GuiQtSurfaceGraph::print(QPaintDevice& pd) {
  render(&pd, QPoint(), QRegion());
}

/* --------------------------------------------------------------------------- */
/* printPlot --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtSurfaceGraph::printPlot()
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
void GuiQtSurfaceGraph::update(GuiQt3dData& data) {
  setData( data );
}

/* --------------------------------------------------------------------------- */
/* axisWidget --                                                               */
/* --------------------------------------------------------------------------- */
QAbstract3DAxis* GuiQtSurfaceGraph::axisWidget(GuiElement::GuiAxisType axis) const {
  // NOTE we swapped Y <==> Z axis
  switch(axis){
  case GuiElement::yLeft:
  case GuiElement::yRight:
    return m_surface->axisZ();
  case GuiElement::xBottom:
  case GuiElement::xTop:
    return m_surface->axisX();
  case GuiElement::zAxis:
    return m_surface->axisY();
  }
  return 0;
}


/* --------------------------------------------------------------------------- */
/* setPlotStyle --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtSurfaceGraph::setPlotStyle(Gui3dPlot::Style plotStyle){
  switch(plotStyle){
  case Gui3dPlot::BAR:
    m_surface->setOrthoProjection(false);
    m_surface->setCameraZoomLevel(80.0f); // 85%
    m_surface->setCameraXRotation(45.0f);
    m_surface->setCameraYRotation(45.0f);
    break;
  case Gui3dPlot::SURFACE:
    m_surface->setOrthoProjection(false);
    m_surface->setCameraZoomLevel(75.0f); // 85%
    m_surface->setCameraXRotation(45.0f);
    m_surface->setCameraYRotation(45.0f);
    break;
  case Gui3dPlot::CONTOUR:
    m_surface->setOrthoProjection(true);
    m_surface->setCameraZoomLevel(100.0f);
    m_surface->setCameraXRotation(0.0f);
    m_surface->setCameraYRotation(90.0f);
    QQuickWidget::update();
    updateGeometry();
    break;
  }
}

/* --------------------------------------------------------------------------- */
/*getQwtColorMap --                                                            */
/* --------------------------------------------------------------------------- */
QwtColorMap* GuiQtSurfaceGraph::getQwtColorMap(){
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
void GuiQtSurfaceGraph::getConfigData(ConfigData& configData){
  configData.rotationX = m_surface->cameraXRotation();
  configData.rotationY = m_surface->cameraYRotation();
  configData.zoom = m_surface->cameraZoomLevel();

  configData.orthoProjection = m_surface->isOrthoProjection();
  configData.selectionMode = m_surface->selectionMode();
  configData.showGrid = m_surface->activeTheme()->isGridVisible();
  configData.showMesh = m_series->drawMode() & QSurface3DSeries::DrawWireframe;
  configData.showSmooth = m_series->isMeshSmooth();

}

/* --------------------------------------------------------------------------- */
/* updateConfigData --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtSurfaceGraph::updateConfigData(ConfigData& configData){
  m_surface->setCameraXRotation(configData.rotationX);
  m_surface->setCameraYRotation(configData.rotationY);
  m_surface->setCameraZoomLevel(configData.zoom);

  m_surface->setOrthoProjection(configData.orthoProjection);
  QtGraphs3D::SelectionFlags sm = static_cast<QtGraphs3D::SelectionFlags>(configData.selectionMode);
  m_surface->setSelectionMode(static_cast<QtGraphs3D::SelectionFlags>(configData.selectionMode));
  m_surface->activeTheme()->setGridVisible(configData.showGrid);
  if (configData.showMesh)
    m_series->setDrawMode(m_series->drawMode()|QSurface3DSeries::DrawWireframe);
  else{
    auto dm = m_series->drawMode();
    dm &= ~QSurface3DSeries::DrawWireframe;
    m_series->setDrawMode(dm);
  }
  m_series->setMeshSmooth(configData.showSmooth);

  if (configData.rangeMinX != std::numeric_limits<double>::quiet_NaN() &&
      configData.rangeMaxX != std::numeric_limits<double>::quiet_NaN())  {
    GuiQt3dData::Interval interval = m_plot->getPlot3dData()->getXInterval();
    double delta = interval.max - interval.min;
    // m_surface->axisX()->setRange(interval.min + (configData.rangeMinX/100.) * delta,
    //                              interval.max);
    m_surface->axisX()->setRange(configData.rangeMinX, configData.rangeMaxX);
  }
  if (configData.rangeMinY != std::numeric_limits<double>::quiet_NaN() &&
      configData.rangeMaxY != std::numeric_limits<double>::quiet_NaN())  {
    GuiQt3dData::Interval interval = m_plot->getPlot3dData()->getYInterval();
    double delta = interval.max - interval.min;
    // m_surface->axisZ()->setRange(interval.min + (configData.rangeMinY/100.) * delta,
    //                              interval.max);
    m_surface->axisZ()->setRange(configData.rangeMinY, configData.rangeMaxY);
  }
}

/* --------------------------------------------------------------------------- */
/* printLog --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtSurfaceGraph::printLog(){
  auto p = m_surface->cameraTargetPosition();
  auto pn = p.normalized();
  //m_surface->setAspectRatio(10);
  std::cout << " cameraTargetPosition: " << p.x() << ", " << p.y() << ", " << p.z() << std::endl;
return;
  std::cout << " camera rotation x: " << m_surface->cameraXRotation() << " y: " << m_surface->cameraYRotation() << ", zoom: " << m_surface->cameraZoomLevel() << std::endl;
  std::cout << " camera rotation x: " << m_surface->minCameraXRotation() << ", " << m_surface->maxCameraXRotation() << std::endl;
  std::cout << " camera rotation y: " << m_surface->minCameraYRotation() << ", " << m_surface->maxCameraYRotation() << std::endl;
  std::cout << " aspectRatio: " << m_surface->aspectRatio() << ", " << m_surface->horizontalAspectRatio()  << std::endl;
  std::cout << " selectionMode: " << m_surface->selectionMode()  << std::endl;
}

/* --------------------------------------------------------------------------- */
/* printLog --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtSurfaceGraph::setTitle(const std::string title){
  setWindowTitle(QString::fromStdString(title));
}
#endif
