cmake_minimum_required(VERSION 3.13)

project("Project_PIWO_Player")

set(TARGET_NAME Player)

include_directories(include)

set (CMAKE_CXX_STANDARD 20)

find_library(mipc_LIBRARY
             NAMES mipc
             PATHS "${PROJECT_SOURCE_DIR}/libs/libmipc/lib"
             REQUIRED)
find_library(piwo_LIBRARY
             NAMES piwo
             PATHS "${PROJECT_SOURCE_DIR}/libs/libpiwo/lib"
             REQUIRED)
find_library(fmt_LIBRARY
             NAMES fmt
             PATHS "${PROJECT_SOURCE_DIR}/libs/libfmt/lib"
             REQUIRED)
find_library(BASS_LIBRARY
             NAMES bass
             PATHS "${PROJECT_SOURCE_DIR}/libs/libbass"
             REQUIRED)
if(WIN32)
  set(SYSTEM_SPECIFIC_LIBS ws2_32 Winmm)
else()
  set(SYSTEM_SPECIFIC_LIBS pthread X11 rt)
endif()

set(SRC_FILES
    main.cpp
)

add_subdirectory(src)

add_executable(${TARGET_NAME} ${SRC_FILES})

option(BUILD_TESTS "enable tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(core_test)
endif()

target_compile_options("${TARGET_NAME}" PUBLIC -Wall -Wextra -Wl,rpath=.)

target_link_libraries("${TARGET_NAME}"
  ${BASS_LIBRARY}
  ${fmt_LIBRARY}
  ${piwo_LIBRARY}
  ${mipc_LIBRARY}
  ${SYSTEM_SPECIFIC_LIBS}
  piwo_gui
)
