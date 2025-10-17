#ifndef GUIQWTSCALEENGINE_H
#define GUIQWTSCALEENGINE_H

#include <qwt_scale_engine.h>

class RealConverter;

//---------------------------------------------------
// Klasse GuiQwtScaleEngine
//---------------------------------------------------
class GuiQwtScaleEngine  {
public:
  GuiQwtScaleEngine();

  void setDateType(bool bDate);
  void setAnnotationType(bool bAnno);
  void setAnnotationLabelValues( std::vector<double> vals );

private:
  GuiQwtScaleEngine(const GuiQwtScaleDraw&);

protected:
  enum MonthFirstDate {First, Near_First, Next_First};
  QDate getFirstMonthDate(long julian, MonthFirstDate fd) const;

  bool      m_bDate;
  bool      m_bAnnotation;
  QList<double> m_annotationValues;

  /// Threshold number of days above swithing to month scaling
  static  const int m_dayThreshold;
};

//---------------------------------------------------
// Klasse GuiQwtLinearScaleEngine
//---------------------------------------------------
class GuiQwtLinearScaleEngine: public QwtLinearScaleEngine, public GuiQwtScaleEngine {
public:
  GuiQwtLinearScaleEngine() {}

  virtual void autoScale(int maxSteps,
			 double &x1, double &x2, double &stepSize) const;
  virtual QwtScaleDiv divideScale(double x1, double x2,
				  int numMajorSteps, int numMinorSteps,
				  double stepSize = 0.0) const;
private:
  GuiQwtLinearScaleEngine(const GuiQwtScaleDraw&);
};

//---------------------------------------------------
// Klasse GuiQwtLogScaleEngine
//---------------------------------------------------
class GuiQwtLogScaleEngine: public QwtLogScaleEngine, public GuiQwtScaleEngine {
public:
  GuiQwtLogScaleEngine() {}

  virtual void autoScale(int maxSteps,
			 double &x1, double &x2, double &stepSize) const;
  virtual QwtScaleDiv divideScale(double x1, double x2,
				  int numMajorSteps, int numMinorSteps,
				  double stepSize = 0.0) const;
private:
  GuiQwtLogScaleEngine(const GuiQwtScaleDraw&);
};

#endif
