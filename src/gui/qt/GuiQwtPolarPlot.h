
#ifdef HAVE_QWT_POLAR_H

#if !defined(GUI_QWT_POLARPLOT_INCLUDED_H)
#define GUI_QWT_POLARPLOT_INCLUDED_H
#include <qwt_polar_plot.h>

class QwtPolarGrid;
class QwtPolarCurve;
class GuiQWTPlot;

class GuiQwtPlotPolarData: public QwtArraySeriesData<QwtPointPolar> {
 public:
 GuiQwtPlotPolarData(size_t size ) :
  d_size(size), m_hasNan(false) {}

  void append(const double x, const double y);

  virtual QRectF boundingRect() const;
  virtual size_t size() const { return d_size; }

 private:
  size_t d_size;
  bool m_hasNan;
};

class PlotSettings
{
public:
    enum Curve
    {
        Spiral,
        Rose,

        NumCurves
    };

    enum Flag
    {
        MajorGridBegin,
        MinorGridBegin = MajorGridBegin + QwtPolar::ScaleCount,
        AxisBegin = MinorGridBegin + QwtPolar::ScaleCount,

        AutoScaling = AxisBegin + QwtPolar::AxesCount,
        Inverted,
        Logarithmic,

        Antialiasing,

        CurveBegin,

        NumFlags = CurveBegin + NumCurves
    };

    bool flags[NumFlags];
};

class GuiQwtPolarPlot: public QwtPolarPlot
{

public:
  GuiQwtPolarPlot(GuiQWTPlot* plot, QWidget * = NULL );
    virtual ~GuiQwtPolarPlot() {}
    PlotSettings settings() const;

    //    Q_OBJECT
public Q_SLOTS:
    void applySettings( const PlotSettings & );

private:
    QwtPolarCurve *createCurve( int id ) const;

    QwtPolarGrid *d_grid;
    QwtPolarCurve *d_curve[PlotSettings::NumCurves];
    GuiQWTPlot*   m_plot;
};

#endif

#endif
