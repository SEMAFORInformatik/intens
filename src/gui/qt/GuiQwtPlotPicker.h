
#ifndef GUI_QWT_PLOT_PICKER_H
#define GUI_QWT_PLOT_PICKER_H

#include <qwt_plot_picker.h>
#include "gui/Gui2dPlot.h"

class GuiQWTPlot;
class QwtPlotCurve;
class QwtText;

//---------------------------------------------------
// Klasse GuiQwtPlotPicker
//---------------------------------------------------
class GuiQwtPlotPicker : public QwtPlotPicker{
public:
  GuiQwtPlotPicker( int xAxis, int yAxis
		   , int selectionFlags
		   , RubberBand rubberBand
		   , DisplayMode trackerMode
		   , QwtPlot *qwtplot
                   , GuiQWTPlot* plot );
  virtual ~GuiQwtPlotPicker(){}
  QwtText trackerText(const QPoint &pos)  const;

  Q_OBJECT
signals:
  void selectedPoint(const QwtPlotCurve*, int, const QPointF&, const QPointF&);

public:
  /** clear all points of the curve (e.g GuiUpdate) */
  bool clearSelection(QwtPlotCurve* curve);

  /**  get selected points
       (only coordinates, no dependencies to curve)
   */
  void getSelectionPoints(Gui2dPlot::tPointVector& pts,
			  std::vector<int>& axisType, std::vector<std::string>& axisTitle);
  /** refresh drawing */
  void refresh();

private:
  class SelectedCurvePoints {
  public:
    struct CurvePoint {
      QwtPlotCurve* curve;
      QwtText* curveLabel; // after a GuiUpdate curve was deleted, but label should mostly be the same
      int pointIdx;
      double xpos, ypos;
      CurvePoint(QwtPlotCurve* cur, int idx);
    };
    typedef std::vector<CurvePoint*>  CurvePoints;
    typedef std::vector<CurvePoint*>::iterator CurvePointIterator;

    CurvePoints&  data() { return m_curvePoints; }
    /** last select point */
    CurvePoint* back()   {
      if (m_curvePoints.size())
	return m_curvePoints.back();
      else
	return NULL;
    }
  private:
    std::vector<CurvePoint*> m_curvePoints;

  };

  bool eventFilter( QObject *object, QEvent *event );
  void showSelectedPoints();
  void hideSelectedPoint(QwtPlotCurve* curve, int index);
  void select( const QPoint &pos, bool unselect = false );
  void shiftPointCursor( bool up );
  /** append (or remove if exsists) a point */
  bool append(QwtPlotCurve* curve, int pointIdx, bool unselect = false );
  /** make intens happy (after an GuiUpdate)
      for the old previously deleted QwtPlotCurve* SelectedCurvePoints::CurvePoint::curve
      the new QwtPlotCurve* will be found
   */
  QwtPlotCurve* getCurve( const SelectedCurvePoints::CurvePoint* curvePoint);
  void timerEvent ( QTimerEvent * event );

private:
  std::string m_nextText;
  QwtPlot* m_qwtPlot;
  GuiQWTPlot* m_plot;

  SelectedCurvePoints m_selectedCurvePoints;
  /** mouse picking pixel tolerance */
  const static int PIXEL_TOLERANCE;
};

#endif
