if(WIN32)
  set(BIN_SCRIPTS scim scim.bat)
else()
  set(BIN_SCRIPTS scim)
endif(WIN32)

set(DIST_BIN_SCRIPTS)

# copy scim to build/scripts
add_custom_target(copy_scripts_files ALL
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${BIN_SCRIPTS}
  ${CMAKE_CURRENT_BINARY_DIR}
  BYPRODUCTS ${BIN_SCRIPTS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

install(PROGRAMS ${BIN_SCRIPTS}
  ${DIST_BIN_SCRIPTS}
  TYPE BIN
)
