cmake_minimum_required(VERSION 2.8.11)
project(dota_metrics_libs)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)

add_library(dota_metrics_libs SHARED
  DataBase.hpp
  DataBase.cpp
  MatchDetails.hpp
  MatchDetails.cpp
  Hero.hpp
  Hero.cpp
  Player.hpp
  Player.cpp
  ApiRequester.hpp
  ApiRequester.cpp
  Match.hpp
  Match.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../res/resources.qrc)

target_link_libraries(dota_metrics_libs Qt5::Core Qt5::Network Qt5::Sql)