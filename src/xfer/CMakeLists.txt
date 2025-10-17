
add_subdirectory(gdtoa)

add_library(xfer OBJECT
        Scale.cc
        XferConverter.cc
        XferDataItem.cc
        XferDataItemIndex.cc
        XferDataParameter.cc
        XferGenericParameter.cc
        XferParameterComplex.cc
        XferParameterInteger.cc
        XferParameterReal.cc
        XferParameterString.cc
        XferScale.cc
        )

add_dependencies(xfer gdtoa)

target_include_directories(xfer
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        )

target_link_libraries(xfer
   PRIVATE
   LibXml2::LibXml2
   Qt::Core
   Qt::Widgets)
