#ifndef GUIQTGRAPHSPLOTDATA_H_
#define GUIQTGRAPHSPLOTDATA_H_

#include <QtGraphsWidgets/q3dsurfacewidgetitem.h>

#include <limits>

class XferDataItemIndex;
class GuiPlotDataItem;
class QVector3D;

typedef std::map<std::string, GuiPlotDataItem*> DataItemType;
/**
 * main class of GuiQtGraphsPlotData
 */

class GuiQtGraphsPlotData: public QSurfaceDataArray {
public:
  struct Interval {
    Interval(double min, double max): min(min), max(max) {}
    double min;
    double max;
  };

  GuiQtGraphsPlotData(DataItemType& dataitems);
  virtual ~GuiQtGraphsPlotData();

  void updateData();

  virtual double value(double x, double y) const;

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
  Interval getXInterval();
  /**
     get y interval
  */
  Interval getYInterval();
  /**
     get z interval
  */
  Interval getZInterval();
private:
  QVector3D getZValue(int ix, int iy);

private:
  // -----------------------------------------------------------------------
  //  Unimplemented Constructors
  // -----------------------------------------------------------------------
  //    GuiQtGraphsPlotData();
  GuiQtGraphsPlotData(const GuiQtGraphsPlotData&);
  GuiQtGraphsPlotData& operator=(const GuiQtGraphsPlotData&);

private:
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
  double         minZ;
  double         maxZ;
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
