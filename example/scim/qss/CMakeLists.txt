add_custom_command(OUTPUT scim.qss
  COMMAND sed
  -e 's/_MARGIN_TOP_/${MARGIN_TOP}/g'
  -e 's/_PADDING_TOP_/${PADDING_TOP}/g'
  ${CMAKE_CURRENT_SOURCE_DIR}/scim.qss.in > scim.qss
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scim.qss.in
)

add_custom_target(qss ALL
  DEPENDS scim.qss
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/scim.qss
  DESTINATION qss
)
