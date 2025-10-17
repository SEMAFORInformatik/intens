add_custom_command(OUTPUT ${application}.qss
  COMMAND sed
  -e 's/_MARGIN_TOP_/@@{MARGIN_TOP}/g'
  -e 's/_PADDING_TOP_/@@{PADDING_TOP}/g'
  @@{CMAKE_CURRENT_SOURCE_DIR}/${application}.qss.in > ${application}.qss
  DEPENDS @@{CMAKE_CURRENT_SOURCE_DIR}/${application}.qss.in
)

add_custom_target(qss ALL
  DEPENDS ${application}.qss
)

install(FILES @@{CMAKE_CURRENT_BINARY_DIR}/${application}.qss
  DESTINATION qss
)
