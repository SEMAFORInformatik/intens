#if HAVE_QGRAPHS
#include <QPrinter>
#include <QPrintDialog>
#include <QContextMenuEvent>

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
    QQuickWidget::contextMenuEvent(event);
  }
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtSurfaceGraph::print(QPaintDevice& pd) {
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
/* axisWidget --                                                               */
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

void GuiQtSurfaceGraph::printLog(){
  auto p = m_surface->cameraTargetPosition();
  auto pn = p.normalized();
  //m_surface->setAspectRatio(10);
  std::cout << " cameraTargetPosition: " << p.x() << ", " << p.y()
            << ", " << p.z() << std::endl;
  std::cout << " camera rotation x: " << m_surface->cameraXRotation() << " y: " << m_surface->cameraYRotation() << ", zoom: " << m_surface->cameraZoomLevel() << std::endl;
  std::cout << " camera rotation x: " << m_surface->minCameraXRotation() << ", " << m_surface->maxCameraXRotation() << std::endl;
  std::cout << " camera rotation y: " << m_surface->minCameraYRotation() << ", " << m_surface->maxCameraYRotation() << std::endl;
  std::cout << " aspectRatio: " << m_surface->aspectRatio() << ", " << m_surface->horizontalAspectRatio()  << std::endl;
}

#endif
