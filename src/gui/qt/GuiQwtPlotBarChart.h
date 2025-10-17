
#ifndef GUI_QWT_PLOT_BARCHART_H
#define GUI_QWT_PLOT_BARCHART_H

#include <qwt_plot_multi_barchart.h>
#include "gui/qt/GuiQwtPropertyDialog.h"

class QwtPlotMarker;

#if QWT_VERSION >= 0x060100
class GuiQwtPlotBarChart: public QwtPlotMultiBarChart
{
  public:
  /**
     Konstruktor
   */
  GuiQwtPlotBarChart( GuiQwtPropertyDialog::Settings::BarChart settings, bool plotItemGrouped );

  virtual ~GuiQwtPlotBarChart();

  /** override setSamples Methode
      hier werden die DataTip Marker erstellt
   */
  void setSamples (QwtSeriesData< QPointF > *series);
  void clearSamples();

  /** closetPoint Methode gibt es leider hier nicht,
      deshalb müssen wir sie selber machen */
  int closestPoint( const QPoint &pos, double *dist ) const;

  /** set bar char settings */
  void setBarChartSettings(const GuiQwtPropertyDialog::Settings::BarChart& settings);

  /** get bar char settings */
  const GuiQwtPropertyDialog::Settings::BarChart getBarChartSettings() { return m_settings; }

  /** setzen der Kurvenfarbe */
  void setColor(const QColor &color);

  /** setzen des Kurventitel */
  void setTitle(const QString& title);

  /** Annotation label hinzufügen */
  void addAnnoLabel(const std::string& label);

  /** Annotation label */
  std::string getAnnoLabel(int index);

  /** grouped option */
  bool isBarStyleOptionPlotItemGrouped() { return m_plotItemGrouped; }

  /** add Scale Factors */
  void addScaleFactor( double xScaleFactor, double yScaleFactor );

  /** get xScale Factor */
  double getXScaleFactor(int index);
  /** get yScale Factor */
  double getYScaleFactor(int index);
private:
  void createMarker();

private:
  GuiQwtPropertyDialog::Settings::BarChart m_settings;
  QList<QColor>                d_colors;
  std::vector<QwtPlotMarker*>  m_markers;
  std::vector<std::string>     m_annoLabels;
  bool                         m_plotItemGrouped;
  std::vector<double>          m_xScaleFactor;
  std::vector<double>          m_yScaleFactor;
};

#else
typedef QwtPlotItem GuiQwtPlotBarChart;
#endif

#endif
