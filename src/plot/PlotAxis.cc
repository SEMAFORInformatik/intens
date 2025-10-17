
#include <limits>
#include "plot/PlotAxis.h"
#include "xfer/XferDataItem.h"
#include "xfer/Scale.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/* --------------------------------------------------------------------------- */
/* Constructor/Destructor --                                                   */
/* --------------------------------------------------------------------------- */

PlotAxis::PlotAxis()
  : m_isLogarithmic( false)
  , m_isOriginDefined( false )
  , m_scaleEnable( false )
  , m_origin( 0.0 )
  , m_minStartValue( 0 )
  , m_maxStartValue( 0 )
  , m_scaleEnabledStartValue( 0 )
  , m_aspectRatioStartValue( 0 )
  , m_min( 0 )
  , m_max( 0 )
  , m_aspectRatio( 0 )
  , m_xferScaleMin( 0 )
  , m_xferScaleMax( 0 )
  , m_xferScaleEnable( 0 )
  , m_xferAspectRatio( 0 )
  , m_max_scale( 0 )
  , m_min_scale( 0 )
  , m_fmt('g')
  , m_prec(4)
  , m_fieldwidth(0) {
}

PlotAxis::PlotAxis(const PlotAxis& plotaxis)
  : m_isLogarithmic( plotaxis.m_isLogarithmic )
  , m_isOriginDefined( plotaxis.m_isOriginDefined )
  , m_scaleEnable( plotaxis.m_scaleEnable )
  , m_origin( plotaxis.m_origin )
  , m_minStartValue( plotaxis.m_minStartValue )
  , m_maxStartValue( plotaxis.m_maxStartValue )
  , m_scaleEnabledStartValue( plotaxis.m_scaleEnabledStartValue )
  , m_aspectRatioStartValue( plotaxis.m_aspectRatioStartValue )
  , m_min( plotaxis.m_min )
  , m_max( plotaxis.m_max )
  , m_aspectRatio( plotaxis.m_aspectRatio )
  , m_xferScaleMin( plotaxis.m_xferScaleMin )
  , m_xferScaleMax( plotaxis.m_xferScaleMax )
  , m_xferScaleEnable( plotaxis.m_xferScaleEnable )
  , m_xferAspectRatio( plotaxis.m_xferAspectRatio )
  , m_max_scale( plotaxis.m_max_scale )
  , m_min_scale( plotaxis.m_min_scale )
  , m_fmt( plotaxis.m_fmt )
  , m_prec( plotaxis.m_prec )
  , m_fieldwidth( plotaxis.m_fieldwidth ) {
}

PlotAxis::~PlotAxis(){
  delete m_minStartValue;
  delete m_maxStartValue;
  delete m_scaleEnabledStartValue;
  delete m_aspectRatioStartValue;
  delete m_min;
  delete m_max;
}

/* --------------------------------------------------------------------------- */
/* setAllXferScale --                                                          */
/* --------------------------------------------------------------------------- */

void PlotAxis::setAllXferScale( XferDataItem *xferScaleMin
				, XferDataItem *xferScaleMax
				, XferDataItem *xferScaleEnable
				, XferDataItem *xferAspectRatio){
  m_xferScaleMin = xferScaleMin;
  m_xferScaleMax = xferScaleMax;
  m_xferScaleEnable = xferScaleEnable;
  m_xferAspectRatio = xferAspectRatio;

  setStartValues();
}

/* --------------------------------------------------------------------------- */
/* setAllXferScale --                                                          */
/* --------------------------------------------------------------------------- */

void PlotAxis::setAllXferScale( XferDataItem *xferScaleMin, Scale *min_scale
				, XferDataItem *xferScaleMax, Scale *max_scale
				, XferDataItem *xferScaleEnable
				, XferDataItem *xferAspectRatio){
  m_xferScaleMin = xferScaleMin;
  m_xferScaleMax = xferScaleMax;
  m_min_scale = min_scale;
  m_max_scale = max_scale;
  m_xferScaleEnable = xferScaleEnable;
  m_xferAspectRatio = xferAspectRatio;
}

/* --------------------------------------------------------------------------- */
/* setXferAspectRatio --                                                       */
/* --------------------------------------------------------------------------- */

void PlotAxis::setXferAspectRatio(XferDataItem *xferAspectRatio) {
  m_xferAspectRatio = xferAspectRatio;
}

/* --------------------------------------------------------------------------- */
/* setStartValues --                                                           */
/* --------------------------------------------------------------------------- */

void PlotAxis::setStartValues() {
  double dvalue;

  // min
  if( m_xferScaleMin != 0){
    if( m_xferScaleMin->isValid() ){
      m_xferScaleMin->getValue( dvalue );
    }
  }
  if( m_minStartValue == 0 )
    m_minStartValue = new double( dvalue );
  else
    *m_minStartValue = dvalue;

  // max
  if( m_xferScaleMax != 0){
    if( m_xferScaleMax->isValid() ){
      m_xferScaleMax->getValue( dvalue );
    }
  }
  if( m_maxStartValue == 0 )
    m_maxStartValue = new double( dvalue );
  else
    *m_maxStartValue = dvalue;

  // scale enabled
  int value;
  if( m_xferScaleEnable != 0){
    if( m_xferScaleEnable->isValid() ){
      m_xferScaleEnable->getValue( value );
    }
  }
  if( m_scaleEnabledStartValue == 0 )
    m_scaleEnabledStartValue = new bool( value );
  else
    *m_scaleEnabledStartValue = value;

  // ascpect ratio
  if( m_xferAspectRatio != 0){
    if( m_xferAspectRatio->isValid() ){
      m_xferAspectRatio->getValue( value );
    }
  }
  if( m_aspectRatioStartValue == 0 )
    m_aspectRatioStartValue = new int( value );
  else
    *m_aspectRatioStartValue = value;
}

/* --------------------------------------------------------------------------- */
/* setLogarithmic --                                                           */
/* --------------------------------------------------------------------------- */

void PlotAxis::setLogarithmic( bool value ){
  m_isLogarithmic = value;
}

/* --------------------------------------------------------------------------- */
/* setScaleEnable --                                                           */
/* --------------------------------------------------------------------------- */

void PlotAxis::setScaleEnable( bool value ){
  m_scaleEnable = value;
}

/* --------------------------------------------------------------------------- */
/* setScaleEnable --                                                           */
/* --------------------------------------------------------------------------- */

void PlotAxis::setScaleEnable(){
  int value = isScaleEnabled();
  if( m_xferScaleEnable != 0){
    if( m_xferScaleEnable->isValid() ){
      m_xferScaleEnable->getValue( value );
    }
  }
  else{
    if( m_xferScaleMin && m_xferScaleMax ){
      value = 1;
    }
  }
  setScaleEnable( value ? true : false );
}

/* --------------------------------------------------------------------------- */
/* resetScaleValues --                                                         */
/* --------------------------------------------------------------------------- */

void PlotAxis::resetScaleValues(){
  double value;
  int ivalue;
  bool status = 0;

  if( getMinStartValue( value ) && m_xferScaleMin ){
    setMin( value );
    m_xferScaleMin->setValue( value );
  }
  if( getMaxStartValue( value ) &&  m_xferScaleMax ){
    setMax( value );
    m_xferScaleMax->setValue( value );
    BUG_DEBUG("resetScaleValues max: " << value << ", VN" << m_xferScaleMax->getFullName(true));
  }
  if( getScaleEnableStartValue( status ) && m_xferScaleEnable )
    m_xferScaleEnable->setValue( status );
  if( getAspectRatioStartValue( ivalue ) && m_xferAspectRatio )
    m_xferAspectRatio->setValue( ivalue );
}

/* --------------------------------------------------------------------------- */
/* setOrigin --                                                                */
/* --------------------------------------------------------------------------- */

void PlotAxis::setOrigin( const double value ){
  m_isOriginDefined = true;
  m_origin = value;
}

/* --------------------------------------------------------------------------- */
/* getMinStartValue --                                                         */
/* --------------------------------------------------------------------------- */

bool PlotAxis::getMinStartValue( double &value ){
  if( m_minStartValue == 0 )
    return false;
  value = *m_minStartValue;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getMaxStartValue --                                                         */
/* --------------------------------------------------------------------------- */

bool PlotAxis::getMaxStartValue( double &value ){
  if( m_maxStartValue == 0 )
    return false;
  value = *m_maxStartValue;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getScaleEnableStartValue --                                                 */
/* --------------------------------------------------------------------------- */

bool PlotAxis::getScaleEnableStartValue( bool &value ){
  if( m_scaleEnabledStartValue == 0 )
    return false;
  value = *m_scaleEnabledStartValue;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getAspectRatioStartValue --                                                 */
/* --------------------------------------------------------------------------- */

bool PlotAxis::getAspectRatioStartValue( int &value ){
  if( m_aspectRatioStartValue == 0 )
    return false;
  value = *m_aspectRatioStartValue;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getMin --                                                                   */
/* --------------------------------------------------------------------------- */

bool PlotAxis::getMin( double &value, bool with_factor ){
  value = std::numeric_limits<double>::min();
  if(isScaleEnabled()){
    if( m_min != 0 ){
      value = *m_min;
    }
    else
      return false;
  }
  else{
    if( m_minStartValue != 0 )
      value = *m_minStartValue;
    else
      return false;
  }
  if( with_factor && m_min_scale ){
    value = value*m_min_scale->getValue();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getMax --                                                                   */
/* --------------------------------------------------------------------------- */

bool PlotAxis::getMax( double &value, bool with_factor ){
  value = std::numeric_limits<double>::max();
  if(isScaleEnabled()){
    if( m_max != 0 )
      value = *m_max;
    else
      return false;
  }
  else{
    if( m_maxStartValue != 0 )
      value = *m_maxStartValue;
    else
      return false;
  }
  if( with_factor && m_max_scale ){
    value = value*m_max_scale->getValue();
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setMin --                                                                   */
/* --------------------------------------------------------------------------- */

void PlotAxis::setMin( const double value ){
  if( m_min == 0 )
    m_min = new double( value );
  else
    *m_min = value;
  if( m_minStartValue == 0 )
    m_minStartValue = new double( value );
}

/* --------------------------------------------------------------------------- */
/* setMin --                                                                   */
/* --------------------------------------------------------------------------- */

void PlotAxis::setMin(){
  double value;
  if( m_xferScaleMin != 0)
    if( m_xferScaleMin->getValue( value ) )
      setMin( value );
}

/* --------------------------------------------------------------------------- */
/* setMax --                                                                   */
/* --------------------------------------------------------------------------- */

void PlotAxis::setMax( const double value ){
  if( m_max == 0 )
    m_max = new double( value );
  else
    *m_max = value;
  if( m_maxStartValue == 0 )
    m_maxStartValue = new double( value );
}

/* --------------------------------------------------------------------------- */
/* setMax --                                                                   */
/* --------------------------------------------------------------------------- */

void PlotAxis::setMax(){
  double value;
  if( m_xferScaleMax != 0)
    if( m_xferScaleMax->getValue( value ) )
      setMax( value );
}

/* --------------------------------------------------------------------------- */
/* setMinStartValue --                                                         */
/* --------------------------------------------------------------------------- */

void PlotAxis::setMinStartValue( const double value ){
  // called  by gui update
  // if( m_minStartValue == 0 )
  //   m_minStartValue = new double( value );
  // else
  //   *m_minStartValue = value;

  if( m_xferScaleMin != 0 )
    if( !(m_xferScaleMin->isValid()) )
      m_xferScaleMin->setValue( value );
}

/* --------------------------------------------------------------------------- */
/* setMaxStartValue --                                                         */
/* --------------------------------------------------------------------------- */

void PlotAxis::setMaxStartValue( const double value ){
  // called  by gui update
  // if( m_maxStartValue == 0 )
  //   m_maxStartValue = new double( value );
  // else
  //   *m_maxStartValue = value;

  if( m_xferScaleMax != 0 )
    if( !(m_xferScaleMax->isValid()) ) {
      m_xferScaleMax->setValue( value );
      BUG_DEBUG("setMaxStartValue max: " << value << ", VN" << m_xferScaleMax->getFullName(true));
    }
}

// --------------------------------------------------------------------------- */
// getOrigin --
// --------------------------------------------------------------------------- */

bool PlotAxis::getOrigin( double &value ){
  value = m_origin;
  return m_isOriginDefined;
}

// --------------------------------------------------------------------------- */
// isUpdated --
// --------------------------------------------------------------------------- */

bool PlotAxis::isUpdated( TransactionNumber trans ){
  if( m_xferScaleMin ){
    if( m_xferScaleMin->isDataItemUpdated( trans ) ){
      return true;
    }
  }
  if( m_xferScaleMax ){
    if( m_xferScaleMax->isDataItemUpdated( trans ) ){
      return true;
    }
  }
  if( m_xferAspectRatio ){
    if( m_xferAspectRatio->isDataItemUpdated( trans ) ){
      return true;
    }
  }
  if( m_xferScaleEnable ){
    if( m_xferScaleEnable->isDataItemUpdated( trans ) ){
      return true;
    }
  }

  return false;
}


// --------------------------------------------------------------------------- */
// acceptIndex --
// --------------------------------------------------------------------------- */

bool  PlotAxis::acceptIndex( const std::string &name, int inx ) {
  if (m_xferScaleMin)
    if (! m_xferScaleMin->acceptIndex( name, inx ) )
      return  false;
  if (m_xferScaleMax)
    if (! m_xferScaleMax->acceptIndex( name, inx ) )
      return  false;
  if (m_xferScaleEnable)
    if (! m_xferScaleEnable->acceptIndex( name, inx ) )
      return  false;
  if (m_xferAspectRatio)
    if (! m_xferAspectRatio->acceptIndex( name, inx ) )
      return  false;
  return true;
}

// --------------------------------------------------------------------------- */
// setIndex --
// --------------------------------------------------------------------------- */

void  PlotAxis::setIndex( const std::string &name, int inx ) {
  if (m_xferScaleMin)
    m_xferScaleMin->setIndex( name, inx );
  if (m_xferScaleMax)
    m_xferScaleMax->setIndex( name, inx );
  if (m_xferScaleEnable)
    m_xferScaleEnable->setIndex( name, inx );
  if (m_xferAspectRatio)
    m_xferAspectRatio->setIndex( name, inx );
}

//---------------------------------------------------------------------------
// isScaleEnabled
//---------------------------------------------------------------------------
bool PlotAxis::isScaleEnabled() {
  if (m_xferScaleMin) {
    BUG_DEBUG("isScaleEnabled m_xferScaleMin Valid: " << m_xferScaleMin->isValid() << ", VN" << m_xferScaleMin->getFullName(true));
  }
  if (m_xferScaleMax) {
    BUG_DEBUG("isScaleEnabled m_xferScaleMax Valid: " << m_xferScaleMax->isValid() << ", VN" << m_xferScaleMax->getFullName(true));
  }
  if(m_xferScaleMin != 0 && !m_xferScaleMin->isValid() ||
     m_xferScaleMax != 0 && !m_xferScaleMax->isValid()){
    return false;
  }
  return m_scaleEnable;
}

//---------------------------------------------------------------------------
// serializeXML
//---------------------------------------------------------------------------

void PlotAxis::serializeXML(std::ostream &os, std::string name) {
  double min, max;
  getMin(min);
  getMax(max);
  os << "<intens:PlotAxis"
     << " name=\""<<name << "\""
     << " logarithmic=\""<< m_isLogarithmic << "\""
     << " originDefined=\""<< m_isOriginDefined << "\""
     << " scaleEnable=\""<< isScaleEnabled() << "\""
     << " origin=\""<< m_origin << "\""
     << " min=\""<< min << "\""
     << " max=\""<< max << "\""
     << " m_fmt=\""<< m_fmt << "\""
     << " fieldwidth=\""<< m_fieldwidth << "\""
     << " precision=\""<< m_prec << "\""
     << "/>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool PlotAxis::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  double min, max;
  getMin(min);
  getMax(max);
  jsonObj["logarithmic"] = m_isLogarithmic;
  jsonObj["originDefined"] = m_isOriginDefined;
  jsonObj["scaleEnable"] = isScaleEnabled();
  jsonObj["origin"] = m_origin;
  if (isScaleEnabled()) {
    jsonObj["min"] = min;
    jsonObj["max"] = max;
  }
  jsonObj["m_fmt"] = m_fmt;
  jsonObj["fieldwidth"] = m_fieldwidth;
  jsonObj["precision"] = m_prec;
  return false;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool PlotAxis::serializeProtobuf(in_proto::Plot2D::Axis* axis, bool onlyUpdated) {
  double min, max;
  axis->set_logarithmic(m_isLogarithmic);
  if (isScaleEnabled()) {
    getMin(min);
    getMax(max);
    axis->set_min(min);
    axis->set_max(max);
  }
  return false;
}
#endif
