# needed for IMPLICIT_DEPENDS
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(TEMPLATES ${PROJECT_SOURCE_DIR}/templates/datapool.mako
  ${PROJECT_SOURCE_DIR}/templates/functions.mako
  ${PROJECT_SOURCE_DIR}/templates/projfuncs.mako
  ${PROJECT_SOURCE_DIR}/templates/ui.mako
)

add_custom_command(OUTPUT datapool.inc
  COMMAND ${CMAKE_C_COMPILER} -E -ffreestanding -C -x c -I.
  -I${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/datapool.inc.in > datapool.inc
  IMPLICIT_DEPENDS C ${CMAKE_CURRENT_SOURCE_DIR}/datapool.inc.in
)

add_custom_command(OUTPUT persist.xml
  COMMAND intens --startToken DATAPOOL --persistfile persist.xml datapool.inc
  COMMAND python ${PROJECT_SOURCE_DIR}/scripts/persistgen.py persist.xml
  DEPENDS ${PROJECT_SOURCE_DIR}/scripts/persistgen.py
  DEPENDS datapool.inc
  DEPENDS ${TEMPLATES}
  BYPRODUCTS IntensBugFile
  update-db.json
  g_project_functions.inc
  g_logbookDb_datapool.inc
  g_logbookDb_functions.inc
  g_logbookDb_uiManager.inc
# BEGIN BLOCK COMPONENT
#  g_<component>_datapool.inc
#  g_<component>_functions.inc
#  g_<component>_uiManager.inc
# END BLOCK COMPONENT
  g_motor_datapool.inc
  g_motor_functions.inc
  g_motor_uiManager.inc
)

add_custom_command(OUTPUT scim.des
  COMMAND ${CMAKE_C_COMPILER} -E -ffreestanding -C -x c -I.
  -I${CMAKE_CURRENT_SOURCE_DIR} ${DES_FLAGS}
  "-D_VERSION_=\\\"${PACKAGE_VERSION}\\\""
  ${CMAKE_CURRENT_SOURCE_DIR}/scim.des.in > scim.des
  DEPENDS ${TEMPLATES}
  DEPENDS persist.xml
  IMPLICIT_DEPENDS C ${CMAKE_CURRENT_SOURCE_DIR}/scim.des.in
)

add_custom_command(OUTPUT scim.ini
  COMMAND sed
  -e 's/_VERSION_/${PACKAGE_VERSION}/g'
  -e 's/_FONT_/${FONT}/g'
  ${CMAKE_CURRENT_SOURCE_DIR}/scim.ini.in >scim.ini
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scim.ini.in
)

add_custom_command(OUTPUT flyway.sql
  DEPENDS persist.xml
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/flyway.py persist.xml flyway.sql
)

add_custom_target(datapool
  DEPENDS datapool.inc
)

add_custom_target(g_files
  DEPENDS persist.xml
)

add_custom_target(scim-des ALL
  DEPENDS scim.des
)

add_custom_target(scim-ini ALL
  DEPENDS scim.ini
)

add_custom_target(flyway
  DEPENDS flyway.sql
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/scim.des
  ${CMAKE_CURRENT_BINARY_DIR}/scim.ini
  DESTINATION etc
)
