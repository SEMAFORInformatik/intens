
#ifndef PLOT_AXIS_H
#define PLOT_AXIS_H

#if HAVE_PROTOBUF
#include "protobuf/Plot2D.pb.h"
#endif
#include <string>
#include <jsoncpp/json/value.h>

typedef long TransactionNumber;

class XferDataItem;
class Scale;
class PlotAxis {
 public:
  PlotAxis();
  virtual ~PlotAxis();
  PlotAxis( const PlotAxis & );
public:
  enum AxisType{
    yAxis
    , xAxis
  };
 public:
  void setFormat( char fmt, int fieldwidth, int prec ){
    m_fmt = fmt;
    m_fieldwidth = fieldwidth;
    m_prec = prec;
  }
  char getFmt(){ return m_fmt; }
  int  getFieldWidth(){ return m_fieldwidth; }
  int  getPrec(){ return m_prec; }
  bool isUpdated( TransactionNumber trans );
  void setLogarithmic( bool value = true );
  void setScaleEnable( bool value );
  void setScaleEnable();
  void setOrigin( const double value );
  bool isLogarithmic() const { return m_isLogarithmic; }
  bool getOrigin( double &value );
  bool getMin( double &value, bool with_factor = false );
  bool getMax( double &value, bool with_factor = false );
  double getAspectRatio() { return m_aspectRatio; };
  void setMin( const double min );
  void setMin();
  void setMax( const double max );
  void setMax();
  void setAspectRatio(double v) { m_aspectRatio = v; };
  void setXferAspectRatio(XferDataItem *xferAspectRatio);
  void setMinStartValue( const double value );
  void setMaxStartValue( const double value );
  void resetScaleValues();
  void setAllXferScale( XferDataItem *xferScaleMin
			, XferDataItem *xferScaleMax
			, XferDataItem *xferScaleEnable
			, XferDataItem *xferAspectRatio);
  void setAllXferScale( XferDataItem *xferScaleMin, Scale *min_scale
			, XferDataItem *xferScaleMax, Scale *max_scale
			, XferDataItem *xferScaleEnable
			, XferDataItem *xferAspectRatio);
  XferDataItem *getXferScaleMin(){ return m_xferScaleMin; }
  Scale *getMinScale(){ return m_min_scale; }
  XferDataItem *getXferScaleMax(){ return m_xferScaleMax; }
  Scale *getMaxScale(){ return m_max_scale; }
  bool isScaleEnabled();
  XferDataItem *getXferScaleEnable(){ return m_xferScaleEnable; }
  XferDataItem *getXferAspectRatio(){ return m_xferAspectRatio; }

  // GuiIndexListener methods
  bool acceptIndex( const std::string &, int );
  void setIndex( const std::string &name, int inx );
  void serializeXML(std::ostream&, std::string name);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::Plot2D::Axis* axis, bool onlyUpdated = false);
#endif
private:
  bool getMinStartValue( double &value );
  bool getMaxStartValue( double &value );
  bool getScaleEnableStartValue( bool &value );
  bool getAspectRatioStartValue( int &value );
  void setStartValues();
  private:
  bool          m_isLogarithmic;
  bool          m_isOriginDefined;
  bool          m_scaleEnable;
  double        m_origin;
  double       *m_minStartValue;
  double       *m_maxStartValue;
  bool         *m_scaleEnabledStartValue;
  int          *m_aspectRatioStartValue;
  double       *m_min;
  double       *m_max;
  double        m_aspectRatio;
  XferDataItem *m_xferScaleMin;
  Scale        *m_min_scale;
  XferDataItem *m_xferScaleMax;
  Scale        *m_max_scale;
  XferDataItem *m_xferScaleEnable;
  XferDataItem *m_xferAspectRatio;
  char          m_fmt;
  int           m_fieldwidth;
  int           m_prec;
};

#endif
