cmake_minimum_required(VERSION 3.11)
project(smart-input-filter LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

# Set appropriate warning levels and include debug symbols.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11 -g")

# Generate the file needed for YCM integration
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
  EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
ENDIF()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FetchContent)
include(fetch)

file(GLOB LIBRARY_SOURCES "src/*.cpp")
add_library(smart-input-filter ${LIBRARY_SOURCES})

target_include_directories(smart-input-filter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:src>)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) 
  FetchContent_Declare(googletest
    GIT_REPOSITORY      https://github.com/google/googletest.git
    GIT_TAG             release-1.12.1
  )
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  file(GLOB TEST_SOURCES "test/*.cpp")

  add_executable(filtertest ${TEST_SOURCES})

  target_link_libraries(filtertest smart-input-filter gmock_main gtest)
  add_test(filtertest filtertest)
endif()
