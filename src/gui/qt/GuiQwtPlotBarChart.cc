
#include <iostream>
#include <algorithm>
#include <limits>
#include <sstream>
#include <math.h>

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_column_symbol.h>
#include <qwt_text.h>

#include "gui/qt/GuiQwtPlotBarChart.h"
#include "utils/Debugger.h"

INIT_LOGGER();

#if QWT_VERSION >= 0x060200
#define AXIS_Y_LEFT QwtAxis::YLeft
#define AXIS_X_BOTTOM QwtAxis::XBottom
#else
#define AXIS_Y_LEFT QwtPlot::yLeft
#define AXIS_X_BOTTOM QwtPlot::xBottom
#endif

//
// Konstructor
//
GuiQwtPlotBarChart::GuiQwtPlotBarChart( GuiQwtPropertyDialog::Settings::BarChart settings, bool plotItemGrouped )
  : QwtPlotMultiBarChart( "Bar Chart" )
  , m_plotItemGrouped( plotItemGrouped )
{
  m_settings = settings;
  setLayoutPolicy( (QwtPlotMultiBarChart::LayoutPolicy) settings.layoutPolicy );
  setLayoutHint( settings.layoutHint );
  setOrientation( settings.orientation == 2 ? Qt::Vertical : Qt::Horizontal);
}


//
// Destructor
//
GuiQwtPlotBarChart::~GuiQwtPlotBarChart() {

  // delete markers
  std::vector< QwtPlotMarker*>::iterator it = m_markers.begin();
  for (; it != m_markers.end(); ++it) {
    delete (*it);
  }
}

//
// setBarChartLayoutSettings
//
void GuiQwtPlotBarChart::setBarChartSettings(const GuiQwtPropertyDialog::Settings::BarChart& settings) {
  setLayoutHint( settings.layoutHint );
  setLayoutPolicy((QwtPlotMultiBarChart::LayoutPolicy) settings.layoutPolicy);
  setOrientation( settings.orientation == 2 ? Qt::Vertical : Qt::Horizontal);

  m_settings = settings;
}

void GuiQwtPlotBarChart::clearSamples() {
  QwtPlotMultiBarChart::setSamples( QVector<QwtSetSample>() );
}
//
// setSamples
//
void GuiQwtPlotBarChart::setSamples (QwtSeriesData< QPointF > *series) {
  BUG_DEBUG("setSamples  Series-size: " << series->size());
  QwtArraySeriesData<QwtSetSample>* mySeries = dynamic_cast< QwtArraySeriesData<QwtSetSample>* >(data());
  QVector<QwtSetSample> seriesNew( mySeries->samples() );
  if (m_plotItemGrouped) {
    // Grouped BarStyle
    // jedes PlotItem wird als einzelne Bar zusammengefasst
    for (int ii =0; ii < series->size(); ++ii) {
      if (ii==0) {
	// neu
	QVector<double> values;
	values += series->sample(0).y();
	seriesNew += QwtSetSample(seriesNew.size(), values);
      } else {
	// append
	seriesNew.back().set.append( series->sample(ii).y() );
      }
    }
  } else {

    int maxIdx = 0;
    for (int ii =0; ii < series->size(); ++ii) {
      // Normaler BarStyle
      // jedes PlotItem wird gleich wie die anderen dargestellt
      // die Lücken werden mit Nullen aufgefüllt
      // Lücken entstehen durch x-Werte,
      // welche nur in anderen PlotItems verwendet werden
      if (ii==0 && seriesNew.size() && seriesNew[0].set.size()) {
        maxIdx = seriesNew[0].set.size();
      }
      int i =0;
      for (i =0; i < seriesNew.size(); ++i) {
        if ( seriesNew[i].value == series->sample(ii).x()) {
          seriesNew[i].set.append( series->sample(ii).y() );
          break;
        }
      }
      // neues x,y
      if (i == seriesNew.size()) {
        QVector<double> values;
        while (values.size() < maxIdx)
          values += 0;//std::numeric_limits<double>::quiet_NaN();
        values += series->sample(ii).y();
        seriesNew += QwtSetSample(series->sample(ii).x(), values);
      }
    }

    // ist die Länge gleich, evtl auffüllen
    for (int i =0; i < seriesNew.size(); ++i) {
      if ( maxIdx == seriesNew[i].set.size()) {
        seriesNew[i].set.append( 0 ); //std::numeric_limits<double>::quiet_NaN();
      }
    }
  }

  QwtPlotMultiBarChart::setSamples( seriesNew );

  // clear old markers
  std::vector< QwtPlotMarker*>::iterator it = m_markers.begin();
  for (; it != m_markers.end(); ++it) {
    (*it)->detach();
    delete (*it);
  }
  m_markers.clear();

  if (m_settings.dataTipAlignment == 0) return;

  createMarker();
}

//
// createMarker
//
void GuiQwtPlotBarChart::createMarker() {
  QwtArraySeriesData<QwtSetSample>* seriesData = dynamic_cast< QwtArraySeriesData<QwtSetSample>* >(data());
  QVector<QwtSetSample> series( seriesData->samples() );
  // get minimum threshold
  QRectF r = boundingRect();
  double threshold = std::min(r.bottom(), r.top()) + m_settings.dataTipMinThreshold/100. * r.height();

  // marker color
  QColor bg( QString::fromStdString(m_settings.dataTipBackgroundColor) );
  QColor fg( Qt::black );

  //
  const size_t numSamples = series.size();
  for ( uint i = 0; i < numSamples; i++ ) {
    const double sampleX = series[i].value;
    double sampleYMin = series[i].set[0];
    double sampleYMax = series[i].set[0];
    for (int x=1; x < series[i].set.size(); ++x) {
      if (sampleYMin > series[i].set[x]) sampleYMin = series[i].set[x];
      if (sampleYMax < series[i].set[x]) sampleYMax = series[i].set[x];
    }

    if (sampleYMax < threshold) {
      // under minimum threshold
      continue;
    }
    QwtPlotMarker *mPos = new QwtPlotMarker( "Marker" );
    mPos->setLineStyle((QwtPlotMarker::LineStyle) m_settings.dataTipLineStyle );
    mPos->setLinePen( Qt::transparent );
    mPos->setSpacing( 10 );
    mPos->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    mPos->setItemAttribute( QwtPlotItem::Legend, false );
    mPos->setValue( QPointF( sampleX, sampleYMax ) );

    // build string
    std::ostringstream os;
    if (isBarStyleOptionPlotItemGrouped() )
      os <<  "X: " <<  getAnnoLabel(i);
    else
      os <<  "X: " <<  sample(i).value;
    os <<  "  Y: " << (sample(i).set.size() > 1 ? "[": "");
    for (int x=0; x < sample(i).set.size(); ++x) {
      os << (x ? ", " : "");// << sample(i).set[x];
      os << (getYScaleFactor(i) != 0 ?
	     (sample(i).set[x] /getYScaleFactor(i)) :
	     sample(i).set[x]);
    }
    os <<  (sample(i).set.size() > 1 ? "]": "");

    QwtText text( QString::fromStdString( os.str() ) );
    //    QwtText text(  QString( " x: %1\n y: %2" ).arg( sample.x(),5).arg(sample.y(),5) );
    text.setBackgroundBrush( QBrush( bg/*.light(280)*/ ) );
    text.setColor( fg );
    text.setBorderRadius(0);
    QFont font = text.font();
    font.setStyleHint(QFont::Courier);
    text.setFont(font);
    text.setBorderPen( QColor( QString::fromStdString(m_settings.dataTipBorderColor) ) );
    mPos->setLabel( text );
    mPos->setLabelAlignment( (Qt::Alignment) m_settings.dataTipAlignment);
    mPos->attach( plot() );
    m_markers.push_back(mPos);
#if QWT_VERSION >= 0x060100
    // magic code: only to autoscale top alignment markers
    if ( Qt::AlignTop & (Qt::Alignment) m_settings.dataTipAlignment) {
      QwtInterval val = plot()->axisInterval(AXIS_Y_LEFT);
      double sample2YMax = sampleYMax + 0.1 * abs(sampleYMax);
      QwtPlotMarker *m2Pos = new QwtPlotMarker( "Marker" );
      m2Pos->setItemAttribute( QwtPlotItem::AutoScale, true );
      m2Pos->setValue( QPointF( sampleX, sample2YMax ) );
      m2Pos->setAxes(AXIS_X_BOTTOM, AXIS_Y_LEFT);
      m2Pos->attach( plot() );
      m_markers.push_back(m2Pos);
    }
#endif
  }
}

//
// closestPoint
//
int GuiQwtPlotBarChart::closestPoint( const QPoint &pos, double *dist ) const {
    const size_t numSamples = dataSize();

    if ( plot() == NULL || numSamples <= 0 ) {
      return -1;
    }

    const QwtSeriesData<QwtSetSample> *series = data();

    const QwtScaleMap xMap = plot()->canvasMap( xAxis() );
    const QwtScaleMap yMap = plot()->canvasMap( yAxis() );

    int index = -1;
    double dmin = *dist; //1.0e10;
    bool yAxisBase = orientation() == Qt::Horizontal;

    for ( uint i = 0; i < numSamples; i++ )
    {
      for ( uint j = 0; j < series->size(); j++ ) {
	double sum = 0;
	for (int x=0; x < series->sample( j ).set.size(); ++x)
	  sum += series->sample( j ).set[x];
	const QPointF sample(series->sample( j ).value, sum/series->sample( j ).set.size());

	const double cx =  yAxisBase ?
	  yMap.transform( sample.x() ) - pos.y() :
	  xMap.transform( sample.x() ) - pos.x();
	//        const double cy = yMap.transform( sample.y() ) - pos.y();

        const double cyBsLne = yAxisBase ? xMap.transform( baseline() ) : yMap.transform( baseline() );
	const double cySmpl = yAxisBase ? xMap.transform( sample.y() ) : yMap.transform( sample.y() );
        if (!yAxisBase && (pos.y() < cySmpl || pos.y() > cyBsLne))
	  continue;
        if (yAxisBase && (pos.x() > cySmpl || pos.x() < cyBsLne))
	  continue;

        const double f = fabs(cx);
        if ( f < dmin )
        {
            index = j;
            dmin = f;
        }
      }
    }
    if ( dist )
      *dist = dmin; //qSqrt( dmin );

    return index;
}

//
// setColor
//
void GuiQwtPlotBarChart::setColor(const QColor &color) {
  d_colors += color;
  // itemChanged();
        QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setLineWidth( 2 );
        symbol->setFrameStyle( QwtColumnSymbol::Raised );
        symbol->setPalette( QPalette( color ) );

       setSymbol( d_colors.size()-1, symbol );
}

//
// setTitle
//
void GuiQwtPlotBarChart::setTitle(const QString& title) {

  QList<QwtText> titles(barTitles());
  titles += title;
  setBarTitles( titles );
  setLegendIconSize( QSize( 10, 14 ) );
}

//
// addAnnoLabel
//
void GuiQwtPlotBarChart::addAnnoLabel(const std::string& label) {
  m_annoLabels.push_back( label );
}

//
// addAnnoLabel
//
std::string GuiQwtPlotBarChart::getAnnoLabel(int index) {
  if (index < m_annoLabels.size())
    return m_annoLabels[ index ];
  return "";
}

void GuiQwtPlotBarChart::addScaleFactor( double xScaleFactor, double yScaleFactor ) {
  m_xScaleFactor.push_back(xScaleFactor);
  m_yScaleFactor.push_back(yScaleFactor);
}

double GuiQwtPlotBarChart::getXScaleFactor(int index) {
  if (index <  m_xScaleFactor.size())
    return m_xScaleFactor[index];
  return 0;
}

double GuiQwtPlotBarChart::getYScaleFactor(int index) {
  if (index <  m_yScaleFactor.size())
    return m_yScaleFactor[index];
  return 0;
}
