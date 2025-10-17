
#include <limits>

#include "gui/GuiThermo.h"

#include "xfer/XferDataItem.h"
#include "xfer/Scale.h"
#include "app/ColorSet.h"

#include "utils/JsonUtils.h"

INIT_LOGGER();

//------------------------------------------------------------------
// Constuctor / Destructor
//------------------------------------------------------------------

GuiThermo::GuiThermo()
  : m_xfer(0)
  , m_hideLabel( false )
  , m_hideUnits( false )
  , m_xfer_label(0)
  , m_xfer_units(0)
  , m_scale(0)
  , m_offset(0)
  , m_min(0)
  , m_max(100)
  , m_xfer_offset(0)
  , m_xfer_min(0)
  , m_xfer_max(0)
  , m_offset_scale(0)
  , m_min_scale(0)
  , m_max_scale(0)
  , m_width(0)
  , m_height(0)
  , m_colorScaleOption( false )
  , m_colorScaleOptionScaled( false )
  , m_colorset(0)
  , m_orientation( GuiElement::orient_Horizontal )
  , m_alarm_level(std::numeric_limits<double>::max())
  , m_xfer_alarm_level(0)
  , m_alarm_level_scale(0)
  , m_inverted( true )
{}

GuiThermo::GuiThermo( const GuiThermo &thermo )
  : m_xfer(0)
  , m_hideLabel( thermo.m_hideLabel )
  , m_hideUnits( thermo.m_hideUnits )
  , m_label( thermo.m_label )
  , m_xfer_label(0)
  , m_units( thermo.m_units )
  , m_xfer_units(0)
  , m_scale(0)
  , m_offset( thermo.m_offset )
  , m_min( thermo.m_min )
  , m_max( thermo.m_max )
  , m_xfer_offset(0)
  , m_xfer_min(0)
  , m_xfer_max(0)
  , m_offset_scale(0)
  , m_min_scale(0)
  , m_max_scale(0)
  , m_width( thermo.m_width )
  , m_height( thermo.m_height )
  , m_colorsetName( thermo.m_colorsetName )
  , m_colorScaleOption( thermo.m_colorScaleOption )
  , m_colorScaleOptionScaled( thermo.m_colorScaleOptionScaled )
  , m_colorset( thermo.m_colorset )
  , m_orientation( thermo.m_orientation )
  , m_alarm_level( thermo.m_alarm_level )
  , m_xfer_alarm_level(0)
  , m_alarm_level_scale(0)
  , m_alarmColorName( thermo.m_alarmColorName )
  , m_inverted( true )
{
  if(thermo.m_xfer)
    m_xfer=new XferDataItem( *(thermo.m_xfer) );
  if (thermo.m_xfer_label)
    m_xfer_label=new XferDataItem( *(thermo.m_xfer_label) );
  if (thermo.m_xfer_units)
    m_xfer_units=new XferDataItem( *(thermo.m_xfer_units) );
  if(thermo.m_scale)
    m_scale=thermo.m_scale->copy();
  if(thermo.m_xfer_offset)
    m_xfer_offset=new XferDataItem( *(thermo.m_xfer_offset) );
  if(thermo.m_xfer_min)
    m_xfer_min=new XferDataItem( *(thermo.m_xfer_min) );
  if(thermo.m_xfer_max)
    m_xfer_max=new XferDataItem( *(thermo.m_xfer_max) );
  if(thermo.m_offset_scale)
    m_offset_scale=thermo.m_offset_scale->copy();
  if(thermo.m_min_scale)
    m_min_scale=thermo.m_min_scale->copy();
  if(thermo.m_max_scale)
    m_max_scale=thermo.m_max_scale->copy();
  if (thermo.m_xfer_alarm_level)
    m_xfer_alarm_level=new XferDataItem( *(thermo.m_xfer_alarm_level) );
  if (thermo.m_alarm_level_scale)
    m_alarm_level_scale=thermo.m_alarm_level_scale->copy();
}

GuiThermo::~GuiThermo(){
  delete m_xfer;
  delete m_xfer_label;
  delete m_xfer_units;
  delete m_scale;
  delete m_xfer_offset;
  delete m_xfer_min;
  delete m_xfer_max;
  delete m_offset_scale;
  delete m_min_scale;
  delete m_max_scale;
  delete m_xfer_alarm_level;
  delete m_alarm_level_scale;
}

//-------------------------------------------------------------------
// isDataItemUpdated --
//-------------------------------------------------------------------

bool GuiThermo::isDataItemUpdated( TransactionNumber trans ) {
  if( m_xfer ){
    if( m_xfer->isDataItemUpdated(trans) ){
      return true;
    }
  }
  if( m_scale ){
    if( m_scale->isUpdated(trans) ){
      return true;
    }
  }
  if( m_xfer_min ){
    if( m_xfer_min->isDataItemUpdated(trans)  ){
      return true;
    }
  }
  if( m_min_scale ){
    if( m_min_scale->isUpdated(trans) ){
      return true;
    }
  }
  if( m_xfer_max ){
    if( m_xfer_max->isDataItemUpdated(trans) ){
      return true;
    }
  }
  if( m_max_scale ){
    if( m_max_scale->isUpdated(trans) ){
      return true;
    }
  }
  if( m_xfer_label ){
    if( m_xfer_label->isUpdated(trans) ){
      return true;
    }
  }
  if( m_xfer_units ){
    if( m_xfer_units->isUpdated(trans) ){
      return true;
    }
  }
  if( m_xfer_alarm_level ){
    if( m_xfer_alarm_level->isDataItemUpdated(trans)  ){
      return true;
    }
  }
  if( m_alarm_level_scale ){
    if( m_alarm_level_scale->isUpdated(trans) ){
      return true;
    }
  }
  return false;
}


//-------------------------------------------------------------------
// setAllDimensionIndizes --
//-------------------------------------------------------------------

void GuiThermo::setAllDimensionIndizes(){
  if (m_xfer) m_xfer->setDimensionIndizes();
  if (m_xfer_label) m_xfer_label->setDimensionIndizes();
  if (m_xfer_units) m_xfer_units->setDimensionIndizes();
  if (m_xfer_offset) m_xfer_offset->setDimensionIndizes();
  if (m_xfer_min) m_xfer_min->setDimensionIndizes();
  if (m_xfer_max) m_xfer_max->setDimensionIndizes();
  if (m_xfer_alarm_level) m_xfer_alarm_level->setDimensionIndizes();
}

//-------------------------------------------------------------------
// getLabel --
//-------------------------------------------------------------------

const std::string &GuiThermo::getLabel(){
  if( m_xfer_label ){
    if( !m_xfer_label->getValue(m_label) ){
      m_label="";
    }
  }
  else if( !m_label.empty() ){
  }
  else{
    m_label=m_xfer->getLabel();
  }
  return m_label;
  }

//-------------------------------------------------------------------
// getUnits --
//-------------------------------------------------------------------

const std::string GuiThermo::getUnits(){
  if( m_xfer_units ){
    if( !m_xfer_units->getValue(m_units) ){
      m_units="";
    }
  }
  else if( !m_units.empty() ){
  }
  else{
    m_units=m_xfer ? m_xfer->getUnit() : "";
  }
  return m_units;
}

//-------------------------------------------------------------------
// setMinMaxOffset --
//-------------------------------------------------------------------

void GuiThermo::setMinMaxOffset(){
  double _min=0,_max=100;
  // min, max
  if( xfer_min() && xfer_max() ){
    xfer_min()->getValue(_min);
    if( min_scale() )
      _min = _min * min_scale()->getValue();
    xfer_max()->getValue(_max);
    if( max_scale() )
      _max = _max * max_scale()->getValue();
    if( _min != min() || _max != max() ){
      min()=_min;
      max()=_max;
    }
  }
  // offset
  if( xfer_offset() ){
    xfer_offset()->getValue(offset());
    if( offset_scale() )
      offset() = offset() * offset_scale()->getValue();
  }
  BUG_DEBUG("range["<<min()<<", "<<max()<<"] offset["<<offset()<<"]");
  BUG_DEBUG("range["<<_min<<", "<<_max<<"] offset["<<offset()<<"]");
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiThermo::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  setMinMaxOffset();
  getElement()->writeJsonProperties(jsonObj);
  jsonObj["width"] = m_width;
  jsonObj["height"] = m_height;
  jsonObj["label"] = getLabel();
  jsonObj["unit"] = getUnits();
  jsonObj["min"] = min();
  jsonObj["max"] = max();
  jsonObj["offset"] = offset();
  jsonObj["colorset"] = m_colorsetName;
  jsonObj["orientation"] = m_orientation == GuiElement::orient_Vertical ? "vertical" : "horizontal";
  // colorset data
  double sfac = scale() ? scale()->getValue() : 1.0 ;
  jsonObj["scale"] = ch_semafor_intens::JsonUtils::getRealValueNotJson5(sfac);
  ColorSet::ColorMap dataColorMap;
  if(colorset()) {
    colorset()->getColorMap(dataColorMap, min(), max(),  (sfac < 0));
  }
  Json::Value jsonAry = Json::Value(Json::arrayValue);

  for(ColorSet::ColorMap::iterator it = dataColorMap.begin();
       it != dataColorMap.end(); ++it) {
    ///    if (std::isnan(it->first*sfac)) continue;
    Json::Value jsonElem = Json::Value(Json::objectValue);
    jsonElem["value"] = ch_semafor_intens::JsonUtils::getRealValueNotJson5(it->first*sfac);
    jsonElem["color"] = it->second;
    jsonAry.append(jsonElem);
  }
  jsonObj["colorlist"] = jsonAry;
  return true;
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiThermo::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  setMinMaxOffset();
  auto element = eles->add_thermos();
  element->set_allocated_base(getElement()->writeProtobufProperties());

  element->mutable_base()->set_width(width());
  element->mutable_base()->set_height(height());
  element->set_label(getLabel());
  // colorset data
  double sfac = scale() ? scale()->getValue() : 1.0 ;
  element->set_scale(sfac);
  ColorSet::ColorMap dataColorMap;
  if(colorset()) {
    colorset()->getColorMap(dataColorMap, min(), max(),  (sfac < 0));
  }
  Json::Value jsonAry = Json::Value(Json::arrayValue);

  for(ColorSet::ColorMap::iterator it = dataColorMap.begin();
       it != dataColorMap.end(); ++it) {
    ///    if (std::isnan(it->first*sfac)) continue;
    Json::Value jsonElem = Json::Value(Json::objectValue);
    auto entry = element->add_color_list();
    entry->set_value(it->first*sfac);
    entry->set_color(it->second);
  }
  return true;
}
#endif
