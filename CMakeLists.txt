cmake_minimum_required(VERSION 3.13.0)

project("Project_PIWO_Player")

set(CMAKE_AUTOUIC_SEARCH_PATHS Forms)
set(TARGET_NAME Player)

include_directories(include)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set (CMAKE_CXX_STANDARD 17)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Gui REQUIRED)

find_package(OpenGL REQUIRED)

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

set(FORM_FILES)

set(MOC_HEADERS)

set(SRC_FILES
    main.cpp
  )

message(STATUS ${FORM_FILES})
add_executable(${TARGET_NAME} ${SRC_FILES} ${MOC_HEADERS} ${FORM_FILES} )

option(BUILD_TESTS "enable tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(core_test)
endif()

if(MSVC)
  target_compile_options("${TARGET_NAME}" PUBLIC /W4 /WX)
else()
  target_compile_options("${TARGET_NAME}" PUBLIC -Wall -Wextra -Wl,rpath=.)
endif()


target_link_libraries( "${TARGET_NAME}"
  ${BASS_LIBRARY}
  ${fmt_LIBRARY}
  ${piwo_LIBRARY}
  ${mipc_LIBRARY}
  ${SYSTEM_SPECIFIC_LIBS}
  stdc++fs
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::OpenGL
)

if(WIN32)
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${COMILER_DIRECTORY}/libstdc++-6.dll" "${CMAKE_CURRENT_BINARY_DIR}"
    RESULT_VARIABLE LIB_COPY_RESULT
  )
  if("${LIB_COPY_RESULT}" EQUAL  "0")
    message(STATUS "libstdc++-6.dll has been found")
  else()
    message(WARNING "libstdc++-6.dll hasn't been found. This might cause problems during player dll loading")
  endif()
endif()

