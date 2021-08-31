cmake_minimum_required(VERSION 3.15)

include(CTest)

add_executable(${PROJECT_NAME}Benchmarks src/bayer2rgb_benchmark.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${CMAKE_PROJECT_NAME}Benchmarks
                           PRIVATE ${CMAKE_SOURCE_DIR}/src)

find_package(fmt)
find_package(Threads REQUIRED)
find_package(benchmark REQUIRED)
target_link_libraries(
  ${CMAKE_PROJECT_NAME}Benchmarks
  PRIVATE benchmark::benchmark ${CMAKE_THREAD_LIBS_INIT} fmt::fmt-header-only
          debayer)

add_test(NAME ${CMAKE_PROJECT_NAME}Benchmarks
         COMMAND ${CMAKE_PROJECT_NAME}Benchmarks)
