
#if !defined(GUI_THERMO_H)
#define GUI_THERMO_H

#include <string>

#include "gui/GuiElement.h"
#include "xfer/XferDataItem.h"

typedef long TransactionNumber;

class ColorSet;
class XferDataItem;
class Scale;

class GuiThermo{
public:
  GuiThermo();
  GuiThermo( const GuiThermo &thermo );
  virtual ~GuiThermo();

public:
  void setRange( const double min, const double max ){
    m_min = min;
    m_max = max;
  }

  void setRange( XferDataItem * min, Scale * min_scale, XferDataItem * max, Scale * max_scale ){
    m_xfer_min = min;
    m_xfer_max = max;
    m_min_scale = min_scale;
    m_max_scale = max_scale;
  }

  void setOffset( const double offset ){
    m_offset = offset;
  }

  void setOffset( XferDataItem * offset, Scale * offset_scale ){
    m_xfer_offset = offset;
    m_offset_scale = offset_scale;
  }

  void setXfer( XferDataItem *xfer, Scale *scale ){
    m_xfer = xfer;
    m_scale = scale;
  }
  void setOrientation( GuiElement::Orientation o ){
    m_orientation = o;
  }
  void setSize( const int width, const int height ){
    m_width = width;
    m_height = height;
  }

  void setColorsetName( const std::string &name ){
    m_colorsetName = name;
  }

  void setColorScaleOption( bool colorScale ){
    m_colorScaleOption = colorScale;
  }

  void setColorScaleOptionScaled( bool colorScaleScaled ){
    m_colorScaleOptionScaled = colorScaleScaled;
  }

  virtual void setScaleFormat( const std::string& format ) = 0;

  void hideLabel(){ m_hideLabel=true; }
  void hideUnits(){ m_hideUnits=true; }
  void setLabel( const std::string &label ){
    m_label=label;
  }
  void setUnits( const std::string &units ){
    m_units=units;
  }
  void setLabel( XferDataItem *xfer ){
    m_xfer_label=xfer;
  }
  void setUnits( XferDataItem *xfer ){
    m_xfer_units=xfer;
  }

  bool isDataItemUpdated( TransactionNumber trans );

  void setAlarmLevel( const double dbl ){
    m_alarm_level = dbl;
  }
  void setAlarmLevel( XferDataItem * min, Scale * min_scale){
    m_xfer_alarm_level = min;
    m_alarm_level_scale = min_scale;
  }
  void setAlarmColorName( const std::string &name ){
    m_alarmColorName = name;
  }
  void setInverted(bool inv){ m_inverted=inv; }
  bool Inverted(){ return m_inverted; }

protected:
  void setAllDimensionIndizes();
  bool showLabel(){ return !m_hideLabel; }
  bool showUnits(){ return !m_hideUnits; }
  XferDataItem *xfer(){ return m_xfer;}
  int width(){ return m_width; }
  int height(){ return m_height; }
  XferDataItem *xfer_offset(){ return m_xfer_offset; }
  XferDataItem *xfer_min(){ return m_xfer_min; }
  XferDataItem *xfer_max(){ return m_xfer_max; }
  GuiElement::Orientation orientation(){ return m_orientation; }
  void setMinMaxOffset();
  double &offset(){ return m_offset; }
  double &min(){ return m_min; }
  double &max(){ return m_max; }
  const std::string &colorsetName(){ return m_colorsetName; }
  bool colorScaleOption(){ return m_colorScaleOption; }
  bool colorScaleOptionScaled(){ return m_colorScaleOptionScaled; }
  void setColorset( ColorSet*set ){ m_colorset=set; }
  Scale *offset_scale(){ return m_offset_scale; }
  Scale *min_scale(){ return m_min_scale; }
  Scale *max_scale(){ return m_max_scale; }
  Scale *scale(){ return m_scale; }
  ColorSet *colorset(){ return m_colorset; }
  const std::string &getLabel();
  const std::string getUnits();

  double &alarm_level(){ return m_alarm_level; }
  XferDataItem *xfer_alarm_level(){ return m_xfer_alarm_level; }
  Scale *alarm_level_scale(){ return m_alarm_level_scale; }
  const std::string &alarmColorName(){ return m_alarmColorName; }

  virtual GuiElement * getElement() = 0;
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
private:
  XferDataItem *m_xfer;
  bool          m_hideLabel;
  bool          m_hideUnits;
  std::string   m_label;
  XferDataItem *m_xfer_label;
  std::string   m_units;
  XferDataItem *m_xfer_units;

  Scale                  *m_scale;
  double                  m_offset;
  double                  m_min;
  double                  m_max;
  XferDataItem           *m_xfer_offset;
  XferDataItem           *m_xfer_min;
  XferDataItem           *m_xfer_max;
  Scale                  *m_offset_scale;
  Scale                  *m_min_scale;
  Scale                  *m_max_scale;
  int                     m_width;
  int                     m_height;
  std::string             m_colorsetName;
  bool                    m_colorScaleOption;
  bool                    m_colorScaleOptionScaled;
  ColorSet               *m_colorset;
  GuiElement::Orientation m_orientation;

  double                  m_alarm_level;
  XferDataItem           *m_xfer_alarm_level;
  Scale                  *m_alarm_level_scale;
  std::string             m_alarmColorName;
  bool                    m_inverted;

};

#endif
