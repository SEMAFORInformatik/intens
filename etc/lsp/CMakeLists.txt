set(VENV_DIR ${CMAKE_CURRENT_BINARY_DIR}/venv)
if(WIN32)
  set(VENV_BIN "${VENV_DIR}/Scripts")
  set(VENV_PYTHON "${VENV_BIN}/python.exe")
  set(TS_NATIVE_GRAMMAR "libtree-sitter-intens.dll")
else()
  set(VENV_BIN "${VENV_DIR}/bin")
  set(VENV_PYTHON "${VENV_BIN}/python")
  set(TS_NATIVE_GRAMMAR "libtree-sitter-intens.so")
endif()


add_custom_command(
  OUTPUT "${VENV_PYTHON}"
  "${VENV_DIR}"
  COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} python -m venv ${CMAKE_CURRENT_BINARY_DIR}/venv
)


add_custom_command(
  OUTPUT "${VENV_BIN}/pyinstaller"
  DEPENDS requirements.txt
  DEPENDS "${VENV_PYTHON}"
  COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} ${VENV_PYTHON} -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
)

add_custom_target(
  lsp ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pybuild/lsp
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pybuild/lsp
  DEPENDS tree-sitter-grammar
  DEPENDS src/lsp.py
  DEPENDS src/autoCompleter.py
  DEPENDS src/completions.yaml
  DEPENDS "${VENV_BIN}/pyinstaller"
  COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} ${VENV_PYTHON} -m PyInstaller
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lsp.py
    --add-data=${CMAKE_CURRENT_BINARY_DIR}/../tree-sitter-intens/${TS_NATIVE_GRAMMAR}:.
    --add-data=${CMAKE_CURRENT_SOURCE_DIR}/src/completions.yaml:.
    --distpath ${CMAKE_CURRENT_BINARY_DIR}/pybuild
    --clean -y -F --strip
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
  install(
    PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/pybuild/lsp
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    RENAME intens-language-server.exe
  )
else()
  install(
    PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/pybuild/lsp
    TYPE BIN
    RENAME intens-language-server
  )
endif()
