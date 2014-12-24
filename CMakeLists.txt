cmake_minimum_required(VERSION 2.8.11)

project(dota_metrics)
set(CMAKE_BUILD_TYPE Release)

#Make this portable
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-Wall" COMPILER_GNU_WALL)
CHECK_CXX_COMPILER_FLAG("/W4" COMPILER_VS_WALL)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_STD11)

if (COMPILER_STD11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#else -- Maybe it's visual studio. Hope for the best.
endif()

if (COMPILER_GNU_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (COMPILER_VS_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

add_subdirectory(src/libs)
add_subdirectory(src/crawler)
