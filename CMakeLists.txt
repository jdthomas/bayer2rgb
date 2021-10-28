cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "Vcpkg toolchain file")

project(
  "bayer2rgb"
  VERSION 0.1.0
  LANGUAGES C CXX)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)
include(cmake/CompilerWarnings.cmake)
# TODO: Cleanup the warnings and enable!
set(WARNINGS_AS_ERRORS OFF)
set_project_warnings(project_warnings)


configure_file(src/bayer.c src/bayer.cpp COPYONLY)
add_library(debayer_cpp src/bayer.cpp src/bayer.h)
set_target_properties(debayer_cpp PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(debayer_cpp PRIVATE src)
target_link_libraries(debayer_cpp m project_options project_warnings)

add_library(debayer src/bayer.c src/bayer.h)
set_target_properties(debayer PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(debayer m project_options project_warnings)

add_executable(${PROJECT_NAME} src/bayer2rgb.c)
target_link_libraries(${PROJECT_NAME} debayer project_options project_warnings)

enable_testing()
add_subdirectory(test)
add_subdirectory(benchmark)
