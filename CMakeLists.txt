cmake_minimum_required(VERSION 3.21)
project(Peachnx)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_QT_UI "Enable the use of the GUI")
option(USE_BUNDLE_SDL2 "Enable the internal version of the SDL2 library")

if(USE_BUNDLE_SDL2)
    add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
else()
    find_package(SDL2)
endif()

if(USE_QT_UI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    qt_standard_project_setup()
endif()

add_subdirectory(src)