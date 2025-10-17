#ifndef LINEPLOTPICKER_H
#define LINEPLOTPICKER_H

#include <qwt_plot_picker.h>

class GuiQtLinePlot;

class LinePlotPicker: public QwtPlotPicker{
 public:
    LinePlotPicker(GuiQtLinePlot *p, QWidget *c);
    virtual ~LinePlotPicker(){}
};
#endif
