
#ifndef GUIQWTCONTOURPLOT_H_
#define GUIQWTCONTOURPLOT_H_

#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#if QWT_VERSION >= 0x060000
#include <qwt_matrix_raster_data.h>
#endif

class GuiQwtContourPlotData;
class QwtLinearColorMap;
class QwtPlotSpectrogram;
class QwtRasterData;
class QwtPlotCanvas;
class GuiQt3dPlot;

  class GuiQwtContourPlot : public QwtPlot
  {
  public:
    GuiQwtContourPlot(GuiQt3dPlot* plot, QwtLinearColorMap* colormap,
		      GuiQwtContourPlotData* data);

    /** update data */
    void update(QwtRasterData& data);
    /** show Contour levels */
    void showContour(bool on);
#if QWT_VERSION >= 0x060000
    /** set resample mode */
    void setResampleMode(QwtMatrixRasterData::ResampleMode mode);
#endif
    /** reset zoom */
    void resetScale();
    /**  print to paint device (eg. Printer) */
    void print(QPaintDevice& pd);

  private:
    /** set new data */
    void setData(GuiQwtContourPlotData* data);
#if QWT_VERSION >= 0x060000
    /** set new colormap */
    void setColorMap(const QwtInterval& interval, QwtLinearColorMap* colormap);
#endif
    /** update z axis (colormap) */
    void updateZAxis(GuiQwtContourPlotData* data);
    /** set zoom base  */
    void zoomBase();
    /** zoom out */
    void zoomOut(double factor=2.);
    /** contextMenuEvent */
    void contextMenuEvent ( QContextMenuEvent* event );


#ifndef QT_NO_PRINTER
    void printPlot();
#endif

    Q_OBJECT
      private Q_SLOTS:
    void slot_zoomerRect(const QRectF &rect);

    void slot_axisMajorTickDblClicked(QwtScaleWidget* scale, double value, QRect boundingRect);
    void slot_colorStopPositionChanged(QwtScaleWidget* scale, double newValue, int tickIdx);
    void slot_colorStopColorChanged(QwtScaleWidget* scale, QColor newColor, int tickIdx);
    void slot_colorStopColorNew(QwtScaleWidget* scale, QColor color, double value);
    void slot_colorStopColorDelete(QwtScaleWidget* scale, int tickIdx);

    void slot_EditingFinished(QwtScaleWidget* scaleWidget, double newValue);
    void slot_EditingResetScale(QwtScaleWidget*);

  private:
    void showSpectrogram(bool on);
    void setContourLevels();
    Qt::Axis getQtAxisId(int i);
    QwtLinearColorMap*  getColorMapClone(const QwtLinearColorMap* colormap);
    void setNewColorStops(QwtScaleWidget* scale,
			  QwtLinearColorMap *colorMap);
    void setNewColorStops(QwtScaleWidget* scale,
			  const QwtLinearColorMap *colorMap,
			  const QVector< double >& colorsNew,
			  const QVector< double >& colorsOld);

    GuiQwtContourPlot();
    GuiQwtContourPlot(const GuiQwtContourPlot& );

    //
    // private MyZoomer class
    class MyZoomer: public QwtPlotZoomer {
    public:
      MyZoomer(GuiQwtContourPlot* p, QWidget *canvas);
      virtual QwtText trackerTextF(const QPointF &pos) const;
   private:
      GuiQwtContourPlot *plot;
    };

  private:
    QwtPlotSpectrogram *d_spectrogram;
    int                 currentEditedAxisLabelId;

    QwtPlotZoomer      *zoomer;
    GuiQwtContourPlotData *m_data;
    GuiQt3dPlot       *m_plot;
  };

#endif // !defined(GUIQWTCONTOURPLOT_H__INCLUDED_)
