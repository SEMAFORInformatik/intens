
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwt_plot.h>
#include <qwt_plot_barchart.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_draw.h>

#include "gui/qt/GuiQwtPropertyDialog.h"
#include "gui/qt/GuiQwtPlotBarChart.h"
#include "gui/qt/ArrowKeyLineEdit.h"
#include "xfer/XferConverter.h"
#include "utils/gettext.h"

class UpdateAdapter: public ArrowKeyListener {
public:
  UpdateAdapter( GuiQwtPropertyDialog *s ): m_panel(s){}
  void valueChanged(){ m_panel->valueChanged(); }
private:
  GuiQwtPropertyDialog *m_panel;
};

//---------------------------------------------------
// Klasse GuiQwtPropertyDialog
//---------------------------------------------------
GuiQwtPropertyDialog::GuiQwtPropertyDialog( QWidget *parent ):
    QDialog( parent )
{
  setWindowTitle(_("Plot2D Properties"));

  // layout
  QGroupBox *generalPropBox = new QGroupBox( "General Properties" );
  QGridLayout *generalPropBoxLayout = new QGridLayout( generalPropBox );

  // create widgets
  d_general.positionBox = new QComboBox();
  d_general.positionBox->addItem( "Left", QwtPlot::LeftLegend );
  d_general.positionBox->addItem( "Right", QwtPlot::RightLegend );
  d_general.positionBox->addItem( "Bottom", QwtPlot::BottomLegend );
  d_general.positionBox->addItem( "Top", QwtPlot::TopLegend );

  int row = 0;
  generalPropBoxLayout->addWidget( new QLabel( _("Legend Position") ), row, 0 );
  generalPropBoxLayout->addWidget( d_general.positionBox, row, 1 );

  // Major Ticks
  row++;
  generalPropBoxLayout->addWidget( new QLabel( _("Max. Major Ticks (xAxis)") ), row, 0 );
  d_general.majorTicks = new ArrowKeyLineEdit(this, new RealConverter(2, 0, 0, true, true) );
  d_general.majorTicks->setValidator( new QIntValidator( 0, 50, d_general.majorTicks ) );
  d_general.majorTicks->attachListener( new UpdateAdapter( this ) );
  generalPropBoxLayout->addWidget( d_general.majorTicks, row, 1 );

  // Minor Ticks
  row++;
  generalPropBoxLayout->addWidget( new QLabel( _("Max. Minor Ticks (xAxis)") ), row, 0 );
  d_general.minorTicks = new ArrowKeyLineEdit(this, new RealConverter(2, 0, 0, true, true) );
  d_general.minorTicks->setValidator( new QIntValidator( 0, 50, d_general.minorTicks ) );
  d_general.minorTicks->attachListener( new UpdateAdapter( this ) );
  generalPropBoxLayout->addWidget( d_general.minorTicks, row, 1 );

  // Annotation Angle
  row++;
  generalPropBoxLayout->addWidget( new QLabel( _("Annotation Angle (xAxis)") ), row, 0 );
  d_general.annoAngle = new ArrowKeyLineEdit(this, new RealConverter(4, 0, 0, true, true) );
  d_general.annoAngle->setValidator( new QIntValidator( -90, 90, d_general.annoAngle ) );
  d_general.annoAngle->attachListener( new UpdateAdapter( this ) );
  generalPropBoxLayout->addWidget( d_general.annoAngle, row, 1 );

  // BarChart GroupBox
  QGroupBox *barChartBox = new QGroupBox( _("BarChart") );
  QGridLayout *barChartBoxLayout = new QGridLayout( barChartBox );

  // Orientation
  row = 0;
  d_barChart.orientationBox = new QComboBox();
  d_barChart.orientationBox->addItem( _("Horizontal"), Qt::Horizontal );
  d_barChart.orientationBox->addItem( _("Vertical"), Qt::Vertical);
// #if _DEBUG
  barChartBoxLayout->addWidget( new QLabel( "Orientation" ), row, 0 );
  barChartBoxLayout->addWidget( d_barChart.orientationBox, row, 1 );
  row++;
// #endif

  // Layout Policy
  d_barChart.layoutPolicyBox = new QComboBox();
  d_barChart.layoutPolicyBox->addItem( _("AutoAdjustSamples"), QwtPlotBarChart::AutoAdjustSamples );
  d_barChart.layoutPolicyBox->addItem( _("ScaleSamplesToAxes"), QwtPlotBarChart::ScaleSamplesToAxes);
  d_barChart.layoutPolicyBox->addItem( _("ScaleSampleToCanvas"), QwtPlotBarChart::ScaleSampleToCanvas);
  d_barChart.layoutPolicyBox->addItem( _("FixedSampleSize"), QwtPlotBarChart::FixedSampleSize);
  barChartBoxLayout->addWidget( new QLabel( "Layout Policy" ), row, 0 );
  barChartBoxLayout->addWidget( d_barChart.layoutPolicyBox, row, 1 );

  // Layout Hint
  row++;
  barChartBoxLayout->addWidget( new QLabel( "Layout Hint" ), row, 0 );
  d_barChart.layoutHintEdit = new ArrowKeyLineEdit(this, new RealConverter(12,2, 0, true, true) );
  d_barChart.layoutHintEdit->setValidator( new QDoubleValidator( 0, 1000, 2, d_barChart.layoutHintEdit ) );
  d_barChart.layoutHintEdit->attachListener( new UpdateAdapter( this ) );
  barChartBoxLayout->addWidget( d_barChart.layoutHintEdit, row, 1 );

  // DataTip GroupBox
  row++;
  QGroupBox *barChartDataTipBox = new QGroupBox( "DataTip" );
  QGridLayout *barChartDataTipBoxLayout = new QGridLayout( barChartDataTipBox );
  barChartBoxLayout->addWidget( barChartDataTipBox, row, 0, 1, -1 );

  // DataTip Alignment
  row = 0;
  d_barChart.dataTipAlignmentBox = new QComboBox();
  d_barChart.dataTipAlignmentBox->addItem( _("Invisible"), (int)  0 );
  d_barChart.dataTipAlignmentBox->addItem( _("Top"),    (int) Qt::AlignCenter|Qt::AlignTop );
  d_barChart.dataTipAlignmentBox->addItem( _("Center"), (int) Qt::AlignCenter|Qt::AlignVCenter );
  d_barChart.dataTipAlignmentBox->addItem( _("Bottom"), (int) Qt::AlignCenter|Qt::AlignBottom );
  d_barChart.dataTipAlignmentBox->addItem( _("TopRight"),    (int) Qt::AlignRight|Qt::AlignTop );
  d_barChart.dataTipAlignmentBox->addItem( _("CenterRight"), (int) Qt::AlignRight|Qt::AlignVCenter );
  d_barChart.dataTipAlignmentBox->addItem( _("BottomRight"), (int) Qt::AlignRight|Qt::AlignBottom );
  d_barChart.dataTipAlignmentBox->addItem( _("TopLeft"),    (int) Qt::AlignLeft|Qt::AlignTop );
  d_barChart.dataTipAlignmentBox->addItem( _("CenterLeft"), (int) Qt::AlignLeft|Qt::AlignVCenter );
  d_barChart.dataTipAlignmentBox->addItem( _("BottomLeft"), (int) Qt::AlignLeft|Qt::AlignBottom );
  barChartDataTipBoxLayout->addWidget( new QLabel( _("Alignment") ), row, 0 );
  barChartDataTipBoxLayout->addWidget( d_barChart.dataTipAlignmentBox, row, 1 );

  // DataTip Line Style
  row++;
  d_barChart.dataTipLineStyleBox = new QComboBox();
  d_barChart.dataTipLineStyleBox->addItem( _("Point"), QwtPlotMarker::NoLine);
  d_barChart.dataTipLineStyleBox->addItem( _("Horizontal Line"), QwtPlotMarker::HLine);
  d_barChart.dataTipLineStyleBox->addItem( _("Vertical Line"), QwtPlotMarker::VLine);
  d_barChart.dataTipLineStyleBox->addItem( _("Cross Line"), QwtPlotMarker::Cross);
  barChartDataTipBoxLayout->addWidget( new QLabel( _("Line Style") ), row, 0 );
  barChartDataTipBoxLayout->addWidget( d_barChart.dataTipLineStyleBox, row, 1 );

  // DataTip Background Color
  row++;
  d_barChart.dataTipBackgroundColorBox = new QComboBox();
  d_barChart.dataTipBackgroundColorBox->addItem( _("Transparent"), "transparent");
  d_barChart.dataTipBackgroundColorBox->addItem( _("White"), "white");
  d_barChart.dataTipBackgroundColorBox->addItem( _("Blue"), QColor("blue").lighter(180).name());
  d_barChart.dataTipBackgroundColorBox->addItem( _("Cyan"), QColor("cyan").lighter(180).name());
  d_barChart.dataTipBackgroundColorBox->addItem( _("Green"), QColor("green").lighter(180).name());
  d_barChart.dataTipBackgroundColorBox->addItem( _("Yellow"), QColor("yellow").lighter(180).name() );
  d_barChart.dataTipBackgroundColorBox->addItem( _("Magenta"), QColor("magenta").lighter(180).name() );
  d_barChart.dataTipBackgroundColorBox->addItem( _("Red"), QColor("red").lighter(180).name() );
  barChartDataTipBoxLayout->addWidget( new QLabel( _("Background Color") ), row, 0 );
  barChartDataTipBoxLayout->addWidget( d_barChart.dataTipBackgroundColorBox, row, 1 );

  // DataTip Border Color
  row++;
  d_barChart.dataTipBorderColorBox = new QComboBox();
  d_barChart.dataTipBorderColorBox->addItem( _("Transparent"), "transparent");
  d_barChart.dataTipBorderColorBox->addItem( _("Black"), "black");
  d_barChart.dataTipBorderColorBox->addItem( _("White"), "white");
  d_barChart.dataTipBorderColorBox->addItem( _("Blue"), "blue");
  d_barChart.dataTipBorderColorBox->addItem( _("Red"), "red");
  barChartDataTipBoxLayout->addWidget( new QLabel( _("Border Color") ), row, 0 );
  barChartDataTipBoxLayout->addWidget( d_barChart.dataTipBorderColorBox, row, 1 );

  // DataTip Minimum Threshold
  row++;
  d_barChart.dataTipMinThresholdEdit = new ArrowKeyLineEdit(this, new RealConverter(12,1, 0, true, true) );
  d_barChart.dataTipMinThresholdEdit->setValidator( new QDoubleValidator( 0, 100, 1, d_barChart.dataTipMinThresholdEdit ) );
  d_barChart.dataTipMinThresholdEdit->attachListener( new UpdateAdapter( this ) );
  barChartDataTipBoxLayout->addWidget( new QLabel( "Minimum Threshold [%]" ), row, 0 );
  barChartDataTipBoxLayout->addWidget( d_barChart.dataTipMinThresholdEdit, row, 1 );

  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->addWidget( generalPropBox );
  layout->addWidget( barChartBox );
  layout->addStretch( 10 );

  connect( d_general.positionBox,
	   SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
  connect( d_general.majorTicks,
	   SIGNAL( textEdited( const QString & ) ), SIGNAL( edited() ) );
  connect( d_general.minorTicks,
	   SIGNAL( textEdited( const QString & ) ), SIGNAL( edited() ) );
  connect( d_general.annoAngle,
	   SIGNAL( textEdited( const QString & ) ), SIGNAL( edited() ) );

  connect( d_barChart.orientationBox,
	   SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
  connect( d_barChart.layoutPolicyBox,
	   SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
  connect( d_barChart.layoutHintEdit,
	   SIGNAL( textEdited( const QString & ) ), SIGNAL( edited() ) );
  connect( d_barChart.dataTipAlignmentBox,
	   SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
  connect( d_barChart.dataTipLineStyleBox,
	   SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
  connect( d_barChart.dataTipBackgroundColorBox,
	   SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
  connect( d_barChart.dataTipBorderColorBox,
	   SIGNAL( currentIndexChanged( int ) ), SIGNAL( edited() ) );
  connect( d_barChart.dataTipMinThresholdEdit,
	   SIGNAL( textEdited( const QString & ) ), SIGNAL( edited() ) );
}

//---------------------------------------------------
// setSettings
//---------------------------------------------------
void GuiQwtPropertyDialog::setSettings( const QwtPlot *qwtPlot)
{
  blockSignals( true );
  d_general.positionBox->setCurrentIndex( qwtPlot->plotLayout()->legendPosition() );
  d_general.majorTicks->setText( QString::number( qwtPlot->axisMaxMajor(QwtPlot::xBottom) ) );
  d_general.minorTicks->setText( QString::number( qwtPlot->axisMaxMinor(QwtPlot::xBottom) ) );
  d_general.annoAngle->setText( QString::number( qwtPlot->axisScaleDraw(QwtPlot::xBottom)->labelRotation()) );

  QwtPlotItemList barChartList = qwtPlot->itemList( QwtPlotItem::Rtti_PlotMultiBarChart );
  if (barChartList.count()) {
    GuiQwtPlotBarChart* barBarChart = dynamic_cast<GuiQwtPlotBarChart*>( barChartList[0] );
    if (barBarChart) {
      d_barChart.orientationBox->setCurrentIndex( barBarChart->orientation() ==  Qt::Vertical ? 1 : 0);
      d_barChart.layoutPolicyBox->setCurrentIndex( barBarChart->layoutPolicy() );
      d_barChart.layoutHintEdit->setText( QString::number( barBarChart->layoutHint() ) );
      int index = d_barChart.dataTipAlignmentBox->findData( barBarChart->getBarChartSettings().dataTipAlignment );
      d_barChart.dataTipAlignmentBox->setCurrentIndex( index );

      index = d_barChart.dataTipLineStyleBox->findData( barBarChart->getBarChartSettings().dataTipLineStyle );
      d_barChart.dataTipLineStyleBox->setCurrentIndex( index );

      index = d_barChart.dataTipBackgroundColorBox->findData( QString::fromStdString(barBarChart->getBarChartSettings().dataTipBackgroundColor) );
      d_barChart.dataTipBackgroundColorBox->setCurrentIndex( index );

      index = d_barChart.dataTipBorderColorBox->findData( QString::fromStdString(barBarChart->getBarChartSettings().dataTipBorderColor) );
      d_barChart.dataTipBorderColorBox->setCurrentIndex( index );

      d_barChart.dataTipMinThresholdEdit->setText( QString::number( barBarChart->getBarChartSettings().dataTipMinThreshold ) );
    }
  } else {
    d_barChart.orientationBox->setCurrentIndex( 0 );
    d_barChart.layoutPolicyBox->setCurrentIndex( -1 );
    d_barChart.layoutHintEdit->setText( "1" );
    d_barChart.dataTipAlignmentBox->setCurrentIndex( 0 );
    d_barChart.dataTipLineStyleBox->setCurrentIndex( 0 );
    d_barChart.dataTipBackgroundColorBox->setCurrentIndex( 0 );
    d_barChart.dataTipBorderColorBox->setCurrentIndex( 0 );
    d_barChart.dataTipMinThresholdEdit->setText( "0" );
  }

  blockSignals( false );
}

//---------------------------------------------------
// valueChanged
//---------------------------------------------------
void GuiQwtPropertyDialog::valueChanged() {
  emit edited();
}

//---------------------------------------------------
// settings
//---------------------------------------------------
GuiQwtPropertyDialog::Settings GuiQwtPropertyDialog::settings() const
{
  Settings s;
  s.general.legend_position = d_general.positionBox->currentIndex();
  s.general.major_ticks = d_general.majorTicks->text().toInt();
  s.general.minor_ticks = d_general.minorTicks->text().toInt();
  s.general.annotation_angle = d_general.annoAngle->text().toInt();
  s.barChart.orientation = d_barChart.orientationBox->currentIndex() + 1;
  s.barChart.layoutPolicy = d_barChart.layoutPolicyBox->currentIndex();
  s.barChart.layoutHint = d_barChart.layoutHintEdit->text().toDouble();
  s.barChart.dataTipAlignment = d_barChart.dataTipAlignmentBox->currentIndex();
  s.barChart.dataTipLineStyle = d_barChart.dataTipLineStyleBox->currentIndex();
  int dataTipBorderColor = d_barChart.dataTipBorderColorBox->currentIndex();
  int dataTipBackgroundColor = d_barChart.dataTipBackgroundColorBox->currentIndex();
  s.barChart.dataTipMinThreshold = d_barChart.dataTipMinThresholdEdit->text().toDouble();

  // not first select, we are interested in userdata
  if (s.barChart.dataTipAlignment) {
    s.barChart.dataTipAlignment =  d_barChart.dataTipAlignmentBox->itemData( d_barChart.dataTipAlignmentBox->currentIndex() ).toInt();
  }
  if (s.barChart.dataTipLineStyle) {
    s.barChart.dataTipLineStyle =  d_barChart.dataTipLineStyleBox->itemData( d_barChart.dataTipLineStyleBox->currentIndex() ).toInt();
  }
  if (dataTipBorderColor) {
    s.barChart.dataTipBorderColor =  d_barChart.dataTipBorderColorBox->itemData( dataTipBorderColor ).toString().toStdString();
  }
  if (dataTipBackgroundColor) {
    s.barChart.dataTipBackgroundColor =  d_barChart.dataTipBackgroundColorBox->itemData( dataTipBackgroundColor ).toString().toStdString();
  }
  return s;
}
