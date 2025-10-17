
#ifndef GUI_QWT_PLOT_ZOOMER_H
#define GUI_QWT_PLOT_ZOOMER_H

#include <qwt_plot_zoomer.h>
#include <qwt_plot_curve.h>
#if QWT_VERSION < 0x060100
#include <qwt_interval.h>
#include <qwt_point_3d.h>
#include <qwt_compat.h>
#endif

class GuiQWTPlot;
//---------------------------------------------------
// Klasse GuiQwtPlotZoomer
//---------------------------------------------------
#include <qwt_plot_zoomer.h>

class GuiQwtPlotZoomer : public QwtPlotZoomer{
public:
  GuiQwtPlotZoomer( int xAxis, int yAxis
		   , int selectionFlags
		   , DisplayMode cursorLabelMode
		   , QWidget *canvas );
  virtual ~GuiQwtPlotZoomer(){}

  Q_OBJECT
signals:
  void selectedCurve(QwtPlotCurve&);

public:
  virtual void zoom(int up) { QwtPlotZoomer::zoom(up); }
  virtual void begin();
  virtual bool end(bool ok=true);
  QwtText trackerText(const QPoint &pos)  const;
 private:
  void removeMouseButtonPattern(int key);

  void widgetMousePressEvent(QMouseEvent* mouseEvent);
  void select( const QPoint &pos );

  /** mouse picking pixel tolerance */
  const static int PIXEL_TOLERANCE;
};


#endif
