# SBOM Generation CMake Module
# SPDX-FileCopyrightText: 2026 SEMAFOR Informatik & Energie AG
# SPDX-License-Identifier: Apache-2.0
# 
# inspired by
# https://sbomgenerator.com/guides/cpp#cmake-integration-for-sbom-tracking
function(sbom_init)
  file(WRITE "${CMAKE_BINARY_DIR}/dependencies.txt"  "")
  find_package(PkgConfig)
endfunction()

function(sbom_add_library name version)
  if(ENABLE_SBOM_GENERATION)
    file(APPEND  "${CMAKE_BINARY_DIR}/dependencies.txt" "${name};${version}\n")
  endif()
endfunction()

function(generate_sbom_data target_name)
  # Get target properties
  get_target_property(target_type ${target_name} TYPE)
  #get_target_property(target_sources ${target_name} SOURCES)
  get_target_property(link_libraries ${target_name} LINK_LIBRARIES)

  # Create SBOM metadata file
  set(sbom_metadata_file "${CMAKE_BINARY_DIR}/sbom_metadata.json")

  # Generate metadata at configure time
  configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/sbom_template.json.in"
    "${sbom_metadata_file}"
    @ONLY
  )

  # Add custom command to generate SBOM at build time
  add_custom_command(
    TARGET ${target_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Generating SBOM for ${target_name}"
    COMMAND python3 "${CMAKE_SOURCE_DIR}/scripts/cmake_sbom_generator.py"
      --build-dir "${CMAKE_BINARY_DIR}"
      --target "${target_name}"
      --output "${CMAKE_BINARY_DIR}/sbom-${target_name}.cyclonedx.json"
    COMMENT "Generating SBOM for ${target_name}"
  )
endfunction()

# Function to extract dependency information
# not yet used
function(extract_dependency_info)
    # Export target information for SBOM generation
    export(TARGETS myapp FILE "${CMAKE_BINARY_DIR}/myapp-targets.cmake")

    # Generate dependency graph
    execute_process(
        COMMAND ${CMAKE_COMMAND} --graphviz=deps.dot .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        RESULT_VARIABLE graphviz_result
    )

    if(graphviz_result EQUAL 0)
        message(STATUS "Generated dependency graph: deps.dot")
    endif()
endfunction()
