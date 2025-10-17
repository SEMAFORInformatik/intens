
add_custom_target(
  tree-sitter-grammar
  DEPENDS tree-sitter-intens
  )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/grammar.json
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/node-types.json
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/parser.c
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/tree_sitter
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tree-sitter.json
  COMMAND tree-sitter generate --abi 14 ${CMAKE_CURRENT_SOURCE_DIR}/grammar.js
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tree-sitter.json
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/tree-sitter.json ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(
  tree-sitter-intens SHARED
  ${CMAKE_CURRENT_BINARY_DIR}/src/parser.c
)

install(
  TARGETS tree-sitter-intens
)
