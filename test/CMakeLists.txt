cmake_minimum_required(VERSION 3.16)

include(CTest)

add_executable(${PROJECT_NAME}Tests src/bayer2rgb_test.cpp)
set_target_properties(${PROJECT_NAME}Tests PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}Tests PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME}Tests
                           PRIVATE ${PROJECT_SOURCE_DIR}/src)

message("Added include dir: ${PROJECT_SOURCE_DIR}/src")

find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
# include(FetchContent)
# FetchContent_Declare(googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG master
#   )
# FetchContent_MakeAvailable(googletest)
#
# FetchContent_Declare(fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG master
#   )
# FetchContent_MakeAvailable(fmt)
find_package(Threads REQUIRED)

target_link_libraries(
  ${PROJECT_NAME}Tests
  PUBLIC GTest::GTest GTest::Main debayer ${CMAKE_THREAD_LIBS_INIT}
  PRIVATE fmt::fmt-header-only)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}Tests)
