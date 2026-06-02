
# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
# SPDX-License-Identifier: Apache-2.0

set(CMAKE_AUTOMOC ON)
message("-- CMAKE_SYSTEM_INFO_FILE: ${CMAKE_SYSTEM_INFO_FILE}")
message("-- CMAKE_SYSTEM_NAME:      ${CMAKE_SYSTEM_NAME}")
message("-- CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("-- CMAKE_SYSTEM:           ${CMAKE_SYSTEM}")
message("-- CMAKE_SYSTEM_VERSION:   ${CMAKE_SYSTEM_VERSION}")
message("-- CMAKE_INSTALL_PREFIX:   ${CMAKE_INSTALL_PREFIX}")

find_package(Gettext REQUIRED)
  if(NOT Gettext_VERSION)
    if(gettext_version)
      set(Gettext_VERSION "${gettext_version}")
    elseif(GETTEXT_VERSION_STRING)
      set(Gettext_VERSION "${GETTEXT_VERSION_STRING}")
    endif()
  endif()
  sbom_add_library("gettext" "${Gettext_VERSION}")
find_package(Boost REQUIRED COMPONENTS iostreams)
  sbom_add_library("boost" "${Boost_VERSION}")
find_package(cppzmq REQUIRED)
  sbom_add_library("zmq" "${cppzmq_VERSION}")
find_package(CURL REQUIRED)
if(NOT CURL_VERSION)
  pkg_check_modules(PC_curl QUIET libcurl)
  set(CURL_VERSION ${PC_curl_VERSION})
endif()
  sbom_add_library("curl" "${CURL_VERSION}")
find_package(LibXml2 REQUIRED)
  if(NOT LibXml2_VERSION)
    pkg_check_modules(PC_libxml2 QUIET libxml2)
    set(LibXml2_VERSION ${PC_libxml2_VERSION})
  endif()
  sbom_add_library("libxml2" "${LibXml2_VERSION}")
find_package(LibXslt REQUIRED)
if(NOT LibXslt_VERSION)
  pkg_check_modules(PC_libxslt QUIET libxslt)
  set(LibXslt_VERSION ${PC_libxslt_VERSION})
endif()
  sbom_add_library("libxslt" "${LibXslt_VERSION}")
find_package(Ltdl REQUIRED)
#  sbom_add_library("ltdl" ${Ldtl_VERSION})
find_package(Intl REQUIRED)
#  sbom_add_library("intl" ${Intl_VERSION})
pkg_check_modules(PC_jsoncpp "jsoncpp")
#message("JSONCPP ${PC_jsoncpp_VERSION}")
sbom_add_library("jsoncpp" ${PC_jsoncpp_VERSION})

if (USE_MATLAB)
  find_path (MATLAB_INCLUDE_DIR include/engine.h
    HINTS /usr/local/matlab /matlab)
  if(MATLAB_INCLUDE_DIR)
    message("-- Found matlab in ${MATLAB_INCLUDE_DIR}/include")
    include_directories("${MATLAB_INCLUDE_DIR}/include")
    add_compile_definitions(HAVE_MATLAB)
  endif()
endif(USE_MATLAB)

if (USE_LOG4CPLUS)
  if (log4cplus_FOUND)
     message("-- Found log4cplus in " ${log4cplus_DIR})
  endif()
endif()
if (USE_QT6)
  set(QT_NO_PRIVATE_MODULE_WARNING ON)
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network SvgWidgets PrintSupport)
  if (USE_QT6CHARTS)
    find_package(Qt6 REQUIRED COMPONENTS Charts)
  endif(USE_QT6CHARTS)
  if (USE_QT6GRAPHS)
    find_package(Qt6 REQUIRED COMPONENTS GraphsWidgets QuickWidgets)
  endif(USE_QT6GRAPHS)
  sbom_add_library("qt6" "${Qt6_VERSION}")
endif()
if (USE_OAUTH)
  if (USE_QT6)
    find_package(Qt6 REQUIRED COMPONENTS NetworkAuth)
  endif()
endif()
if(USE_QT6)
  set(CMAKE_AUTOUIC false)
  qt_standard_project_setup()
endif()
find_package(Qwt REQUIRED)
  sbom_add_library("qwt" "${QWT_VERSION_STRING}")
find_package(ZLIB REQUIRED)
  sbom_add_library("zlib" "${ZLIB_VERSION}")

add_custom_command(
  OUTPUT intens_revision.h_
  COMMAND ./scripts/create-revision.sh ${CMAKE_CURRENT_BINARY_DIR}/intens_revision.h_
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
add_custom_command(
  OUTPUT intens_revision.h
  DEPENDS intens_revision.h_
  COMMAND diff intens_revision.h_ intens_revision.h >/dev/null 2>&1 || cp intens_revision.h_ intens_revision.h
  COMMAND rm intens_revision.h_
  )

add_custom_target(
  revision
  DEPENDS intens_revision.h
  )

if (MINGW)
set(BUILD_SHARED_LIBS OFF)
endif (MINGW)

if (Protobuf_FOUND)
add_subdirectory(protobuf)
endif (Protobuf_FOUND)
add_subdirectory(app)
add_subdirectory(datapool)
add_subdirectory(gui)
add_subdirectory(job)
add_subdirectory(operator)
add_subdirectory(parser)
add_subdirectory(plot)
add_subdirectory(streamer)
add_subdirectory(utils)
add_subdirectory(xfer)
add_subdirectory(xml)

add_dependencies(app parser)
add_dependencies(app revision)

if (MINGW)
  set(res "intensres.rc")
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
endif (MINGW)

add_executable(intens main.cc ${res})

target_include_directories(intens
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src )

target_link_libraries(intens
  PRIVATE
  $<TARGET_OBJECTS:app>
  $<TARGET_OBJECTS:datapool>
  $<TARGET_OBJECTS:gui>
  $<TARGET_OBJECTS:gui_qt>
  $<TARGET_OBJECTS:job>
  $<TARGET_OBJECTS:operator>
  $<TARGET_OBJECTS:parser>
  $<TARGET_OBJECTS:plot>
  $<TARGET_OBJECTS:plot_qt>
  $<TARGET_OBJECTS:streamer>
  $<TARGET_OBJECTS:utils>
  $<TARGET_OBJECTS:xfer>
  $<TARGET_OBJECTS:xml>
  jsoncpp
  Intl::Intl
  Qwt::Qwt
  Qt::PrintSupport
  Qt::Network
  Qt::Widgets
  Qt::Gui
  Qt::Core
  cppzmq
  CURL::libcurl
  Ltdl::Ltdl
  ZLIB::ZLIB
  LibXml2::LibXml2
  LibXslt::LibXslt
  Boost::iostreams
)

if (Protobuf_FOUND)
target_link_libraries(intens
        PRIVATE
        ${Protobuf_LIBRARIES}
        ${ABSL_LIBRARIES}
        intens_protobuf)
endif()
if (USE_OAUTH)
target_link_libraries(intens
        PRIVATE
        Qt::NetworkAuth)
endif()
if (USE_QT6)
target_link_libraries(intens
        PRIVATE
        Qt::SvgWidgets
        )
 if (USE_QT6GRAPHS)
 target_link_libraries(intens
        PRIVATE
        Qt::GraphsWidgets
        Qt::QuickWidgets
        )
 else()
# needed for os windows
###  target_link_libraries(intens
###         PRIVATE
###         Qt::Charts
###         )
 endif(USE_QT6GRAPHS)
 if (USE_QT6CHARTS)
 target_link_libraries(intens
        PRIVATE
        Qt::Charts
        )
 endif(USE_QT6CHARTS)
else()
target_link_libraries(intens
        PRIVATE
        Qt::Svg )
endif()
if (MINGW)
target_link_libraries(intens
        PRIVATE
        sodium
        wldap32
        xml2
        ssh2
        gcrypt
        gpg-error
        idn2
        dl
        ws2_32
        psl
        nghttp2
        unistring
        bcrypt
        )
endif (MINGW)
if (USE_LOG4CPLUS)
  if(QT_FEATURE_shared)
    set(LIB_LOG4CPLUS log4cplus::log4cplus)
  else()
    set(LIB_LOG4CPLUS log4cplus::log4cplusS)
  endif()
  target_link_libraries(intens
    PRIVATE
  ${LIB_LOG4CPLUS}
)
endif()

add_executable(
  mq_request
  mq_request.cc
  )
target_include_directories(
  mq_request
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/third-party
  )
target_link_libraries(
  mq_request
  PUBLIC
  cppzmq
  jsoncpp
  )
if (MINGW)
  target_link_libraries(
    mq_request
    PUBLIC
    sodium
    ws2_32
    iphlpapi
    )
endif (MINGW)

add_executable(
  mq_push
  mq_push.cc
  )
target_include_directories(
  mq_push
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  )
target_link_libraries(
  mq_push
  PUBLIC
  cppzmq
  jsoncpp
  )
if (MINGW)
  target_link_libraries(
    mq_push
    PUBLIC
    sodium
    ws2_32
    iphlpapi
    )
endif (MINGW)

### messagebox ###
add_executable(
  messagebox
  messagebox.cc
  )
target_link_libraries(
  messagebox
  PUBLIC
  Qt::Widgets
  )

# SBOM generation integration
if(ENABLE_SBOM_GENERATION)
    generate_sbom_data(intens)
endif()
 
# todo: should collect libraries but gives error:
#   "could not find requested file"
#include(GetRuntimeDependencies)
#get_runtime_dependencies(
#    EXECUTABLES intens
#    TARGET_PATH ${CMAKE_BINARY_DIR}/bin
#    DESTINATION ${CMAKE_BINARY_DIR}/deploy
#    EXCLUDE_SYSTEM_RUNTIME ON
#    EXCLUDE_SYSTEM_DEPS ON)
#message("TARGET_FILE $<TARGET_FILE:Qt::Core>")
#message("qt6 install prefix ${QT6_INSTALL_PREFIX}")
#add_custom_command(TARGET intens POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy
#        $<TARGET_FILE:Qt::Core>
#        $<TARGET_FILE_DIR:intens>)
### make install ###
install(
  TARGETS intens mq_request messagebox mq_push
  )
if(WIN32)
  if (QT_FEATURE_shared)
    install(FILES $<TARGET_RUNTIME_DLLS:intens> TYPE BIN)
    install(FILES ${QT6_INSTALL_PREFIX}/lib/qwt.dll TYPE BIN)
    install(DIRECTORY ${QT6_INSTALL_PREFIX}/plugins/platforms DESTINATION bin)
    # QT6_INSTALL_PREFIX /usr/src/mxe/usr/x86_64-w64-mingw32.shared/qt6
    #
    #foreach(lib in LISTS jsoncpp curl sodium idn2)
    # install(FILES $<TARGET_RUNTIME:jsoncpp> TYPE BIN)
    #endforeach()

    foreach(lib IN ITEMS libjsoncpp.dll libgcc_s_seh-1.dll libstdc++-6.dll libcurl-4.dll
      libintl-8.dll liblog4cplus.dll libltdl-7.dll libxml2-16.dll
      libxslt-1.dll libzmq.dll zlib1.dll libiconv-2.dll libwinpthread-1.dll
      libbrotlidec.dll libidn2-0.dll libnghttp2-14.dll libpsl-5.dll
      libssh2-1.dll libzstd.dll libdl.dll libsodium-23.dll icuuc74.dll
      icuin74.dll libpcre2-16-0.dll libharfbuzz-0.dll libfreetype-6.dll
      libpng16-16.dll libbrotlicommon.dll libunistring-5.dll icudt74.dll
      libglib-2.0-0.dll libpcre2-8-0.dll libbz2.dll liblzma-5.dll)
      install(FILES /usr/src/mxe/usr/x86_64-w64-mingw32.shared/bin/${lib} TYPE BIN)
    endforeach()
    if (USE_OAUTH)
      install(FILES /usr/src/mxe/usr/x86_64-w64-mingw32.shared/qt6/plugins/tls/qopensslbackend.dll DESTINATION ${CMAKE_INSTALL_BINDIR}/tls)
    endif()
  endif()
else()
  if(EXISTS "/usr/local/qwt-6.3.0/lib/libqwt.so.6.3.0")
    #install(FILES $<TARGET_FILE:Qwt::Qwt> TYPE LIB)
    install(FILES /usr/local/qwt-6.3.0/lib/libqwt.so.6.3.0 TYPE LIB RENAME libqwt.so.6.3)
    #sbom_add_library( .. DESTINATION lib FOLLOW_SYMLINK_CHAIN)
    # cp  /usr/local/qwt.6.3.0/lib/libqwt.so -> libqwt.so.6.3.0
    #install(IMPORTED_RUNTIME_ARTIFACTS Qwt::Qwt)
 endif()
endif()
