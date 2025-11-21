# Intens - Copyright (C) 1990-2025
# SEMAFOR Informatik & Energie AG, Basel, Switzerland
#
# See the LICENSE file in the Intens root directory for license information.

cmake_minimum_required(VERSION 3.20)

if(CMAKE_VERSION VERSION_GREATER "3.31")
  cmake_policy(SET CMP0177 NEW) # install()`` ``DESTINATION`` paths are normalized.
endif()
if(CMAKE_VERSION VERSION_GREATER "3.17")
  cmake_policy(SET CMP0100 NEW) # Let AUTOMOC, AUTOUIC process .hh files
endif()

project(intens LANGUAGES C CXX)
# project does not support non numeric suffixes:
# project(intens VERSION 5.1.10dev LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)  # valid values 11 14 17 20 24
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PACKAGE_NAME ${PROJECT_NAME})
# The version number.
set(PACKAGE_VERSION_MAJOR 6)
set(PACKAGE_VERSION_MINOR 0)
set(PACKAGE_VERSION_PATCH 4dev)
string(CONCAT PACKAGE_VERSION
  "${PACKAGE_VERSION_MAJOR}"
  ".${PACKAGE_VERSION_MINOR}"
  ".${PACKAGE_VERSION_PATCH}"
)
if(UNIX)
  include(GNUInstallDirs)
endif()

# change the default without overriding a user-provided value
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR WIN32)
  if(WIN32)
    set(CMAKE_INSTALL_PREFIX
      ./semafor/${PACKAGE_NAME}/${PACKAGE_VERSION} CACHE PATH
        "Choose the installation directory; by default it installs
         in the semafor/${PACKAGE_NAME}/${PACKAGE_VERSION} directory."
         FORCE)
  else()
    set(CMAKE_INSTALL_PREFIX
      /usr/local/${PACKAGE_NAME}/${PACKAGE_VERSION} CACHE PATH
        "Choose the installation directory; by default it installs
         in the /usr/local/${PACKAGE_NAME}/${PACKAGE_VERSION} directory."
         FORCE)
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
    # don't used RPATH when building
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  endif()
  set(CMAKE_INSTALL_DATADIR "." CACHE PATH "public dataroot dir" FORCE)
endif()

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${PROJECT_SOURCE_DIR}/cmake/Modules
)

# define _DEBUG in debug mode (cmake -DCMAKE_BUILD_TYPE=Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Options
option(NLS "Enable native language support" ON)
option(USE_LOG4CPLUS "use log4cplus Library" ON)
option(USE_OAUTH "use OAuth / QtNetworkAuth" ON)
option(USE_PROTOBUF "use protobuf" ON)
option(USE_QT6 "use Qt6 Library" ON)
option(USE_LSP "build the language server (requires tree-sitter)" OFF)
option(USE_MATLAB "use the matlab API (requires matlab)" ON)

if (NLS)
  add_compile_definitions(ENABLE_NLS)
endif()

if (USE_LOG4CPLUS)
  find_package(log4cplus QUIET)
  if (log4cplus_FOUND)
    add_compile_definitions(HAVE_LOG4CPLUS)
    set(INTENS_INSTALL_FLAGS "-DHAVE_LOG4CPLUS=1")
  endif()
endif()

if (USE_OAUTH)
  add_compile_definitions(HAVE_OAUTH)
endif()

if (USE_QT6)
  message("-- USE_QT6: ${USE_QT6}")
endif()

if (USE_PROTOBUF)
find_package(Protobuf)
if (Protobuf_FOUND)
  message("-- Found Protobuf")
  add_compile_definitions(HAVE_PROTOBUF)
else()
  message("-- Protobuf not found")
endif()
endif()

# i18n
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/po/")
include(Gettext_helpers)
set (YEAR 2025)
configure_gettext(
    DOMAIN "intens"
    TARGET_NAME "update-po"
    POTFILE_DESTINATION "${CMAKE_SOURCE_DIR}/po"
    POFILE_DESTINATION "${CMAKE_SOURCE_DIR}/po"
    INSTALL_DESTINATION "share/locale/"
    INSTALL_COMPONENT "translation"
    LANGUAGES "de" "en" "fr" "pl"
    XGETTEXT_ARGS
        "--c++" "--from-code=UTF-8"
        "--keyword=_" "--keyword=N_" "--keyword=P_:1,2"
        "--package-name=${PROJECT_NAME}"
        "--package-version=${PACKAGE_VERSION}"
        "--copyright-holder=SEMAFOR Informatik \\& Energie AG, Switzerland"
        "--msgid-bugs-address=bugs@semafor.ch"
    SOURCES
        "etc/MessageQueueReply.inc"
        "src/app/App.cc"
        "src/app/AppData.cc"
        "src/app/DataPoolIntens.cc"
        "src/app/HelpFile.cc"
        "src/app/HelpManager.cc"
        "src/app/HelpText.cc"
        "src/app/QuitApplication.cc"
        "src/app/ReportGen.cc"
        "src/app/UiManager.cc"
        "src/datapool/DataComplexElement.cc"
        "src/datapool/DataIntegerElement.cc"
        "src/datapool/DataRealElement.cc"
        "src/gui/DataProcessing.h"
        "src/gui/DialogProgressBar.cc"
        "src/gui/Gui3dPlot.cc"
        "src/gui/GuiComboBox.cc"
        "src/gui/GuiDataField.cc"
        "src/gui/GuiIndex.cc"
        "src/gui/GuiIndexMenu.cc"
        "src/gui/GuiScrolledlist.cc"
        "src/gui/GuiTable.cc"
        "src/gui/qt/ArrowKeyLineEdit.cc"
        "src/gui/qt/DateValidator.cc"
        "src/gui/qt/GuiMFMSettings.cc"
        "src/gui/qt/GuiQtCycleButton.cc"
        "src/gui/qt/GuiQtCycleDialog.cc"
        "src/gui/qt/GuiQtDataField.cc"
        "src/gui/qt/GuiQtDiagram.cc"
        "src/gui/qt/GuiQtElement.cc"
        "src/gui/qt/GuiQtFactory.cc"
        "src/gui/qt/GuiQtFolder.cc"
        "src/gui/qt/GuiQtForm.cc"
        "src/gui/qt/GuiQtImage.cc"
        "src/gui/qt/GuiQtIndex.cc"
        "src/gui/qt/GuiQtLinePlot.cc"
        "src/gui/qt/GuiQtList.cc"
        "src/gui/qt/GuiQtManager.cc"
        "src/gui/qt/GuiQtNavigator.cc"
        "src/gui/qt/GuiQtPrinterDialog.cc"
        "src/gui/qt/GuiQtScrolledlist.cc"
        "src/gui/qt/GuiQtScrolledText.cc"
        "src/gui/qt/GuiQtSvgView.cc"
        "src/gui/qt/GuiQtTableDataItem.cc"
        "src/gui/qt/GuiQtTableViewBase.cc"
        "src/gui/qt/GuiQtText.cc"
        "src/gui/qt/GuiQtTextfield.cc"
        "src/gui/qt/GuiQtTimetable.cc"
        "src/gui/qt/GuiQtToggle.cc"
        "src/gui/qt/GuiQwt3dPlot.cc"
        "src/gui/qt/GuiQwtPlot.cc"
        "src/gui/qt/GuiQwtPlotPicker.cc"
        "src/gui/qt/GuiQwtPlotZoomer.cc"
        "src/gui/qt/GuiQwtPropertyDialog.cc"
        "src/gui/qt/GuiQwtSlider.cc"
        "src/gui/qt/GuiQwtThermo.cc"
        "src/gui/qt/QtDialogCompare.cc"
        "src/gui/qt/QtDialogConfirmation.cc"
        "src/gui/qt/QtDialogCopyright.cc"
        "src/gui/qt/QtDialogInputExt.cc"
        "src/gui/qt/QtDialogInputExt.h"
        "src/gui/qt/QtDialogProgressBar.cc"
        "src/gui/qt/QtDialogUserPassword.cc"
        "src/gui/qt/QtDialogWarning.cc"
        "src/gui/qt/QtDialogWorkClock.cc"
        "src/gui/qt/QtIconManager.cc"
        "src/gui/qt/QtInputDialog.cc"
        "src/gui/qt/QtMessageBox.cc"
        "src/gui/qt/QtMessageBoxExt.cc"
        "src/gui/qt/QtMultiFontString.cc"
        "src/job/JobCodeOthers.cc"
        "src/job/JobStackData.cc"
        "src/operator/Channel.cc"
        "src/operator/ClientSocket.cc"
        "src/operator/FileStream.cc"
        "src/operator/IntensServerSocket.cc"
        "src/operator/MatlabInitProcess.cc"
        "src/operator/MatlabProcess.cc"
        "src/operator/MessageQueuePublisher.cc"
        "src/operator/MessageQueueReply.cc"
        "src/operator/MessageQueueRequest.cc"
        "src/operator/MessageQueueSubscriber.cc"
        "src/operator/Process.cc"
        "src/operator/ProcessGroup.cc"
        "src/operator/ProcessWrapper.cc"
        "src/operator/QtClientSocket.cc"
        "src/operator/QtServerSocket.cc"
        "src/operator/ReportStream.cc"
        "src/operator/RestService.cc"
        "src/parser/errorhandler.cc"
        "src/parser/InterpreterConfigurator.cc"
        "src/parser/parseryacc.yy"
        "src/plot/CyclesDialog.cc"
        "src/plot/ListGraph.cc"
        "src/plot/ListPlot.cc"
        "src/plot/qt/QtListPlot.cc"
        "src/plot/qt/QtPlot2dConfigDialog.cc"
        "src/plot/qt/QtSimpel.cc"
        "src/plot/ScaleDialog.cc"
        "src/streamer/BasicStream.cc"
        "src/streamer/PlotGroupStreamParameter.cc"
        "src/streamer/Stream.cc"
        "src/xml/XMLDocumentHandler.cc"
)

### TODO #####################################
add_compile_definitions(
        HAVE_QT
        PACKAGE="intens"
        VERSION="${PACKAGE_VERSION}"
)
if (UNIX)
add_compile_definitions(
        LTDL_SHLIB_EXT=".so"
)
elseif (MINGW)
add_compile_definitions(
        LTDL_SHLIB_EXT=".dll"
        ZMQ_STATIC
        LIBXSLT_STATIC
        CURL_STATICLIB
)
endif (UNIX)

add_subdirectory(third-party EXCLUDE_FROM_ALL)
add_subdirectory(src)
add_subdirectory(qss)
add_subdirectory(etc)
add_subdirectory(scripts)
add_subdirectory(templates)
add_subdirectory(bitmaps)
add_subdirectory(reportconv)

# enable testing
enable_testing()
add_subdirectory(test)

# packaging
SET(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})
SET(CPACK_PACKAGE_NAME "semafor-${PROJECT_NAME}")
SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGING_INSTALL_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_PACKAGE_VENDOR "SEMAFOR Informatik & Energie AG, Switzerland")
SET(CPACK_PACKAGE_CONTACT "info@semafor.ch")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
  "integration and development framework and runtime platform for scientific application")
set(CPACK_DEBIAN_PACKAGE_SECTION Miscellaneous) # Science?
set(CPACK_SET_DESTDIR ON) # use DESTDIR mechanism when packaging.
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

if(WIN32)
  set(CPACK_GENERATOR ZIP)
else()
  set(CPACK_GENERATOR TGZ)
endif()
set(CPACK_STRIP_FILES ON)
include(CPack)

add_custom_target(
  docker-image
  COMMENT "build docker image"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND "GITHUB_REF_NAME=${PACKAGE_VERSION}" "docker" "buildx" "bake"
  )

add_custom_target(
  docker-dev-image
  COMMENT "build dev docker image"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND "GITHUB_REF_NAME=${PACKAGE_VERSION}" "docker" "buildx" "bake" "dev"
  )

add_custom_target(
  docker-linux-build-image
  COMMENT "build linux-build docker image"
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/docker/create-linux-build-image.sh" ${PACKAGE_VERSION}
  )

add_custom_target(
  intens-windows-build
  COMMENT "compile intens for windows"
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/dockcross/build-windows-release.sh" "static"
  )
add_custom_target(
  intens-ubuntu-build
  COMMENT "compile intens for Ubuntu"
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/dockcross/build-ubuntu.sh"
  )

add_custom_target(
  intens-windows-shared-build
  COMMENT "compile intens for windows"
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/dockcross/build-windows-release.sh" "shared"
  )

### TODO:
### - config.h
### - flex version
### - headless
### - gdtoa
