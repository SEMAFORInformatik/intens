
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0

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