cmake_minimum_required(VERSION 3.18)

project(bench-unchecked-push-back LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Catch2)

add_executable(tests
    benches.cpp
    tests.cpp
    vector-simple.hpp
)

target_link_libraries(tests
  PRIVATE
    Catch2::Catch2WithMain
)

if (NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()