configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/log4cplus.properties
  ${CMAKE_CURRENT_BINARY_DIR}/log4cplus.properties
  COPYONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/log4cplus.properties
  DESTINATION config
)
