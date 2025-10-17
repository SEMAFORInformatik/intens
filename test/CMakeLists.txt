add_subdirectory(behave)

find_package(GTest)
if(GTest_FOUND)
# https://google.github.io/googletest
add_executable(gdtoa_test gdtoa_test.cc)
target_include_directories(gdtoa_test
  PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src/xfer/gdtoa)

target_link_libraries(gdtoa_test
 PRIVATE
  GTest::gtest_main
        gdtoa)

include(GoogleTest)
gtest_discover_tests(gdtoa_test)
endif(GTest_FOUND)
