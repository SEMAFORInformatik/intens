
add_subdirectory(qt)

add_library(plot OBJECT
        CyclesDialog.cc
        Frame.cc
        Gdev.cc
        GdevHPGL.cc
        GdevPS.cc
        GdevSVG.cc
        ListGraph.cc
        PSPlot.cc
        Plot2dMenuDescription.cc
        PlotAxis.cc
        ScaleDialog.cc
        ListPlot.cc
        )

if (Protobuf_FOUND)
add_dependencies(plot intens_protobuf)
endif (Protobuf_FOUND)
target_include_directories(plot
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        )

target_link_libraries(plot
   PRIVATE
   LibXml2::LibXml2
   Qt::Widgets
   Qt::Gui)

if (Protobuf_FOUND)
target_link_libraries(plot
        PRIVATE
        ${Protobuf_LIBRARIES})
endif()
