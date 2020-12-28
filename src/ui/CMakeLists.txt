cmake_minimum_required(VERSION 3.13)

set(TARGET_NAME piwo_gui)

set(CMAKE_AUTOUIC_SEARCH_PATHS forms)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extensions)
add_subdirectory(forms)

set(PROJECT_SOURCES
    main_window.cpp
    main_window.hpp
    modules_configuration.cpp
    modules_configuration.hpp
    modules_verify.cpp
    modules_verify.hpp
    visual_debug.cpp
    visual_debug.hpp
    player.cpp
    player.hpp
    settings.cpp
    settings.hpp
    ${UI_SOURCES}
    ${UI_EXT_SOURCE}
    ${UI_EXT_HEADERS}
)

add_library(${TARGET_NAME}
  ${PROJECT_SOURCES}
)

target_include_directories(${TARGET_NAME} PUBLIC extensions)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${TARGET_NAME} PUBLIC 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
)
