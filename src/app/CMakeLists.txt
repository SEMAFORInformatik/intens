
add_library(app OBJECT
  App.cc
  AppData.cc
  AppData.h
  ColorSet.cc
  DataAlterGetDbModified.cc
  DataAlterSetDbModified.cc
  DataEventsIntens.cc
  DataPoolIntens.cc
  DataPoolUndoListener.cc
  DataSet.cc
  HardCopyListener.cc
  HelpBrowser.cc
  HelpFile.cc
  HelpHTML.cc
  HelpManager.cc
  HelpText.cc
  HelpURL.cc
  IntensInterface.cc
  ItemAttr.cc
  ModuleFactory.cc
  Plugin.cc
  QuitApplication.cc
  ReportGen.cc
  SmartIntensInterface_Impl.cc
  UiManager.cc
  UserAttr.cc
  log.cc
  md5.cc
)
target_include_directories(app
  PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
)

if (USE_OAUTH)
  target_sources(app
    PRIVATE
    oauthclient.cc)
  target_link_libraries(app PRIVATE Qt::NetworkAuth)
endif()

target_link_libraries(app
  PRIVATE
  LibXml2::LibXml2
  Qt::Core
  Qt::Widgets
  Qt::Gui )
