cmake_minimum_required(VERSION 2.8.11)
project(dota_metrics_libs)

qt5_add_resources(LIB_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../res/resources.qrc)

add_library(dota_metrics_libs SHARED
  Utils.hpp
  Utils.cpp
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
  ${LIB_RESOURCES})

target_link_libraries(dota_metrics_libs Qt5::Core Qt5::Network Qt5::Sql)