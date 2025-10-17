
add_library(plot_qt OBJECT
        Canvas.cc
        GdevQt.cc
        QtCyclesDialog.cc
        QtListPlot.cc
        QtPlot2dConfigDialog.cc
        QtScaleDialog.cc
        QtSimpel.cc
        qtcanvas.cpp
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
