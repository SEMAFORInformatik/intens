set(CMAKE_RC_COMPILER_INIT windres)
enable_language(RC)
set(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>"
)

set(CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} -static"
)

# Build an executable with a WinMain entry point on windows.
add_executable(scim WIN32 scim.cc res.rc)

install(TARGETS scim DESTINATION ".")
