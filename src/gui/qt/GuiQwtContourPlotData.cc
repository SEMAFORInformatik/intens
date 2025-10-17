#include <iostream>
#include <sstream>
#include <algorithm>
#include <qwt_plot.h>
#include <qwt_interval.h>

#if QWT_VERSION >= 0x060000

#include "GuiQwtContourPlotData.h"
#include "gui/GuiPlotDataItem.h"

INIT_LOGGER();

GuiQwtContourPlotData::GuiQwtContourPlotData( DataItemType& dataitems )
    : minXUser(std::numeric_limits<double>::max())
    , maxXUser(std::numeric_limits<double>::min())
    , bXUserScale(false)
    , minYUser(std::numeric_limits<double>::max())
    , maxYUser(std::numeric_limits<double>::min())
    , bYUserScale(false)
    , minZUser(std::numeric_limits<double>::max())
    , maxZUser(std::numeric_limits<double>::min())
    , bZUserScale(false)
    , minZ(std::numeric_limits<double>::max())
    , maxZ(std::numeric_limits<double>::min())
    , maxDiffDelta(std::numeric_limits<double>::min())
    , m_columns(0), m_rows(0)
    , minStepX(std::numeric_limits<double>::max())
    , minStepY(std::numeric_limits<double>::max())
    , m_dataitems(dataitems)
    , m_xaxis(0), m_yaxis(0), m_zaxis(0)
    , m_reverseX(false), m_reverseY(false) {

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

  setResampleMode( BilinearInterpolation );
}

GuiQwtContourPlotData::~GuiQwtContourPlotData() {
}

QRectF GuiQwtContourPlotData::pixelHint( const QRectF&  area ) const {
  BUG(BugGui, "GuiQwtContourPlotData::pixelHint");
  if (m_rows > 1 && resampleMode() == BilinearInterpolation) {
    return QwtMatrixRasterData::pixelHint(area);
  }
  QRectF r = QRectF(0, 0, minStepX/10., minStepY/10.);
  BUG_MSG("Rectangle xy["<<r.x()<<", "<<r.y()<<"] wh["<<r.width()<<", "<<r.height()<<"]");
  return r;
}

void GuiQwtContourPlotData::updateData() {
  BUG(BugGui, "GuiQwtContourPlotData::updateData");
  //  discardRaster();

  // get Dimension
  m_rows = m_ixrows->getDimensionSize( m_zaxis->Data() );
  m_columns = 0;
  for( int i=0; i < m_rows; i++ ) {
    m_ixrows->setIndex( m_zaxis->Data(), i );
    int tmp = m_ixcols->getDimensionSize( m_zaxis->Data() );
    if( tmp > m_columns ) m_columns = tmp;
  }

  // clear old data
  m_vector.resize( m_columns );
  m_xvalues.clear();
  m_xvalues.insert(m_xvalues.begin(), m_columns, std::numeric_limits<double>::quiet_NaN());
  m_yvalues.clear();
  m_yvalues.insert(m_yvalues.begin(), m_rows > 1 ? m_rows : m_columns,
                   std::numeric_limits<double>::quiet_NaN());
  minStepX = std::numeric_limits<double>::max();
  minStepY = std::numeric_limits<double>::max();
  setInterval(Qt::XAxis, QwtInterval());
  setInterval(Qt::YAxis, QwtInterval());
  setInterval(Qt::ZAxis, QwtInterval());
  m_reverseX = false;
  m_reverseY = false;

  // reverse check
  if (m_columns && m_rows) {
    this->getZValue(0, 0);
    this->getZValue(m_columns-1,m_rows-1);
    if (m_xvalues.size())
      m_reverseX = (m_xvalues[0] > m_xvalues.back()) ? true: false;
    if (m_yvalues.size())
      m_reverseY = (m_yvalues[0] > m_yvalues.back()) ? true: false;
  }

  // get new z values
  // NOTE: Interpolation works only with matrix data
  if (m_rows > 1 && resampleMode() == BilinearInterpolation) {
    QVector<double> values;
    for (long y = 0; y < m_rows; ++y) {
      for (long x = 0; x < m_columns; ++x) {
        int _y = m_rows > 1 ? y : x;
         values +=  this->getZValue( m_reverseX ? m_columns-1-x : x,
                                     m_reverseY ? m_rows-1-_y : _y);
      }
    }
    setValueMatrix(values, m_columns);
  } else {
    for (long x = 0; x < m_columns; ++x) {
      m_vector[x].resize(m_rows);
      for (long y = 0; y < m_rows; ++y) {
        m_vector[x][y] = this->getZValue(x, m_rows > 1 ? y : x);
      }
    }
  }
  updateZInterval();
  QwtInterval xInterval(interval(Qt::XAxis));
  QwtInterval yInterval(interval(Qt::YAxis));
  // maxDiffDelta is now max diff
  // to shrink "value area size", decrease size eg. 1e-3
  maxDiffDelta = sqrt(pow(xInterval.width(),2) + pow(yInterval.width(),2));
}

double GuiQwtContourPlotData::getCachedZValue(int ix, int iy) const {
  if (m_vector.size()==0)
    return std::numeric_limits<double>::quiet_NaN();
  if (ix >= m_vector.size())     ix = m_vector.size()-1;
  if (iy >= m_vector[ix].size()) iy = m_vector[ix].size()-1;

  return m_vector[ix][iy];
}

void GuiQwtContourPlotData::getIndex(double x, double y, int& ix, int& iy)  const {
  if (m_xvalues.size() == 0 || m_yvalues.size() == 0 ) {
    ix = iy = 0;
    return;
  }

  // vector z data
  if (m_rows == 1){
    ix = iy = -1;
    double diffMax=maxDiffDelta;
    std::vector<double>::const_iterator itX, itY;
    for(itX = m_xvalues.begin(), itY = m_yvalues.begin();
        itX != m_xvalues.end(), itY != m_yvalues.end();
        ++itX, ++itY) {
      double d = sqrt(pow((*itX - x), 2) + pow((*itY - y), 2));
      if (d < diffMax){
        diffMax = d;
        ix = iy = itX - m_xvalues.begin();
      }
    }
    if (diffMax > maxDiffDelta) {
      ix = iy = -1;
    }
    return;
  }
  std::vector<double>::const_iterator it;
  double diff, diffPrev;

  // matrix z data
  // get X index
  if (m_reverseX) {
    ix = 1;
    for(it = m_xvalues.begin()+1; it != m_xvalues.end(); ++it, ++ix) {
      diff = *it -x;
      diffPrev = x- *(it-1);
      if ( *(it-1) >= x && x >= *(it)) { if (diff < diffPrev) --ix;  break; }
    }
  } else {
    ix = 1;
    for(it = m_xvalues.begin()+1; it != m_xvalues.end(); ++it, ++ix) {
      diff = *it -x;
      diffPrev = x- *(it-1);
      if ( *(it-1) <= x && x <= *(it)) { if (diff > diffPrev) --ix;  break; }
    }
  }

  // get Y index
  if (m_reverseY) {
    iy = 1;
    for(it = m_yvalues.begin()+1; it != m_yvalues.end(); ++it, ++iy) {
      diff = *it -y;
      diffPrev = y - *(it-1);
      if ( *(it-1) >= y && y >= *(it)) { if (diff < diffPrev) --iy;  break; }
    }
  } else {
    iy = 1;
    for(it = m_yvalues.begin()+1; it != m_yvalues.end(); ++it, ++iy) {
      diff = *it -y;
      diffPrev = y - *(it-1);
      if ( *(it-1) <= y && y <= *(it)) { if (diff > diffPrev) --iy;  break; }
    }
  }
}

double GuiQwtContourPlotData::value(double x, double y)  const {
  if (m_rows > 1 && resampleMode() == BilinearInterpolation)
    return QwtMatrixRasterData::value(x,y);
  int ix, iy;
  getIndex(x,y, ix, iy);
  return ix >= 0 && iy >= 0 ? getCachedZValue(ix,iy) : std::numeric_limits<double>::quiet_NaN();
}

QwtInterval GuiQwtContourPlotData::getInterval (Qt::Axis axis) {
  if (axis == Qt::XAxis && bXUserScale) {
   return QwtInterval(minXUser, maxXUser);
  }
  if (axis == Qt::YAxis && bYUserScale) {
    return QwtInterval(minYUser, maxYUser);
  }
  return QwtMatrixRasterData::interval(axis);
}


double GuiQwtContourPlotData::getZValue(int ix, int iy) {
  BUG(BugGui, "GuiQwtContourPlotData::getZValue");
  double x, y;
  double z = std::numeric_limits<double>::quiet_NaN();
  QwtInterval xInterval(interval(Qt::XAxis));
  QwtInterval yInterval(interval(Qt::YAxis));
  QwtInterval zInterval(interval(Qt::ZAxis));

  // z-Wert ermitteln
  m_ixrows->setIndex( m_zaxis->Data(), m_rows > 1 ? iy : 0 );
  m_ixcols->setIndex( m_zaxis->Data(), ix );
  if (m_zaxis->getValue(z)) {
    z *= m_zaxis->getScaleFactor();
    zInterval.isValid() ? setInterval(Qt::ZAxis, zInterval.extend(z))
      : setInterval(Qt::ZAxis, QwtInterval(z, z));
  } else
    z = std::numeric_limits<double>::quiet_NaN();

  // y-Wert ermitteln
  if( m_yaxis ) {
    if( m_yaxis->XferData() ) {
      XferDataItemIndex *dix = m_yaxis->getDataItemIndexWildcard( 1 );
      dix->setIndex( m_yaxis->Data(), iy );
      if (m_yaxis->getValue(y)) {
        y *= m_yaxis->getScaleFactor();
        yInterval.isValid() ? setInterval(Qt::YAxis, yInterval.extend(y))
          : setInterval(Qt::YAxis, QwtInterval(y, y));
      } else
        y = std::numeric_limits<double>::quiet_NaN();
    }
  }

  // x-Wert ermitteln
  if( m_xaxis ) {
    if( m_xaxis->XferData() ) {
      XferDataItemIndex *dix = m_xaxis->getDataItemIndexWildcard( 1 );
      dix->setIndex( m_xaxis->Data(), ix );
      if (m_xaxis->getValue(x)) {
        x *= m_xaxis->getScaleFactor();
        xInterval.isValid() ? setInterval(Qt::XAxis, xInterval.extend(x))
          : setInterval(Qt::XAxis, QwtInterval(x, x));
      } else
        x = std::numeric_limits<double>::quiet_NaN();
    }
  }

  // cache data
  if (std::isnan(m_xvalues[ix])) {
    m_xvalues[ix] = x;
    if (ix) minStepX = std::min(fabs(x-m_xvalues[ix-1]), minStepX);
  }
  if (std::isnan(m_yvalues[iy])) {
    m_yvalues[iy] = y;
    if (iy) minStepY = std::min(fabs(y-m_yvalues[iy-1]), minStepY);
  }
  BUG_DEBUG("ix: " << ix << ", iy: " << iy << " => z:" << z
            << ", x:" << m_xvalues[ix] << ", y:" << m_yvalues[iy]);
  return z;
}

void GuiQwtContourPlotData::updateZInterval() {
  // get minimum and maximum from data
  double minZOld = minZ;
  double maxZOld = maxZ;
  QwtInterval zInterval(interval(Qt::ZAxis));
  minZData = zInterval.minValue();
  maxZData = zInterval.maxValue();

  // get user or data minimum or maximum
  minZ = minZUser != std::numeric_limits<double>::max() ? minZUser : zInterval.minValue();
  maxZ = maxZUser != std::numeric_limits<double>::min() ? maxZUser : zInterval.maxValue();

  // set new interval
  setInterval( Qt::ZAxis, QwtInterval(minZ, maxZ) );
}

  void GuiQwtContourPlotData::setMinZUser(double newOffset) {
    minZUser = newOffset;
    if (maxZUser == std::numeric_limits<double>::min())
      maxZUser = maxZData;
    updateData();
  }

  void GuiQwtContourPlotData::setMaxZUser(double newOffset) {
    maxZUser = newOffset;
    if (minZUser == std::numeric_limits<double>::max())
      minZUser = minZData;
    updateData();
  }

  void GuiQwtContourPlotData::resetZUser() {
    // already reseted
    if (!bZUserScale) return;

    bZUserScale= false;
    minZUser = std::numeric_limits<double>::max();
    maxZUser = std::numeric_limits<double>::min();
    setInterval( Qt::ZAxis, QwtInterval() );

    updateData();
  }

  void GuiQwtContourPlotData::setRangeXUser(double minXValue, double maxXValue) {
    bXUserScale= true;
    minXUser = minXValue;
    maxXUser = maxXValue;
  }

  void GuiQwtContourPlotData::resetXUser() {
    bXUserScale= false;
    minXUser = std::numeric_limits<double>::max();
    maxXUser = std::numeric_limits<double>::min();
  }

  void GuiQwtContourPlotData::setRangeYUser(double minYValue, double maxYValue) {
    bYUserScale= true;
    minYUser = minYValue;
    maxYUser = maxYValue;
  }

  void GuiQwtContourPlotData::resetYUser() {
    bYUserScale= false;
    minYUser = std::numeric_limits<double>::max();
    maxYUser = std::numeric_limits<double>::min();
  }

  void GuiQwtContourPlotData::setRangeZUser(double minXValue, double maxXValue) {
    bZUserScale= true;
    setMinZUser(minXValue);
    setMaxZUser(maxXValue);
  }


  QRectF GuiQwtContourPlotData::getZoomBase() const {
    QwtInterval xInterval(interval(Qt::XAxis));
    QwtInterval yInterval(interval(Qt::YAxis));
    QRectF rect(0,0, xInterval.isValid() ? xInterval.width() : 1,
		yInterval.isValid() ? yInterval.width() : 1);
    // if (regionProfiles.getMinimumX() != std::numeric_limits<double>::max())  {
    //   rect.setWidth( regionProfiles.getRangeX() );
    // }
    // if (regionProfiles.getMinimumY() != std::numeric_limits<double>::max()) {
    //   rect.setHeight( regionProfiles.getRangeY() );
    // }
    return rect;

  }

#endif
