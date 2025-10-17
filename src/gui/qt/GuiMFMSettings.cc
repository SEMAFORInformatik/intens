
#include <qdialog.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qobject.h>
#include <qvalidator.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <QApplication>
#include <QtCore/QSettings>

#include "utils/Debugger.h"
INIT_LOGGER();
#include "gui/qt/GuiQtManager.h"

#include "utils/gettext.h"
#include "gui/GuiImage.h"
#include "gui/qt/GuiMFMSettings.h"
#include "gui/qt/ArrowKeyLineEdit.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtIconManager.h"

#include "gui/DataProcessing.h"

// utility classes for arrow key handling
class WeightAdapter: public ArrowKeyListener {
public:
    WeightAdapter( GuiMFMSettings *s ): m_settings(s){}
    void valueChanged(){ m_settings -> weightHasChanged(); }
private:
    GuiMFMSettings *m_settings;
};

class MinMaxRangeAdapter: public ArrowKeyListener {
public:
    MinMaxRangeAdapter( GuiMFMSettings *s ): m_settings(s){}
    void valueChanged(){ m_settings -> rangeHasChanged(); }
private:
    GuiMFMSettings *m_settings;
};

class ScaleAdapter: public ArrowKeyListener {
public:
    ScaleAdapter( GuiMFMSettings *s ): m_settings(s){}
    void valueChanged(){ m_settings -> scaleHasChanged(); }
private:
    GuiMFMSettings *m_settings;
};

// utility function for labels
QLabel *addLabel( const QString &text, QGridLayout *grid,
		  QWidget *parent, int row, int col){
  QLabel *label=new QLabel( text, parent );
  grid->addWidget( label, row, col );
  label->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
  QFont font = label->font();
  label->setFont( QtMultiFontString::getQFont( "@label@", font ) );
  return label;
}

QLabel *addTitleLabel( const QString &text, QGridLayout *grid,
		       QWidget *parent, int row, int span=3, Qt::AlignmentFlag alignment=Qt::AlignLeft){
  QLabel *label=new QLabel( text, parent );
  grid->addWidget( label, row, 1, 1, span, alignment );
  label->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
  QFont font = label->font();
  label->setFont( QtMultiFontString::getQFont( "@title@", font ) );
  return label;
}

void setReadOnly( QLineEdit *l, bool flag ){
    QColor background, foreground;
    QPalette pal=  QApplication::palette();
    enum Qt::FocusPolicy focpol = Qt::NoFocus;
    // override qt default colors
    if( flag ){
	background = GuiQtManager::readonlyBackgroundColor();
	foreground = GuiQtManager::readonlyForegroundColor();
    }
    else {
	background = GuiQtManager::editableBackgroundColor();
	foreground = GuiQtManager::editableForegroundColor();
	focpol = Qt::StrongFocus;
    }
    pal.setColor(l->backgroundRole(),      background );
    pal.setColor(l->foregroundRole(),      foreground );
    pal.setColor( QPalette::Disabled, l->backgroundRole(), background.darker(115) );
    pal.setColor( QPalette::Disabled, QPalette::Base, background.darker(115) );

    l -> setDisabled( flag );
    l -> setPalette( pal );
    l -> setFocusPolicy( focpol );

}

void setFieldLength( QLineEdit *w, int length ){
  QFont font = w->font();
    w->setFont( QtMultiFontString::getQFont( "@text@", font ) );
    QSize mhs =  w->minimumSizeHint();
    int charlen = w->fontMetrics().horizontalAdvance( '8' );
    int delta = 2*(mhs.width() - charlen);
    w->setMinimumSize( delta+length*charlen,
		       w->fontMetrics().height()+4);
    w->setMaximumSize( delta+length*charlen,
		       w->fontMetrics().height()+4);
    w->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
}

class MyLayout : public QGridLayout{
public:
  MyLayout( QWidget*parent, int rows, int cols, int space )
    : QGridLayout() {//( parent, rows, cols, space ){
    parent->setLayout( this );

  }
  virtual ~MyLayout(){
    assert( false );
//     deleteAllItems();
  }
};

GuiMFMSettings *GuiMFMSettings::s_instance = 0;
//---------------------------------------------------------------------------
// Constructor / Destructor
//---------------------------------------------------------------------------
GuiMFMSettings::GuiMFMSettings()
  : m_image( 0 )
  , m_currentMin( 0 )
  , m_currentMax( 0 )
  , m_avg( 0 )
  , m_stddev( 0 )
  , m_weight( 0 )
  , m_channel( 0 )
  , m_direction( 0 )
  , m_link_checkbox( 0 )
  , m_scale_checkbox( 0 )
  , m_mappingMin( 0 )
  , m_mappingMax( 0 )
  , m_dialog( 0 )
  , m_grid( 0 )
  , m_directionBox( 0 )
  , m_channelBox( 0 )
  , m_mappingBox( 0 )
  , m_factor( 1 )
  , m_processingBox( 0 )
  , m_titleLabel( 0 )
  , m_converter(12,2, 0, true, true)
  , m_scaleConverter(8,2, 0, true, true){
}

GuiMFMSettings::~GuiMFMSettings(){
  delete m_grid; // löscht auch alle items !!
  delete m_dialog;
}

//---------------------------------------------------------------------------
// getDialog
//---------------------------------------------------------------------------
GuiMFMSettings &GuiMFMSettings::getDialog(){
  if( s_instance == 0 ){
    s_instance = new GuiMFMSettings;
  }
  return *s_instance;
}

//---------------------------------------------------------------------------
// createProcessingBox
//---------------------------------------------------------------------------
void GuiMFMSettings::createProcessingBox( int row ){
  QStringList list;
  list << _("Raw");
  list << _("Average");
  list << _("Line Fit");
  list << _("Plane Fit");

  m_processingBox = new QComboBox( m_dialog );
  m_processingBox->setEditable( false );
  m_processingBox->addItems( list );
  m_grid->addWidget( m_processingBox, row, 2, 1, 3 );
  addLabel( _("Processing:"), m_grid, m_dialog, row, 1 );

  connect( m_processingBox, SIGNAL( activated ( int ) ), this,
	   SLOT(slot_activateProcessing( int ) ) );
}

//---------------------------------------------------------------------------
// createMappingBox
//---------------------------------------------------------------------------
void GuiMFMSettings::createMappingBox( int row ){
  QStringList list;
  list << _("Automatic") << _("Manual") << _("Standard Deviation");

  m_mappingBox = new QComboBox( m_dialog );
  m_mappingBox->setEditable( false );
  m_mappingBox->addItems( list );
  m_grid->addWidget( m_mappingBox, row, 2, 1, 3 );
  addLabel( _("Mapping:"), m_grid, m_dialog, row, 1 );
  connect( m_mappingBox, SIGNAL( activated ( int ) ), this,
	   SLOT(slot_activateMapping( int ) ) );
}

//---------------------------------------------------------------------------
// rebuildChannelBox
//---------------------------------------------------------------------------
void GuiMFMSettings::rebuildChannelBox(){
  if( m_channelBox == 0 ){
    return;
  }
  createChannelBox( -1 );
}



//---------------------------------------------------------------------------
// createChannelBox
//---------------------------------------------------------------------------
void GuiMFMSettings::createChannelBox( int row ){
  if( m_dialog == 0 ){
    return;
  }
  if( m_channelBox == 0 && row < 0 ){
    assert( false );
  }
  QStringList list;
  std::map<const int,std::string> chl;
  std::map<const int,std::string>::iterator iter;
  int i=0;
  int n=GuiImage::getChannelList(chl);
  m_channelMap.clear();
  if( m_channelBox ){
    m_channelBox->clear();
  }
  bool empty_entry ( false );
  if( n>0 ){
    for(iter=chl.begin(); iter!=chl.end(); ++iter, ++i ){
      if ( (*iter).first != 15 ) { // channel 15 (no channel) comes last
	list << QString::fromStdString((*iter).second);
	m_channelMap.insert(std::pair<const int, int>(i,(*iter).first) );
      } else {
	empty_entry = true;
      }
    }
    if ( empty_entry ) { // channel 15 (no channel) comes last
      list << QString("");
      m_channelMap.insert(std::pair<const int, int>(i,15) );
    }
    if( m_channelBox == 0 ){
      addLabel(_("Channel:"), m_grid, m_dialog, row, 1 );

      m_channelBox = new QComboBox( m_dialog );
      m_channelBox->setEditable( false );
      m_grid->addWidget( m_channelBox, row, 2, 1, 3 );

      connect( m_channelBox, SIGNAL( activated ( int ) ), this,
	     SLOT(slot_activateChannel( int ) ) );
    }
    m_channelBox->addItems( list );
    if ( m_image )
      setChannel( m_image -> getDirection(), m_image -> getChannel() );
  }
// amg: 19.08.2005 WAS soll das hier????
//   else if( m_channel == 0 ){
//     m_channel = new QLineEdit ( m_dialog );
//     m_channel->setReadOnly( true );
//     m_channel->setFocusPolicy( Qt::NoFocus );
//     m_grid->addWidget( m_channel, row, 2 );
//     addLabel(_("Channel:"), m_grid, m_dialog, row, 1 );
//   }
}

void GuiMFMSettings::createDirectionBox( int row ){
  QStringList list;

  std::vector<std::string> tpl;
  int n=m_image -> getDirectionList( tpl );
  if( n > 0 ){
    for( int i=0; i<n; ++i ){
      list << QString::fromStdString(tpl[i]);
      m_directionVector.push_back(i);
    }
    m_directionBox = new QComboBox( m_dialog );
    m_directionBox->setEditable( false );
    m_directionBox->addItems( list );
    m_grid->addWidget( m_directionBox, row, 2, 1, 3 );

    connect( m_directionBox, SIGNAL( activated ( int ) ), this,
	     SLOT(slot_activateChannel( int ) ) );
  }
  else{
    m_direction = new QLineEdit ( m_dialog );
    m_direction->setDisabled( true );
    m_direction->setFocusPolicy( Qt::NoFocus );
    m_grid->addWidget( m_direction, row, 2 );
  }
  addLabel( _("Direction:"), m_grid, m_dialog, row, 1 );
}
//
//---------------------------------------------------------------------------
// openDialog
//---------------------------------------------------------------------------
void GuiMFMSettings::openDialog( QWidget *parent ){
  if( !m_dialog ){
    m_dialog = new QDialog( parent );
    m_dialog->setWindowTitle( _("View Settings") );

    // set Icon
    QPixmap logo;
    if( QtIconManager::Instance().getPixmap( GuiQtManager::Settings()->value
					     ( "Intens/ApplicationIcon", "intens").toString().toStdString()
					     , logo ) ){
      m_dialog->setWindowIcon( QIcon(logo) );
    } else if( QtIconManager::Instance().getPixmap( "intens", logo ) ){
      m_dialog->setWindowIcon( logo );
    }

    int nrows=12;
    m_grid = new MyLayout ( m_dialog, nrows, 3, 19 );
    int margin = 3;
    m_grid->setSpacing( margin );
    m_grid->setContentsMargins(margin, margin, margin, margin);

    int row=1;

    m_titleLabel = addTitleLabel( _("Settings"), m_grid, m_dialog,
				  row, 3, Qt::AlignHCenter );

    row++;

    createChannelBox( row++ );
    createDirectionBox( row++ );
    createProcessingBox( row++ );
    createMappingBox( row++ );

    m_grid -> addItem( new QSpacerItem( 0, 5 ), row++, 1 );
    addTitleLabel( _("Mapping Parameters"),
		   m_grid, m_dialog, row );

    row++;
    addLabel(_("Maximum:"), m_grid, m_dialog, row, 1 );
    m_mappingMax = new ArrowKeyLineEdit ( m_dialog, &m_converter );
    m_mappingMax -> setValidator( new QDoubleValidator( m_mappingMax ) );
    m_grid -> addWidget( m_mappingMax, row, 2 );
    setFieldLength( m_mappingMax, 12 );
    m_units.push_back( addLabel( " ",m_grid, m_dialog, row, 3 ) );
    m_mappingMax -> attachListener( new MinMaxRangeAdapter( this ) );
    setReadOnly( m_mappingMax, true );

    row++;
    addLabel(_("Minimum:"), m_grid, m_dialog, row, 1 );
    m_mappingMin = new ArrowKeyLineEdit ( m_dialog, &m_converter );
    m_mappingMin -> setValidator( new QDoubleValidator( m_mappingMin ) );
    setFieldLength( m_mappingMin, 12 );
    m_grid -> addWidget( m_mappingMin, row, 2 );
    m_units.push_back( addLabel(" ",m_grid, m_dialog, row, 3 ) );
    m_mappingMin -> attachListener( new MinMaxRangeAdapter( this ) );
    setReadOnly( m_mappingMin, true );

    row++;
    addLabel( _("Weight:"), m_grid, m_dialog, row, 1 );
    m_weight = new ArrowKeyLineEdit ( m_dialog, &m_converter );
    m_weight -> setValidator( new QDoubleValidator( m_weight ) );
    m_weight -> attachListener( new WeightAdapter( this ) );
    setReadOnly( m_weight, true );
    setFieldLength( m_weight, 12 );
    m_grid -> addWidget( m_weight, row, 2 );

    row++;
    m_grid->addItem( new QSpacerItem( 0, 5 ), row++, 1 );
    addTitleLabel(_("Current Values"), m_grid, m_dialog, row );

    row++;
    addLabel( _("Average:"), m_grid, m_dialog, row, 1 );
    m_avg = new QLineEdit ( m_dialog );
    m_avg->setAlignment(Qt::AlignRight);
    setReadOnly( m_avg, true );
    setFieldLength( m_avg, 12 );
    m_grid -> addWidget( m_avg, row, 2 );
    m_units.push_back( addLabel( " ",m_grid, m_dialog, row, 3 ) );

    row++;
    addLabel(_("Std. Dev.:"), m_grid, m_dialog, row, 1 );
    m_stddev = new QLineEdit ( m_dialog );
    m_stddev->setAlignment(Qt::AlignRight);
    setReadOnly( m_stddev, true );
    setFieldLength( m_stddev, 12 );
    m_grid -> addWidget( m_stddev, row, 2 );
    m_units.push_back( addLabel( " ",m_grid, m_dialog, row, 3 ) );

    row++;
    addLabel( _("Maximum:"), m_grid, m_dialog, row, 1 );
    m_currentMax = new QLineEdit ( m_dialog );
    m_currentMax->setAlignment(Qt::AlignRight);
    setReadOnly( m_currentMax, true );
    setFieldLength( m_currentMax, 12 );
    m_grid -> addWidget( m_currentMax, row, 2 );
    m_units.push_back( addLabel( " ",m_grid, m_dialog, row, 3 ) );

    row++;
    addLabel(_("Minimum:"), m_grid, m_dialog, row, 1 );
    m_currentMin = new QLineEdit ( m_dialog );
    m_currentMin->setAlignment(Qt::AlignRight);
    setReadOnly( m_currentMin, true );
    setFieldLength( m_currentMin, 12 );
    m_grid -> addWidget( m_currentMin, row, 2 );
    m_units.push_back( addLabel( " ",m_grid, m_dialog, row, 3 ) );

    row++;
    m_grid -> addItem( new QSpacerItem( 0, 5 ), row++, 1 );
    addTitleLabel( _("Image Scaling"),
		   m_grid, m_dialog, row++, 2 );

    //    m_link_checkbox=new QCheckBox("Link View",m_dialog );
    //    m_grid -> addWidget( m_link_checkbox, row, 2 );

    m_scale_checkbox=new QCheckBox(_("Manual"),m_dialog );
    m_grid -> addWidget( m_scale_checkbox, row, 3 );
    row++;
    addLabel(_("Scale X:"), m_grid, m_dialog, row, 1 );
    m_scale_x = new QLineEdit ( m_dialog );
    m_scale_x->setAlignment(Qt::AlignRight);
    setReadOnly( m_scale_x, true );
    m_grid -> addWidget( m_scale_x, row, 2 );
    setFieldLength( m_scale_x, 8 );

    m_scale_x_manual = new ArrowKeyLineEdit ( m_dialog, &m_scaleConverter );
    m_scale_x_manual->setValidator( new QDoubleValidator( m_scale_x_manual ) );
    m_grid -> addWidget( m_scale_x_manual, row, 3 );
    setFieldLength( m_scale_x_manual, 8 );
    m_scale_x_manual -> attachListener( new ScaleAdapter( this ) );
    setReadOnly( m_scale_x_manual, true );

    row++;
    addLabel(_("Scale Y:"), m_grid, m_dialog, row, 1 );
    m_scale_y = new QLineEdit ( m_dialog );
    m_scale_y->setAlignment(Qt::AlignRight);
    setReadOnly( m_scale_y, true );
    m_grid->addWidget( m_scale_y, row, 2 );
    setFieldLength( m_scale_y, 8 );

    m_scale_y_manual = new ArrowKeyLineEdit ( m_dialog, &m_scaleConverter );
    m_scale_y_manual -> setValidator( new QDoubleValidator( m_scale_y_manual ) );
    setReadOnly( m_scale_y_manual, true );

    m_grid -> addWidget( m_scale_y_manual, row, 3 );
    setFieldLength( m_scale_y_manual, 8 );
    m_scale_y_manual -> attachListener( new ScaleAdapter( this ) );

    if( m_scale_checkbox ){
      connect( m_scale_checkbox, SIGNAL(toggled(bool)), this,
	       SLOT(slot_scale_toggled(bool)));
    }
    if( m_link_checkbox ){
      connect( m_link_checkbox, SIGNAL(toggled(bool)), this,
	       SLOT(slot_link_toggled(bool)));
    }
    connect( m_mappingMin, SIGNAL( returnPressed () ), this,
	     SLOT(slot_lostFocusRange() ) );
    connect( m_mappingMin, SIGNAL( editingFinished () ), this,
	     SLOT(slot_lostFocusRange() ) );
    connect( m_mappingMax, SIGNAL( returnPressed () ), this,
	     SLOT(slot_lostFocusRange() ) );
    connect( m_mappingMax, SIGNAL( editingFinished () ), this,
	     SLOT(slot_lostFocusRange() ) );

    connect( m_weight, SIGNAL( returnPressed () ), this,
	     SLOT(slot_lostFocusWeight() ) );
    connect( m_weight, SIGNAL( editingFinished () ), this,
	     SLOT(slot_lostFocusWeight() ) );

    connect( m_scale_x_manual, SIGNAL( returnPressed () ), this,
	     SLOT(slot_lostFocusScale() ) );
    connect( m_scale_x_manual, SIGNAL( editingFinished () ), this,
	     SLOT(slot_lostFocusScale() ) );
    connect( m_scale_y_manual, SIGNAL( returnPressed () ), this,
	     SLOT(slot_lostFocusScale() ) );
    connect( m_scale_y_manual, SIGNAL( editingFinished () ), this,
	     SLOT(slot_lostFocusScale() ) );
  }
  if( !m_dialog -> isHidden() ){
    m_dialog -> raise();
  } else
    m_dialog -> show();
}

void GuiMFMSettings::setUnits( const std::string &units ){
  std::vector<QLabel*>::iterator i;
  for( i=m_units.begin(); i<m_units.end(); ++i ){
    (*i)->setText( QString::fromStdString(units) );
  }
}

//---------------------------------------------------------------------------
// setMinMax
//---------------------------------------------------------------------------
void GuiMFMSettings::setCurrentRange( double min, double max ){
  if( m_dialog ){
    setFormattedValue( m_converter, m_currentMin, min*m_factor );
    setFormattedValue( m_converter, m_currentMax, max*m_factor );
  }
}
//---------------------------------------------------------------------------
// setScale
//---------------------------------------------------------------------------
void GuiMFMSettings::setScale( bool is_manual,
			       double auto_x, double auto_y,
			       double manual_x, double manual_y ){
  if( m_dialog ){
    if( !is_manual ){
      setFormattedValue( m_scaleConverter, m_scale_x, auto_x );
      setFormattedValue( m_scaleConverter, m_scale_y, auto_y );
    }
    else {
      setFormattedValue( m_scaleConverter, m_scale_x, manual_x );
      setFormattedValue( m_scaleConverter, m_scale_y, manual_y );
    }
    setFormattedValue( m_scaleConverter, m_scale_x_manual, manual_x );
    setFormattedValue( m_scaleConverter, m_scale_y_manual, manual_y );

m_scale_checkbox->blockSignals(true);
setReadOnly( m_scale_x_manual, !is_manual );
setReadOnly( m_scale_y_manual, !is_manual );
    m_scale_checkbox -> setChecked( is_manual );
m_scale_checkbox->blockSignals(false);
  }
}

//---------------------------------------------------------------------------
// setWeight
//---------------------------------------------------------------------------
void GuiMFMSettings::setWeight( double w ){
  if( m_dialog ){
    setFormattedValue( m_converter, m_weight, w );
  }
}
//---------------------------------------------------------------------------
// setStdDev
//---------------------------------------------------------------------------
void GuiMFMSettings::setStdDev( double s ){
  if( m_dialog ){
    setFormattedValue( m_converter, m_stddev, s*m_factor );
  }
}
//---------------------------------------------------------------------------
// setAverage
//---------------------------------------------------------------------------
void GuiMFMSettings::setAverage( double a ){
  if( m_dialog ){
    setFormattedValue( m_converter, m_avg, a*m_factor );
  }
}

//---------------------------------------------------------------------------
// setChannel
//---------------------------------------------------------------------------
void GuiMFMSettings::setChannel( int dir
				 , int channel ){
  std::vector<int>::iterator iter;
  if( !m_directionVector.empty() ){
    int i(0);
    for( iter = m_directionVector.begin();
	 iter != m_directionVector.end(); ++iter, ++i ){
      if( *iter == dir ){
	dir = i;
	break;
      }
    }
  }

  std::map<const int, int>::iterator iiter;
  int i(0);
  if( !m_channelMap.empty() ){
    for( iiter = m_channelMap.begin();
	 iiter != m_channelMap.end(); ++iiter, ++i ){
      if( (*iiter).second == channel ){
	channel = i;
	break;
      }
    }
  }

  if( m_directionBox ){
    m_directionBox->setCurrentIndex( dir );
  }
  else if( m_direction ){
    m_direction->setText( QString::number( dir ) );
  }
  if( m_channelBox ){
    m_channelBox->setCurrentIndex( channel );
  }
  else if( m_channel ){
    m_channel->setText( QString::number( channel ) );
  }
}

//---------------------------------------------------------------------------
// setProcessing
//---------------------------------------------------------------------------
void GuiMFMSettings::setProcessing( int index ){
  if( m_processingBox ){
    m_processingBox->setCurrentIndex( index );
  }
}

//---------------------------------------------------------------------------
// setMapping
//---------------------------------------------------------------------------
void GuiMFMSettings::setMapping( int m ){
  if( m_mappingBox ){
    m_mappingBox->setCurrentIndex( m );

    switch( m ){
	case 1:
	    setReadOnly( m_mappingMin, false );
	    setReadOnly( m_mappingMax, false );
	    setReadOnly( m_weight, true );
	    break;

	case 2:
	    setReadOnly( m_mappingMin, true );
	    setReadOnly( m_mappingMax, true );
	    setReadOnly( m_weight, false );
	    break;

	default:
	    setReadOnly( m_mappingMin, true );
	    setReadOnly( m_mappingMax, true );
	    setReadOnly( m_weight, true );

    }
  }
}
//---------------------------------------------------------------------------
// getMapping
//---------------------------------------------------------------------------
int GuiMFMSettings::getMapping( ){
  if( m_mappingBox ){
    return m_mappingBox->currentIndex( );
  }
  return 0;
}

//---------------------------------------------------------------------------
// setMappingRange
//---------------------------------------------------------------------------
void GuiMFMSettings::setMappingRange( double min
			       , double max ){
  if( m_dialog ){
    setFormattedValue( m_converter, m_mappingMin, min*m_factor );
    setFormattedValue( m_converter, m_mappingMax, max*m_factor );
  }
}

//---------------------------------------------------------------------------
// setImage
//---------------------------------------------------------------------------
void GuiMFMSettings::setImage( GuiImage *image, const std::string &name ){
  m_image = image;
  if( m_link_checkbox ){
    if( m_image->getType() == GuiImage::e_Plot ){
      m_link_checkbox->setEnabled ( false );
    }
    else{
      m_link_checkbox->setEnabled ( true );
    }
  }
  if( image ){
    setWeight( image -> getWeight() );
    setUnits( image -> getChannelUnit() );
    setChannel( image -> getDirection(), image -> getChannel() );
    setProcessing( image -> getProcessingIndex() );
    setFactor( image -> getChannelUnitFactor() );
    setMapping( image -> getMapping() );

    double min = image -> getMinValue();
    if( min == std::numeric_limits<double>::max() ) min=0;
    double max = image -> getMaxValue();
    if( max == -std::numeric_limits<double>::max() ) max=0;
    setCurrentRange( min, max );

    if( image -> getAutoRange() ){
	min = image -> getRangeMin();
	if( min == std::numeric_limits<double>::max() ) min=0;
	max = image -> getRangeMax();
	if( max == -std::numeric_limits<double>::max() ) max=0;
	setMappingRange( min, max );
    }
    else {
	setMappingRange( image -> getManualRangeMin(),
			 image -> getManualRangeMax() );
    }

    setAverage( image -> getAverage() );
    setStdDev( image -> getStdDev() );
    setScale( image -> isManualScale(),
	      image -> getAutoScaleX(), image -> getAutoScaleY(),
	      image -> getManualScaleX(), image -> getManualScaleY() );

    if( m_titleLabel ){
      if( !name.empty() ){
	m_titleLabel->setText( QString::fromStdString(name) );
      }
    }
  }
}

//---------------------------------------------------------------------------
// rangeHasChanged
//---------------------------------------------------------------------------
void GuiMFMSettings::rangeHasChanged(){
  double min = m_mappingMin -> text().toDouble()/m_factor;
  double max = m_mappingMax -> text().toDouble()/m_factor;
  setFormattedValue( m_converter, m_mappingMin, min*m_factor );
  setFormattedValue( m_converter, m_mappingMax, max*m_factor );
  min = m_mappingMin -> text().toDouble()/m_factor;
  max = m_mappingMax -> text().toDouble()/m_factor;

  bool isManual = getMapping() == 1;
  m_image->setRange( isManual, min, max );
}

//---------------------------------------------------------------------------
// setActiveWindow
//---------------------------------------------------------------------------
void GuiMFMSettings::setActiveWindow(){
  if( m_dialog ){
    m_dialog->raise();
  }
}

bool GuiMFMSettings::isLinked(){
  if( m_link_checkbox ){
    return m_link_checkbox->isChecked() && m_link_checkbox->isEnabled();
  }
  return false;
}

//---------------------------------------------------------------------------
// slot_scale_toggled
//---------------------------------------------------------------------------
void GuiMFMSettings::slot_scale_toggled( bool toggle ){
//    BUG_DEBUG( "GuiMFMSettings::slot_scale_toggled " );
  if ( isLinked() ){
    std::vector<GuiImage*> images = GuiImage::getImages();
    std::vector<GuiImage*>::iterator iter;
    for( iter=images.begin();iter!=images.end();++iter ){
      (*iter) -> setManualScale( toggle );
    }
  }
  else{
    m_image -> setManualScale( toggle );
  }
  if( toggle ){  // scale is defined manually
    setReadOnly( m_scale_x_manual, false );
    setReadOnly( m_scale_y_manual, false );
    if( m_scale_x_manual->isModified() ||
	m_scale_y_manual->isModified() || isLinked() ){
      scaleHasChanged();
    }
    else {
      resizeImage( m_image );
    }
  }
  else { // restore original size: (scale is defined automatically)
    int orig_width, orig_height;
    setFormattedValue( m_scaleConverter, m_scale_x, m_image -> getAutoScaleX() );
    setFormattedValue( m_scaleConverter, m_scale_y, m_image -> getAutoScaleY() );

    if ( isLinked() ){
      std::vector<GuiImage*> images = GuiImage::getImages();
      std::vector<GuiImage*>::iterator iter;
      for( iter=images.begin();iter!=images.end();++iter ){
	(*iter) -> resize();
      }
    }
    else{
      m_image -> resize();
    }

    setReadOnly( m_scale_x_manual, true );
    setReadOnly( m_scale_y_manual, true );
  }
}

void GuiMFMSettings::slot_link_toggled( bool toggle ){
  std::vector<GuiImage*> images = GuiImage::getImages();
  std::vector<GuiImage*>::iterator iter;
  for( iter=images.begin();iter!=images.end();++iter ){
    (*iter) -> setManualScale( m_scale_checkbox->isChecked() );
  }

  if( !toggle ){
    return;
  }
  if( m_scale_checkbox->isChecked() ){
    scaleHasChanged();
  }
  else{
    std::vector<GuiImage*>::iterator iter;
    for( iter=images.begin();iter!=images.end();++iter ){
      (*iter) -> resize();
    }
  }
}

//---------------------------------------------------------------------------
// slot_activateChannel
//---------------------------------------------------------------------------
void GuiMFMSettings::slot_activateChannel( int index ){
  int direction = m_directionVector[m_directionBox->currentIndex()];
  std::map<const int, int>::iterator iter=m_channelMap.begin();
  for( int i = 0; i < m_channelBox->currentIndex();++i )
    ++iter;
  int channel = (*iter).second;
//  BUG_DEBUG( "GuiMFMSettings::slot_activateChannel .." );
  m_image->setChannel( direction, channel );
  setUnits( m_image->getChannelUnit() );
  m_image->updateUnit();
  setFactor( m_image->getChannelUnitFactor() );
  m_image->redraw( false );
//  BUG_DEBUG( "GuiMFMSettings::slot_activateChannel .. done" );
}

//---------------------------------------------------------------------------
// slot_activateProcessing
//---------------------------------------------------------------------------
void GuiMFMSettings::slot_activateProcessing( int index ){
    assert( m_image );
    m_image->setProcessing( index );
    m_image->redraw( false );
}

//---------------------------------------------------------------------------
// slot_activateMapping
//---------------------------------------------------------------------------
void GuiMFMSettings::slot_activateMapping( int index ){
    setMapping( index );
    m_image->setMapping( index );
    rangeHasChanged();
    m_image->redraw( false );
}

//---------------------------------------------------------------------------
// lostFocusRange
//---------------------------------------------------------------------------
void GuiMFMSettings::slot_lostFocusRange(){
  if( m_mappingMin -> isModified() || m_mappingMax -> isModified() ){
    m_mappingMin -> setModified(false);
    m_mappingMax -> setModified(false);
    rangeHasChanged();
  }
}

//---------------------------------------------------------------------------
// slot_lostFocusScale
//---------------------------------------------------------------------------
void GuiMFMSettings::slot_lostFocusScale(){
  if( m_scale_x_manual -> isModified() || m_scale_y_manual -> isModified() ){
      m_scale_y_manual -> setModified(false);
      m_scale_x_manual -> setModified(false);
      scaleHasChanged();
  }
}

void GuiMFMSettings::scaleHasChanged(){
  double x = m_scale_x_manual -> text().toDouble();
  double y = m_scale_y_manual -> text().toDouble();
  BUG_DEBUG( "GuiMFMSettings::scaleHasChanged " << x << "/" << y );
  if( x <= 0 ){
    x=m_image -> getManualScaleX();
  }
  if( y <= 0 ){
    y=m_image -> getManualScaleY();
  }
  setFormattedValue( m_scaleConverter, m_scale_x_manual, x );
  setFormattedValue( m_scaleConverter, m_scale_y_manual, y );
  x = m_scale_x_manual -> text().toDouble();
  y = m_scale_y_manual -> text().toDouble();

  if( isLinked() ){
    int xx=0,yy=0;
    double xf=0,yf=0;
    m_image->getSize( xx, yy );

    xf = (double)xx/x;
    yf = (double)yy/y;

    std::vector<GuiImage*> images = GuiImage::getImages();
    std::vector<GuiImage*>::iterator iter;
    for( iter=images.begin();iter!=images.end();++iter ){
      (*iter)->getSize(xx,yy);
      (*iter)->setManualScale( (double)xx/xf, (double)yy/yf);
      if( (*iter)->isManualScale() ){
	resizeImage( (*iter) );
      }
    }
  }
  else{
    m_image -> setManualScale( x, y );
    if( m_image -> isManualScale() ){
      resizeImage( m_image );
    }
  }
}

void GuiMFMSettings::resizeImage( GuiImage* image ){
  if( image == 0 ){
    image=m_image;
  }
  int w,h;
  // image->getSize(w,h);
  // double scan_width = w / image->getScaleX();
  // double scan_height = h / image->getScaleY();
  int scan_width, scan_height;
  image->getScanSize(scan_width,scan_height);

  if ( scan_width == 0 || scan_height == 0 ) {
    return;
  }

  double man_x = image -> getManualScaleX();
  double man_y = image -> getManualScaleY();

  BUG_DEBUG( "GuiMFMSettings::resizeImage "
		   << scan_width << "/" << scan_height
		 << " " << man_x << "/" << man_y );

  w = static_cast<int>( scan_width * man_x );
  h = static_cast<int>( scan_height * man_y );
  if( w > 4000 ){ w=4000; }
  if( h > 4000 ){ h=4000; }
  if( w < 10 ) { w=10; }
  if( h < 10 ) { h=10; }

  if( w == 4000 || w == 10 ){
    man_x = (double)w/scan_width;
  }
  if( h == 4000 || h == 10 ){
    man_y = (double)h/scan_height;
  }
  if( image == m_image ){
    setFormattedValue( m_scaleConverter, m_scale_x, man_x );
    setFormattedValue( m_scaleConverter, m_scale_y, man_y );
    setFormattedValue( m_scaleConverter, m_scale_x_manual, man_x );
    setFormattedValue( m_scaleConverter, m_scale_y_manual, man_y );
    man_x = m_scale_x -> text().toDouble();
    man_y = m_scale_y -> text().toDouble();
  }

  w=static_cast<int>( scan_width * man_x );
  h=static_cast<int>( scan_height * man_y );

  image -> setManualScale( man_x, man_y );
  image -> resize( w, h );
}

//---------------------------------------------------------------------------
// lostFocusWeight
//---------------------------------------------------------------------------
void GuiMFMSettings::slot_lostFocusWeight(){
  if( m_weight->isModified() ){
    m_weight->setModified(false);
    weightHasChanged();
  }
}

void GuiMFMSettings::weightHasChanged(){
  double weight = m_weight->text().toDouble();
  m_image->setWeight( weight );
  setWeight( weight );
  if ( getMapping() == 2 ) m_image->redraw( false );
}

//---------------------------------------------------------------------------
// hide
//---------------------------------------------------------------------------
void GuiMFMSettings::hide(){
  if (m_dialog && !m_dialog->isHidden()  )
    m_dialog->hide();
}

//---------------------------------------------------------------------------
// setFormattedValue
//---------------------------------------------------------------------------
void GuiMFMSettings::setFormattedValue( RealConverter &c,
					QLineEdit *w, double x ){
  std::ostringstream output;

  c.write( x, output );
  std::string str(output.str());
  GuiQtTextfield::setCursor(w, str );
}
