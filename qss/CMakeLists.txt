# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
#
# SPDX-License-Identifier: Apache-2.0

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/default.qss
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/default.qss
  COMMAND cp default.qss ${CMAKE_CURRENT_BINARY_DIR}/default.qss
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

add_custom_target(
  qss ALL
  DEPENDS default.qss
  )

### make install ###
install(
  FILES default.qss
  DESTINATION ${CMAKE_INSTALL_DATADIR}/qss
)
