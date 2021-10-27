cmake_minimum_required(VERSION 3.16)

project(
  "bayer2rgb"
  VERSION 0.1.0
  LANGUAGES C)

add_library(debayer src/bayer.c src/bayer.h)
set_target_properties(debayer PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(debayer m)

add_executable(${PROJECT_NAME} src/bayer2rgb.c)
target_link_libraries(${PROJECT_NAME} debayer)
