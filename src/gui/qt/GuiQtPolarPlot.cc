
#include <QLayout>
#include <QGraphicsLayout>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QCategoryAxis>

#include "gui/Gui2dPlot.h"
#include "gui/qt/GuiQtPolarPlot.h"
#include "utils/Debugger.h"

INIT_LOGGER();

//----------------------------------------------------
// GuiQtChartView::resizeEvent
//----------------------------------------------------
void GuiQtChartView::resizeEvent(QResizeEvent *event) {
  QChartView::resizeEvent(event);

  QSize s = parentWidget()->size();
  // 2021-03-09 Qt 5.15 unschön, aber es funktioniert
  if (chart()) {
    chart()->layout()->invalidate();
    chart()->layout()->activate();
  }
  if (parentWidget()) {
    parentWidget()->layout()->invalidate();
    parentWidget()->layout()->activate();
    if (parentWidget()->parentWidget()) {
       parentWidget()->parentWidget()->layout()->invalidate();
       parentWidget()->parentWidget()->layout()->activate();
    }
  }
}

//----------------------------------------------------
// create
//----------------------------------------------------
void GuiQtPolarChart::create(GuiQtChartView* polarPlot) {
  // setParent(m_polarChart);
  polarPlot->setChart(this);
  polarPlot->setRenderHint(QPainter::Antialiasing);

  m_angularAxis = new QCategoryAxis();
  dynamic_cast<QCategoryAxis*> (m_angularAxis)->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
  //  m_angularAxis = new QValueAxis();
  m_angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
  m_angularAxis->setLabelFormat("%.1f");
  for(unsigned int i = 0; i < 360; i += 45)
    if (i <=90)
      dynamic_cast<QCategoryAxis*> (m_angularAxis)->append(QString::number(90-i), i);
    else if (i <= 360)
      dynamic_cast<QCategoryAxis*> (m_angularAxis)->append(QString::number(450-i), i);
  m_angularAxis->setShadesVisible(true);
  m_angularAxis->setReverse(false);
  m_angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
  addAxis(m_angularAxis, QPolarChart::PolarOrientationAngular);
  m_angularAxis->setRange(0, 360);

  m_radialAxis = new QValueAxis();
  m_radialAxis->setTickCount(9);
  m_radialAxis->setLabelFormat("%d");
  m_radialAxis->setRange(0, 12);
  addAxis(m_radialAxis, QPolarChart::PolarOrientationRadial);
}

//----------------------------------------------------
// append
//----------------------------------------------------
void GuiQtPolarChart::append(QList<QPointF>& series, double x, double y) {
  series.append(QPointF(x, y));
}


//----------------------------------------------------
// addZeroPoints
//----------------------------------------------------
void GuiQtPolarChart::addZeroPoints(QList<QPointF>& series) {
  if (series.count() <= 3) return;
  QPointF first(series.at(0));
  QPointF last(series.at(series.count()-1));
  double angle = fmod(last.x() - first.x(), 360.);
  double ydiff = last.y() - first.y();
  double angleDiff = angle > 250. ? (360 - angle) : 0;
  double y = first.y() + ydiff*first.x()/angleDiff;
  BUG_DEBUG("First[" << first.x() << ", " << first.y() << "] Last["
            << last.x() << ", " << last.y() << "] angle[" << (angleDiff) << "] ydiff["
            << ydiff << "] ==> Zero y: " << y);
  if (angle == 0) return;  // angle zu gross, return
  series.insert(0, QPointF(0.0, y));
  series.append(QPointF(360, y));
}

//----------------------------------------------------
// showCurve
//----------------------------------------------------
bool GuiQtPolarChart::pointLessX(const QPointF &v1, const QPointF &v2) {
  return v1.x() < v2.x();
}

//----------------------------------------------------
// showCurve
//----------------------------------------------------
QLineSeries* GuiQtPolarChart::addSeries(const std::vector<Gui2dPlot::Point>& pts,
                                        std::string& label,
                                        GuiQtChartView* chartView,
                                        QPen& pen) {
  QLineSeries *series = new QLineSeries();
  QList<QPointF> ptData;

  series->setPen(pen);
  for (Gui2dPlot::tPointVector::const_iterator pit=pts.begin();
       pit!=pts.end(); ++pit) {
    if ((*pit).x <=90) {
      append(ptData,  90.0 - (*pit).x, (*pit).y);
    } else {
      append(ptData,  450.0  -(*pit).x, (*pit).y);
    }
  }
#if QT_VERSION > 0x050600
  std::sort(ptData.begin(), ptData.end(), pointLessX);
#else
  qSort(ptData.begin(), ptData.end(), pointLessX);
#endif
  addZeroPoints(ptData);
  series->append(ptData);

  series->setName( QString::fromStdString(label) );
  QPolarChart::addSeries(series);
  series->attachAxis(m_radialAxis);
  series->attachAxis(m_angularAxis);
  chartView->setChart(this);
  return series;
}
