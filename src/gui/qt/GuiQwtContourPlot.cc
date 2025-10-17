
#include <qwt_plot_spectrogram.h>

#include <QPrinter>
#include <QPrintDialog>
#include <QContextMenuEvent>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_text_label.h>
#include <qwt_interval.h>
#include <qwt_scale_map.h>
#if QWT_VERSION >= 0x060000
#include <qwt_plot_renderer.h>
#endif
#include "GuiQwtContourPlot.h"
#include "GuiQwtContourPlotData.h"
#include "GuiQwtPlotScalePicker.h"
#include "GuiQwtScaleDraw.h"
#include "GuiQwt3dPlot.h"
#include "gui/qt/QtMultiFontString.h"

GuiQwtContourPlot::GuiQwtContourPlot(GuiQwt3dPlot* plot, QwtLinearColorMap* colormap,
				     GuiQwtContourPlotData* data) :
  currentEditedAxisLabelId(-1)
  , m_data(data)
  , m_plot(plot)
{
#if QWT_VERSION >= 0x060000
  d_spectrogram = new QwtPlotSpectrogram();
  d_spectrogram->setRenderThreadCount(0); // use system specific thread count
  d_spectrogram->setColorMap( getColorMapClone(colormap) );

  d_spectrogram->setData(data);
  d_spectrogram->attach(this);
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,  QSizePolicy::MinimumExpanding ) );

  QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
  rightAxis->setMinBorderDist(10,10);
  rightAxis->setAcceptDrops (true);
  rightAxis->setColorBarEnabled(true);
  setAxisScale(QwtPlot::yRight, 0, 1);
  setAxisScale(QwtPlot::yLeft, 0, 1);
  setAxisScale(QwtPlot::xBottom, 0, 1);
  rightAxis->setColorMap( QwtInterval(0,1), colormap);
  enableAxis(QwtPlot::yRight);
  setAxisAutoScale (QwtPlot::yRight, true);

  // yRight
  GuiQwtScaleDraw* scaleDraw = new GuiQwtScaleDraw(GuiQwtScaleDraw::type_colormap);
  scaleDraw->setColorMap( colormap );
  setAxisScaleDraw( QwtPlot::yRight,scaleDraw);
  showSpectrogram(true);
  showContour(true);

  plotLayout()->setAlignCanvasToScales(true);

  // Zoomer
  zoomer = new MyZoomer(this, canvas());
  connect(zoomer, SIGNAL(zoomed (const QRectF&)),
	  SLOT(slot_zoomerRect(const QRectF&)) );

  QwtPlotPanner *panner = new QwtPlotPanner(canvas());
  panner->setAxisEnabled(QwtPlot::yRight, false);
  panner->setMouseButton(Qt::MiddleButton);

  // Avoid jumping when labels with more/less digits
  // appear/disappear when scrolling vertically

  const QFontMetrics fm(axisWidget(QwtPlot::yRight)->font());
  QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yRight);
  sd->setMinimumExtent( fm.horizontalAdvance("100.1234") );

  const QColor c(Qt::darkBlue);
  zoomer->setRubberBandPen(c);
  zoomer->setTrackerPen(c);

  // create scalePicker and connect majorTickDblCklicked slot
  GuiQwtPlotScalePicker* scalePicker = new GuiQwtPlotScalePicker(this);
  connect(scalePicker, SIGNAL(axisMajorTickDblClicked(QwtScaleWidget*, double, QRect)),
	  SLOT(slot_axisMajorTickDblClicked(QwtScaleWidget*, double, QRect)) );
  // connect colorstop slots
  connect(scalePicker, SIGNAL(colorStopPositionChanged(QwtScaleWidget*, double, int)),
	  SLOT(slot_colorStopPositionChanged(QwtScaleWidget*, double, int)) );
  connect(scalePicker, SIGNAL(colorStopColorChanged(QwtScaleWidget*, QColor, int)),
	  SLOT(slot_colorStopColorChanged(QwtScaleWidget*, QColor, int)) );
  connect(scalePicker, SIGNAL(colorStopColorNew(QwtScaleWidget*, QColor, double)),
	  SLOT(slot_colorStopColorNew(QwtScaleWidget*, QColor, double)) );
  connect(scalePicker, SIGNAL(colorStopColorDelete(QwtScaleWidget*, int)),
	  SLOT(slot_colorStopColorDelete(QwtScaleWidget*, int)) );
#endif

  // set fonts
  // title
  QFont font = titleLabel()->font();
  titleLabel()->setFont( QtMultiFontString::getQFont( "@plotTitle@", font ) );
  // plotAxis and plotAxisTitle
  QwtText pa;
  font =  QtMultiFontString::getQFont( "@plotAxis@", font );
  pa.setFont(QtMultiFontString::getQFont( "@plotAxisTitle@", font ));
  for (int atype=0; atype< axisCnt; ++atype) {
    setAxisFont( atype, font );
    setAxisTitle( atype, pa );
  }

}

void GuiQwtContourPlot::setData(GuiQwtContourPlotData* data) {
#if QWT_VERSION >= 0x060000
  m_data = data;

  bool b = axisAutoScale (QwtPlot::yRight);
  d_spectrogram->setData(data);
  const QwtInterval&  yaxis = data->getInterval(Qt::YAxis);
  if (yaxis.isValid()) {
    setAxisScale( QwtPlot::yLeft, yaxis.minValue(), yaxis.maxValue());
  }
  const QwtInterval&  xaxis = data->getInterval(Qt::XAxis);
  if (xaxis.isValid()) {
    setAxisScale( QwtPlot::xBottom, xaxis.minValue(), xaxis.maxValue());
  }

  setAxisAutoScale (QwtPlot::yRight, b);
  updateZAxis(data);

  // set zoomer base
  zoomer->setZoomBase();
#endif
}

QwtLinearColorMap*  GuiQwtContourPlot::getColorMapClone(const QwtLinearColorMap* colormap) {
#if QWT_VERSION >= 0x060000
  QwtLinearColorMap* map = new QwtLinearColorMap(colormap->color1(), colormap->color2());
  QVector<double> cStops = colormap->colorStops();
  for (int i = 1; i < cStops.size()-1; i++){
    map->addColorStop (cStops.at(i),
		       colormap->color(QwtInterval(cStops.front(),cStops.back()),
				       cStops.at(i)));
  }
  return map;
#endif
}

void GuiQwtContourPlot::setNewColorStops(QwtScaleWidget* scale,
					 QwtLinearColorMap *newColorMap) {
#if QWT_VERSION >= 0x060000
  // set new colormap
  scale->setColorMap(scale->colorBarInterval(), newColorMap);
  d_spectrogram->setColorMap( getColorMapClone(newColorMap) );
  GuiQwtScaleDraw* cplot = dynamic_cast<GuiQwtScaleDraw*>(scale->scaleDraw());
  if (cplot)
    cplot->setColorMap( newColorMap );
  replot();
#endif
}

void GuiQwtContourPlot::setNewColorStops(QwtScaleWidget* scale,
					 const QwtLinearColorMap *colorMap,
					 const QVector< double >& colorsNew,
					 const QVector< double >& colorsOld) {
#if QWT_VERSION >= 0x060000
  // create new colormap
  const QwtInterval interval = scale->colorBarInterval();
  QwtLinearColorMap* newColorMap = new QwtLinearColorMap(colorMap->color1(), colorMap->color2());
  for (int i=1; i < colorsNew.size()-1; ++i) {
    newColorMap->addColorStop( colorsNew.at(i), colorMap->color(interval, colorsOld.at(i)) );
  }

  // set new colormap
  setNewColorStops(scale, newColorMap);
#endif
}

void GuiQwtContourPlot::updateZAxis(GuiQwtContourPlotData* data) {
#if QWT_VERSION >= 0x060000
  if (!data) return;

  const QwtInterval&  zaxis = data->interval(Qt::ZAxis);
  if (zaxis.isValid() && axisAutoScale(QwtPlot::yRight)) {
    setAxisScale(QwtPlot::yRight, zaxis.minValue(), zaxis.maxValue());
    updateAxes();
    setAxisAutoScale (QwtPlot::yRight, true);

    QwtScaleWidget* scaleWidget = axisWidget(QwtPlot::yRight);
    QwtLinearColorMap* newColorMap = getColorMapClone((const QwtLinearColorMap*)scaleWidget->colorMap());
    dynamic_cast<GuiQwtScaleDraw*>(axisScaleDraw(QwtPlot::yRight))->setColorMap( newColorMap );

    axisScaleDraw(QwtPlot::yRight)->scaleMap().setScaleInterval(0,1);
  }
#endif
}

void GuiQwtContourPlot::zoomBase() {
#if QWT_VERSION >= 0x060000
  if (!m_data)
    return;
  QRectF rect = m_data->getZoomBase();

  zoomer->zoom(rect);
  zoomer->setZoomBase();
#endif
}

void GuiQwtContourPlot::resetScale() {
  zoomBase();
  zoomer->zoom(0);
}

void GuiQwtContourPlot::contextMenuEvent ( QContextMenuEvent* event ){
  m_plot->popupMenu(event);
  event->ignore();
}

void GuiQwtContourPlot::zoomOut(double factor) {
  if (!m_data)
    return;
  QRectF rect = zoomer->zoomRect();
  double scaleFactor = factor;
  double w=rect.width();
  double h=rect.height();
  rect.setX( rect.x() - w/scaleFactor );
  rect.setY( rect.y() - h/scaleFactor );
  rect.setHeight( scaleFactor * h );
  rect.setWidth( scaleFactor * w );

  zoomer->zoom(rect);
  zoomer->setZoomBase();
}

void GuiQwtContourPlot::slot_zoomerRect(const QRectF &rect) {
}

void GuiQwtContourPlot::slot_axisMajorTickDblClicked(QwtScaleWidget* scale, double value, QRect boundingRect) {
#if QWT_VERSION >= 0x060000
  if (!scale ||
      (m_data && !m_data->isInteractiveZUserScale()))
    return;
  const QwtLinearColorMap* linearColormap = dynamic_cast<const QwtLinearColorMap*>
    (scale->colorMap());
  // only interested in linear colormaps
  if (!linearColormap) return;

  // loop to get clicked label
  QVector< double > colors = linearColormap->colorStops();
  for (int i=0; i < colors.size()-0; ++i) {

    if (colors.at(i) == value) {
      currentEditedAxisLabelId = i;

      // move boundingBox for EditLine
      if (colors.at(0) == value) {
	boundingRect.moveBottom(boundingRect.bottom()+height());
      } else
	if (colors[colors.size()-1] == value) {
	  boundingRect.moveTop(boundingRect.top()-height());
	} else {
	  boundingRect.moveTop(boundingRect.top()+3);
	}

      // get scaled value to edit
      const QwtInterval interval = scale->colorBarInterval();
      const QwtInterval intervalDraw = scale->scaleDraw()->scaleDiv().interval();
      value = intervalDraw.width()*(value-interval.minValue())/interval.width()+intervalDraw.minValue();
      if (value<1e-10)  // minor improvement
	value = 0.;

      // create editLine
      QwtPlotScaleLineEdit *le = new QwtPlotScaleLineEdit(this, scale, boundingRect, value, true);
      connect( le, SIGNAL(finished(QwtScaleWidget*, double)),
	       SLOT(slot_EditingFinished(QwtScaleWidget*, double)) );
      connect( le, SIGNAL(resetScale(QwtScaleWidget*)),
	       SLOT(slot_EditingResetScale(QwtScaleWidget*)) );
      break;
    }
  }
#endif
}

void GuiQwtContourPlot::slot_colorStopPositionChanged(QwtScaleWidget* scale, double newValue, int tickIdx) {
#if QWT_VERSION >= 0x060000
  // colormap
  if (scale->colorMap()) {
    const QwtLinearColorMap *oldColorMap  =  dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
    QVector< double > colorsOld =  oldColorMap->colorStops();
    QVector< double > colors =  colorsOld;
    // modify value
    colors[tickIdx] = newValue;

    // create new colormap
    setNewColorStops(scale, oldColorMap, colors, colorsOld);
  }
#endif
}

void GuiQwtContourPlot::slot_colorStopColorChanged(QwtScaleWidget* scale, QColor newColor, int tickIdx) {
#if QWT_VERSION >= 0x060000
  QwtLinearColorMap* newColorMap;

  // colormap
  if (scale->colorMap()) {
    const QwtLinearColorMap *colorMap  =  dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
    QVector< double > colors =  colorMap->colorStops();

    // create new colormap
    const QwtInterval interval = scale->colorBarInterval();
    newColorMap = new QwtLinearColorMap(tickIdx == 0 ? newColor : colorMap->color1(),
					tickIdx == colors.size()-1 ? newColor : colorMap->color2());
    for (int i=1; i < colors.size()-1; ++i) {
      if (i == tickIdx)
	newColorMap->addColorStop( colors.at(i), newColor );
      else
	newColorMap->addColorStop( colors.at(i), colorMap->color(interval, colors.at(i)) );
    }
    setNewColorStops(scale, newColorMap);
  }
#endif
}

void GuiQwtContourPlot::slot_colorStopColorNew(QwtScaleWidget* scale, QColor color, double value) {
#if QWT_VERSION >= 0x060000
  // colormap
  if (scale->colorMap()) {
    const QwtLinearColorMap *colorMap  =  dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
    QVector< double > colors =  colorMap->colorStops();

    // create new colormap
    const QwtInterval interval = scale->colorBarInterval();
    QwtLinearColorMap* newColorMap = new QwtLinearColorMap(colorMap->color1(), colorMap->color2());
    for (int i=1; i < colors.size()-1; ++i) {
      newColorMap->addColorStop( colors.at(i), colorMap->color(interval, colors.at(i)) );
    }
    newColorMap->addColorStop( value, color );
    setNewColorStops(scale, newColorMap);
  }
#endif
}

void GuiQwtContourPlot::slot_colorStopColorDelete(QwtScaleWidget* scale, int tickIdx) {
#if QWT_VERSION >= 0x060000
  // colormap
  if (scale->colorMap()) {
    const QwtLinearColorMap *colorMap  =  dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
    QVector< double > colors =  colorMap->colorStops();

    // create new colormap
    const QwtInterval interval = scale->colorBarInterval();
    QwtLinearColorMap* newColorMap = new QwtLinearColorMap(colorMap->color1(), colorMap->color2());
    for (int i=1; i < colors.size()-1; ++i) {
      if (i != tickIdx)
	newColorMap->addColorStop( colors.at(i), colorMap->color(interval, colors.at(i)) );
    }
    setNewColorStops(scale, newColorMap);
  }
#endif
}

Qt::Axis GuiQwtContourPlot::getQtAxisId(int i) {
  switch (i) {
  case QwtPlot::yLeft:
    return Qt::YAxis;
  case QwtPlot::yRight:
    return Qt::ZAxis;
  case QwtPlot::xTop:
  case QwtPlot::xBottom:
    return Qt::XAxis;
  default:
    return Qt::YAxis;
  }
}

void GuiQwtContourPlot::slot_EditingResetScale(QwtScaleWidget* scale) {
#if QWT_VERSION >= 0x060000
  if (scale->colorMap()) {
    m_data->resetZUser();
    updateZAxis(m_data);
  }
  if (!m_data) return;
  int i = 0;
  while (i < QwtPlot::axisCnt) {
    if (axisWidget(i) == scale) {
      setAxisAutoScale(i, true);
      break;
    }
    ++i;
  }
  replot();
#endif
}

void GuiQwtContourPlot::slot_EditingFinished(QwtScaleWidget* scale, double newValue) {
#if QWT_VERSION >= 0x060000
  // colormap
  if (scale->colorMap() && scale->alignment() == QwtScaleDraw::RightScale) {
    const QwtInterval intervalDraw = scale->scaleDraw()->scaleDiv().interval();
    const QwtLinearColorMap *oldColorMap  =  dynamic_cast<const QwtLinearColorMap*>(scale->colorMap());
    QVector< double > colorsOld =  oldColorMap->colorStops();
    QVector< double > colors =  colorsOld;

    // modify value
    if (currentEditedAxisLabelId >=0 && currentEditedAxisLabelId < colors.size()) {
      if (currentEditedAxisLabelId == 0) {
	if (m_data)
	  m_data->setMinZUser( newValue );
	updateZAxis(m_data);
      } else
	if (currentEditedAxisLabelId==colors.size()-1) {
	  if (m_data)
	    m_data->setMaxZUser( newValue );
	  updateZAxis(m_data);
	} else {
	  double newVal = (newValue-intervalDraw.minValue())/intervalDraw.width();
	  if (newVal > colors[0] && newVal < colors[colors.size()-1]) {
	    colors[currentEditedAxisLabelId] = newVal;
	  }
	}
    }

    // create new colormap
    setNewColorStops(scale, oldColorMap, colors, colorsOld);

    currentEditedAxisLabelId = -1;  // reset id

    replot();
  }
#endif
}

void GuiQwtContourPlot::showContour(bool on)
{
  d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, on);
  replot();
}

void GuiQwtContourPlot::showSpectrogram(bool on)
{
  d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, on);
  d_spectrogram->setDefaultContourPen(on ? QPen() : QPen(Qt::NoPen));
  replot();
}

#if QWT_VERSION >= 0x060000
void GuiQwtContourPlot::setResampleMode(QwtMatrixRasterData::ResampleMode mode) {
  if (m_data) {
    m_data->setResampleMode(mode);
    m_data->updateData();
  }
  replot();
}

void GuiQwtContourPlot::setColorMap(const QwtInterval& interval, QwtLinearColorMap* colormap) {
  if (d_spectrogram) {
    d_spectrogram->setColorMap(colormap);
    axisWidget(QwtPlot::yRight)->setColorMap( interval, colormap);
  }
}
#endif

void GuiQwtContourPlot::print(QPaintDevice& pd) {
#if QWT_VERSION < 0x060000
  QwtPlotPrintFilter filter;
  int options = QwtPlotPrintFilter::PrintAll;
  options &= ~QwtPlotPrintFilter::PrintBackground;
  options |= QwtPlotPrintFilter::PrintFrameWithScales;
  filter.setOptions(options);
  QwtPlot::print(pd, filter);
#else
  QwtPlotRenderer renderer;
  renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground);
  renderer.setLayoutFlag(QwtPlotRenderer::FrameWithScales);
  renderer.renderTo(this, pd);
#endif
}

void GuiQwtContourPlot::printPlot()
{
#if QWT_VERSION >= 0x060000
#if 1
  QPrinter printer;
#else
  QPrinter printer(QPrinter::HighResolution);
#endif
  printer.setPageOrientation(QPageLayout::Landscape);
  printer.setOutputFileName("image.pdf");
  QPrintDialog dialog(&printer);
  if ( dialog.exec() ) {
    QwtPlotRenderer renderer;
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
#if QWT_VERSION < 0x060100
    renderer.setLayoutFlag(QwtPlotRenderer::KeepFrames, true);
#else
    renderer.setLayoutFlag(QwtPlotRenderer::FrameWithScales, true);
#endif
    renderer.renderTo(this, printer);
  }
#endif
}

GuiQwtContourPlot::MyZoomer::MyZoomer(GuiQwtContourPlot* p, QWidget *canvas):
#if QWT_VERSION >= 0x060100
  QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas), plot(p)
#else
  QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, dynamic_cast<QwtPlotCanvas*>(canvas)), plot(p)
#endif
{
  setTrackerMode(AlwaysOn);
}

QwtText GuiQwtContourPlot::MyZoomer::trackerTextF(const QPointF &pos) const
{
#if QWT_VERSION >= 0x060000
  QColor bg(Qt::white);
  bg.setAlpha(200);
  QwtText text("");

  const GuiQwtContourPlotData *data = plot->m_data;
  if (data) {
    // point data
    std::string textPt = QwtPlotZoomer::trackerTextF(pos).text().toStdString();

    // get z -value
    double z = data->value(pos.x(), pos.y());
    std::ostringstream os;
    os << textPt << " Value: " << z;

    text = QString::fromStdString(os.str());
    text.setBackgroundBrush( QBrush( bg ));
  }
  return text;
#endif
}

void GuiQwtContourPlot::setContourLevels() {
#if QWT_VERSION >= 0x060000
  const QwtInterval&  zInterval = d_spectrogram->data()->interval(Qt::ZAxis);
  double step= zInterval.width()/ (m_plot->getCountContourLevels() != 0 ?
				   ((double) m_plot->getCountContourLevels()): 1.);
#if _WIN32
  double exp10_step = ldexp( 10, (int) log10(step) );
#else
  double exp10_step = exp10( (int) log10(step) );
#endif

  // set countour levels
  QList<double> contourLevels;
  for ( double level = zInterval.minValue() + step; level < zInterval.maxValue(); level += step ) {
    double t_level = level / (exp10_step != 0 ? exp10_step : 1);
    contourLevels += exp10_step * floor(0.5+t_level);
  }
  d_spectrogram->setContourLevels(contourLevels);
#endif
}

void GuiQwtContourPlot::update(QwtRasterData& data) {
#if QWT_VERSION >= 0x060000
  setData( dynamic_cast<GuiQwtContourPlotData*>(&data) );
  setContourLevels();
#endif
}
