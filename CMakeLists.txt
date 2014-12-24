cmake_minimum_required(VERSION 2.8.11)

project(dota_metrics)

set(CMAKE_AUTOMOC ON)

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

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)

add_executable(dota_metrics
  src/DataBase.hpp
  src/DataBase.cpp
  src/MatchDetails.hpp
  src/MatchDetails.cpp
  src/Hero.hpp
  src/Hero.cpp
  src/Player.hpp
  src/Player.cpp
  src/ApiRequester.hpp
  src/ApiRequester.cpp
  src/Match.hpp
  src/Match.cpp
  src/Crawler.hpp
  src/Crawler.cpp
  src/main.cpp)

target_link_libraries(dota_metrics Qt5::Core Qt5::Network Qt5::Sql)