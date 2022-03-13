cmake_minimum_required(VERSION 3.15)
project(thh_map LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  thh_handles SOURCE_DIR /Users/tomhultonharrop/Documents/Projects/cpp-handles)
FetchContent_MakeAvailable(thh_handles)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_link_libraries(${PROJECT_NAME} INTERFACE thh_handles)

add_executable(${PROJECT_NAME}_test)
target_sources(${PROJECT_NAME}_test PRIVATE main.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME})

option(THH_MAP_ENABLE_BENCH "Enable benchmarking" OFF)

if(THH_MAP_ENABLE_BENCH)
  set(BENCHMARK_ENABLE_TESTING OFF)
  FetchContent_Declare(benchmark
                       GIT_REPOSITORY https://github.com/google/benchmark.git)
  FetchContent_MakeAvailable(benchmark)
  add_executable(${PROJECT_NAME}_bench)
  target_sources(${PROJECT_NAME}_bench PRIVATE bench.cpp)
  target_link_libraries(${PROJECT_NAME}_bench ${PROJECT_NAME} benchmark)
  target_compile_options(
    ${PROJECT_NAME}_bench
    PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>
            $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>: -Wall -Wextra
            -pedantic>)
endif()
