if(WIN32)
  set(BIN_SCRIPTS ${application} ${application}.bat)
else()
  set(BIN_SCRIPTS ${application})
endif(WIN32)

set(DIST_BIN_SCRIPTS)

# copy ${application} to build/scripts
add_custom_target(copy_scripts_files ALL
  COMMAND @@{CMAKE_COMMAND} -E copy_if_different
  @@{BIN_SCRIPTS}
  @@{CMAKE_CURRENT_BINARY_DIR}
  BYPRODUCTS @@{BIN_SCRIPTS}
  WORKING_DIRECTORY @@{CMAKE_CURRENT_SOURCE_DIR}
)

### make install ###
install(PROGRAMS @@{BIN_SCRIPTS}
  @@{DIST_BIN_SCRIPTS}
  TYPE BIN
)
