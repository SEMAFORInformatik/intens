
find_path(Ltdl_INCLUDE_DIR
        NAMES ltdl.h
        )

find_library(Ltdl_LIBRARY
        NAMES ltdl
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ltdl
        FOUND_VAR Ltdl_FOUND
        REQUIRED_VARS Ltdl_INCLUDE_DIR Ltdl_LIBRARY
        )

if(Ltdl_FOUND AND NOT TARGET Ltdl::Ltdl)
    add_library(Ltdl::Ltdl UNKNOWN IMPORTED)
    set_target_properties(Ltdl::Ltdl
            PROPERTIES
            IMPORTED_LOCATION ${Ltdl_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES "${Ltdl_INCLUDE_DIR}"
            )
endif()

mark_as_advanced(Ltdl_INCLUDE_DIR Ltdl_LIBRARY)
