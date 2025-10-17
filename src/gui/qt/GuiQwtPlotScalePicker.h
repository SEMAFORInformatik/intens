
#ifndef GUI_QWT_PLOT_SCALE_PICKER_H_
#define GUI_QWT_PLOT_SCALE_PICKER_H_

#include <QObject>
#include <QRect>
#include <QLineEdit>
#include <qwt_color_map.h>

class QwtPlot;
class QPoint;
class QwtScaleWidget;

/**
   * GuiQwtPlotScalePicker class
   * picker to observe all events on qwt scales
   */
  class GuiQwtPlotScalePicker: public QObject {
    Q_OBJECT
      public:
    /**
     * Constructor of GuiQwtPlotScalePicker
     * @param plot QwtPlot to observe all events
     */
    GuiQwtPlotScalePicker(QwtPlot *plot);

    /** get bounding box of selection region  (make sense for Rect type)
     * @return pointer to observed QwtPlot
     */
    QwtPlot *plot() { return (QwtPlot *)parent(); }

  signals:
    /** Emitted when the user right-clicks on an axis (but not its title).
        The argument specifies the axis' QwtScaleDraw::Alignment.
    */
    void axisRightClicked(int);
    /** Emitted when the user right-clicks on the title of an axis.
        The argument specifies the axis' QwtScaleDraw::Alignment.
    */
    void axisTitleRightClicked(int);

    /** Emitted when the user double-clicks on an axis (but not its title).
        The argument specifies the axis' QwtScaleDraw::Alignment.
    */
    void axisDblClicked(int);

    /** Emitted when the user double-clicks on an axis title.
        The argument specifies the axis' QwtScaleDraw::Alignment.
    */
    void axisTitleDblClicked(int);

    /** Emitted when the user double-clicks on an major tick of an axis.
        The first argument specifies the pointer to axis scale widget.
        The second argument specifies the value of the picked major tick.
        The third argument specifies the boundingbox of the picked major tick.
    */
    void axisMajorTickDblClicked(QwtScaleWidget*, double, QRect);

    /** Emitted when the user drap & drop a color of a colorstops
    */
    void colorStopPositionChanged(QwtScaleWidget*, double newValue, int tickIdx);

    /** Emitted when the user changes the color of a colorstops
    */
    void colorStopColorChanged(QwtScaleWidget*, QColor newColor, int tickIdx);

    /** Emitted when the user inserts a new color to the colorstops
    */
    void colorStopColorNew(QwtScaleWidget*, QColor newColor, double value);

    /** Emitted when the user deletes the color of a colorstops
    */
    void colorStopColorDelete(QwtScaleWidget*, int tickIdx);

  private:
    void refresh();
    bool eventFilter(QObject *, QEvent *);
    QRect scaleRect(const QwtScaleWidget *scale) const;

    void mouseLeftClicked(QwtScaleWidget *scale, const QPoint &pos);
    void dragMove(QwtScaleWidget *scale, QDragMoveEvent* dme);
    void drop(QwtScaleWidget *scale, const QDropEvent* dropEvent);
    void mouseMiddleClicked(QwtScaleWidget *scale, const QPoint &pos);
    void mouseRightClicked(QwtScaleWidget *scale, const QPoint &pos);
    void mouseDblClicked(QwtScaleWidget *scale, const QPoint &pos);

    QRect getTickLabelBoundingRect(const QwtScaleWidget *scale, int tickIdx);
  };

  /**
   * QwtPlotScaleLineEdit class
   * picker to observe all events on qwt scales
   */
  //
  class QwtPlotScaleLineEdit : public QLineEdit {
    Q_OBJECT
  public:
    QwtPlotScaleLineEdit(QwtPlot* plot, QwtScaleWidget* scale, QRect boundingRect, double value, bool typeColorMap=false);
    virtual ~QwtPlotScaleLineEdit();

  signals:
    /** Emitted when the user finished editing
    */
    void finished(QwtScaleWidget*, double newValue);
    void resetScale(QwtScaleWidget*);
  private slots:
      void slot_editingFinished();
  private:
      QwtPlot* plot;
      QwtScaleWidget* scaleWidget;
      double oldValue;
      bool   typeColorMap;
    };

#endif // !defined(SCANIMAGEPLOT_H__INCLUDED_)
