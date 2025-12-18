#if HAVE_QGRAPHS
#include <QPrinter>
#include <QPrintDialog>
#include <QContextMenuEvent>

#include "GuiQtGraphsPlot.h"
#include "GuiQtGraphsPlotData.h"
#include "GuiQt3dPlot.h"

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiQtGraphsPlot::GuiQtGraphsPlot(GuiQt3dPlot* plot, GuiQtGraphsPlotData* _data) :
  currentEditedAxisLabelId(-1)
  , m_data(_data)
  , m_plot(plot){
  m_surface = new Q3DSurfaceWidgetItem();
  m_surface->setWidget(this);
  m_surface->widget()->setMinimumSize(QSize(756, 756));

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
  auto *theme = new QGraphsTheme/*Q3DTheme(Q3DTheme::ThemeQt)*/;
  // theme->setTheme(QGraphsTheme::Theme::UserDefined);
  // theme->setBackgroundColor(QColor(QRgb(0x99ca53)));
  // theme->setBackgroundVisible(true);
  // QList<QColor> colors = { QColor(QRgb(0x209fdf)) };
  // theme->setSeriesColors(colors);
  // theme->setColorStyle(QGraphsTheme::ColorStyle::Uniform);
  theme->setLabelFont(QFont(QStringLiteral("Impact"), 20));
  // theme->setGridVisible(true);
  // auto gridline = theme->grid();
  // gridline.setMainColor(QColor(QRgb(0x99ca53)));
  // theme->setGrid(gridline);
  // theme->setLabelBackgroundColor(QColor(0xf6, 0xa6, 0x25, 0xa0));
  // theme->setLabelBackgroundVisible(true);
  // theme->setLabelBorderVisible(true);
  // theme->setLabelTextColor(QColor(QRgb(0x404044)));
  // theme->setMultiHighlightColor(QColor(QRgb(0x6d5fd5)));
  // theme->setSingleHighlightColor(QColor(QRgb(0xf6a625)));
  // theme->setBackgroundColor(QColor(QRgb(0xffffff)));
  m_surface->setActiveTheme(theme);

  // camera zoom rotation
  if (plot->getPlotStyle() == Gui3dPlot::CONTOUR){
    ///    m_surface->setCameraZoomLevel(150.0f);
    m_surface->setCameraYRotation(90.0f);
    m_surface->setOrthoProjection(true);
  }else{
    m_surface->setCameraZoomLevel(85.0f); // 85%
    m_surface->setCameraXRotation(45.0f);
    m_surface->setCameraYRotation(45.0f);
  }

  // Choose a nice gradient
  QLinearGradient gradient;
  gradient.setColorAt(0.0, Qt::blue);
  gradient.setColorAt(0.2, Qt::green);
  gradient.setColorAt(0.8, QColorConstants::Svg::orange);
  gradient.setColorAt(1.0, Qt::red);
  m_series->setBaseGradient(gradient);
  m_series->setColorStyle(QGraphsTheme::ColorStyle::RangeGradient);

  // setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,  QSizePolicy::MinimumExpanding ) );
}

/* --------------------------------------------------------------------------- */
/* setData --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlot::setData(GuiQtGraphsPlotData* data) {
  m_data = data;

  // NOTE we changed Y <==> Z axis
  GuiQtGraphsPlotData::Interval interval = data->getXInterval();
  m_surface->axisX()->setRange(interval.min, interval.max);
  interval = data->getYInterval();
  m_surface->axisZ()->setRange(interval.min, interval.max);
  interval = data->getZInterval();
  m_surface->axisY()->setRange(interval.min, interval.max);

  m_series->dataProxy()->resetArray(*data);
  m_series->setDataArray(*data);
}

/* --------------------------------------------------------------------------- */
/* contextMenuEvent --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlot::contextMenuEvent ( QContextMenuEvent* event ){
  m_plot->popupMenu(event);
  event->ignore();
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlot::print(QPaintDevice& pd) {
}

/* --------------------------------------------------------------------------- */
/* printPlot --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlot::printPlot()
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
void GuiQtGraphsPlot::update(GuiQtGraphsPlotData& data) {
  if (data.size())
    setData( &data );
}

/* --------------------------------------------------------------------------- */
/* axisWidget --                                                               */
/* --------------------------------------------------------------------------- */
QValue3DAxis* GuiQtGraphsPlot::axisWidget(GuiElement::GuiAxisType axis) const {
  // NOTE we changed Y <==> Z axis
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
#endif
