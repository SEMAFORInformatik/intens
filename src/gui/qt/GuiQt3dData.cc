#include <iostream>
#include <sstream>
#include <algorithm>

#if HAVE_QGRAPHS

#include "gui/GuiPlotDataItem.h"
#include "gui/qt/GuiQt3dData.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */
GuiQt3dData::GuiQt3dData( DataItemType& dataitems )
  : minXUser(std::numeric_limits<double>::max())
  , maxXUser(std::numeric_limits<double>::min())
  , bXUserScale(false)
  , minYUser(std::numeric_limits<double>::max())
  , maxYUser(std::numeric_limits<double>::min())
  , bYUserScale(false)
  , minZUser(std::numeric_limits<double>::max())
  , maxZUser(std::numeric_limits<double>::min())
  , bZUserScale(false)
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
}

/* --------------------------------------------------------------------------- */
/* Destructor --                                                               */
/* --------------------------------------------------------------------------- */
GuiQt3dData::~GuiQt3dData() {
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
bool GuiQt3dData::update() {
  BUG_DEBUG("GuiQt3dData::update");
  clearBoundingBox();

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
  if (!m_columns || !m_rows) {
    return false;
  }
  getValue(0, 0);
  getValue(m_columns-1,m_rows-1);
  if (m_xvalues.size())
    m_reverseX = (m_xvalues[0] > m_xvalues.back()) ? true: false;
  if (m_yvalues.size())
    m_reverseY = (m_yvalues[0] > m_yvalues.back()) ? true: false;

  return true;
}

/* --------------------------------------------------------------------------- */
/* getSurfaceDataArray --                                                      */
/* --------------------------------------------------------------------------- */
const QSurfaceDataArray& GuiQt3dData::getSurfaceDataArray(){
  m_surfaceDataArray.clear();

  if (!update())
    return m_surfaceDataArray;

  // get new z values
  if (m_rows > 1) {
    m_surfaceDataArray.reserve(m_rows);
    for (int y = 0; y < m_rows; ++y) {
      QSurfaceDataRow dataRow;
      dataRow.reserve(m_columns);
      for (long x = 0; x < m_columns; ++x) {
        int _y = m_rows > 1 ? y : x;
        QVector3D pt = getValue( m_reverseX ? m_columns-1-x : x,
                                 m_reverseY ? m_rows-1-_y : _y);
        dataRow << QSurfaceDataItem(pt.x(), pt.z(), pt.y());
      }
      m_surfaceDataArray << dataRow;
    }
  } else {
    MatrixVector3dData matrix_data;
    int cntColMax;
    getMatrixData(matrix_data, cntColMax);
    // add data
    // swap axis, need increasíng x value
    int cNum = cntColMax;
    int rNum = matrix_data.size();
    for (int r=0; r<cNum; ++r){
      QSurfaceDataRow dataRow(cntColMax);
      for (int c=0; c<rNum; ++c){
        int rmin = std::min(r, (int)matrix_data[c].size()-1);
        QVector3D &pt = matrix_data[c][rmin];
        dataRow[c] = QSurfaceDataItem(pt.x(), pt.z(), pt.y());
      }
      m_surfaceDataArray << dataRow;
    }
  }
  return m_surfaceDataArray;
}

/* --------------------------------------------------------------------------- */
/* getBarDataArray --                                                          */
/* --------------------------------------------------------------------------- */
const QBarDataArray& GuiQt3dData::getBarDataArray(QStringList& row_labels, QStringList& column_labels){

  m_barDataArray.clear();
  row_labels.clear();
  column_labels.clear();

  if (!update())
    return m_barDataArray;

  // fill data
  if (m_rows > 1) {
    m_barDataArray.reserve(m_rows);
    for (int y = 0; y < m_rows; ++y) {
      QBarDataRow dataRow(m_columns);
      dataRow.reserve(m_columns);
      for (long x = 0; x < m_columns; ++x) {
        int _y = m_rows > 1 ? y : x;
        QVector3D pt = getValue( m_reverseX ? m_columns-1-x : x,
                                 m_reverseY ? m_rows-1-_y : _y);
        dataRow[x].setValue(pt.z());
      }
      m_barDataArray << dataRow;
    }

    // get axis labels
    for(auto val: m_xvalues) {
      column_labels.append(QString::number(val, 'g'));
    }
    for(auto val: m_yvalues) {
      row_labels.append(QString::number(val, 'g'));
    }
  } else {
#if 1
    MatrixVector3dData matrix_data;
    int cntColMax;
    getMatrixData(matrix_data, cntColMax);
    QList<double> row_values;
    double row_delta;

    // add data
    int cNum = cntColMax;
    int rNum = matrix_data.size();
    for (int r=0; r<rNum; ++r){
      QBarDataRow dataRow(cntColMax);
      int coff=0;
      for (int c=0; c<matrix_data[r].size(); ++c){
        QVector3D &pt = matrix_data[r][c];
        // process starting gap
        if (!c && cNum != (int)matrix_data[r].size()){
          for (int i=0; i<row_values.size(); ++i){
            if (pt.y() < (row_values[i]+row_delta)){
              coff = i;
              break;
            }
            dataRow[i].setValue(std::numeric_limits<double>::quiet_NaN());
          }
        }

        // add point
        if (c+coff < cNum)
          dataRow[c+coff].setValue(pt.z());

        // only once, add label
        if (!(c-coff)) {
          row_labels.append(QString::number(pt.x(), 'g'));
        }
        // only once, add label
        if (!r) {
          column_labels.append(QString::number(pt.y(), 'g'));
          row_values.append(pt.y());
          row_delta = 1e-2 * (row_values.back() - row_values[0]);
        }
      }
      m_barDataArray << dataRow;
    }
#endif
    return m_barDataArray;
  }

  return m_barDataArray;
}

/* --------------------------------------------------------------------------- */
/* getMatrixData --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::getMatrixData(MatrixVector3dData& matrix_data, int& cntColMax){
    // special case
    // all matrix_data(x, y, z) are vector matrix_data
  matrix_data.clear();
  cntColMax = 0;
  QVector3D pt0 = getValue(0,0);
  double oldX = pt0.x();
  int cnt = 0;

  // get matrix_data
  matrix_data.push_back(std::vector<QVector3D>());
  matrix_data.back().push_back(pt0);
  for (int x = 1; x < m_columns; ++x) {
    QVector3D pt = getValue(m_reverseX ? m_columns-1-x : x,
                            m_reverseX ? m_columns-1-x : x);
    if (std::abs(pt.x() - oldX) > 1e3*std::numeric_limits<double>::epsilon()){
      oldX = pt.x();
      cntColMax = std::max(cnt, cntColMax);
      cnt = 0;
      matrix_data.push_back(std::vector<QVector3D>());
    }
    matrix_data.back().push_back(pt);
    ++cnt;
  }
}

/* --------------------------------------------------------------------------- */
/* getIndex --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::getIndex(double x, double y, int& ix, int& iy)  const {
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
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */
QVector3D GuiQt3dData::getValue(int ix, int iy) {
  BUG_DEBUG("GuiQt3dData::getValue");
  double x, y;
  double z = std::numeric_limits<double>::quiet_NaN();

  // z value
  m_ixrows->setIndex( m_zaxis->Data(), m_rows > 1 ? iy : 0 );
  m_ixcols->setIndex( m_zaxis->Data(), ix );
  if (m_zaxis->getValue(z)) {
    z *= m_zaxis->getScaleFactor();
  } else
    z = std::numeric_limits<double>::quiet_NaN();

  // y value
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

  // x value
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
  updateBoundingBox(x, y, z);
  return QVector3D(x, y, z);
}

/* --------------------------------------------------------------------------- */
/* getXInterval --                                                             */
/* --------------------------------------------------------------------------- */
GuiQt3dData::Interval GuiQt3dData::getXInterval() const {
  // // get user or data minimum or maximum
  double minX = minXUser != std::numeric_limits<double>::max() ? minXUser : minXData;
  double maxX = maxXUser != std::numeric_limits<double>::min() ? maxXUser : maxXData;
  return Interval(minX, maxX);
}

/* --------------------------------------------------------------------------- */
/* getYInterval --                                                             */
/* --------------------------------------------------------------------------- */
GuiQt3dData::Interval GuiQt3dData::getYInterval() const {
  // // get user or data minimum or maximum
  double minY = minYUser != std::numeric_limits<double>::max() ? minYUser : minYData;
  double maxY = maxYUser != std::numeric_limits<double>::min() ? maxYUser : maxYData;
  return Interval(minY, maxY);
}

/* --------------------------------------------------------------------------- */
/* getZInterval --                                                             */
/* --------------------------------------------------------------------------- */
GuiQt3dData::Interval GuiQt3dData::getZInterval() const {
  // // get user or data minimum or maximum
  double minZ = minZUser != std::numeric_limits<double>::max() ? minZUser : minZData;
  double maxZ = maxZUser != std::numeric_limits<double>::min() ? maxZUser : maxZData;
  return Interval(minZ, maxZ);
}

/* --------------------------------------------------------------------------- */
/* setMinZUser --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::setMinZUser(double newOffset) {
  minZUser = newOffset;
  if (maxZUser == std::numeric_limits<double>::min())
    maxZUser = maxZData;
  update();
}

/* --------------------------------------------------------------------------- */
/* setMaxZUser --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::setMaxZUser(double newOffset) {
  maxZUser = newOffset;
  if (minZUser == std::numeric_limits<double>::max())
    minZUser = minZData;
  update();
}

/* --------------------------------------------------------------------------- */
/* resetZUser --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::resetZUser() {
  // already reseted
  if (!bZUserScale) return;

  bZUserScale= false;
  minZUser = std::numeric_limits<double>::max();
  maxZUser = std::numeric_limits<double>::min();

  update();
}

/* --------------------------------------------------------------------------- */
/* setRangeXUser --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::setRangeXUser(double minXValue, double maxXValue) {
  bXUserScale= true;
  minXUser = minXValue;
  maxXUser = maxXValue;
}

/* --------------------------------------------------------------------------- */
/* resetXUser --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::resetXUser() {
  bXUserScale= false;
  minXUser = std::numeric_limits<double>::max();
  maxXUser = std::numeric_limits<double>::min();
}

/* --------------------------------------------------------------------------- */
/* setRangeYUser --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::setRangeYUser(double minYValue, double maxYValue) {
  bYUserScale= true;
  minYUser = minYValue;
  maxYUser = maxYValue;
}

/* --------------------------------------------------------------------------- */
/* resetYUser --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::resetYUser() {
  bYUserScale= false;
  minYUser = std::numeric_limits<double>::max();
  maxYUser = std::numeric_limits<double>::min();
}

/* --------------------------------------------------------------------------- */
/* setRangeZUser --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::setRangeZUser(double minXValue, double maxXValue) {
  bZUserScale= true;
  setMinZUser(minXValue);
  setMaxZUser(maxXValue);
}

/* --------------------------------------------------------------------------- */
/* clearBoundingBox --                                                         */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::clearBoundingBox() {
  minXData =  std::numeric_limits<double>::max();
  maxXData =  -std::numeric_limits<double>::max();
  minYData =  std::numeric_limits<double>::max();
  maxYData =  -std::numeric_limits<double>::max();
  minZData =  std::numeric_limits<double>::max();
  maxZData =  -std::numeric_limits<double>::max();
}

/* --------------------------------------------------------------------------- */
/* updateBoundingBox --                                                        */
/* --------------------------------------------------------------------------- */
void GuiQt3dData::updateBoundingBox(double x, double y, double z) {
  if (minZData > z) minZData = z;
  if (maxZData < z) maxZData = z;
  if (minXData > x) minXData = x;
  if (maxXData < x) maxXData = x;
  if (minYData > y) minYData = y;
  if (maxYData < y) maxYData = y;
}

#endif
