
find_package(PkgConfig)
pkg_check_modules(PC_cppzmq QUIET libzmq)
set(cppzmq_VERSION ${PC_cppzmq_VERSION})
find_path(cppzmq_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_cppzmq_INCLUDE_DIRS}
        )
# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
# SPDX-License-Identifier: Apache-2.0

find_library(cppzmq_LIBRARY
        NAMES zmq
        PATHS ${PC_cppzmq_LIBRARY_DIRS}
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cppzmq
        FOUND_VAR cppzmq_FOUND
        REQUIRED_VARS cppzmq_INCLUDE_DIR cppzmq_LIBRARY
        VERSION_VAR PC_cppzmq_VERSION
        )

if(cppzmq_FOUND AND NOT TARGET cppzmq)
    add_library(cppzmq UNKNOWN IMPORTED)
    set_target_properties(cppzmq
            PROPERTIES
            VERSION ${PC_cppzmq_VERSION}
            IMPORTED_LOCATION ${cppzmq_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES "${cppzmq_INCLUDE_DIR}"
            INTERFACE_COMPILE_OPTIONS "${PC_cppzmq_CFLAGS_OTHER}"
    )
endif()

mark_as_advanced(cppzmq_LIBRARY cppzmq_INCLUDE_DIR)
