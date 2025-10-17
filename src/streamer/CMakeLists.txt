
add_library(streamer OBJECT
        BasicStream.cc
        CharDataStreamParameter.cc
        ComplexMatrixParameter.cc
        ComplexStreamParameter.cc
        DataStreamParameter.cc
        DatasetStreamParameter.cc
        GuiIndexStreamParameter.cc
        IndexParameter.cc
        IntStreamParameter.cc
        JSONStreamParameter.cc
        MatrixDataParameter.cc
        NumericMatrixParameter.cc
        PlotGroupStreamParameter.cc
        RealStreamParameter.cc
        SerializeFormStreamParameter.cc
        SkipStreamParameter.cc
        Stream.cc
        StreamManager.cc
        StreamParameter.cc
        StreamParameterGroup.cc
        StringDateStreamParameter.cc
        StringMatrixParameter.cc
        StringStreamParameter.cc
        StructStreamParameter.cc
        TableStreamGroup.cc
        TargetContainer.cc
        TokenStreamParameter.cc
        VarStreamParameter.cc
        XMLPlotGroupStreamParameter.cc
        XMLStreamDocumentHandler.cc
        XMLStreamParameter.cc
        )

target_include_directories(streamer
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third-party/rapidjson/include
        ${CMAKE_BINARY_DIR}/src
        )

target_link_libraries(streamer
    PRIVATE
    LibXml2::LibXml2
    Qt::Core
    Qt::Widgets)
