
#if !defined(GUI_QWT_PLOT_CURVE_DATA_H)
#define GUI_QWT_PLOT_CURVE_DATA_H

#include <qwt_plot.h>
#include <QPointer>
#include <QRectF>

#if QWT_VERSION >= 0x060000 // unter 6.0
#include <qwt_series_data.h>

class GuiQwtPlotCurveData: public QwtArraySeriesData<QPointF>
#else
class GuiQwtPlotCurveData
#endif
{
public:
  GuiQwtPlotCurveData(int size);

  virtual size_t size() const;

  virtual QRectF boundingRect() const;

  void clearData();
  void reserveData(int newSize);
  void append(double x, double y);

  bool hasValuesNan() const { return m_hasNan; }

#if QWT_VERSION < 0x060000
  double* getXArray() { return m_xArray; }
  double* getYArray() { return m_yArray; }

  long   m_idx;
  double *m_xArray;
  double *m_yArray;
#endif
  QRectF d_boundingRect;
private:
  GuiQwtPlotCurveData();
  bool    m_hasNan;
};

#endif
