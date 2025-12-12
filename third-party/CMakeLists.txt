# SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
#
# SPDX-License-Identifier: Apache-2.0

if(CMAKE_VERSION VERSION_GREATER "3.31")
  cmake_policy(SET CMP0077 NEW) # honors normal variables.
endif()

# log4cplus, override options
if (USE_LOG4CPLUS AND NOT log4cplus_FOUND)
  message("-- Compile third-party log4cplus")
  option(WITH_UNIT_TESTS "" OFF)
  option(LOG4CPLUS_BUILD_TESTING "" OFF)
  option(LOG4CPLUS_BUILD_LOGGINGSERVER "" OFF)
  if (MINGW)
    option(UNICODE "" OFF)
  endif(MINGW)
  add_subdirectory(log4cplus)
endif()

# jwt-cpp
set(JWT_BUILD_EXAMPLES OFF)
add_subdirectory(jwt-cpp)
