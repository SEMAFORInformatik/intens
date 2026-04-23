#if !defined(GUI_QT_POLARPLOT_INCLUDED_H)
#if HAVE_QCHARTS
#define GUI_QT_POLARPLOT_INCLUDED_H

#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
QT_USE_NAMESPACE

class GuiQtChartView : public QChartView {
 public:
 GuiQtChartView()
   : QChartView() {}
 GuiQtChartView(QPolarChart* chart)
   : QChartView(chart) {}

 private:
  void resizeEvent(QResizeEvent *event);
};

class GuiQtPolarChart: public QPolarChart {
 public:
  GuiQtPolarChart() {}
  void create(GuiQtChartView*);
  QLineSeries* addSeries(const std::vector<Gui2dPlot::Point>& pts,
                         std::string& label,
                         GuiQtChartView* chartView,
                         QPen& pen);
private:
  void append(QList<QPointF>& series, double x, double y);
  void addZeroPoints(QList<QPointF>& series);
  static bool pointLessX(const QPointF &v1, const QPointF &v2);

private:
  QValueAxis*  m_angularAxis;
  QValueAxis*  m_radialAxis;
  GuiQtChartView* m_chartView;
};

#endif
#endif
