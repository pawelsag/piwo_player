cmake_minimum_required(VERSION 3.10)

set (TEST_NAME scene_test)
include_directories(../include)

file( GLOB T_SRC "*.cpp")
find_library(libgtest gtest REQUIRED)
find_library(libgtestmain gtest_main REQUIRED)
find_library(libgmock gmock REQUIRED)
find_library(libpthread pthread REQUIRED)
pkg_check_modules(fmt REQUIRED fmt)

add_executable(${TEST_NAME} ${T_SRC})

target_link_libraries(${TEST_NAME} ${fmt_LIBRARIES} ${libgtestmain} ${libgtest} ${libgmock} ${libpthread})
  
