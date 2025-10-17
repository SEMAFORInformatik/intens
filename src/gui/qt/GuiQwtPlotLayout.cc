
#include "utils/Debugger.h"
#include "gui/qt/GuiQwtPlot.h"
#include "gui/qt/GuiQwtPlotLayout.h"

INIT_LOGGER();

//----------------------------------------------------
// GuiQwtPlotLayout Constructor
//----------------------------------------------------
GuiQwtPlotLayout::GuiQwtPlotLayout(GuiQWTPlot& plot):
  QwtPlotLayout()
  , m_plot(plot)
  , m_canvasWidth(-1)
  , m_canvasHeight(-1)
{
}

//----------------------------------------------------
// minimumSizeHint
//----------------------------------------------------
QSize GuiQwtPlotLayout::minimumSizeHint(const QwtPlot* plot) const {
  QSize hs = QwtPlotLayout::minimumSizeHint(plot);
  BUG_DEBUG("GuiQwtPlotLayout::minimumSizeHint " << hs.width() << ", " << hs.height());
  return hs;
}

//----------------------------------------------------
// activate
//----------------------------------------------------
void GuiQwtPlotLayout::activate(const QwtPlot* plot,
                                const QRectF& rect, Options options) {
  QRectF _rect(rect);
  // aspect ratio
  if (m_plot.AspectRatioMode()) {
    QRectF crect(canvasRect());
    QRectF trect(titleRect());
    QRectF frect(footerRect());
    QRectF lrect(legendRect());
    QRectF slrect(scaleRect(QwtPlot::yLeft));
    QRectF srrect(scaleRect(QwtPlot::yRight));
    QRectF sbrect(scaleRect(QwtPlot::xBottom));
    QRectF strect(scaleRect(QwtPlot::xTop));

    BUG_DEBUG("GuiQwtPlotLayout::activate rect pos: "
              << rect.x() << ", " << rect.y() << ", size: " << rect.width() << ", " << rect.height()
              << ", Canvas: " << crect.x() << ", " << crect.y()
              << ", size: " << crect.width() << ", " << crect.height());
    BUG_DEBUG(" =Title= : " << trect.x() << ", " << trect.y()
              << ", size: " << trect.width() << ", " << trect.height());
    BUG_DEBUG(" =Footer= : " << frect.x() << ", " << frect.y()
              << ", size: " << frect.width() << ", " << frect.height());
    BUG_DEBUG(" =Legend= : " << lrect.x() << ", " << lrect.y()
              << ", size: " << lrect.width() << ", " << lrect.height());

    BUG_DEBUG(" =Left= : " << slrect.x() << ", " << slrect.y()
              << ", size: " << slrect.width() << ", " << slrect.height());
    BUG_DEBUG(" =Right= : " << srrect.x() << ", " << srrect.y()
              << ", size: " << srrect.width() << ", " << srrect.height());
    BUG_DEBUG(" =Bottom= : " << sbrect.x() << ", " << sbrect.y()
              << ", size: " << sbrect.width() << ", " << sbrect.height());
    BUG_DEBUG(" =Top= : " << strect.x() << ", " << strect.y()
              << ", size: " << strect.width() << ", " << strect.height());

    double _yAspectRatio(std::max(m_plot.getAxis(0).getAspectRatio(), m_plot.getAxis(1).getAspectRatio()));
    double _xAspectRatio(std::max(m_plot.getAxis(2).getAspectRatio(), m_plot.getAxis(3).getAspectRatio()));

    BUG_DEBUG("AspectRatio y: " << _yAspectRatio << ", x: " << _xAspectRatio);
    if (sbrect.height() || slrect.width() || trect.width() || frect.width()) {
      if ((_rect.width()/_xAspectRatio) < (_rect.height()/_yAspectRatio)) {
        int nh = crect.width()*_yAspectRatio/_xAspectRatio;
        //  yAxis Reference
        _rect.setY(rect.y() + 0.5*(_rect.height()-nh) - strect.height());
        _rect.setHeight(nh + sbrect.height() + strect.height() + trect.height() + frect.height());
        BUG_DEBUG(" = Fix Y, new height: " << _rect.height() << " " << nh);
      } else {
        //  xAxis Reference
        int nw =  crect.height()*_xAspectRatio/_yAspectRatio;
        _rect.setX(rect.x() + 0.5 * (_rect.width() - nw) - srrect.width() - slrect.width());
        _rect.setWidth(nw + slrect.width() + srrect.width());
        BUG_DEBUG(" = Fix X, new width: " << _rect.width() << " " << nw);
      }
    } else {
      if ((_rect.width()/_xAspectRatio) < (_rect.height()/_yAspectRatio)) {
        int nh = rect.width()*_yAspectRatio/_xAspectRatio;
        //  yAxis Reference
        _rect.setY(0.5*(_rect.height()-nh));
        _rect.setHeight(nh);
        BUG_DEBUG(" = Fix Y, new height: " << _rect.height() << " " << nh);
      } else {
        //  xAxis Reference
        int nw = rect.height()*_xAspectRatio/_yAspectRatio;
        _rect.setX(0.5 * (_rect.width() - nw));
        _rect.setWidth(nw);
        BUG_DEBUG(" = Fix X, new width: " << _rect.width() << " " << nw);
      }
    }
  }
  BUG_DEBUG("GuiQwtPlotLayout::activate New rect pos: "
            << _rect.x() << ", " << _rect.y() << ", size: " << _rect.width() << ", " << _rect.height());

  // call base method
  QwtPlotLayout::activate(plot, _rect, options);
  QRectF crect = canvasRect();
  BUG_DEBUG("GuiQwtPlotLayout::activate NEW rect pos: "
            << rect.x() << ", " << rect.y() << ", size: " << rect.width() << ", " << rect.height()
            << ", Canvas: " << crect.x() << ", " << crect.y()
            << ", size: " << crect.width() << ", " << crect.height());
}

//----------------------------------------------------
// invalidate
//----------------------------------------------------
void GuiQwtPlotLayout::invalidate() {
  QwtPlotLayout::invalidate();
}


//----------------------------------------------------
// setCanvasSize
//----------------------------------------------------
void GuiQwtPlotLayout::setCanvasSize(int w, int h) {
  m_canvasWidth = w;
  m_canvasHeight = h;
}
