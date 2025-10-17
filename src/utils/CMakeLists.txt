
add_library(utils OBJECT
        Date.cc
        Debugger.cc
        FileUtilities.cc
        HTMLConverter.cc
        IntensMutex.cc
        IntensThread.cc
        JsonUtils.cc
        LaTeXConverter.cc
        NumLim.cc
        OsFileName.cc
        Root.cc
        StringUtils.cc
        UrlConverter.cc
        base64.cc
        utils.cc
        gzstream.cc
        )

target_include_directories(utils
  PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src)

target_link_libraries(utils
    PRIVATE
    Qt::Core
    Qt::Widgets)
