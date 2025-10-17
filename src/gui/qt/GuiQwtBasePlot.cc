
#include <limits>
#include <QPrinter>
#include <QPainter>

#include "gui/GuiPlotDataItem.h"
#include "gui/qt/GuiQwtPlot.h"
#include "gui/qt/GuiQwtPlotZoomer.h"
#include "gui/qt/GuiQwt3dPlot.h"

#include <qprintdialog.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>

#include "gui/qt/GuiQwtBasePlot.h"

GuiQwtBasePlot::ColorMap::ColorMap()
  :QwtLinearColorMap(Qt::darkCyan, Qt::red){
  addColorStop(0.1, Qt::cyan);
  addColorStop(0.6, Qt::green);
  addColorStop(0.95, Qt::yellow);
}

GuiQwtBasePlot::GuiQwtBasePlot(GuiQwt3dPlot* plot, QwtRasterData& data, QWidget *parent):
  QwtPlot(parent)
  , m_plot( plot )
  , d_spectrogram(0)
  , m_zoomer(0)
{
  update(data);
}

void GuiQwtBasePlot::showContour(bool on)
{
#if QWT_VERSION >= 0x060000
    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, on);
    replot();
#endif
}

void GuiQwtBasePlot::showSpectrogram(bool on)
 {
#if QWT_VERSION >= 0x060000
     d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, on);
     d_spectrogram->setDefaultContourPen(on ? QPen() : QPen(Qt::NoPen));
     replot();
#endif
 }

#if HAVE_QWTPLOT3D
 void Plot3D::printPlot(QPrinter* printer){
   if (printer) {
     render(printer);
   }
 }
#endif

 void GuiQwtBasePlot::printPlot(QPrinter* printer){
   if (printer) {
#if QWT_VERSION >= 0x060000
     QPainter painter;
     painter.begin(printer);
     drawCanvas(&painter);
     painter.end();
#endif
   } else {
     QPrinter printer;
     printer.setPageOrientation(QPageLayout::Landscape);
 #if QT_VERSION < 0x040000
     printer.setColorMode(QPrinter::Color);
     printer.setOutputFileName("/tmp/spectrogram.ps");
     if (printer.setup())
 #else
     printer.setOutputFileName("/tmp/spectrogram.pdf");
     QPrintDialog dialog(&printer);
     if ( dialog.exec() )
 #endif
     {
#if QWT_VERSION >= 0x060000
       QPainter painter;
       painter.begin(&printer);
       drawCanvas(&painter);
       painter.end();
#endif
     }
   }
 }


void GuiQwtBasePlot::drawCanvas(QPainter* painter) {
    QwtPlot::drawCanvas(painter);
}

void GuiQwtBasePlot::update(QwtRasterData& data) {

#if QWT_VERSION >= 0x060000
   if (!d_spectrogram) {
     d_spectrogram = new QwtPlotSpectrogram();
     d_spectrogram->setColorMap( new ColorMap);
     d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
     d_spectrogram->attach(this);
  }
  d_spectrogram->setData(&data);

  double diff= d_spectrogram->data()->interval(Qt::ZAxis).width();
  double min = d_spectrogram->data()->interval(Qt::ZAxis).minValue();
  double max = d_spectrogram->data()->interval(Qt::ZAxis).maxValue();
  std::cout << "  width["<<diff<<"] min["<<min<<"] max["<<max<<"] Valid["<<d_spectrogram->data()->interval(Qt::ZAxis).isValid()<<"] ["<<data.interval(Qt::ZAxis).isValid()<<"] \n";
#endif
  double step= diff/20.;
#if _WIN32
  double exp10_diff = ldexp( 10, (int) log10(diff) );
  double exp10_step = ldexp( 10, (int) log10(step) );
#else
  double exp10_diff = exp10( (int) log10(diff) );
  double exp10_step = exp10( (int) log10(step) );
#endif

  // set countour levels
  QList<double> contourLevels;
  for ( double level = min + step; level < max; level += step )
  {
    double t_level = level / (exp10_step != 0 ? exp10_step : 1);
    contourLevels += exp10_step * floor(0.5+t_level);
  }
  d_spectrogram->setContourLevels(contourLevels);

  QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
  rightAxis->setColorBarEnabled(true);
  if(m_zoomer) m_zoomer->zoom(0);
#if QWT_VERSION >= 0x060000
  if (d_spectrogram->data()->interval(Qt::ZAxis).isValid())
    rightAxis->setColorMap(d_spectrogram->data()->interval(Qt::ZAxis),
   			 new ColorMap());
  setAxisScale(QwtPlot::yRight,
	       d_spectrogram->data()->interval(Qt::ZAxis).minValue(),
	       d_spectrogram->data()->interval(Qt::ZAxis).maxValue() );

#endif
  // set y and x Scales
  QRectF bRect=d_spectrogram->boundingRect();

  if (bRect.isValid()) {
    setAxisScale(QwtPlot::xBottom, bRect.x(), bRect.x()+bRect.width());
    setAxisScale(QwtPlot::yLeft,   bRect.y(), bRect.y()+bRect.height());
  }

  enableAxis(QwtPlot::yRight);

  plotLayout()->setAlignCanvasToScales(true);
  //  replot();
  // LeftButton for the zooming
  // MiddleButton for the panning
  // RightButton: zoom out by 1
  // Ctrl+RighButton: zoom out to full size

  if (!m_zoomer) {
#if QWT_VERSION >= 0x060000
    m_zoomer= new  GuiQwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,
				   QwtPicker::RectRubberBand, QwtPicker::AlwaysOff, canvas() );
#endif
    m_zoomer->setZoomBase();

    QwtPlotPanner *panner = new QwtPlotPanner(canvas());
    panner->setAxisEnabled(QwtPlot::yRight, false);
    panner->setMouseButton(Qt::MiddleButton);

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
    QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
    sd->setMinimumExtent( fm.horizontalAdvance("100.00") );

    m_zoomer->setRubberBandPen( QColor(Qt::blue) );
    m_zoomer->setTrackerPen(QColor(Qt::darkBlue));
  } else {
    m_zoomer->setZoomBase();
  }
  updateAxes();
}

  void GuiQwtBasePlot::resetScale() {
    m_zoomer->zoom(0);
  }

  void GuiQwtBasePlot::contextMenuEvent ( QContextMenuEvent* event ){
    m_plot->popupMenu(event);
  }

/////////////////////////////////////////////////////////////////////////////////////////


Plot3dData::Data::Data() {
  reset(0,0);
}

void Plot3dData::Data::reset(long col, long row)
{
  m_columns = col;
  m_rows = row;
  m_xmin = m_ymin = m_zmin = std::numeric_limits<double>::max();
  m_xmax = m_ymax = m_zmax = std::numeric_limits<double>::min();
}

#if HAVE_QWTPLOT3D
Plot3dData::Plot3dData(Qwt3D::SurfacePlot* pw, DataItemType& dataitems)
  : Qwt3D::ParametricSurface(pw) //Function(pw),
  , m_dataitems(dataitems)
  , m_xaxis(0), m_yaxis(0), m_zaxis(0)
  , m_ixrows(0), m_ixcols(0)
  , m_vector(0)
  , m_copy(0)
{
  m_range = new Data();

  GuiPlotDataItem *xaxis=0, *yaxis=0, *zaxis=0;
  // Datenitems fuer x-, y- und z-Achse zuordnen, z-Achse muss vorhanden sein
  DataItemType::iterator iter = m_dataitems.find( "XAXIS" );
  if( iter != m_dataitems.end() )
    m_xaxis = (*iter).second;
  iter = m_dataitems.find( "YAXIS" );
  if( iter != m_dataitems.end() )
    m_yaxis = (*iter).second;
  iter = m_dataitems.find( "ZAXIS" );
  if( iter != m_dataitems.end() )
    m_zaxis = (*iter).second;
  m_ixrows = m_zaxis->getDataItemIndexWildcard( 1 );
  m_ixcols = m_zaxis->getDataItemIndexWildcard( 2 );
}
#endif

Plot3dData::Plot3dData(const Plot3dData& data) :
#if HAVE_QWTPLOT3D
  Qwt3D::ParametricSurface(data), //Function(data),
#endif
  m_dataitems(data. m_dataitems)
, m_range( data.m_range )  // only pointer assignment
, m_xaxis(data.m_xaxis), m_yaxis(data.m_yaxis), m_zaxis(data.m_zaxis)
, m_ixrows(data. m_ixrows), m_ixcols(data.m_ixcols)
, m_xvalues(data.m_xvalues)
, m_yvalues(data.m_yvalues)
, m_vector(data.m_vector)
, m_copy((Plot3dData*) &data)
{}

QwtRasterData *Plot3dData::copy() const {
  //  return (QwtRasterData*) this;
  return new Plot3dData( *this );
}

bool Plot3dData::update(Plot3D* plot3d) {
  assert(m_copy == 0);

  // Rasterausdehnung berrechnen
  long rows = m_ixrows->getDimensionSize( m_zaxis->Data() );
  long columns = 0;
  //  for( int i=0; i < m_range->getCountRows(); i++ ) {
  for( int i=0; i < rows; i++ ) {
    m_ixrows->setIndex( m_zaxis->Data(), i );
    int tmp = m_ixcols->getDimensionSize( m_zaxis->Data() );
    if( tmp > columns ) columns = tmp;
  }

  m_range->reset(columns, rows);

  // update Data in my shadow
  if (! m_range->getCountRows()*m_range->getCountColumns() )
    assert(false);

  // clear old data
  m_vector.resize(m_range->getCountColumns());
  m_xvalues.clear();
  m_xvalues.insert( m_xvalues.begin(), m_range->getCountColumns(), std::numeric_limits<double>::max());
  m_yvalues.clear();
  m_yvalues.insert( m_yvalues.begin(), m_range->getCountRows(), std::numeric_limits<double>::max());
  for (long x = 0; x < m_range->getCountColumns(); ++x) {
    m_vector[x].resize(m_range->getCountRows());
    for (long y = 0; y < m_range->getCountRows(); ++y) {
#if HAVE_QWTPLOT3D
      this->operator()(x,y);
#endif
    }
  }
  if (plot3d) {
#if HAVE_QWTPLOT3D
    setMesh(m_range->getCountColumns(), m_range->getCountRows());
    setDomain(0, m_range->getCountColumns()-1, 0, m_range->getCountRows()-1);
    return Qwt3D::ParametricSurface::create(*plot3d);//Function::create();
#endif
  } else {
    return true;
  }
  return true;
}

double Plot3dData::getValue(double x, double y)  const {
long ix = (int) floor(x+0.5);
long iy = (int) floor(y+0.5);
 return getCachedData(ix,iy);
}
#if HAVE_QWTPLOT3D
Qwt3D::Triple  Plot3dData::operator()(double x, double y)   {
  double z = operator()(1,x, y);
  return Qwt3D::Triple(x,y,z);
}
#endif
double Plot3dData::operator()(int, double x, double y)   {
  int ix = (int) floor(x+0.5);
  int iy = (int) floor(y+0.5);
  double z;

  // z-Wert ermitteln
  m_ixrows->setIndex( m_zaxis->Data(), iy );
  m_ixcols->setIndex( m_zaxis->Data(), ix );
  if (m_zaxis->getValue(z)) {
    z *= m_zaxis->getScaleFactor();
    if (z < m_range->m_zmin) m_range->m_zmin = z;
    if (z > m_range->m_zmax ||
	 m_range->m_zmax == std::numeric_limits<double>::min()) m_range->m_zmax = z;
  }  else
    z = std::numeric_limits<double>::max();

  // y-Wert ermitteln (beachte Achsendreher)
  if( m_yaxis ) {
    if( m_yaxis->XferData() ) {
      XferDataItemIndex *dix = m_yaxis->getDataItemIndexWildcard( 1 );
      dix->setIndex( m_yaxis->Data(), ix );
      if (m_yaxis->getValue(x)) {
	x *= m_yaxis->getScaleFactor();
	if (x < m_range->m_xmin) m_range->m_xmin = x;
	if (x > m_range->m_xmax ||
	    m_range->m_xmax == std::numeric_limits<double>::min()) m_range->m_xmax = x;
      } else
	x = std::numeric_limits<double>::max();
    }
  }

  // x-Wert ermitteln (beachte Achsendreher)
  if( m_xaxis ) {
    if( m_xaxis->XferData() ) {
      XferDataItemIndex *dix = m_xaxis->getDataItemIndexWildcard( 1 );
      dix->setIndex( m_xaxis->Data(), iy );
      if (m_xaxis->getValue(y)) {
	y *= m_xaxis->getScaleFactor();
	if (y < m_range->m_ymin) m_range->m_ymin = y;
	if (y > m_range->m_ymax ||
	    m_range->m_ymax == std::numeric_limits<double>::min()) m_range->m_ymax = y;
     } else
	y = std::numeric_limits<double>::max();
    }
  }

  // cache data
  m_vector[ix][iy] = z;
  if (m_xvalues[ix] == std::numeric_limits<double>::max() )
    m_xvalues[ix] = x;
  if (m_yvalues[iy] == std::numeric_limits<double>::max() )
    m_yvalues[iy] = y;
  //std::cout << "op() x["<<x<<"] y["<<y<<"] ix["<<ix<<"] iy["<<iy<<"]  => x["<<x<<"] y["<<y<<"] z["<<z<<"] VnZ["<<m_zaxis->XferData()->getFullName(true)<<"]  Rows["<<m_range->getCountRows()<<"]  Cols["<<m_range->getCountColumns()<<"] iy["<<iy<<"]\n";
  assert(iy<m_range->getCountRows());

  return z;
}

#if HAVE_QWTPLOT3D
Qwt3D::Triple  Plot3dData::getScales() {
  double xdiff = m_range->m_xmax - m_range->m_xmin;
  double ydiff = m_range->m_ymax - m_range->m_ymin;
  double zdiff = m_range->m_zmax - m_range->m_zmin;
  double maxdiff = xdiff;
  if (maxdiff < ydiff) maxdiff = ydiff;
  if (maxdiff < zdiff) maxdiff = zdiff;
  //std::cout << "  =i== Plot3D::getScales  x["<<m_range->m_xmax<<", "<<m_range->m_xmin<<"] y["<<m_range->m_ymax<<", "<<m_range->m_ymin<<"] z["<<m_range->m_zmax<<", "<<m_range->m_zmin<<"]\n";
  return Qwt3D::Triple(maxdiff/xdiff, maxdiff/ydiff, maxdiff/zdiff);
}
#endif

double  Plot3dData::getCachedData(long x, long y) const {
  if (x >= m_vector.size()) x= m_vector.size()-1;
  if (y >= m_vector[x].size()) y= m_vector[x].size()-1;

  if (m_vector[x][y] == std::numeric_limits<double>::max()) {
    //    assert(false);
  }
  return m_vector[x][y];
}

long Plot3dData::getXMinIndex(double x) const {
  if (m_copy)
    return m_copy->getXMinIndex(x);
  for (long i=0; i < m_range->getCountColumns(); ++i) {
    //    std::cout << this << "m_copy["<<m_copy<<"] getXMinIndex x["<<x<<"] size["<<m_xvalues.size()<<"] cols["<<m_range->getCountColumns()<<"]\n";
    if (m_xvalues[i] > x) {
      return i ? i-1 : 0;
    }
    if (m_xvalues[i] == x) {
      return i;
    }
  }
  return m_range->getCountColumns()-1;
}

long Plot3dData::getYMinIndex(double y) const {
  if (m_copy)
    return m_copy->getYMinIndex(y);
  for (long i=0; i < m_range->getCountRows(); ++i) {
    //    std::cout << " getYMinIndex inY["<<y<<"] i["<<i<<"] (m_yvalues[i]["<<m_yvalues[i]<<"]\n";
    if (m_yvalues[i] > y)
      return i ?  i-1 : 0;
    if (m_yvalues[i] == y)
      return i;
  }
  return m_range->getCountRows()-1;
}

double Plot3dData::value(double x, double y) const {
  if (m_copy)
    return m_copy->value(x,y);

  if (!m_range->getCountColumns() || !m_range->getCountRows()) return 0;

  // get x and y index ranges
  long xminId = getXMinIndex(x);
  long yminId = getYMinIndex(y);
  long xmaxId = (xminId+1) >=m_range->getCountColumns() ? xminId-1 : xminId+1;
  long ymaxId = (yminId+1) >=m_range->getCountRows()    ? yminId-1 : yminId+1;
  assert(yminId > -1 && xminId > -1);

  // get cached z values of this points
  double z_lt  = getCachedData(xminId,yminId);
  double z_lb = getCachedData(xminId,ymaxId);
  double z_rt  = getCachedData(xmaxId,yminId);
  double z_rb = getCachedData(xmaxId,ymaxId);

  // interpolate
  double xdiff = m_xvalues[xmaxId]-m_xvalues[xminId];
  double ydiff = m_yvalues[ymaxId]-m_yvalues[yminId];
  double z_t = xdiff ? z_lt + ((x-m_xvalues[xminId])/xdiff) * (z_rt-z_lt) : z_lt;
  double z_b = xdiff ? z_lb + ((x-m_xvalues[xminId])/xdiff) * (z_rb-z_lb) : z_rt;
  double z   = ydiff ? z_t  + ((y-m_yvalues[yminId])/ydiff) * (z_b -z_t ) : z_t;
  //std::cout << "  value x["<<x<<"] y["<<y<<"] z["<<z<<"] lt["<<z_lt<<"]b["<<z_lb<<"] rt["<<z_rt<<"]b["<<z_rb<<"] x["<<xminId<<", "<<xmaxId<<"] y["<<yminId<<", "<<ymaxId<<"] row["<<m_range->getCountRows()<<"] col["<<m_range->getCountColumns()<<"]\n";
  return z;
}

QwtInterval Plot3dData::interval(Qt::Axis axis) const {
  if (m_copy)
    return m_copy->interval(axis);
  if (axis == Qt::XAxis) {
    if (m_range->m_xmin == std::numeric_limits<double>::max() ||
        m_range->m_xmax == std::numeric_limits<double>::min())
      return QwtInterval();
    else {
      return QwtInterval(m_range->m_xmin, m_range->m_xmax);
    }
  }
  if (axis == Qt::YAxis) {
    if (m_range->m_ymin == std::numeric_limits<double>::max() ||
        m_range->m_ymax == std::numeric_limits<double>::min())
      return QwtInterval();
    else {
      return QwtInterval(m_range->m_ymin, m_range->m_ymax);
    }
  }
  if (axis == Qt::ZAxis) {
    if (m_range->m_zmin == std::numeric_limits<double>::max() ||
        m_range->m_zmax == std::numeric_limits<double>::min())
      return QwtInterval();
    else {
      return QwtInterval(m_range->m_zmin, m_range->m_zmax);
    }
  }
  return QwtInterval();
}


void Plot3dData::initRaster(const QRectF& dr,const QSize & raster) {
  if (m_copy)
    ;//    m_copy->initRaster(dr, raster);
else
  QwtRasterData::initRaster(dr, raster);
}


#if HAVE_QWTPLOT3D
void Plot3D::createData() {
  BUG(BugGui, "Plot3D::createData");
  Qwt3D::SurfacePlot::createData();
}

void Plot3D::updateData(bool qwtplot3d) {

  BUG(BugGui, "Plot3D::updateData");
  if (!qwtplot3d)
    return;

  // bas: 2010-12-02
  // m_plot->getPlot3dData() could be 0 -> segmentation fault
  // scales is not used anyway!
  // therefore, I comment the following line
  //Qwt3D::Triple  scales = m_plot->getPlot3dData()->getScales();

  Qwt3D::SurfacePlot::updateData();
  updateGL();
}

void Plot3D::resetScale() {
  setScale(1,1,1);
  setShift(0.15,0,0);
  setZoom(0.9);
}

void Plot3D::resetRotation() {
  //  setRotation(30,0,15);
  setRotation(45,0, 45);
}

void Plot3D::contextMenuEvent ( QContextMenuEvent* event ){
  m_plot->popupMenu(event);
}

Plot3D::Plot3D(GuiQwt3dPlot* plot, DataItemType& dataitems)
  : m_plot(plot) {

  resetScale();
  resetRotation();

  for (unsigned i=0; i!=coordinates()->axes.size(); ++i) {
    coordinates()->axes[i].setMajors(7);
    coordinates()->axes[i].setMinors(4);
  }


  coordinates()->axes[Qwt3D::X1].setLabelString("x-axis");
  coordinates()->axes[Qwt3D::Y1].setLabelString("y-axis");
  coordinates()->axes[Qwt3D::Z1].setLabelString(QChar(0x38f)); // Omega - see http://www.unicode.org/charts/


  setCoordinateStyle(Qwt3D::BOX); // [NOCOORD, FRAME, BOX]
  updateData();
  updateGL();
  }
#endif
