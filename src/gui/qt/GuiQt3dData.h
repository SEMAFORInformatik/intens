#ifndef GUIQT3DDATAARRAY_H_
#define GUIQT3DDATAARRAY_H_

#include <QtGraphsWidgets/q3dsurfacewidgetitem.h>
#include <QtGraphsWidgets/q3dbarswidgetitem.h>

#include <limits>

class XferDataItemIndex;
class GuiPlotDataItem;
class QVector3D;

typedef std::map<std::string, GuiPlotDataItem*> DataItemType;
/**
 * main class of GuiQt3dData
 */

class GuiQt3dData {
public:
  struct Interval {
    Interval(double min, double max): min(min), max(max) {}
    double min;
    double max;
  };

  GuiQt3dData(DataItemType& dataitems);
  virtual ~GuiQt3dData();

  bool update();
  const QSurfaceDataArray& getSurfaceDataArray();
  const QBarDataArray& getBarDataArray(QStringList& row_labels, QStringList& column_labels);

  /**
     get index for datapool or catched data
  */
  void getIndex(double x, double y, int& ix, int& iy)  const;

  /**
     set minimum user scaled Z value
  */
  void setMinZUser(double newOffset);

  /**
     set maximum user scaled Z value
  */
  void setMaxZUser(double newOffset);

  /**
     reset scaled Z values
  */
  void resetZUser();

  /**
     set Z Range (through description file)
  */
  void setRangeZUser(double minZValue, double maxZValue);

  /**
     is user interactive user z scaling
  */
  bool isInteractiveZUserScale() { return bZUserScale==false; }

  /**
     set X Range (through description file)
  */
  void setRangeXUser(double minXValue, double maxXValue);

  /**
     reset scaled X values
  */
  void resetXUser();

  /**
     set Y Range (through description file)
  */
  void setRangeYUser(double minYValue, double maxYValue);

  /**
     reset scaled X values
  */
  void resetYUser();
  /**
     get x interval
  */
  Interval getXInterval() const;
  /**
     get y interval
  */
  Interval getYInterval() const;
  /**
     get z interval
  */
  Interval getZInterval() const;

private:
  typedef std::vector<std::vector<QVector3D>> MatrixVector3dData;
  void getMatrixData(MatrixVector3dData &data, int& cntColMax);
  QVector3D getValue(int ix, int iy);
  void clearBoundingBox();
  void updateBoundingBox(double x, double y, double z);

private:
  // -----------------------------------------------------------------------
  //  Unimplemented Constructors
  // -----------------------------------------------------------------------
  //    GuiQt3dData();
  GuiQt3dData(const GuiQt3dData&);
  GuiQt3dData& operator=(const GuiQt3dData&);

private:
  QSurfaceDataArray m_surfaceDataArray;
  QBarDataArray     m_barDataArray;

  // colormap offsets
  double         minXUser;
  double         maxXUser;
  bool           bXUserScale;
  double         minYUser;
  double         maxYUser;
  bool           bYUserScale;
  double         minZUser;
  double         maxZUser;
  bool           bZUserScale;
  double         minZData;
  double         maxZData;
  double         minXData;
  double         maxXData;
  double         minYData;
  double         maxYData;
  double         maxDiffDelta;

  int m_rows;
  int m_columns;
  double         minStepX;
  double         minStepY;

  GuiPlotDataItem *m_xaxis, *m_yaxis, *m_zaxis;
  XferDataItemIndex *m_ixrows;
  XferDataItemIndex *m_ixcols;
  std::vector<double> m_xvalues;
  std::vector<double> m_yvalues;
  bool m_reverseX;
  bool m_reverseY;

  typedef DataItemType::value_type DataItemPair;
  DataItemType&         m_dataitems;
};

#endif // !defined(GUIQTGRAPHSPLOTDATA_H__INCLUDED_)
