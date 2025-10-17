
#if !defined(GUI_QWT_PLOT_LAYOUT_INCLUDED_H)
#define GUI_QWT_PLOT_LAYOUT_INCLUDED_H
#include <qwt_plot_layout.h>

class GuiQWTPlot;

class GuiQwtPlotLayout: public QwtPlotLayout
{

public:
    GuiQwtPlotLayout(GuiQWTPlot& plot);
    virtual ~GuiQwtPlotLayout() {}

    virtual QSize minimumSizeHint( const QwtPlot * ) const;

    virtual void activate( const QwtPlot *,
                           const QRectF &rect, Options options = Options() );

    virtual void invalidate();

    /* set canvas size */
    void setCanvasSize(int w, int h);

 private:
    GuiQWTPlot& m_plot;
    int         m_canvasWidth;
    int         m_canvasHeight;
};

#endif
