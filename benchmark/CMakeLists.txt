cmake_minimum_required(VERSION 3.15)

include(CTest)

add_executable(${PROJECT_NAME}Benchmarks src/bayer2rgb_benchmark.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${CMAKE_PROJECT_NAME}Benchmarks
                           PRIVATE ${CMAKE_SOURCE_DIR}/src)

find_package(fmt REQUIRED)
find_package(benchmark REQUIRED)
# include(FetchContent)
# FetchContent_Declare(googlebenchmark
#   GIT_REPOSITORY https://github.com/google/benchmark
#   GIT_TAG main
#   )
# FetchContent_MakeAvailable(googlebenchmark)
#
# FetchContent_Declare(fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG master
#   )
# FetchContent_MakeAvailable(fmt)
find_package(Threads REQUIRED)


target_link_libraries(
  ${CMAKE_PROJECT_NAME}Benchmarks
  PRIVATE benchmark::benchmark ${CMAKE_THREAD_LIBS_INIT} fmt::fmt-header-only
          debayer)

add_test(NAME ${CMAKE_PROJECT_NAME}Benchmarks
         COMMAND ${CMAKE_PROJECT_NAME}Benchmarks)
