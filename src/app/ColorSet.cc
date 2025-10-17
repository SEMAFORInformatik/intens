#include <assert.h>
#include <limits>
#include <iostream>
#include "xfer/XferDataItem.h"
#include "xfer/Scale.h"
#include "app/ColorSet.h"
#ifdef HAVE_QT
#include <QColor>
#endif
#if !defined HAVE_HEADLESS
#include <qwt_color_map.h>
#endif

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

ColorSet::ColorSet()
  : m_current( 0 ){
}

ColorSet::~ColorSet(){
}

ColorSetItem::~ColorSetItem(){
  delete m_xfer_lower_value;
  delete m_xfer_upper_value;
  delete m_scale_lower;
  delete m_scale_upper;
}



/*=============================================================================*/
/* Member Functions                                                            */
/*=============================================================================*/


/* --------------------------------------------------------------------------- */
/* setLowerbound --                                                            */
/* --------------------------------------------------------------------------- */

void ColorSet::setLowerbound( int what, const std::string &val ){
  assert( m_current == 0 );
  m_current = new ColorSetStringItem( what, val );
}

/* --------------------------------------------------------------------------- */
/* setLowerbound --                                                            */
/* --------------------------------------------------------------------------- */

bool ColorSet::setLowerbound( int what, XferDataItem *xfer, Scale *scale ){
  assert( m_current == 0 );
  switch( xfer->getDataType() ){
  case DataDictionary::type_Real:
  case DataDictionary::type_Integer:
    m_current = new ColorSetRealItem( what, xfer, scale );
    break;
  case DataDictionary::type_String:
    m_current = new ColorSetStringItem( what, xfer );
    break;
  default:
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setLowerbound --                                                            */
/* --------------------------------------------------------------------------- */

void ColorSet::setLowerbound( int what, double val ){
  assert( m_current == 0 );
  m_current = new ColorSetRealItem( what, val );
}

/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

bool ColorSet::setUpperbound( int what, const std::string &val ){
  assert( m_current != 0 );
  return m_current->setUpperbound( what, val );
}

/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

bool ColorSet::setUpperbound( int what, XferDataItem *xfer, Scale *scale ){
  assert( m_current != 0 );
  return m_current->setUpperbound( what, xfer, scale );
}

/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

bool ColorSet::setUpperbound( int what, double val ){
  assert( m_current != 0 );
  return m_current->setUpperbound( what, val );
}

/* --------------------------------------------------------------------------- */
/* setInvalid --                                                               */
/* --------------------------------------------------------------------------- */

void ColorSet::setInvalid(){
  assert( m_current == 0 );
  m_current = new ColorSetItem();
}
/* --------------------------------------------------------------------------- */
/* setAlways --                                                                */
/* --------------------------------------------------------------------------- */

void ColorSet::setAlways(){
  assert( m_current == 0 );
  m_current = new ColorSetItemAlways();
}

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void ColorSet::setColors( const std::string &bg, const std::string &fg ){
  assert( m_current != 0 );
  m_current->setColors( bg, fg );
  m_list.push_back( m_current );
  m_current = 0;
}

void ColorSet::setColors( XferDataItem *bg, XferDataItem *fg ){
  assert( m_current != 0 );
  m_current->setColors( bg, fg );
  m_list.push_back( m_current );
  m_current = 0;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool ColorSet::isDataItemUpdated(TransactionNumber trans) const {
  ColorList::const_iterator i;
  for( i = m_list.begin(); i != m_list.end(); ++i ){
    if( (*i)->isDataItemUpdated(trans) ){
      return true;
    }
  }
  return false;
}


/* --------------------------------------------------------------------------- */
/* getColorItem --                                                             */
/* --------------------------------------------------------------------------- */

ColorSetItem *ColorSet::getColorItem( const std::string &val ){
  ColorList::const_iterator i;
  for( i = m_list.begin(); i != m_list.end(); ++i ){
    if( (*i)->equal( val ) ){
      return (*i);
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getColorMap --                                                              */
/* --------------------------------------------------------------------------- */
bool ColorSet::getColorMap(ColorMap& colorMap, double minValue, double maxValue, bool reverse) {
  BUG_DEBUG("ColorMap minValue["<<minValue<<"] maxValue["<<maxValue<<"]");
  colorMap.clear();
  for( ColorList::const_iterator i = m_list.begin(); i != m_list.end(); ++i ){
    ColorSetRealItem* ri = dynamic_cast<ColorSetRealItem*>(*i);
    if (!ri) continue;
    double min=ri->getLowerbound();
    double max=ri->getUpperbound();
    // std::string bg=ri->background();
    std::string fg=ri->foreground();
    double v = (min+max)/2.;
    double delta = 100*std::numeric_limits<double>::epsilon();
    BUG_DEBUG("ColorMap min["<<min<<"] max["<<max<<"] delta["<<delta<<"]");
    BUG_DEBUG("ColorMap pos["<< ( reverse ? (1-(v-minValue)/(maxValue-minValue)) :
                                  (v-minValue)/(maxValue-minValue) )
              <<"] color["<<fg<<"] reverse["<<reverse<<"]");
    if (v > (minValue-delta) && v < (maxValue+delta)) {
      if (reverse)
        colorMap[1-(v-minValue)/(maxValue-minValue)] = fg;
      else
        colorMap[(v-minValue)/(maxValue-minValue)] = fg;
    }
  }
  return true;
}

#ifdef HAVE_QT
/* --------------------------------------------------------------------------- */
/* getColorMap --                                                              */
/* --------------------------------------------------------------------------- */
bool ColorSet::getColorMap(QwtLinearColorMap& colorMap, double minValue, double maxValue, bool reverse) {

#ifdef HAVE_HEADLESS
  return false;
#else
  BUG_DEBUG("ColorMap minValue["<<minValue<<"] maxValue["<<maxValue<<"]");
  ColorSet::ColorMap dataColorMap;
  getColorMap(dataColorMap, minValue, maxValue, reverse);
  for( ColorMap::iterator it = dataColorMap.begin();
       it != dataColorMap.end(); ++it) {
    colorMap.addColorStop (it->first, QColor(QString::fromStdString(it->second)) );
  }
  return true;
#endif
}
#endif

/* --------------------------------------------------------------------------- */
/* getInterpolatedColors --                                                    */
/* --------------------------------------------------------------------------- */

bool ColorSet::getInterpolatedColors ( double val, std::string& background, std::string& foreground ) {

  // no interpolation => return color
  ColorSetItem *colorsetItem = getColorItem( val );
  if (colorsetItem) {
    background = colorsetItem->background();
    foreground = colorsetItem->foreground();
    return true;
  }

  // interpolation only if ColorSetRealItems
  // => find lower and upper colors
  std::string  backgroundLow, backgroundUp;
  std::string  foregroundLow, foregroundUp;
  double minLow=std::numeric_limits<double>::max(), minUp=std::numeric_limits<double>::max(), dMin;
  for( ColorList::const_iterator i = m_list.begin(); i != m_list.end(); ++i ){
    ColorSetRealItem* ri = dynamic_cast<ColorSetRealItem*>(*i);
    if (!ri) continue;
    double min=ri->getLowerbound();
    double max=ri->getUpperbound();
    if (val < min && (min-val) < minUp) {
      minUp = min-val;
      backgroundUp = ri->background();
      foregroundUp = ri->foreground();
    } else
      if (val > max && (val-max) < minLow) {
	minLow = val-max;
	dMin = max;
	backgroundLow = ri->background();
	foregroundLow = ri->foreground();
      }
  }

  // we did not found lower and upper band
  if (backgroundLow.size() == 0 && backgroundUp.size() == 0) {
    return false;
  } else if (backgroundLow.size() == 0) {
    // only upper band found
    background = backgroundUp;
    foreground = foregroundUp;
  } else if (backgroundUp.size() == 0) {
    // only lower band found
    background = backgroundLow;
    foreground = foregroundLow;
  }

#ifdef HAVE_QT
  // interpolation of colors
  double coeff = (val-dMin)/(minLow + minUp);
  QColor lowB(QString::fromStdString(backgroundLow));
  QColor lowF(QString::fromStdString(foregroundLow));
  QColor upB(QString::fromStdString(backgroundUp));
  QColor upF(QString::fromStdString(foregroundUp));
  QColor retColorB(
		   lowB.red()   + coeff*(upB.red()   - lowB.red()),
		   lowB.green() + coeff*(upB.green() - lowB.green()),
		   lowB.blue()  + coeff*(upB.blue()  - lowB.blue()),
		   lowB.alpha() + coeff*(upB.alpha() - lowB.alpha())
		   );
  QColor retColorF(
		   lowF.red()   + coeff*(upF.red()   - lowF.red()),
		   lowF.green() + coeff*(upF.green() - lowF.green()),
		   lowF.blue()  + coeff*(upF.blue()  - lowF.blue()),
		   lowF.alpha() + coeff*(upF.alpha() - lowF.alpha())
		   );
  background = retColorB.name(QColor::HexArgb).toStdString();
  foreground = retColorF.name(QColor::HexArgb).toStdString();
#else
  // we do not know how to interpolate => choose lower color
  background = backgroundLow;
  foreground = foregroundLow;
#endif
  return true;
}

/* --------------------------------------------------------------------------- */
/* getColorItem --                                                             */
/* --------------------------------------------------------------------------- */

ColorSetItem *ColorSet::getColorItem( double val ){
  ColorList::const_iterator i;
  for( i = m_list.begin(); i != m_list.end(); ++i ){
    if( (*i)->equal( val ) ){
      return (*i);
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getColorItemInvalid --                                                      */
/* --------------------------------------------------------------------------- */

ColorSetItem *ColorSet::getColorItemInvalid() const{
  ColorList::const_iterator i;
  for( i = m_list.begin(); i != m_list.end(); ++i ){
    if( (*i)->invalid() ){
      return (*i);
    }
  }
   return 0;
}

/* --------------------------------------------------------------------------- */
/* getColorItemAlways --                                                       */
/* --------------------------------------------------------------------------- */

ColorSetItem *ColorSet::getColorItemAlways() const{
  ColorList::const_iterator i;
  for( i = m_list.begin(); i != m_list.end(); ++i ){
    if( (*i)->always() ){
      return (*i);
    }
  }
   return 0;
}

double ColorSetRealItem::getLowerbound(){
  if( m_xfer_lower_value ){
    if(m_xfer_lower_value->getValue( m_lower_value )) {
      m_lower_value = m_lower_value * m_scale_lower->getValue();
    } else
      m_lower_value = std::numeric_limits<double>::quiet_NaN();
  }
  return m_lower_value;
}
double ColorSetRealItem::getUpperbound(){
  if( m_xfer_upper_value ){
    if (m_xfer_upper_value->getValue( m_upper_value )){
      m_upper_value = m_upper_value * m_scale_upper->getValue();
    } else
      m_upper_value = std::numeric_limits<double>::quiet_NaN();
  }
  return std::max(m_upper_value, getLowerbound());
}

/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

bool ColorSetRealItem::setUpperbound( int what, double val ){
  if( val < m_lower_value ){
    return false;
  }
  m_upper_what = what;
  m_upper_value = val;
  return true;
}

/* --------------------------------------------------------------------------- */
/* equal --                                                                    */
/* --------------------------------------------------------------------------- */

bool ColorSetRealItem::equal( double val ){
  if( m_lower_what == 99 ){
    return false;
  }

  if( m_xfer_lower_value ){
    if (m_xfer_lower_value->getValue( m_lower_value )) {
      m_lower_value = m_lower_value * m_scale_lower->getValue();
    } else
      m_lower_value = std::numeric_limits<double>::quiet_NaN();
  }

  if( m_xfer_upper_value ){
    if (m_xfer_upper_value->getValue( m_upper_value )){
      m_upper_value = m_upper_value * m_scale_upper->getValue();
    } else
      m_upper_value = std::numeric_limits<double>::quiet_NaN();
  }

  if( m_upper_what == 99 ){
    if( m_lower_what < 0 ){
      return val < m_lower_value;
    }
    if( m_lower_what > 0 ){
      return val > m_lower_value;
    }
    return val == m_lower_value;
  }

  if( m_lower_what < 0 ){
    if( val < m_lower_value ){
      return true;
    }
  }
  if( m_lower_what == 0 ){
    if( val < m_lower_value ){
      return false;
    }
  }
  if( m_lower_what > 0 ){
    if( val <= m_lower_value ){
      return false;
    }
  }

  if( m_upper_what < 0 ){
    return val < m_upper_value;
  }
  if( m_upper_what == 0 ){
    return val <= m_upper_value;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

bool ColorSetStringItem::setUpperbound( int what, std::string &val ){
  if( val < m_lower_value ){
    return false;
  }
  m_upper_what = what;
  m_upper_value = val;
  return true;
}

/* --------------------------------------------------------------------------- */
/* equal --                                                                    */
/* --------------------------------------------------------------------------- */

bool ColorSetStringItem::equal( const std::string &val ){
  if( m_lower_what == 99 ){
    return false;
  }
  if( m_xfer_lower_value ){
    m_xfer_lower_value->getValue( m_lower_value );
  }

  if( m_xfer_upper_value ){
    m_xfer_upper_value->getValue( m_upper_value );
  }

  if( m_upper_what == 99 ){
    if( m_lower_what < 0 ){
      return val < m_lower_value;
    }
    if( m_lower_what > 0 ){
      return val > m_lower_value;
    }
    return val == m_lower_value;
  }

  if( m_lower_what < 0 ){
    if( val < m_lower_value ){
      return true;
    }
  }
  if( m_lower_what == 0 ){
    if( val < m_lower_value ){
      return false;
    }
  }
  if( m_lower_what > 0 ){
    if( val <= m_lower_value ){
      return false;
    }
  }

  if( m_upper_what < 0 ){
    return val < m_upper_value;
  }
  if( m_upper_what == 0 ){
    return val <= m_upper_value;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void ColorSetItem::setColors( const std::string &bg, const std::string &fg ){
  m_background = bg;
  m_foreground = fg;
}

void ColorSetItem::setColors( XferDataItem *bg, XferDataItem *fg ){
  m_xfer_background = bg;
  m_xfer_foreground = fg;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool ColorSetItem::isDataItemUpdated(TransactionNumber trans) const {
  if (m_xfer_lower_value && m_xfer_lower_value->isDataItemUpdated(trans))
      return true;
  if (m_xfer_upper_value && m_xfer_upper_value->isDataItemUpdated(trans))
      return true;
  if (m_xfer_background && m_xfer_background->isDataItemUpdated(trans))
      return true;
  if (m_xfer_foreground && m_xfer_foreground->isDataItemUpdated(trans))
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* background --                                                               */
/* --------------------------------------------------------------------------- */

const std::string &ColorSetItem::background(){
  if( m_xfer_background ){
    if (!m_xfer_background->getValue( m_background ))
      m_background.clear();
  }
  return m_background;
}

/* --------------------------------------------------------------------------- */
/* foreground --                                                               */
/* --------------------------------------------------------------------------- */

const std::string &ColorSetItem::foreground(){
  if( m_xfer_foreground ){
    if (!m_xfer_foreground->getValue( m_foreground ))
      m_foreground.clear();
  }
  return m_foreground;
}


/* --------------------------------------------------------------------------- */
/* setUpperbound --                                                            */
/* --------------------------------------------------------------------------- */

bool ColorSetItem::setUpperbound( int what, XferDataItem *xfer, Scale *scale ){
  assert( xfer != 0 );
  assert( m_xfer_lower_value != 0 );
  if( xfer->getDataType() != m_xfer_lower_value->getDataType() )
    return false;

  m_upper_what = what;
  m_xfer_upper_value = xfer;
  m_scale_upper = scale;

  return true;
}
