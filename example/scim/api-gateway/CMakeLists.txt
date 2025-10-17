if(WIN32)
  install(CODE "
    execute_process(COMMAND PyInstaller.exe --clean -y --noconfirm
      ${CMAKE_SOURCE_DIR}/api-gateway/api-gateway.spec)
    if(EXISTS ${CMAKE_SOURCE_DIR}/.git)
      execute_process(COMMAND git --git-dir=${CMAKE_SOURCE_DIR}/.git describe
        OUTPUT_FILE ${CMAKE_BINARY_DIR}/dist/api-gateway/vcs.info
        RESULT_VARIABLE GIT_DESCRIBE_RESULT)
    else()
      file(WRITE ${CMAKE_BINARY_DIR}/dist/api-gateway/vcs.info \"\")
    endif()
  ")
  install(DIRECTORY "${CMAKE_BINARY_DIR}/dist/api-gateway"
    DESTINATION .
  )
else()
  add_subdirectory(zrpc)

  set(DIST_BIN_SCRIPTS api-gateway.py
    config.py
    scim_calc.py
  )

  install(PROGRAMS ${DIST_BIN_SCRIPTS}
    DESTINATION api-gateway
  )
endif(WIN32)

if(NOT CMAKE_VERSION VERSION_LESS "3.12")
  find_package(Python3 COMPONENTS Interpreter)
else()
  find_package(PythonInterp REQUIRED)
endif()
if(Python3_Interpreter_FOUND OR PythonInterp_FOUND)
  add_test(NAME api_gateway_tests
    COMMAND pytest -r a -v
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()
