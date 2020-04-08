cmake_minimum_required(VERSION 2.8)
project(smart-input-filter LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

# Download GoogleTest for mocks and test runner.
include(third_party/DownloadProject/DownloadProject.cmake)
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 UPDATE_DISCONNECTED 1
)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

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

file(GLOB LIBRARY_SOURCES "src/*.cpp")
add_library(filter ${LIBRARY_SOURCES})

target_include_directories(filter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR/src}>
  $<INSTALL_INTERFACE:.>)

enable_testing()

file(GLOB TEST_SOURCES "test/*.cpp")

add_executable(filtertest ${TEST_SOURCES})

target_link_libraries(filtertest filter gmock_main gtest)
add_test(filtertest filtertest)
