#include <qwt_plot.h>
#include <qwt_raster_data.h>

#define HAVE_QWTPLOT3D (QWT_VERSION < 0x060000)

#if HAVE_QWTPLOT3D
#include <qwt3d_surfaceplot.h>
#include <qwt3d_function.h>
#include <qwt3d_parametricsurface.h>
#else
#include <qwt_plot_spectrogram.h>
#endif

class XferDataItemIndex;
class GuiPlotDataItem;
typedef std::map<std::string, GuiPlotDataItem*> DataItemType;

class QwtPlotZoomer;

//---------------------------------------------------
// Class GuiQwtBasePlot
//---------------------------------------------------
class GuiQwtBasePlot: public QwtPlot
{
public:
  GuiQwtBasePlot(GuiQt3dPlot* plot, QwtRasterData& data, QWidget *parent = NULL);

public :
  virtual void updateAxes(){
    QwtPlot::updateAxes();
  }

  void showContour(bool on);
  void showSpectrogram(bool on);
  void contextMenuEvent ( QContextMenuEvent* event );
  void update(QwtRasterData& data);

  /** reset Scale */
  void resetScale();
  /** reset Rotation */
  void printPlot(QPrinter* print=0);
  /** drawCanvas */
  virtual void drawCanvas(QPainter* painter);

private:
  class ColorMap: public QwtLinearColorMap
  {
  public:
    ColorMap();
  };

private:
  QwtPlotSpectrogram *d_spectrogram;
  QwtPlotZoomer* m_zoomer;
  GuiQt3dPlot* m_plot;
};

//---------------------------------------------------
// Class Plot3dData
//---------------------------------------------------
class Plot3dData : public QwtRasterData
#if HAVE_QWTPLOT3D
, public Qwt3D::ParametricSurface
#endif
{
  public:
    typedef std::map<std::string, GuiPlotDataItem*> DataItemType;

    Plot3dData(const Plot3dData& data);

  virtual bool update(Plot3D* plot3d=NULL);
#if HAVE_QWTPLOT3D
    Plot3dData(Qwt3D::SurfacePlot* pw, DataItemType& dataitems);
virtual Qwt3D::Triple  operator()(double x, double y);
    Qwt3D::Triple  getScales();
#endif
    long getXMinIndex(double x) const;
    long getYMinIndex(double y) const;
  double  operator()(int, double x, double y);
    double  getCachedData(long x, long y) const;
    double  getValue(double x, double y) const;

  DataItemType&  getDataItems() { return m_dataitems; }

  // virtual QwtRasterData functions
  virtual QwtRasterData *copy() const;
  virtual QwtInterval interval(Qt::Axis) const;
  virtual double value(double x, double y) const;
  void initRaster(const QRectF& dr,const QSize & raster);

  private:
    class Data {
    public:
      Data();
      long getCountRows() { return m_rows; }
      long getCountColumns() { return m_columns; }

      void reset(long row, long col);
      double m_xmin, m_xmax, m_ymin, m_ymax, m_zmin, m_zmax;
    private:
      long m_rows;
      long m_columns;
      //      long m_rows, m_columns;
    };

    Data    *m_range;

    typedef DataItemType::value_type DataItemPair;
    DataItemType&         m_dataitems;

    GuiPlotDataItem *m_xaxis, *m_yaxis, *m_zaxis;
    XferDataItemIndex *m_ixrows;
    XferDataItemIndex *m_ixcols;
    std::vector< std::vector<double> >  m_vector;
    std::vector<double> m_xvalues;
    std::vector<double> m_yvalues;
    Plot3dData* m_copy;
};


#if HAVE_QWTPLOT3D
//---------------------------------------------------
// Class Plot3D
//---------------------------------------------------
class Plot3D : public Qwt3D::SurfacePlot
  {
  public:
    Plot3D(GuiQt3dPlot* plot, DataItemType& dataitems);

    virtual void createData();
    virtual void updateData(bool qwtplot3d=true);

    /** reset Scale */
    void resetScale();
    /** reset Rotation */
    void resetRotation();
    /** print */
    void printPlot(QPrinter* print=0);

  void contextMenuEvent ( QContextMenuEvent* event );

  private:
    GuiQt3dPlot* m_plot;
};
#endif
