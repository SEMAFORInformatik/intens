
find_package(PkgConfig)
pkg_check_modules(PC_Cppzmq libzmq)

find_path(Cppzmq_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_Cppzmq_INCLUDE_DIRS}
        )

find_library(Cppzmq_LIBRARY
        NAMES zmq
        PATHS ${PC_Cppzmq_LIBRARY_DIRS}
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cppzmq
        FOUND_VAR Cppzmq_FOUND
        REQUIRED_VARS Cppzmq_INCLUDE_DIR Cppzmq_LIBRARY
        VERSION_VAR PC_Cppzmq_VERSION
        )

if(Cppzmq_FOUND AND NOT TARGET Cppzmq::Cppzmq)
    add_library(Cppzmq::Cppzmq UNKNOWN IMPORTED)
    set_target_properties(Cppzmq::Cppzmq
            PROPERTIES
            VERSION ${PC_Cppzmq_VERSION}
            IMPORTED_LOCATION ${Cppzmq_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES "${Cppzmq_INCLUDE_DIR}"
            INTERFACE_COMPILE_OPTIONS "${PC_Cppzmq_CFLAGS_OTHER}"
            )
endif()

mark_as_advanced(Cppzmq_LIBRARY Cppzmq_INCLUDE_DIR)
