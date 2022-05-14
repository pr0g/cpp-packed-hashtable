cmake_minimum_required(VERSION 3.15)
project(thh-packed-hashtable LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  thh-handle-vector
  GIT_REPOSITORY https://github.com/pr0g/cpp-handle-container.git
  GIT_TAG 8a0ee9812c4034ec3859b64f529bc8beb215eb5d)
FetchContent_MakeAvailable(thh-handle-vector)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_link_libraries(${PROJECT_NAME} INTERFACE thh-handle-vector)

option(THH_PACKED_HASHTABLE_ENABLE_MEMORY "Enable memory profiling" OFF)
option(THH_PACKED_HASHTABLE_ENABLE_TEST "Enable testing" OFF)
option(THH_PACKED_HASHTABLE_ENABLE_BENCH "Enable benchmarking" OFF)
option(THH_PACKED_HASHTABLE_ENABLE_ECS "Enable ECS experiment" OFF)

if(THH_PACKED_HASHTABLE_ENABLE_MEMORY)
  add_executable(${PROJECT_NAME}-memory)
  target_sources(${PROJECT_NAME}-memory PRIVATE memory.cpp)
  target_link_libraries(${PROJECT_NAME}-memory PRIVATE ${PROJECT_NAME})
  target_compile_features(${PROJECT_NAME}-memory PRIVATE cxx_std_17)
endif()

if(THH_PACKED_HASHTABLE_ENABLE_ECS)
  add_executable(${PROJECT_NAME}-ecs)
  target_sources(${PROJECT_NAME}-ecs PRIVATE ecs.cpp)
  target_link_libraries(${PROJECT_NAME}-ecs PRIVATE ${PROJECT_NAME})
  target_compile_features(${PROJECT_NAME}-ecs PRIVATE cxx_std_17)
endif()

if(THH_PACKED_HASHTABLE_ENABLE_TEST)
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    GIT_TAG f0ab4e793fa863d4a5929ff263ddd3bebec8767e)
  FetchContent_MakeAvailable(doctest)
  add_executable(${PROJECT_NAME}-test)
  target_sources(${PROJECT_NAME}-test PRIVATE test.cpp)
  target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME} doctest)
  target_compile_options(
    ${PROJECT_NAME}-test
    PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4
            /WX>
            $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:
            -Weverything
            -Wall
            -Wextra
            -pedantic
            -Wno-c++98-compat;-Wno-
            -Wno-sign-conversion;-Wno->
            $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall
            -Wextra
            -pedantic>)
  target_compile_features(${PROJECT_NAME}-test PRIVATE cxx_std_17)
endif()

if(THH_PACKED_HASHTABLE_ENABLE_BENCH)
  set(BENCHMARK_ENABLE_TESTING OFF)
  FetchContent_Declare(benchmark
                       GIT_REPOSITORY https://github.com/google/benchmark.git)
  FetchContent_Declare(abseil
                       GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git)
  FetchContent_Declare(
    robin-hood
    GIT_REPOSITORY https://github.com/martinus/robin-hood-hashing.git)
  FetchContent_MakeAvailable(benchmark abseil robin-hood)
  add_executable(${PROJECT_NAME}-bench)
  target_sources(${PROJECT_NAME}-bench PRIVATE bench.cpp)
  target_link_libraries(${PROJECT_NAME}-bench ${PROJECT_NAME} benchmark
                        absl::flat_hash_map robin_hood)
  target_compile_options(
    ${PROJECT_NAME}-bench
    PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX>
            $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang,GNU>: -Wall -Wextra
            -pedantic>)
  target_compile_features(${PROJECT_NAME}-bench PRIVATE cxx_std_17)
endif()
