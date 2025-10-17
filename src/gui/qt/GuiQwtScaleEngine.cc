
#include <QDate>

#include "gui/qt/GuiQwtScaleDraw.h"
#include "gui/qt/GuiQwtScaleEngine.h"

const int GuiQwtScaleEngine::m_dayThreshold = 90;

//---------------------------------------------------
// Klasse GuiQwtScaleEngine
//---------------------------------------------------
//----------------------------------------------------
// Constructor GuiQwtScaleEngine
//----------------------------------------------------

GuiQwtScaleEngine::GuiQwtScaleEngine()
  : m_bDate(false)
  , m_bAnnotation(false)
{
}

//----------------------------------------------------
// GuiQwtScaleEngine::setAnnotationType
//----------------------------------------------------
void GuiQwtScaleEngine::setAnnotationType(bool bAnno) {
  m_bAnnotation = bAnno;
}

//----------------------------------------------------
// GuiQwtScaleEngine::setAnnotationLabelValues
//----------------------------------------------------
void GuiQwtScaleEngine::setAnnotationLabelValues(std::vector<double> vals) {
  std::vector<double>::iterator it = vals.begin();
  m_annotationValues.clear();
  for(;it != vals.end(); ++it) {
    m_annotationValues.append(*it);
  }
}

//----------------------------------------------------
// GuiQwtScaleEngine::setDateType
//----------------------------------------------------
void GuiQwtScaleEngine::setDateType(bool bDate) {
  m_bDate=bDate;
}

//----------------------------------------------------
// GuiQwtScaleEngine::getFirstMonthDate
//----------------------------------------------------
QDate GuiQwtScaleEngine::getFirstMonthDate(long julian, MonthFirstDate fd) const {
  QDate d = QDate::fromJulianDay(julian);
  if (fd == Near_First) {
    fd = (d.day() > (0.5*d.daysInMonth()) ? Next_First : First);
  }
  if (fd == First)
    d = d.addDays( 1 - d.day() );
  else
    if (fd == Next_First)
      d = d.addDays( 1 + d.daysInMonth() - d.day() );
  return d;
}

//---------------------------------------------------
// Klasse GuiQwtLinearScaleEngine
//---------------------------------------------------

//----------------------------------------------------
// GuiQwtLinearScaleEngine:.autoScale
//----------------------------------------------------
void GuiQwtLinearScaleEngine::autoScale(int maxSteps,
					double &x1, double &x2, double &stepSize) const {
  if (m_bDate && fabs(x2-x1) > m_dayThreshold) {
    int xdiff = x2 - x1;
    QDate d1 = getFirstMonthDate(x1, First);
    QDate d2 = getFirstMonthDate(x2, Next_First);
    x1 = d1.toJulianDay();
    x2 = d2.toJulianDay();
    double xdiff2 = x2 - x1;
    stepSize = (double) xdiff2 / maxSteps;
  } else
    QwtLinearScaleEngine::autoScale(maxSteps,x1, x2, stepSize);
}

//----------------------------------------------------
// GuiQwtLinearScaleEngine::divideScale
//----------------------------------------------------
QwtScaleDiv GuiQwtLinearScaleEngine::divideScale(double x1, double x2,
						 int numMajorSteps, int numMinorSteps,
						 double stepSize) const {
  QwtScaleDiv sd = QwtLinearScaleEngine::divideScale(x1, x2,
						     numMajorSteps, numMinorSteps, stepSize);
  if (m_bAnnotation) {
    sd.setTicks(QwtScaleDiv::MajorTick, m_annotationValues);
  } else
  if (m_bDate && fabs(x2-x1) > m_dayThreshold) {
    QList<double> qlist = sd.ticks(QwtScaleDiv::MajorTick);
    for (int i=0; i < qlist.size(); ++i) {
      MonthFirstDate opt = (i == 0 ? First : (i+1 == qlist.size() ? Next_First : Near_First));
      QDate d = getFirstMonthDate(qlist[i], opt);
      qlist[i] = d.toJulianDay();
    }
    sd.setTicks(QwtScaleDiv::MajorTick, qlist);
  }
  return sd;
}

//---------------------------------------------------
// Klasse GuiQwtLogScaleEngine
//---------------------------------------------------

//----------------------------------------------------
// GuiQwtLogScaleEngine:.autoScale
//----------------------------------------------------
void GuiQwtLogScaleEngine::autoScale(int maxSteps,
					double &x1, double &x2, double &stepSize) const {
  if (m_bDate && fabs(x2-x1) > m_dayThreshold) {
    int xdiff = x2 - x1;
    QDate d1 = getFirstMonthDate(x1, First);
    QDate d2 = getFirstMonthDate(x2, Next_First);
    x1 = d1.toJulianDay();
    x2 = d2.toJulianDay();
    double xdiff2 = x2 - x1;
    stepSize = (double) xdiff2 / maxSteps;
  } else
    QwtLogScaleEngine::autoScale(maxSteps,x1, x2, stepSize);
}

//----------------------------------------------------
// GuiQwtLogScaleEngine::divideScale
//----------------------------------------------------
QwtScaleDiv GuiQwtLogScaleEngine::divideScale(double x1, double x2,
						 int numMajorSteps, int numMinorSteps,
						 double stepSize) const {
  QwtScaleDiv sd = QwtLogScaleEngine::divideScale(x1, x2,
						     numMajorSteps, numMinorSteps, stepSize);
  if (m_bAnnotation) {
    sd.setTicks(QwtScaleDiv::MajorTick, m_annotationValues);
  } else
  if (m_bDate && fabs(x2-x1) > m_dayThreshold) {
    QList<double> qlist = sd.ticks(QwtScaleDiv::MajorTick);
    for (int i=0; i < qlist.size(); ++i) {
      MonthFirstDate opt = (i == 0 ? First : (i+1 == qlist.size() ? Next_First : Near_First));
      QDate d = getFirstMonthDate(qlist[i], opt);
      qlist[i] = d.toJulianDay();
    }
    sd.setTicks(QwtScaleDiv::MajorTick, qlist);
  }
  return sd;
}
