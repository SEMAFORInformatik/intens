
#include <cmath>

#include "gui/qt/GuiQwtPlotCurveData.h"
#include "utils/Debugger.h"

GuiQwtPlotCurveData::GuiQwtPlotCurveData(int size)
#if QWT_VERSION < 0x060000
  : m_xArray(0)
  , m_yArray(0)
  , m_idx(0)
#endif
{
  reserveData(size);
  m_hasNan = false;
  d_boundingRect = QRectF(0.0, 0.0, -1.0, -1.0);  // negative width is ignored by qwt plot
}

size_t GuiQwtPlotCurveData::size() const {
#if QWT_VERSION < 0x060000
  return m_idx;
#else
  return  QwtArraySeriesData<QPointF>::size();
#endif
}

QRectF GuiQwtPlotCurveData::boundingRect() const {
  return d_boundingRect;
}

void GuiQwtPlotCurveData::clearData() {
#if QWT_VERSION < 0x060000
  if (m_xArray && m_yArray) {
   delete [] m_xArray;
   delete [] m_yArray;
  }
  m_xArray = 0;
  m_yArray = 0;
  m_idx = 0;
#elif QWT_VERSION < 0x060200
  d_samples.clear();
  d_samples.squeeze();
#else
  m_samples.clear();
  m_samples.squeeze();
#endif
  d_boundingRect = QRectF(0.0, 0.0, -1.0, -1.0);  // negative width is ignored by qwt plot
  m_hasNan = false;
}

void GuiQwtPlotCurveData::reserveData(int newSize) {
#if QWT_VERSION < 0x060000
  if (newSize) {
    m_xArray = new double[newSize];
    m_yArray = new double[newSize];
  }
#else
  //  values.reserve(newSize);
#endif
}

void GuiQwtPlotCurveData::append(const double x, const double y) {
  QPointF sample(x,y);
#if QWT_VERSION < 0x060000
  m_xArray[m_idx]=x;
  m_yArray[m_idx]=y;
  ++m_idx;
#elif QWT_VERSION < 0x060200
  d_samples += sample;
#else
  m_samples += sample;
#endif

  // adjust the bounding rectangle
  if (std::isnan(y) ||std::isnan(x)) {
    m_hasNan = true;
    return;
  }
  if ( d_boundingRect.width() < 0 || d_boundingRect.height() < 0 ) {
    d_boundingRect.setRect(sample.x(), sample.y(), 1e-300, 1e-300); // ! min width+height is needed
  } else {
    if ( sample.x() > d_boundingRect.right() )
      d_boundingRect.setRight(sample.x());
    if ( sample.x() < d_boundingRect.left() )
      d_boundingRect.setLeft(sample.x());

    if ( sample.y() > d_boundingRect.bottom() )
      d_boundingRect.setBottom(sample.y());
    if ( sample.y() < d_boundingRect.top() )
      d_boundingRect.setTop(sample.y());
  }
}
