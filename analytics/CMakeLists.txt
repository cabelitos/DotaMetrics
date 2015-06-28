cmake_minimum_required(VERSION 2.8.11)
project(dota_metrics_analytics)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libs/)

add_executable(dota_metrics_analytics
  NaiveBayes.hpp
  NaiveBayes.cpp
  Analytics.hpp
  Analytics.cpp
  main.cpp)

target_link_libraries(dota_metrics_analytics dota_metrics_libs Qt5::Core)