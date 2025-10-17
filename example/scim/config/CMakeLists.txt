add_custom_command(OUTPUT log4cplus.properties
  COMMAND echo "*** copy log4cplus.properties ***"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${CMAKE_CURRENT_SOURCE_DIR}/log4cplus.properties .
)

add_custom_target(properties ALL
  DEPENDS log4cplus.properties
)

install(FILES log4cplus.properties
  DESTINATION config
)
