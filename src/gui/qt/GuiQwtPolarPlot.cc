
#ifdef HAVE_QWT_POLAR_H

#include <cmath>
#include <qpen.h>
#include <qwt_series_data.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_polar_grid.h>
#include <qwt_polar_curve.h>
#include <qwt_polar_marker.h>
#include <qwt_scale_engine.h>
#include <qwt_text_label.h>
#include "GuiQwtPolarPlot.h"
#include <iostream>
#include "utils/Debugger.h"
#include "gui/qt/QtMultiFontString.h"

INIT_LOGGER();

const QwtInterval radialInterval( 0.0, 10.0 );
const QwtInterval azimuthInterval( 0.0, 360.0 );

void GuiQwtPlotPolarData::append(const double d, const double r) {
  BUG_DEBUG("append d["<<d<<"] r["<<r<<"]");
  QwtPointPolar sample(d, r);
  d_samples += sample;

  // adjust the bounding rectangle
  if (std::isnan(d) ||std::isnan(r))
    m_hasNan = true;
}

QRectF GuiQwtPlotPolarData::boundingRect() const {
  d_boundingRect = qwtBoundingRect( *this );

  BUG_DEBUG("boundingRect w["<<d_boundingRect.left() << ", " << d_boundingRect.right()
            <<"] h["<<d_boundingRect.bottom() << ", " << d_boundingRect.top() << "]");
  return d_boundingRect;
}


GuiQwtPolarPlot::GuiQwtPolarPlot(GuiQWTPlot* plot, QWidget *parent):
  QwtPolarPlot(QwtText(), parent)
  , m_plot(plot)
{
    setAutoReplot( false );
    setPlotBackground( Qt::white );

    // scales
    // setScale( QwtPolar::Azimuth,
    //     azimuthInterval.minValue(), azimuthInterval.maxValue(),
    //     azimuthInterval.width() / 12 );

    setScaleMaxMinor( QwtPolar::Azimuth, 2 );
    // setScale( QwtPolar::Radius,
    //     radialInterval.minValue(), radialInterval.maxValue() );
    setAutoScale(QwtPolar::Radius);
    setAutoScale(QwtPolar::Azimuth);

    // grids, axes
    d_grid = new QwtPolarGrid();
    d_grid->setPen( QPen( Qt::white ) );
    for ( int scaleId = 0; scaleId < QwtPolar::ScaleCount; scaleId++ )
    {
        d_grid->showGrid( scaleId );
        d_grid->showMinorGrid( scaleId );

        QPen minorPen( Qt::gray );
#if 0
        minorPen.setStyle( Qt::DotLine );
#endif
        d_grid->setMinorGridPen( scaleId, minorPen );
    }

    // black pen for all axis
    d_grid->setAxisPen( QwtPolar::AxisAzimuth, QPen( Qt::black ) );
    d_grid->setAxisPen( QwtPolar::AxisLeft, QPen( Qt::black ) );
    d_grid->setAxisPen( QwtPolar::AxisRight, QPen( Qt::black ) );
    d_grid->setAxisPen( QwtPolar::AxisBottom, QPen( Qt::black ) );
    d_grid->setAxisPen( QwtPolar::AxisTop, QPen( Qt::black ) );

    // show all axis
    d_grid->showAxis( QwtPolar::AxisAzimuth, true );
    d_grid->showAxis( QwtPolar::AxisLeft, false );
    d_grid->showAxis( QwtPolar::AxisRight, false );
    d_grid->showAxis( QwtPolar::AxisTop, false );
    d_grid->showAxis( QwtPolar::AxisBottom, true );

    d_grid->showGrid( QwtPolar::Azimuth, true );
    d_grid->showGrid( QwtPolar::Radius, true );

    d_grid->attach( this );

    // set title font
    QFont font =  titleLabel()->font();
    titleLabel()->setFont( QtMultiFontString::getQFont( "@plotTitle@", font ) );
    // set axis font
    for (int atype=0; atype< QwtPolar::AxesCount ; ++atype) {
      QFont font =  d_grid->axisFont(atype);
      d_grid->setAxisFont(atype, QtMultiFontString::getQFont("@plotAxis@", font));
    }
    // set grid font
    d_grid->setFont(QtMultiFontString::getQFont("@plotAxis@", font));
    // set legend font
    if (legend())
      legend()->setFont(QtMultiFontString::getQFont("@plotLegend@", font));

    /**
    // curves

    for ( int curveId = 0; curveId < PlotSettings::NumCurves; curveId++ )
    {
        d_curve[curveId] = createCurve( curveId );
        d_curve[curveId]->attach( this );
    }

    // markers
    QwtPolarMarker *marker = new QwtPolarMarker();
    marker->setPosition( QwtPointPolar( 57.3, 4.72 ) );
    marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::white ), QPen( Qt::green ), QSize( 9, 9 ) ) );
    marker->setLabelAlignment( Qt::AlignHCenter | Qt::AlignTop );

    QwtText text( "Marker" );
    text.setColor( Qt::black );
    QColor bg( Qt::white );
    bg.setAlpha( 200 );
    text.setBackgroundBrush( QBrush( bg ) );

    marker->setLabel( text );
    marker->attach( this );
    */

    QwtLegend *legend = new QwtLegend;
    insertLegend( legend,  QwtPolarPlot::BottomLegend );
}

PlotSettings GuiQwtPolarPlot::settings() const
{
    PlotSettings s;
    for ( int scaleId = 0; scaleId < QwtPolar::ScaleCount; scaleId++ )
    {
        s.flags[PlotSettings::MajorGridBegin + scaleId] =
            d_grid->isGridVisible( scaleId );
        s.flags[PlotSettings::MinorGridBegin + scaleId] =
            d_grid->isMinorGridVisible( scaleId );
    }
    for ( int axisId = 0; axisId < QwtPolar::AxesCount; axisId++ )
    {
        s.flags[PlotSettings::AxisBegin + axisId] =
            d_grid->isAxisVisible( axisId );
    }

    s.flags[PlotSettings::AutoScaling] =
        d_grid->testGridAttribute( QwtPolarGrid::AutoScaling );

    s.flags[PlotSettings::Logarithmic] =
        scaleEngine( QwtPolar::Radius )->transformation();

    const QwtScaleDiv *sd = scaleDiv( QwtPolar::Radius );
    s.flags[PlotSettings::Inverted] = sd->lowerBound() > sd->upperBound();

    s.flags[PlotSettings::Antialiasing] =
        d_grid->testRenderHint( QwtPolarItem::RenderAntialiased );

    for ( int curveId = 0; curveId < PlotSettings::NumCurves; curveId++ )
    {
        s.flags[PlotSettings::CurveBegin + curveId] =
            d_curve[curveId]->isVisible();
    }

    return s;
}

void GuiQwtPolarPlot::applySettings( const PlotSettings& s )
{
return;
    for ( int scaleId = 0; scaleId < QwtPolar::ScaleCount; scaleId++ )
    {
        d_grid->showGrid( scaleId,
            s.flags[PlotSettings::MajorGridBegin + scaleId] );
        d_grid->showMinorGrid( scaleId,
            s.flags[PlotSettings::MinorGridBegin + scaleId] );
    }

    for ( int axisId = 0; axisId < QwtPolar::AxesCount; axisId++ )
    {
        d_grid->showAxis( axisId,
            s.flags[PlotSettings::AxisBegin + axisId] );
    }

    d_grid->setGridAttribute( QwtPolarGrid::AutoScaling,
        s.flags[PlotSettings::AutoScaling] );

    const QwtInterval interval =
        scaleDiv( QwtPolar::Radius )->interval().normalized();
    if ( s.flags[PlotSettings::Inverted] )
    {
        setScale( QwtPolar::Radius,
            interval.maxValue(), interval.minValue() );
    }
    else
    {
        setScale( QwtPolar::Radius,
            interval.minValue(), interval.maxValue() );
    }

    if ( s.flags[PlotSettings::Logarithmic] )
    {
        setScaleEngine( QwtPolar::Radius, new QwtLogScaleEngine() );
    }
    else
    {
    }

    d_grid->setRenderHint( QwtPolarItem::RenderAntialiased,
        s.flags[PlotSettings::Antialiasing] );

    for ( int curveId = 0; curveId < PlotSettings::NumCurves; curveId++ )
    {
        d_curve[curveId]->setRenderHint( QwtPolarItem::RenderAntialiased,
                                         s.flags[PlotSettings::Antialiasing] );
        d_curve[curveId]->setVisible(
            s.flags[PlotSettings::CurveBegin + curveId] );
    }

    replot();
}

/*
QwtPolarCurve *GuiQwtPolarPlot::createCurve( int id ) const
{
    const int numPoints = 200;

    QwtPolarCurve *curve = new QwtPolarCurve();
    curve->setStyle( QwtPolarCurve::Lines );
    //curve->setLegendAttribute( QwtPolarCurve::LegendShowLine, true );
    //curve->setLegendAttribute( QwtPolarCurve::LegendShowSymbol, true );
    switch( id )
    {
        case PlotSettings::Spiral:
        {
            curve->setTitle( "Spiral" );
            curve->setPen( QPen( Qt::yellow, 2 ) );
            curve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
                QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
            curve->setData(
                new SpiralData( radialInterval, azimuthInterval, numPoints ) );
            break;
        }
        case PlotSettings::Rose:
        {
            curve->setTitle( "Rose" );
            curve->setPen( QPen( Qt::red, 2 ) );
            curve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
                QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
            curve->setData(
                new RoseData( radialInterval, azimuthInterval, numPoints ) );
            break;
        }
    }
    return curve;
}
*/
#endif
