
#ifdef HAVE_QT
#include <QDate>
#include <QString>
#endif

#include "datapool/DataPoolDefinition.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferDataItemIndex.h"
#include "xfer/XferDataParameter.h"
#include "xfer/XferParameterInteger.h"
#include "xfer/XferParameterReal.h"
#include "xfer/XferParameterComplex.h"
#include "xfer/XferParameterString.h"

#include "gui/GuiPlotDataItem.h"

#include "streamer/StreamManager.h"
#include "streamer/Stream.h"

class Annotation{
public:
  Annotation()
    : m_values(0)
    , m_labels(0){
  }
public:
  bool setAnnotationLabels( XferDataItem *dataitem );
  bool setAnnotationValues( XferDataItem *dataitem, Scale *scale );
  bool setAnnotationTitle( const std::string &title );
  const std::string &getAnnotationTitle();
   bool getAnnotationLabelsMap( GuiPlotDataItem::AnnotationLabelsMap &annoMap );
  bool isDataItemUpdated( TransactionNumber trans ) const;
  void setIndex( const std::string &name, int inx );
  bool acceptIndex( const std::string &, int );
private:
  XferDataItem     *m_values;
  XferDataItem     *m_labels;
  Scale            *m_scale;
  std::string       m_title;
};

class MarkerLine{
public:
  MarkerLine()
    : m_values(0)
    , m_labels(0){
  }
public:
  bool setLabels( XferDataItem *dataitem );
  bool setXAxis( XferDataItem *dataitem, Scale *scale );

  XferDataItem* getXAxisXferData() { return m_values; }
  Scale*        getXAxisScale()  { return m_scale; }
  // bool setAnnotationTitle( const std::string &title );
  // const std::string &getAnnotationTitle();
  // bool getAnnotationLabelsMap( GuiPlotDataItem::AnnotationLabelsMap &annoMap );
  bool isDataItemUpdated( TransactionNumber trans ) const;
  void setIndex( const std::string &name, int inx );
  bool acceptIndex( const std::string &, int );
private:
  XferDataItem     *m_values;
  XferDataItem     *m_labels;
  Scale            *m_scale;
  std::string       m_title;
};

/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
GuiPlotDataItem::GuiPlotDataItem( XferDataItem *dataitem, int numwildcards )
  : m_dataitem( dataitem )
  , m_defaultRange( true )
  , m_labelXfer(0)
  , m_labelStream(0)
  , m_unitStream(0)
  , m_annotation(0)
  , m_annotationAngle(-90.)
  , m_markerLine(0)
  , m_minRange(0)
  , m_maxRange(0)
  , m_xferScaleMin(0)
  , m_xferScaleMax(0)
  , m_linestyle(LINEAR){
  if( m_dataitem != 0 ){
    int num = m_dataitem->getNumOfWildcards();
    for( int i=num; i < numwildcards; i++ ) {
      XferDataItemIndex *index = m_dataitem->newDataItemIndex();
      index->setWildcard();
    }
    m_dataitem->setDimensionIndizes();
  }
}

GuiPlotDataItem::GuiPlotDataItem(const GuiPlotDataItem& item)
  : m_dataitem( new XferDataItem(*item.m_dataitem) )
  , m_scale( item.m_scale )
  , m_defaultRange( item.m_defaultRange )
  , m_label( item.m_label )
  , m_labelXfer( item.m_labelXfer ? new XferDataItem(*item.m_labelXfer) : 0 )
  , m_labelStream( item.m_labelStream )
  , m_labelVector( item.m_labelVector )
  , m_unit( item.m_unit )
  , m_unitStream( item.m_unitStream )
  , m_annotation( item.m_annotation )
  , m_annotationAngle( item.m_annotationAngle )
  , m_markerLine( item.m_markerLine )
  , m_minRange(item.m_minRange )
  , m_maxRange( item.m_maxRange )
  , m_xferScaleMin( item.m_xferScaleMin )
  , m_xferScaleMax(item.m_xferScaleMax )
  , m_linestyle( item.m_linestyle ){
}

GuiPlotDataItem::~GuiPlotDataItem(){
  if( m_annotation != 0 )
    delete m_annotation;
  if( m_markerLine != 0 )
    delete m_markerLine;
}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
/* --------------------------------------------------------------------------- */
/* setDataReference --                                                         */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::setDataReference( DataReference *dataref ){
  if( dataref != 0 ) {
    if( dataref->getDataType() == DataDictionary::type_String )
      return false;
    else
      return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getScaleFactor --                                                           */
/* --------------------------------------------------------------------------- */
double GuiPlotDataItem::getScaleFactor(){
  return m_scale != 0 ? m_scale->getValue() : 1. ;
}

/* --------------------------------------------------------------------------- */
/* setDataItem --                                                              */
/* --------------------------------------------------------------------------- */
void GuiPlotDataItem::setDataItem( XferDataItem *item ){
  m_dataitem = item;
}

/* --------------------------------------------------------------------------- */
/* setAnnotationLabels --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::setAnnotationLabels( XferDataItem *dataitem ){
  if( m_annotation == 0 )
    m_annotation = new Annotation;
  return m_annotation->setAnnotationLabels( dataitem );
}

/* --------------------------------------------------------------------------- */
/*  setAnnotationValues --                                                     */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::setAnnotationValues( XferDataItem *dataitem, Scale *scale ){
  if( m_annotation == 0 )
    m_annotation = new Annotation;
  return m_annotation->setAnnotationValues( dataitem, scale );
}

/* --------------------------------------------------------------------------- */
/*  setAnnotationTitle --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::setAnnotationTitle( const std::string &title ){
  if( m_annotation == 0 )
    m_annotation = new Annotation;
  return m_annotation->setAnnotationTitle( title );
}

/* --------------------------------------------------------------------------- */
/*  setAnnotationAngle --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::setAnnotationAngle( double angle ){
  m_annotationAngle = angle;
  return true;
}

/* --------------------------------------------------------------------------- */
/* newDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *GuiPlotDataItem::newDataItemIndex( int level ){
  if( m_dataitem != 0 )
    return m_dataitem->newDataItemIndex( level );
  return 0;
}

/* --------------------------------------------------------------------------- */
/* newDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *GuiPlotDataItem::newDataItemIndex(){
  if( m_dataitem != 0 )
    return m_dataitem->newDataItemIndex();
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getNumOfWildcards --                                                        */
/* --------------------------------------------------------------------------- */

int GuiPlotDataItem::getNumOfWildcards(){
  if( m_dataitem != 0 )
    return m_dataitem->getNumOfWildcards();
  return -1;
}

/* --------------------------------------------------------------------------- */
/* getDataItemIndexWildcard --                                                 */
/* --------------------------------------------------------------------------- */

XferDataItemIndex *GuiPlotDataItem::getDataItemIndexWildcard( int num ){
  if( m_dataitem != 0 )
    return m_dataitem->getDataItemIndexWildcard( num );
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */
XferDataItemIndex *GuiPlotDataItem::getDataItemIndex( const std::string& name){
  if( m_dataitem != 0 )
    return m_dataitem->getDataItemIndex( name );
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getNumDimensions --                                                         */
/* --------------------------------------------------------------------------- */

int GuiPlotDataItem::getNumDimensions(){
  if( m_dataitem != 0 )
    return m_dataitem->getNumDimensions();
  return -1;
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */

const std::string GuiPlotDataItem::getName() const {
  if( m_dataitem != 0 )
    return m_dataitem->getName();
  return "";
}

/* --------------------------------------------------------------------------- */
/* getUniqueName --                                                            */
/* --------------------------------------------------------------------------- */

const std::string GuiPlotDataItem::getUniqueName(int xAxisIndex, int yAxisIndex, bool cyclePlotMode) const {
  if( m_dataitem != 0 ) {
    std::stringstream os;
    // unique label name
    std::string title_name(m_label);
    if ( m_labelXfer ) {
      title_name =  m_labelXfer->getFullName(true);
    }
    else if( m_labelStream != 0 ) {
      title_name =  m_labelStream->Name();
    }

    // unique plot item name
    os << m_dataitem->getFullName(false)<< "[" << xAxisIndex << "][" << yAxisIndex << "]@"
       << title_name << "@"
       << (cyclePlotMode ? DataPoolIntens::Instance().currentCycle() : 0);
    //    std::cout << " => {"<<os.str()<<"}\n";
    return os.str();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* getMinRange --                                                              */
/* --------------------------------------------------------------------------- */

double GuiPlotDataItem::getMinRange() {
  if (m_xferScaleMin) {
    double dbl;
    m_xferScaleMin->getValue(dbl);
    return dbl;
  }

  return m_minRange;
}

/* --------------------------------------------------------------------------- */
/* getMaxRange --                                                              */
/* --------------------------------------------------------------------------- */

double GuiPlotDataItem::getMaxRange() {
  if (m_xferScaleMax) {
    double dbl;
    m_xferScaleMax->getValue(dbl);
    return dbl;
  }

  return m_maxRange;
}

/* --------------------------------------------------------------------------- */
/* setMinXferRange --                                                          */
/* --------------------------------------------------------------------------- */

void GuiPlotDataItem::setMinXferRange( XferDataItem *xferMinX ) {
  m_xferScaleMin = xferMinX;
  m_xferScaleMin->setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* setMaxXferRange --                                                          */
/* --------------------------------------------------------------------------- */

void GuiPlotDataItem::setMaxXferRange( XferDataItem *xferMaxX ) {
  m_xferScaleMax = xferMaxX;
  m_xferScaleMax->setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiPlotDataItem::isValid( )const {
  if( m_dataitem != 0 )
    return m_dataitem->isValid();
  return false;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiPlotDataItem::isDataItemUpdated( TransactionNumber trans )const {
  if( m_dataitem != 0 )
    if (m_dataitem->isDataItemUpdated( trans, true ))
      return true;
  if( m_labelXfer != 0 ) {
    if (m_labelXfer->isDataItemUpdated( trans, true ))
      return true;
  }
  if( m_xferScaleMin != 0 )
    if (m_xferScaleMin->isDataItemUpdated( trans, true ))
      return true;
  if( m_xferScaleMax != 0 )
    if (m_xferScaleMax->isDataItemUpdated( trans, true ))
      return true;
  if (isMarkerLineUpdated(trans))
    return true;
  if (isAnnotationLabelsUpdated(trans))
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiPlotDataItem::acceptIndex( const std::string &name, int inx ) {
  if( m_dataitem != 0 ) {
    if (!m_dataitem->acceptIndex( name, inx ))
      return false;
  }
  if( m_labelXfer != 0 ) {
    if (!m_labelXfer->acceptIndex( name, inx ))
      return false;
  }
  if( m_xferScaleMin != 0 ) {
    if (!m_xferScaleMin->acceptIndex( name, inx ))
      return false;
  }
  if( m_xferScaleMax != 0 ) {
    if (!m_xferScaleMax->acceptIndex( name, inx ))
      return false;
  }
  if( m_markerLine != 0 )
    if (!m_markerLine->acceptIndex( name, inx ))
      return false;
  if( m_annotation != 0 )
    if (!m_annotation->acceptIndex( name, inx ))
      return false;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiPlotDataItem::setIndex( const std::string &name, int inx ) {
  if( m_dataitem != 0 ) {
    m_dataitem->setIndex( name, inx );
    if (m_labelXfer)
      m_labelXfer->setIndex( name, inx );
    if (m_xferScaleMin)
      m_xferScaleMin->setIndex( name, inx );
    if (m_xferScaleMax)
      m_xferScaleMax->setIndex( name, inx );
  }
  if( m_markerLine != 0 )
    m_markerLine->setIndex( name, inx );
  if( m_annotation != 0 )
    m_annotation->setIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiPlotDataItem::getValue( double &value )const {
  #ifdef HAVE_QT
  if( m_dataitem != 0 ) {
    if (m_dataitem->getUserAttr()->StringType() == UserAttr::string_kind_date) {
      std::string str;
      m_dataitem->getValue( str );
      QDate d = QDate::fromString( QString::fromStdString(str), Qt::ISODate );
      // Vorkommastellen sind sie Julianischen Tage
      value = d.toJulianDay();
      return true;
    }
    if (m_dataitem->getUserAttr()->StringType() == UserAttr::string_kind_datetime) {
      std::string str;
      m_dataitem->getValue( str );
      QDateTime dt = QDateTime::fromString( QString::fromStdString(str), Qt::ISODate );
      // Nachkommastellen sind Tages mSecs / 60*60*24*1000
      double dMSecs = double(QTime(0, 0, 0, 0).msecsTo(dt.time())) / 86400000;
      // Vorkommastellen sind sie Julianischen Tage
      value = dt.date().toJulianDay() + (double)dMSecs;
      return true;
    }
    if (m_dataitem->getUserAttr()->StringType() == UserAttr::string_kind_time) {
      std::string str;
      m_dataitem->getValue( str );
      QTime t = QTime::fromString( QString::fromStdString(str), Qt::ISODate );
      // Tages mSecs / 60*60*24*1000
      value = QTime(0, 0, 0, 0).msecsTo(t);
      return true;
    }
    return m_dataitem->getValue( value );
  }
  #endif
  // TODO
  return false;
}

/* --------------------------------------------------------------------------- */
/* setLabelStream --                                                           */
/* --------------------------------------------------------------------------- */

void GuiPlotDataItem::setLabel(XferDataItem* labelXfer) {
 m_labelXfer = labelXfer;
 m_labelXfer->setDimensionIndizes();
}


/* --------------------------------------------------------------------------- */
/* setLabelStream --                                                           */
/* --------------------------------------------------------------------------- */

void GuiPlotDataItem::setLabelStream( const std::string &label ) {
  m_labelStream = StreamManager::Instance().getStream( label );
}

/* --------------------------------------------------------------------------- */
/* setUnitStream --                                                            */
/* --------------------------------------------------------------------------- */

void GuiPlotDataItem::setUnitStream( const std::string &unit ) {
  m_unitStream = StreamManager::Instance().getStream( unit );
}

/* --------------------------------------------------------------------------- */
/* convertToString --                                                          */
/* --------------------------------------------------------------------------- */

std::string GuiPlotDataItem::convertToString( Stream *s ) const {
  if ( s != 0 ) {
    std::ostringstream ostr;
    s->write( ostr );
    return ostr.str();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* getLabelAndUnit --                                                          */
/* --------------------------------------------------------------------------- */
void GuiPlotDataItem::getLabelAndUnit( std::string &label, bool annotation ){
  std::ostringstream s;
  if( annotation ){
    std::string annolabel;
    getAnnotationTitle( annolabel );
    s << annolabel;
  }
  else{
    if (getUnit().empty()) {
      s << getLabel();
    } else {
      s << getLabel() << " " << getUnit();
    }
  }
  label = s.str();
}

//-----------------------------------------------------------------------------
// setValueLabelXferIndex
void GuiPlotDataItem::setValueLabelXferIndex(int idxValue){
  if (m_labelXfer) {
    XferDataItemIndex* index =  m_labelXfer->getDataItemIndexWildcard( 1 );
    if (index)
      index->setIndex(m_labelXfer->Data(), idxValue);
  }
}

//-----------------------------------------------------------------------------
// getLabel
std::string GuiPlotDataItem::getLabel() const{
  if( m_labelXfer != 0 ) {
    if (m_labelXfer->isValid()) {
      std::string str;
      m_labelXfer->getValue( str );
      return str;
    }
  }
  if( m_labelStream != 0 )
    return convertToString(m_labelStream);
  if( m_label.empty() )
    return getName();
  return m_label;
}
//-----------------------------------------------------------------------------
// getUnit
std::string GuiPlotDataItem::getUnit(){
  if( m_unitStream != 0 )
    return convertToString(m_unitStream);
  return m_unit;
}

//-----------------------------------------------------------------------------
// getLabels
std::vector<std::string> GuiPlotDataItem::getLabels() const{
  return m_labelVector;
}
/* --------------------------------------------------------------------------- */
/* getAnnotationLabelsMap --                                                   */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::getAnnotationLabelsMap( AnnotationLabelsMap &annoMap ){
  if( m_annotation == 0 ){
    return false;
  }
  return m_annotation->getAnnotationLabelsMap( annoMap );
}

/* --------------------------------------------------------------------------- */
/* getAnnotationTitle --                                                       */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::getAnnotationTitle( std::string &title ){
  if( m_annotation == 0 ){
    return false;
  }
  title =  m_annotation->getAnnotationTitle();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getAnnotationAngle --                                                       */
/* --------------------------------------------------------------------------- */
double GuiPlotDataItem::getAnnotationAngle(){
  return m_annotationAngle;
}

/* --------------------------------------------------------------------------- */
/* isAnnotationLabelsUpdated --                                                */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::isAnnotationLabelsUpdated( TransactionNumber trans ) const {
  if( m_annotation == 0 )
    return false;
  return m_annotation->isDataItemUpdated( trans );
}

/* --------------------------------------------------------------------------- */
/* isMarkerLineUpdated --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::isMarkerLineUpdated( TransactionNumber trans ) const {
  if( m_markerLine == 0 )
    return false;
  return m_markerLine->isDataItemUpdated( trans );
}

/* --------------------------------------------------------------------------- */
/* getAnnotationLabelsMap --                                                         */
/* --------------------------------------------------------------------------- */
bool Annotation::getAnnotationLabelsMap( GuiPlotDataItem::AnnotationLabelsMap &annoMap ){
  if( ( m_labels == 0 ) || ( m_values == 0 ) )
    return false;
  XferDataItemIndex *valueIndex = 0;
  valueIndex = m_values->getDataItemIndexWildcard( 1 );
  assert( valueIndex != 0 );
  XferDataItemIndex *labelIndex = m_labels->getDataItemIndexWildcard( 1 );
  int valueCount = valueIndex->getDimensionSize( m_values->Data() );
  int labelCount = labelIndex->getDimensionSize( m_labels->Data() );
  valueCount = valueCount <= labelCount ? valueCount : labelCount;
  if( valueCount < 1  )
    return false;
  for( int i = 0; i < valueCount; ++i ){
    valueIndex->setIndex( m_values->Data(), i );
    labelIndex->setIndex( m_labels->Data(), i );
    double value;
    m_values->getValue( value );
    if( m_scale != 0 )
      value = value * m_scale->getValue();
    std::string label;
    m_labels->getValue( label );
    GuiPlotDataItem::AnnotationLabelsMap::value_type elem( value, label );
    annoMap.insert( elem );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setAnnotationLabels --                                                      */
/* --------------------------------------------------------------------------- */
bool Annotation::setAnnotationLabels( XferDataItem *dataitem ){
  m_labels = dataitem;
  if( m_labels == 0 )
    return false;
  if (m_labels->getNumOfWildcards() == 0) {
    XferDataItemIndex *index = m_labels->newDataItemIndex();
    index->setWildcard();
  }
  m_labels->setDimensionIndizes();
  return true;
}

/* --------------------------------------------------------------------------- */
/* setAnnotationValues --                                                      */
/* --------------------------------------------------------------------------- */
bool Annotation::setAnnotationValues( XferDataItem *dataitem, Scale *scale ){
  m_values = dataitem;
  m_scale = scale;
  if( m_values == 0 )
    return false;
  if (m_values->getNumOfWildcards() == 0) {
    XferDataItemIndex *index = m_values->newDataItemIndex();
    index->setWildcard();
  }
  m_values->setDimensionIndizes();
  return true;
}

/* --------------------------------------------------------------------------- */
/* setAnnotationTitle --                                                       */
/* --------------------------------------------------------------------------- */
bool Annotation::setAnnotationTitle( const std::string &title ){
  m_title = title;
  return true;
}

/* --------------------------------------------------------------------------- */
/* getAnnotationTitle --                                                       */
/* --------------------------------------------------------------------------- */
const std::string &Annotation::getAnnotationTitle(){
  return m_title;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */
bool Annotation::isDataItemUpdated( TransactionNumber trans ) const {
  if( m_values == 0 || m_labels == 0 )
    return false;
  return ( m_values->isDataItemUpdated( trans ) || m_labels->isDataItemUpdated( trans ) );
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */
void Annotation::setIndex( const std::string &name, int inx ) {
  if( m_values != 0 )
    m_values->setIndex( name, inx );
  if( m_labels != 0 )
    m_labels->setIndex( name, inx );
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */
bool Annotation::acceptIndex( const std::string &name, int inx) {
  if( m_values != 0 )
    if (!m_values->acceptIndex( name, inx ))
      return false;
  if( m_labels != 0 )
    if (!m_labels->acceptIndex( name, inx ))
      return false;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setMarkerLineLabels --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::setMarkerLineLabels( XferDataItem *dataitem ){
  if( m_markerLine == 0 )
    m_markerLine = new MarkerLine;
  return m_markerLine->setLabels( dataitem );
}

/* --------------------------------------------------------------------------- */
/*  setMarkerLineXAxis --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiPlotDataItem::setMarkerLineXAxis( XferDataItem *dataitem, Scale *scale ){
  if( m_markerLine == 0 )
    m_markerLine = new MarkerLine;
  return m_markerLine->setXAxis( dataitem, scale );
}

/* --------------------------------------------------------------------------- */
/* getMarkerLineXAxisXferData --                                               */
/* --------------------------------------------------------------------------- */

XferDataItem* GuiPlotDataItem::getMarkerLineXAxisXferData() {
  return m_markerLine->getXAxisXferData();
}

/* --------------------------------------------------------------------------- */
/* getMarkerLineXAxisScale --                                                                */
/* --------------------------------------------------------------------------- */

double GuiPlotDataItem::getMarkerLineXAxisScaleFactor() {
  return m_markerLine->getXAxisScale() != 0 ?
    m_markerLine->getXAxisScale()->getValue() : 1. ;
}

/* --------------------------------------------------------------------------- */
/* setLabels --                                                                */
/* --------------------------------------------------------------------------- */
bool MarkerLine::setLabels( XferDataItem *dataitem ){
  m_labels = dataitem;
  if( m_labels == 0 )
    return false;
  if (m_labels->getNumOfWildcards() == 0) {
    XferDataItemIndex *index = m_labels->newDataItemIndex();
    index->setWildcard();
  }
  m_labels->setDimensionIndizes();
  return true;
}

/* --------------------------------------------------------------------------- */
/* setAxis --                                                       */
/* --------------------------------------------------------------------------- */
bool MarkerLine::setXAxis( XferDataItem *dataitem, Scale *scale ){
  m_values = dataitem;
  m_scale = scale;
  if( m_values == 0 )
    return false;
  if (m_values->getNumOfWildcards() == 0) {
    XferDataItemIndex *index = m_values->newDataItemIndex();
    index->setWildcard();
  }
  m_values->setDimensionIndizes();
  return true;
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */
bool MarkerLine::isDataItemUpdated( TransactionNumber trans ) const{
  if (m_values)
    if ( m_values->isDataItemUpdated( trans ) )
      return true;
  if (m_labels)
    if ( m_labels->isDataItemUpdated( trans ) )
      return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */
void MarkerLine::setIndex( const std::string &name, int inx ) {
  if( m_values != 0 ) {
    m_values->setIndex( name, inx );
  }
  if( m_labels != 0 ) {
    m_labels->setIndex( name, inx );
  }
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */
bool MarkerLine::acceptIndex( const std::string &name, int inx) {
  if( m_values != 0 )
    if (!m_values->acceptIndex( name, inx ))
      return false;
  if( m_labels != 0 )
    if (!m_labels->acceptIndex( name, inx ))
      return false;
  return true;
}
