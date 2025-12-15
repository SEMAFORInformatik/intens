cmake_minimum_required(VERSION 3.12)
project(${application} C CXX)
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.31")
  cmake_policy(SET CMP0177 NEW)
endif()
enable_testing()

set(PACKAGE_NAME ${application})
# The version number.
set(PACKAGE_VERSION_MAJOR 1)
set(PACKAGE_VERSION_MINOR 0)
set(PACKAGE_VERSION_PATCH "0-snapshot")
set(PACKAGE_VERSION "@@{PACKAGE_VERSION_MAJOR}.@@{PACKAGE_VERSION_MINOR}.@@{PACKAGE_VERSION_PATCH}")

option(USE_DB "use database" ON)
if(USE_DB)
  set(DES_FLAGS @@{DES_FLAGS} -DHAVE_DB)
endif(USE_DB)

option(USE_UNITMANAGER "use unit manager" OFF)
if(USE_UNITMANAGER)
  set(UNITMANAGER --unitManager)
endif(USE_UNITMANAGER)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${applicationName}")
set(CPACK_PACKAGE_VENDOR "SEMAFOR")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "@@{CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#set(CPACK_RESOURCE_FILE_LICENSE "@@{CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
set(CPACK_PACKAGE_VERSION_MAJOR @@{PACKAGE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR @@{PACKAGE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH @@{PACKAGE_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "")

include(InstallRequiredSystemLibraries)

if(WIN32)
  set(CMAKE_INSTALL_PREFIX /c/Semafor/@@{PACKAGE_NAME}/@@{PACKAGE_VERSION})
else()
  set(CMAKE_INSTALL_PREFIX /usr/local/@@{PACKAGE_NAME}/@@{PACKAGE_VERSION})
endif(WIN32)

set(INTENS_VERSION --INTENS-VERSION--)
set(INTENS_DIR "C:\\Semafor\\intens")

set(FONT "Sans Serif")
set(MARGIN_TOP "3ex")
set(PADDING_TOP "0ex")

if(WIN32)
  set(CPACK_GENERATOR "NSIS")
  set(CMAKE_MODULE_PATH @@{PROJECT_SOURCE_DIR}/nsis @@{CMAKE_MODULE_PATH})

  # support for uninstallation
  set(CPACK_NSIS_ENABLE_UNINSTALL ON)

  # # start menu
  set(CPACK_START_MENU_SHORTCUTS "@@{CPACK_PACKAGE_VENDOR};@@{PACKAGE_NAME}")
  # add shortcut during installation
  set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '\$SMPROGRAMS\\\\\$STARTMENU_FOLDER\\\\@@{PACKAGE_NAME}.lnk' '\$INSTDIR\\\\@@{PACKAGE_NAME}.exe'"
  )
  # remove shortcut when uninstalling
  set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '\$SMPROGRAMS\\\\\$START_MENU\\\\@@{PACKAGE_NAME}.lnk'"
  )

  add_definitions(-DINTENS_VERSION=\"@@{INTENS_VERSION}\")
  add_subdirectory(mingw)
  configure_file(
    @@{PROJECT_SOURCE_DIR}/nsis/NSIS.definitions.in
    @@{CMAKE_CURRENT_BINARY_DIR}/nsis/NSIS.definitions.nsh
)  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_PACKAGE_ICON "@@{CMAKE_SOURCE_DIR}\\\\mingw\\\\semafor.ico")
  set(FONT "Arial")
  set(MARGIN_TOP "6ex")
  set(PADDING_TOP "0ex")
endif(WIN32)

include(CPack)
set(CPACK_NSIS_INSTALL_ROOT "C:\\Semafor")

# i18n
include(po/Gettext_helpers.cmake)
configure_gettext(
    DOMAIN "${application}"
    TARGET_NAME "update-po"
    POTFILE_DESTINATION "@@{PROJECT_SOURCE_DIR}/po"
    POFILE_DESTINATION "@@{PROJECT_SOURCE_DIR}/po"
    INSTALL_DESTINATION "share/locale/"
    INSTALL_COMPONENT "translation"
    LANGUAGES "en_US"
    XGETTEXT_ARGS "--c++" "--from-code=UTF-8"
    "--keyword=_" "--keyword=N_" "--keyword=P_:1,2"
    "--package-name=@@{PROJECT_NAME}"
    "--package-version=@@{PACKAGE_VERSION}"
    "--copyright-holder=COPYRIGHT HOLDER"
    "--msgid-bugs-address=bugs@example.com"
    SOURCES "etc/approval_report/approval_report.inc"
    "etc/common/common_datapool.inc"
    "etc/component/component_datapool.inc"
    "etc/element_rename/element_rename_datapool.inc"
    "etc/element_rename/element_rename_functions.inc"
    "etc/element_rename/element_rename_operator.inc"
    "etc/element_rename/element_rename_uiManager.inc"
    "etc/logbook/logbook_datapool.inc"
    "etc/logbook/logbook_operator.inc"
    "etc/logbook/logbook_uiManager.inc"
    "etc/main_window/main_window_uiManager.inc"
    "etc/menu/menu.inc"
    "etc/modifications/modifications_datapool.inc"
    "etc/modifications/modifications_uiManager.inc"
    "etc/project/project_datapool.inc"
    "etc/project/project_functions.inc"
    "etc/project/project_uiManager.inc"
    "etc/python_logger/python_logger_datapool.inc"
    "etc/python_logger/python_logger_uiManager.inc"
    "etc/used_in_projects/used_in_projects_datapool.inc"
    "etc/used_in_projects/used_in_projects_uiManager.inc"
    "etc/user/user_datapool.inc"
    "etc/user/user_uiManager.inc"
    "etc/variant/variant_datapool.inc"
    "etc/variant/variant_functions.inc"
    "etc/variant/variant_uiManager.inc"
# BEGIN BLOCK COMPONENT
#    "etc/<component>/<component>_datapool.inc"
#    "etc/<component>/<component>_uiManager.inc"
# END BLOCK COMPONENT
)

add_subdirectory(scripts)
add_subdirectory(etc)
add_subdirectory(qss)
add_subdirectory(bitmaps)
add_subdirectory(config)
add_subdirectory(api-gateway)
#add_subdirectory(tests)

add_custom_target(update-db
  COMMAND update-database.py --db workbook
)

add_custom_target(create-changelogs
  WORKING_DIRECTORY @@{CMAKE_SOURCE_DIR}
  COMMAND create-changelogs.py db/liquibase
)

add_custom_target(docker-image
  COMMAND "@@{CMAKE_CURRENT_SOURCE_DIR}/docker/create-image.sh" @@{CMAKE_PROJECT_VERSION}
  COMMENT "build docker image"
)

add_custom_target(docker-image-api-gateway
  COMMAND "@@{CMAKE_CURRENT_SOURCE_DIR}/api-gateway/create-image.sh" @@{CMAKE_PROJECT_VERSION}
  COMMENT "build docker image api-gateway"
)

add_custom_target(docker-images)

add_dependencies(docker-images docker-image docker-image-api-gateway)

add_custom_target(docker-up
  COMMAND docker-compose -f @@{CMAKE_CURRENT_SOURCE_DIR}/docker/docker-compose.yml up -d
  COMMENT "docker up"
)

add_custom_target(docker-down
  COMMAND docker-compose -f @@{CMAKE_CURRENT_SOURCE_DIR}/docker/docker-compose.yml down
  COMMENT "docker down"
)

add_custom_target(docker-restart
  COMMAND docker-compose -f @@{CMAKE_CURRENT_SOURCE_DIR}/docker/docker-compose.yml restart
  COMMENT "docker restart"
)

add_custom_target(docker-ps
  COMMAND docker-compose -f @@{CMAKE_CURRENT_SOURCE_DIR}/docker/docker-compose.yml ps
  COMMENT "docker ps"
)

add_custom_target(docker-logs
  COMMAND docker-compose -f @@{CMAKE_CURRENT_SOURCE_DIR}/docker/docker-compose.yml logs
  COMMENT "docker logs"
)

add_custom_target(docker-logsf
  COMMAND docker-compose -f @@{CMAKE_CURRENT_SOURCE_DIR}/docker/docker-compose.yml logs -f
  COMMENT "docker logs -f"
)
