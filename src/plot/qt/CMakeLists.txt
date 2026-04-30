# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
# SPDX-License-Identifier: Apache-2.0

add_library(plot_qt OBJECT
        Canvas.cc
        GdevQt.cc
        QtCyclesDialog.cc
        QtListPlot.cc
        QtPlot2dConfigDialog.cc
        QtScaleDialog.cc
        QtSimpel.cc
        qtcanvas.cc
        )

target_include_directories(plot_qt
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        )
target_link_libraries(plot_qt
   PRIVATE
   LibXml2::LibXml2
   Qt::Core
   Qt::Core
   Qt::Widgets
   Qt::Gui
   Qt::PrintSupport)
