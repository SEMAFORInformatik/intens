#include <iostream>
#include <sstream>
#include <algorithm>

#if HAVE_QGRAPHS

#include "gui/GuiPlotDataItem.h"
#include "gui/qt/GuiQtGraphsPlotData.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiQtGraphsPlotData::GuiQtGraphsPlotData( DataItemType& dataitems )
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

  ///  setResampleMode( BilinearInterpolation );
}

/* --------------------------------------------------------------------------- */
/* Destructor --                                                               */
/* --------------------------------------------------------------------------- */
GuiQtGraphsPlotData::~GuiQtGraphsPlotData() {
}

/* --------------------------------------------------------------------------- */
/* updateData --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::updateData() {
  BUG(BugGui, "GuiQtGraphsPlotData::updateData");
  //  discardRaster();
  minXData =  std::numeric_limits<double>::max();
  maxXData =  -std::numeric_limits<double>::max();
  minYData =  std::numeric_limits<double>::max();
  maxYData =  -std::numeric_limits<double>::max();
  minZData =  std::numeric_limits<double>::max();
  maxZData =  -std::numeric_limits<double>::max();


  // get Dimension
  m_rows = m_ixrows->getDimensionSize( m_zaxis->Data() );
  m_columns = 0;
  for( int i=0; i < m_rows; i++ ) {
    m_ixrows->setIndex( m_zaxis->Data(), i );
    int tmp = m_ixcols->getDimensionSize( m_zaxis->Data() );
    if( tmp > m_columns ) m_columns = tmp;
  }

  // clear old data
  m_xvalues.clear();
  m_xvalues.insert(m_xvalues.begin(), m_columns, std::numeric_limits<double>::quiet_NaN());
  m_yvalues.clear();
  m_yvalues.insert(m_yvalues.begin(), m_rows > 1 ? m_rows : m_columns,
                   std::numeric_limits<double>::quiet_NaN());
  minStepX = std::numeric_limits<double>::max();
  minStepY = std::numeric_limits<double>::max();
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
  clear();
  if (m_rows > 1 /***&& resampleMode() == BilinearInterpolation***/) {
    reserve(m_rows > 1 ? m_rows : m_columns);
    for (int y = 0; y < m_rows; ++y) {
      QSurfaceDataRow dataRow;
      dataRow.reserve(m_columns);
      for (long x = 0; x < m_columns; ++x) {
        int _y = m_rows > 1 ? y : x;
        QVector3D pt = this->getZValue( m_reverseX ? m_columns-1-x : x,
                                        m_reverseY ? m_rows-1-_y : _y);
        dataRow << QSurfaceDataItem(pt.x(), pt.z(), pt.y());
        if (minZData > pt.z()) minZData = pt.z();
        if (maxZData < pt.z()) maxZData = pt.z();
        if (minXData > pt.x()) minXData = pt.x();
        if (maxXData < pt.x()) maxXData = pt.x();
        if (minYData > pt.y()) minYData = pt.y();
        if (maxYData < pt.y()) maxYData = pt.y();
      }
      *this << dataRow;
    }
  } else {
    // special case
    // all data (x, y, z) are vector data
    double oldX = std::numeric_limits<double>::max();
    int maxRow(0);
    int cnt = 0, cntRowMax = 0, cntMax = 0;

    // get max row size
    for (int x = 0; x < m_columns; ++x) {
      for (long y = 0; y < m_rows; ++y) {
        QVector3D pt = this->getZValue(x, x);
        if (oldX == std::numeric_limits<double>::max()) oldX = pt.x();
        if (std::abs(pt.x() - oldX) > 1e3*std::numeric_limits<double>::epsilon()){
          oldX = pt.x();
          cntRowMax = std::max(cnt, cntRowMax);
          cnt = 0;
        }
        ++cnt;
      }
    }
    oldX = std::numeric_limits<double>::max();
    QSurfaceDataRow* dataRow = new QSurfaceDataRow(cntRowMax);
    cnt = 0;
    for (int x = 0; x < m_columns; ++x) {
      for (long y = 0; y < m_rows; ++y) {
        QVector3D pt = this->getZValue(x, m_rows > 1 ? y : x);
        if (minZData > pt.z()) minZData = pt.z();
        if (maxZData < pt.z()) maxZData = pt.z();
        if (minXData > pt.x()) minXData = pt.x();
        if (maxXData < pt.x()) maxXData = pt.x();
        if (minYData > pt.y()) minYData = pt.y();
        if (maxYData < pt.y()) maxYData = pt.y();
        if (oldX == std::numeric_limits<double>::max()) oldX = pt.x();
        if (std::abs(pt.x() - oldX) > 1e3*std::numeric_limits<double>::epsilon()){
          //std::cout << cnt << "=!=" << ".Changed X diff["<<std::abs(pt.x() - oldX)<<"]\n";
          cntMax = std::max(cnt, cntMax);
          while (cnt++ <= cntMax){
            *dataRow << dataRow->last();
          }
          *this << *dataRow ;;
          oldX = pt.x();
          delete dataRow;
          cnt = 0;
          dataRow = new QSurfaceDataRow(cntRowMax);
        }
        ++cnt;
        ///        std::cout <<  "X: " << pt.x() << ", y: " << pt.y() << ", " << pt.z() << std::endl;
        *dataRow << QSurfaceDataItem(pt.x(), pt.z(), pt.y());
      }
    }
    // append last data row
     while (cnt++ <= cntMax && cntRowMax){
       *dataRow << dataRow->last();
     }
     *this << *dataRow;
     delete dataRow;
  }
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::getIndex(double x, double y, int& ix, int& iy)  const {
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

/* --------------------------------------------------------------------------- */
/* value --                                                                    */
/* --------------------------------------------------------------------------- */
double GuiQtGraphsPlotData::value(double x, double y)  const {
  return std::numeric_limits<double>::quiet_NaN();
}

/* --------------------------------------------------------------------------- */
/* getZValue --                                                                */
/* --------------------------------------------------------------------------- */
QVector3D GuiQtGraphsPlotData::getZValue(int ix, int iy) {
  BUG(BugGui, "GuiQtGraphsPlotData::getZValue");
  double x, y;
  double z = std::numeric_limits<double>::quiet_NaN();

  // z-Wert ermitteln
  m_ixrows->setIndex( m_zaxis->Data(), m_rows > 1 ? iy : 0 );
  m_ixcols->setIndex( m_zaxis->Data(), ix );
  if (m_zaxis->getValue(z)) {
    z *= m_zaxis->getScaleFactor();
  } else
    z = std::numeric_limits<double>::quiet_NaN();

  // y-Wert ermitteln
  if( m_yaxis ) {
    if( m_yaxis->XferData() ) {
      XferDataItemIndex *dix = m_yaxis->getDataItemIndexWildcard( 1 );
      dix->setIndex( m_yaxis->Data(), iy );
      if (m_yaxis->getValue(y)) {
        y *= m_yaxis->getScaleFactor();
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
  return QVector3D(x, y, z);
}

/* --------------------------------------------------------------------------- */
/* getXInterval --                                                             */
/* --------------------------------------------------------------------------- */
GuiQtGraphsPlotData::Interval GuiQtGraphsPlotData::getXInterval() {
  // // get user or data minimum or maximum
  double minX = minXUser != std::numeric_limits<double>::max() ? minXUser : minXData;
  double maxX = maxXUser != std::numeric_limits<double>::min() ? maxXUser : maxXData;
  return Interval(minX, maxX);
}

/* --------------------------------------------------------------------------- */
/* getYInterval --                                                             */
/* --------------------------------------------------------------------------- */
GuiQtGraphsPlotData::Interval GuiQtGraphsPlotData::getYInterval() {
  // // get user or data minimum or maximum
  double minY = minYUser != std::numeric_limits<double>::max() ? minYUser : minYData;
  double maxY = maxYUser != std::numeric_limits<double>::min() ? maxYUser : maxYData;
  return Interval(minY, maxY);
}

/* --------------------------------------------------------------------------- */
/* getZInterval --                                                             */
/* --------------------------------------------------------------------------- */
GuiQtGraphsPlotData::Interval GuiQtGraphsPlotData::getZInterval() {
  // // get user or data minimum or maximum
  minZ = minZUser != std::numeric_limits<double>::max() ? minZUser : minZData;
  maxZ = maxZUser != std::numeric_limits<double>::min() ? maxZUser : maxZData;
  return Interval(minZ, maxZ);
}

/* --------------------------------------------------------------------------- */
/* setMinZUser --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::setMinZUser(double newOffset) {
  minZUser = newOffset;
  if (maxZUser == std::numeric_limits<double>::min())
    maxZUser = maxZData;
  updateData();
}

/* --------------------------------------------------------------------------- */
/* setMaxZUser --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::setMaxZUser(double newOffset) {
  maxZUser = newOffset;
  if (minZUser == std::numeric_limits<double>::max())
    minZUser = minZData;
  updateData();
}

/* --------------------------------------------------------------------------- */
/* resetZUser --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::resetZUser() {
  // already reseted
  if (!bZUserScale) return;

  bZUserScale= false;
  minZUser = std::numeric_limits<double>::max();
  maxZUser = std::numeric_limits<double>::min();

  updateData();
}

/* --------------------------------------------------------------------------- */
/* setRangeXUser --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::setRangeXUser(double minXValue, double maxXValue) {
  bXUserScale= true;
  minXUser = minXValue;
  maxXUser = maxXValue;
}

/* --------------------------------------------------------------------------- */
/* resetXUser --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::resetXUser() {
  bXUserScale= false;
  minXUser = std::numeric_limits<double>::max();
  maxXUser = std::numeric_limits<double>::min();
}

/* --------------------------------------------------------------------------- */
/* setRangeYUser --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::setRangeYUser(double minYValue, double maxYValue) {
  bYUserScale= true;
  minYUser = minYValue;
  maxYUser = maxYValue;
}

/* --------------------------------------------------------------------------- */
/* resetYUser --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::resetYUser() {
  bYUserScale= false;
  minYUser = std::numeric_limits<double>::max();
  maxYUser = std::numeric_limits<double>::min();
}

/* --------------------------------------------------------------------------- */
/* setRangeZUser --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtGraphsPlotData::setRangeZUser(double minXValue, double maxXValue) {
  bZUserScale= true;
  setMinZUser(minXValue);
  setMaxZUser(maxXValue);
}


#endif
