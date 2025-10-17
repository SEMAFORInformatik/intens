#ifndef GUIQWTCONTOURPLOTDATA_H_
#define GUIQWTCONTOURPLOTDATA_H_

#if QWT_VERSION >= 0x060000

#include <qwt_matrix_raster_data.h>
#include <limits>

class XferDataItemIndex;
class GuiPlotDataItem;
typedef std::map<std::string, GuiPlotDataItem*> DataItemType;

/**
 * main class of GuiQwtContourPlotData
 */

class GuiQwtContourPlotData: public QwtMatrixRasterData {
 public:

  GuiQwtContourPlotData(DataItemType& dataitems);
  virtual ~GuiQwtContourPlotData();

  void updateData();

  // virtual QwtRasterData functions
  virtual QRectF pixelHint( const QRectF&  area ) const;
  virtual double value(double x, double y) const;
  QwtInterval getInterval (Qt::Axis);

  /**
     get index for datapool or catched data
  */
  void getIndex(double x, double y, int& ix, int& iy)  const;

    /**
       get zoom base (all scan regions are visible)
     */
    QRectF getZoomBase() const;

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

  private:
    double getZValue(int ix, int iy);
    double getCachedZValue(int ix, int iy) const;
    void updateZInterval();

  private:
    // -----------------------------------------------------------------------
    //  Unimplemented Constructors
    // -----------------------------------------------------------------------
    //    GuiQwtContourPlotData();
    GuiQwtContourPlotData(const GuiQwtContourPlotData&);
    GuiQwtContourPlotData& operator=(const GuiQwtContourPlotData&);

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
    std::vector< std::vector<double> >  m_vector;
    std::vector<double> m_xvalues;
    std::vector<double> m_yvalues;
    bool m_reverseX;
    bool m_reverseY;

    typedef DataItemType::value_type DataItemPair;
    DataItemType&         m_dataitems;
};

#endif

#endif // !defined(GUIQWTCONTOURPLOTDATA_H__INCLUDED_)
