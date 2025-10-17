
#ifndef GUI_QWT_PROPERTY_PANEL_H
#define GUI_QWT_PROPERTY_PANEL_H

#include <qdialog.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_barchart.h>

class QComboBox;
class QSpinBox;
class QLineEdit;
class QwtPlot;
class ArrowKeyLineEdit;

class GuiQwtPropertyDialog: public QDialog
{
  Q_OBJECT

 public:
  GuiQwtPropertyDialog( QWidget *parent = NULL );

  class Settings {
  public:
    Settings() {
      general.legend_position = 0;
      general.annotation_angle = 0;
    }

    struct {
      int legend_position;
      int major_ticks;
      int minor_ticks;
      int annotation_angle;
    } general;

    struct BarChart {
    BarChart() :
      orientation( Qt::Vertical ), // Qt::Vertical
	layoutPolicy( QwtPlotBarChart::FixedSampleSize ),
	layoutHint( 11 ),
	dataTipAlignment( 0 ),  // Invisible
	dataTipLineStyle(  QwtPlotMarker::NoLine ),
	dataTipBackgroundColor( "transparent" ),
	dataTipBorderColor( "transparent" ),
	dataTipMinThreshold(12) // %
      {}

    BarChart(const BarChart& bc) :
        orientation( bc.orientation ),
	layoutPolicy( bc.layoutPolicy ),
	layoutHint( bc.layoutHint ),
	dataTipAlignment( bc.dataTipAlignment ),
	dataTipLineStyle( bc.dataTipLineStyle ),
	dataTipBackgroundColor( bc.dataTipBackgroundColor ),
	dataTipBorderColor( bc.dataTipBorderColor ),
	dataTipMinThreshold( bc.dataTipMinThreshold ) {}
      int orientation;
      int layoutPolicy;
      double layoutHint;
      int dataTipAlignment;
      int dataTipLineStyle;
      std::string dataTipBackgroundColor;
      std::string dataTipBorderColor;
      double dataTipMinThreshold;
    } barChart;
  };

  void setSettings( const QwtPlot *qwtPlot );
  Settings settings() const;

  void valueChanged();

 Q_SIGNALS:
  void edited();

 private:
    struct {
      QComboBox *positionBox;
      ArrowKeyLineEdit *majorTicks;
      ArrowKeyLineEdit *minorTicks;
      ArrowKeyLineEdit *annoAngle;
    } d_general;

    struct {
      QComboBox *orientationBox;
      QComboBox *layoutPolicyBox;
      ArrowKeyLineEdit *layoutHintEdit;
      QComboBox *dataTipAlignmentBox;
      QComboBox *dataTipLineStyleBox;
      QComboBox *dataTipBackgroundColorBox;
      QComboBox *dataTipBorderColorBox;
      ArrowKeyLineEdit *dataTipMinThresholdEdit;
    } d_barChart;
};

#endif
