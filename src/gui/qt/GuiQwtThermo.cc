
#include <limits>

#include <QLayout>
#include <QLabel>
#include <QPalette>
#include <qwt_color_map.h>

#include "app/DataPoolIntens.h"
#include "app/ColorSet.h"
#include "xfer/XferDataItem.h"
#include "xfer/Scale.h"
#include "utils/gettext.h"
#include <gui/GuiFactory.h>
#include <gui/GuiPopupMenu.h>
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQwtThermo.h"
#include "gui/qt/QtMultiFontString.h"

INIT_LOGGER();

//---------------------------------------------------
// Klasse MyQwtThermo
//---------------------------------------------------
#include<QResizeEvent>
#include<QPainter>
class MyQwtThermo : public QwtThermo{

public:
  MyQwtThermo( GuiQwtThermo* thermo, QWidget *parent, int width, int height )
    : QwtThermo( parent ), m_thermo(thermo)
  {}

  virtual ~MyQwtThermo(){}
  virtual void resizeEvent(QResizeEvent *e){
    QResizeEvent  *e2 =  new QResizeEvent(e->oldSize(), e->oldSize());
    QwtThermo::resizeEvent( e2 );
    delete e2;
  }
/* --------------------------------------------------------------------------- */
/*  -- contextMenuEvent                                                        */
/* --------------------------------------------------------------------------- */
void contextMenuEvent ( QContextMenuEvent * event ) {
   GuiPopupMenu* m_popupMenu = m_thermo->getPopupMenu();
   if (m_popupMenu) {
     m_popupMenu->setTitle( _("ScaleMenu") );
     m_popupMenu->setTearOff( true );
     if (!m_popupMenu->getElement()->getQtElement()->myWidget())
       m_popupMenu->getElement()->create();
     m_popupMenu->popup();
   }
}

public:
  void setLabelFormat( char c, int prec ){
//qt4    scaleDraw()->setLabelFormat( c, prec );
  }

private:
  GuiQwtThermo* m_thermo;
};


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
GuiQwtThermo::GuiQwtThermo( GuiElement *parent, const std::string &name )
  : GuiQtElement( parent, name )
  , m_thermo(0)
  , m_label(0)
  , m_unit(0)
  , m_fmt('g')
  , m_prec(4){}

GuiQwtThermo::GuiQwtThermo( const GuiQwtThermo &thermo )
  : GuiQtElement( thermo )
  , GuiThermo( thermo )
  , m_thermo(0)
  , m_label(0)
  , m_unit(0)
  , m_fmt( thermo.m_fmt )
  , m_prec( thermo.m_prec )
{
}

GuiQwtThermo::~GuiQwtThermo(){
  delete m_thermo;
}

bool GuiQwtThermo::destroy(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQwtThermo::create(){
  QGridLayout            *_layout;
  m_frame = new QWidget();
//   m_frame->setMinimumSize( width()+50, height()+50 );

  m_thermo = new MyQwtThermo( this, m_frame, width(), height() );

  updateWidgetProperty();

  int elements = 1;
  if( showLabel() ){
    ++elements;
    m_label = new QLabel( QtMultiFontString::getQString( getLabel() ), m_frame );
    QFont font = m_label->font();
    m_label->setFont( QtMultiFontString::getQFont( "@thermoLabel@", font ) );
  }

  if( showUnits() ){
    ++elements;
    m_unit = new QLabel( QtMultiFontString::getQString( getUnits() ), m_frame );
    QFont font = m_unit->font();
    m_unit->setFont( QtMultiFontString::getQFont( "@thermoUnits@", font ) );
    //    m_unit->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    //    m_unit->setMinimumSize( 30, 30 );
  }

  if( orientation() == GuiElement::orient_Horizontal ){
    m_frame->setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    _layout = new QGridLayout();
    int element=0;
    if( m_label ){
      _layout->addWidget( m_label,element,0, Qt::AlignHCenter );
      ++element;
    }
    _layout->addWidget( m_thermo, element, 0 );
    if( m_unit ){
      ++element;
      _layout->addWidget( m_unit,element,0, Qt::AlignHCenter );
    }
  }
  else{
    m_frame->setSizePolicy ( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
    int h=1;
    int v=1;
    if( m_label ) ++v;
    if( m_unit ) ++h;
    _layout = new QGridLayout();  //( m_frame, v, h, 0 );
    if( h > 1 ) _layout->setColumnStretch ( 0, 2 );
_layout->setColumnStretch ( 0, 12 );
    if( v > 1 ) _layout->setRowStretch ( 1, 2 );
    _layout->addWidget( m_thermo, v-1,h-1 , Qt::AlignRight);
    if( m_label ){
      _layout->addWidget( m_label,0,0,1,-1,Qt::AlignHCenter );
    }
    if( m_unit ){
      _layout->addWidget( m_unit,v-1,0, Qt::AlignVCenter| Qt::AlignRight);// | Qt::AlignRight );
    }
  }
  _layout->setContentsMargins(0,0,0,0);

  m_frame->setLayout( _layout );
  if( xfer_min() && xfer_max() ){
    xfer_min()->getValue(min());
    xfer_max()->getValue(max());
  }
  if( xfer_offset() ){
    xfer_offset()->getValue(offset());
  }
#if QWT_VERSION < 0x060100
  m_thermo->setRange( min(), max() );
#else
  m_thermo->setScale( min(), max() );
#endif
  const QColor &color = GuiQtManager::foregroundColor();

#if QWT_VERSION < 0x060000
  m_thermo->setFillColor( color );
  if (std::numeric_limits<double>::max() != alarm_level()) {
    m_thermo->setAlarmLevel( alarm_level() );
    m_thermo->setAlarmColor( QString::fromStdString(alarmColorName()) );
  }
#else
  m_thermo->setFillBrush( QBrush(color) );
  if (std::numeric_limits<double>::max() != alarm_level()) {
    m_thermo->setAlarmLevel( alarm_level() );
    m_thermo->setAlarmBrush( QColor(QString::fromStdString(alarmColorName())) );
  }
#endif

#if QWT_VERSION < 0x060100
  if( orientation() == GuiElement::orient_Horizontal ){
    m_thermo->setOrientation( Qt::Horizontal, QwtThermo::BottomScale );
  }
  else{
    m_thermo->setOrientation( Qt::Vertical, QwtThermo::LeftScale );
  }
#else
  if( orientation() == GuiElement::orient_Horizontal ){
    m_thermo->setOrientation(Qt::Horizontal);
    m_thermo->setScalePosition(QwtThermo::LeadingScale);
  }
  else{
    m_thermo->setOrientation(Qt::Vertical);
    m_thermo->setScalePosition(QwtThermo::TrailingScale);
  }
#endif
  if( height() > 0 ){
//     m_thermo->setMinimumSize( width(), height() );
    m_frame->setMinimumSize( width(), height() );
  }
  if( !colorsetName().empty() )
    setColorset( DataPoolIntens::Instance().getColorSet( colorsetName() ) );

  ((MyQwtThermo*)m_thermo)->setLabelFormat( m_fmt, m_prec );

  QFont font = m_thermo->font();
  m_thermo->setFont( QtMultiFontString::getQFont( "@thermoScale@", font ) );

  // Als letztes muessen die Indizes in der DataReference initialisiert werden.
  // Diese Arbeiten uebernimmt immer das DataItem.
  setAllDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQwtThermo::myWidget(){
  return m_frame;
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQwtThermo::update( UpdateReason reason ){
  if( xfer() == 0 || m_thermo == 0 ) return;
  updateWidgetProperty();

  switch( reason ) {
  case reason_FieldInput:
  case reason_Process:
    if( !isDataItemUpdated( GuiManager::Instance().LastGuiUpdate() ) ){
      return;
    }
    break;
  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
  default:
    ResetLastWebUpdated();  // reason_Always for webtens
    break;
  }

  double value=0,_min=0,_max=100, _al;

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
#if QWT_VERSION < 0x060100
      m_thermo->setRange( _min, _max );
#else
      m_thermo->setScale( _min, _max );
#endif
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

  if( xfer_alarm_level() ){
    if (xfer_alarm_level()->getValue(_al)) {
      if( alarm_level_scale() )
	_al = _al * alarm_level_scale()->getValue();
      if( _al != alarm_level() ){
	alarm_level()=_al;
	m_thermo->setAlarmLevel( _al );
#if QWT_VERSION < 0x060000
	m_thermo->setAlarmColor( QString::fromStdString(alarmColorName()) );
#else
	m_thermo->setAlarmBrush( QColor(QString::fromStdString(alarmColorName())) );
#endif
      }
    }
  }

  if( xfer()->getValue( value ) ){
    if( scale() ){
      value = value*scale()->getValue();
    }
    m_thermo->setValue( value );
  }
  else{
    m_thermo->setValue( min() );
  }
  setColor( value );

  if( m_label ){
    m_label->setText( QtMultiFontString::getQString( getLabel() ) );
  }

  if( m_unit ){
    m_unit->setText( QtMultiFontString::getQString( getUnits() ) );
  }
}

/* --------------------------------------------------------------------------- */
/* setColor --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQwtThermo::setColor( double value){
  if( !colorset() ){
#if QWT_VERSION < 0x060000
    m_thermo->setFillColor( QColor( "blue" ) );
#else
    m_thermo->setFillBrush( QColor( "blue" ) );
#endif
  } else {
    ColorSetItem *color = 0;
    color = colorset()->getColorItem( value );
#if QWT_VERSION < 0x060000
    if( !color )
      m_thermo->setFillColor( QColor( "blue" ) );
    else
      m_thermo->setFillColor( QColor( QString::fromStdString(color->foreground()) ) );
#else
    if( !color )
      m_thermo->setFillBrush( QColor( "blue" ) );
    else
      m_thermo->setFillBrush( QColor( QString::fromStdString(color->foreground()) ) );
#endif
  }

  // invert all if inverted flag is false and max value is less then min value
  if  ( !Inverted() ) {
    QPalette pal =  m_thermo->palette();
    if (!m_bgColorSave.isValid())
      m_bgColorSave = pal.color(QPalette::Window);
#if QWT_VERSION < 0x060100
    if ( m_thermo->maxValue() < m_thermo->minValue() ) {
#else
    if ( m_thermo->upperBound() < m_thermo->lowerBound() ) {
#endif
      // invert colors
#if QWT_VERSION < 0x060000
      pal.setColor(QPalette::Window, m_thermo->fillColor() );
      m_thermo->setPalette(pal);
      m_thermo->setFillColor( m_bgColorSave );
      if ( m_thermo->value() < 0 &&  m_thermo->value() >   m_thermo->alarmLevel()) {
	m_thermo->setAlarmColor( m_thermo->fillColor());
      }
#else
      pal.setColor(QPalette::Window, m_thermo->fillBrush().color());
      m_thermo->setPalette(pal);
      m_thermo->setFillBrush( m_bgColorSave );
      if ( m_thermo->value() < 0 &&  m_thermo->value() >   m_thermo->alarmLevel()) {
	m_thermo->setAlarmBrush( m_thermo->fillBrush());
      }
#endif
    } else  {
      pal.setColor(QPalette::Window, m_bgColorSave );
      m_thermo->setPalette(pal);
    }
  }

    // special case: thermo as color scale
    if ( colorScaleOption() && colorsetName().size()) {    // use colormap as color scale
      QwtLinearColorMap *colorMap = new QwtLinearColorMap();
      double sfac = scale() ? scale()->getValue() : 1.0 ;
      colorset()->getColorMap(*colorMap, min(), max(), (sfac < 0));
      BUG_DEBUG("setColor Range["<<min()<<", "<<max()<<"] offset["<<offset()<<"] Fabs["<<sfac<<"]");
      BUG_DEBUG("setColor Range["<<offset()<<", "<<(offset() + (max() - min()) * fabs(sfac))<<"]");
      m_thermo->setScale(offset(), offset() + (max() - min()) * fabs(sfac));
      m_thermo->setScaleMaxMinor(5);
      m_thermo->setScaleMaxMajor(8);

      colorMap->setMode(colorScaleOptionScaled() ? QwtLinearColorMap::ScaledColors :
                        QwtLinearColorMap::FixedColors);
      m_thermo->setColorMap( colorMap );
      double value;
      if( xfer()->getValue( value ) ) {
        m_thermo->setValue(  value * fabs(sfac) );
        m_thermo->setValue(offset() + (max() - min()) * fabs(sfac));
        BUG_DEBUG("setColor SETVALUE 1 ["<<(value * fabs(sfac))<<"]");
      } else
        m_thermo->setValue(  min() * fabs(sfac) );
    }
  }

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQwtThermo::manage(){
  myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQwtThermo::unmanage(){
  myWidget()->hide();
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */
GuiElement* GuiQwtThermo::clone() {
  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement())
    m_clonedThermo.push_back( new GuiQwtThermo( *this ) );
  else
    return baseElem->clone();
  return m_clonedThermo.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQwtThermo::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_clonedThermo.begin(), m_clonedThermo.end());
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQwtThermo::getSize( int &w, int &h ){
  //  w = width()+50;
  //  h = height()+50;
  //  return;

  if( !m_frame ){
    assert( false );
    w = 0;
    h = 0;
    return;
  }
  QSize size = m_frame->frameSize();
  w = size.width();
  h = size.height();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQwtThermo::serializeXML(std::ostream &os, bool recursive) {
}


/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQwtThermo::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  return GuiThermo::serializeJson(jsonObj, onlyUpdated);
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQwtThermo::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  return GuiThermo::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* setScaleFormat --                                                        */
/* --------------------------------------------------------------------------- */
void GuiQwtThermo::setScaleFormat( const std::string& format ){
  int prec = 0;
  char fmt = 'g';
  std::istringstream is( format );
  is >> m_prec;
  is >> m_fmt;
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQwtThermo::getDialogExpandPolicy() {
  return (GuiElement::Orientation) 0;
}
