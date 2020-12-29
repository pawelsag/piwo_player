cmake_minimum_required(VERSION 3.13.0)

project("Project_PIWO_Player")

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core REQUIRED)

add_library(logger
  logger.cpp
)

target_link_libraries(logger
  Qt5::Core
  ${fmt_LIBRARY}
)

target_include_directories(logger PUBLIC .)
