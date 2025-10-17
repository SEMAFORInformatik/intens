add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/MessageQueueReply.inc
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/MessageQueueReply.inc
  COMMAND ${CMAKE_C_COMPILER}  -E -ffreestanding -C -x c -I. ${INTENS_INSTALL_FLAGS} MessageQueueReply.inc > ${CMAKE_CURRENT_BINARY_DIR}/MessageQueueReply.inc
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

execute_process(
  COMMAND python -c "import mako"
  RESULT_VARIABLE MAKO_FOUND_EXIT_CODE
  OUTPUT_QUIET
)

if (${MAKO_FOUND_EXIT_CODE} EQUAL 0)
  set(G_INTENS_TS_EL ${CMAKE_CURRENT_BINARY_DIR}/g-intens-ts.el)
endif()

add_custom_target(
  etc ALL
  DEPENDS MessageQueueReply.inc ${G_INTENS_TS_EL}
  )

if (USE_LSP)
  add_subdirectory(lsp)
  add_subdirectory(tree-sitter-intens)
endif()


add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/g-intens-ts.el
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/highlights.json
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/generateHighlights.py --format emacs ${CMAKE_CURRENT_BINARY_DIR}/g-intens-ts.el
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )


### make install ###
install(
  FILES  ${CMAKE_CURRENT_BINARY_DIR}/MessageQueueReply.inc UnitManager.inc intens-mode.el intens-ts-mode.el ${G_INTENS_TS_EL}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/etc
  )
