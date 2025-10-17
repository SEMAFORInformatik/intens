
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)


FLEX_TARGET(lexer_generated_sources
        parserlex.ll
        ${CMAKE_CURRENT_BINARY_DIR}/parserflex.cc
        COMPILE_FLAGS -+
        )

BISON_TARGET(parser_generated_sources
        parseryacc.yy
        ${CMAKE_CURRENT_BINARY_DIR}/parseryacc.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parseryacc.hh
        )

set_property(SOURCE parseryacc.hh PROPERTY SKIP_AUTOMOC ON)

ADD_FLEX_BISON_DEPENDENCY(lexer_generated_sources parser_generated_sources)

add_library(parser OBJECT
        Flexer.cc
        HMap.cc
        IdManager.cc
        InterpreterConfigurator.cc
        XMLParserDocumentHandler.cc
        errorhandler.cc
        ${FLEX_lexer_generated_sources_OUTPUTS}
        ${BISON_parser_generated_sources_OUTPUTS}
        )
target_include_directories(parser
        PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(parser PRIVATE Qt::Core LibXml2::LibXml2)
