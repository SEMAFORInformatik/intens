# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
#
# SPDX-License-Identifier: Apache-2.0

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generator/application)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generator/application/CMakeLists.txt
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generator/application/CMakeLists.txt
  COMMAND sed -e "s@--INTENS-VERSION--@${PACKAGE_VERSION}@"
        ${CMAKE_CURRENT_SOURCE_DIR}/generator/application/CMakeLists.txt > CMakeLists.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generator/application
)

add_custom_target(
  templates ALL
  DEPENDS generator/application/CMakeLists.txt
)

### make install ###
install(CODE [[
  set(target_dir "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/templates/generator")
  message(STATUS "Removing directory: ${target_dir}")
  file(REMOVE_RECURSE "${target_dir}")
]])

install(
  DIRECTORY generator
  DESTINATION ${CMAKE_INSTALL_DATADIR}/templates
  USE_SOURCE_PERMISSIONS
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/generator/application/CMakeLists.txt
  DESTINATION ${CMAKE_INSTALL_DATADIR}/templates/generator/application/
)
