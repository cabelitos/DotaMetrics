cmake_minimum_required(VERSION 2.8.11)

project(dota_metrics)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-Wall" COMPILER_GNU_WALL)

if (COMPILER_STD11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (COMPILER_GNU_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

add_subdirectory(libs)
add_subdirectory(crawler)
